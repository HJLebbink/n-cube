#pragma once
#include <set>
#include <string>
#include <filesystem>
#include <iostream>		// std::cout
#include <fstream>
#include <set>

#include "BF.h"
#include "Transformations.h"


namespace cube {
	namespace plot {

		namespace {

			using Trans = int;

			template <typename T>
			struct Edge {
				BF from;
				BF to;
				std::set<int> transitions;
				bool bidirectional;

				Edge(BF from, BF to, const T& transition, bool bidirectional)
					: from(from), to(to), transitions(std::set<T>{transition}), bidirectional(bidirectional)
				{}
				Edge(BF from, BF to, const std::set<T>& transitions, bool bidirectional)
					: from(from), to(to), transitions(transitions), bidirectional(bidirectional)
				{}
			};

			// return true when a is a subset of b
			template <typename T>
			bool subset(const std::set<T>& a, const std::set<T>& b) noexcept {
				for (const T& str_a : a) {
					if (!b.contains(str_a)) return false;
				}
				return true;
			}

			template <typename T>
			std::string to_string(const std::set<T>& transitions) 
			{
				std::string label = "";
				const int count = static_cast<int>(transitions.size())-1;

				int counter = 0;
				for (const T& t : transitions) {
					label += std::to_string(t);
					if (counter < count) {
						label += "\\n";
					}
					counter++;
				}
				return label;
			}

			std::vector<Edge<Trans>> remove_bidirectonal(const std::vector<Edge<Trans>>& edges)
			{
				std::vector<Edge<Trans>> results;
				for (const Edge<Trans>& e : edges)
				{
					bool found = false;
					for (Edge<Trans>& e2 : results)
					{
						if ((e2.from == e.to) && (e2.to == e.from) && (e2.transitions == e.transitions))
						{	// found an existing edge that is pointing the other way around
							e2.bidirectional = true;
							found = true;
							break;
						}
					}
					if (!found) {
						results.push_back(e);
					}
				}
				return results;
			}

			std::vector<Edge<Trans>> remove_equal(const std::vector<Edge<Trans>>& edges) {
				std::vector<Edge<Trans>> results;
				for (const Edge<Trans>& e : edges) {
					bool found = false;
					for (Edge<Trans>& e2 : results) {
						if ((e.from == e2.from) && (e.to == e2.to) && (e.bidirectional == e2.bidirectional)) {
							for (const Trans& t : e.transitions) {
								e2.transitions.insert(t);
								found = true;
								break;
							}
						}
					}
					if (!found) {
						results.push_back(e);
					}
				}
				return results;
			}

			template <int N>
			void write_to_dot_file(BF class_id, const std::vector<Edge<Trans>>& edges, const std::string& filename)
			{
				//const bool already_exists = std::filesystem::exists(filename);
				const bool already_exists = false;

				if (!already_exists) {
					std::fstream myfile(filename, std::fstream::out); // replace existing file
					if (myfile.good()) 
					{
						myfile << "digraph G {" << std::endl;
						for (const Edge<Trans>& edge : edges)
						{
							const std::string label = "label=\"" + to_string(edge.transitions) + "\"";
							const std::string bidirectional = (edge.bidirectional) ? " dir=both" : "";
							const std::string shape = (edge.from == class_id) ? " shape=box" : "";
							const std::string line = to_string_bin<N>(edge.from) + " -> " + to_string_bin<N>(edge.to) + " [" + label + bidirectional + shape + " fontsize=10];";
							std::cout << line << std::endl;
							myfile << line << std::endl;
						}
						myfile << "}" << std::endl;
					}
					else {
						std::cout << "WARNING NPN class file " << filename << " not good" << std::endl;
					}
					myfile.close();
				}
				else {
					std::cout << "WARNING: NPN class file " << filename << " already exists in " << std::filesystem::current_path() << std::endl;
				}
			}
		}

		template <int N>
		void plot_npn_classes(
			const BF class_id,
			const std::set<BF>& equiv_class,
			const std::string& filename)
		{
			const Transformations<N> transformations = create_transformations<N, true>();
			std::vector<std::pair<CubeI<N>, int>> transformations2;

			int transformation_id = 0;
			for (auto& pair : transformations) {
				std::cout << transformation_id << " = " << cube::to_string<N>(std::get<0>(pair)) << " = " << std::get<1>(pair) << std::endl;
				transformations2.push_back(std::make_pair(std::get<0>(pair), transformation_id));
				transformation_id++;
			}

			std::vector<Edge<Trans>> all_edges;
			for (const BF bf : equiv_class)
			{
				for (const auto& pair : transformations2)
				{
					const BF next_bf = transform<N>(bf, std::get<0>(pair));
					all_edges.push_back(Edge(bf, next_bf, std::get<1>(pair), false));
				}
			}

			const std::vector<Edge<Trans>> edges1 = remove_equal(all_edges);
			const std::vector<Edge<Trans>> edges2 = remove_bidirectonal(edges1);
			write_to_dot_file<N>(class_id, edges2, filename);
		}
	}
}