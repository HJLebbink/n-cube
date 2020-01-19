#pragma once
#include <set>
#include <string>
#include <filesystem>
#include <iostream>		// std::cout
#include <fstream>
#include <set>
#include <unordered_map>

#include "BF.h"
#include "Transformations.h"


namespace cube {
	namespace plot {

		namespace {

			using Trans = std::string;

			template <typename T1, typename T2>
			struct Edge {
				T1 from;
				T1 to;
				std::set<T2> transitions;
				bool bidirectional;

				Edge(T1 from, T1 to, const T2& transition, bool bidirectional)
					: from(from), to(to), transitions(std::set<T2>{transition}), bidirectional(bidirectional)
				{}
				Edge(T1 from, T1 to, const std::set<T2>& transitions, bool bidirectional)
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
					label += t;// std::to_string(t);
					if (counter < count) {
						label += "\\n";
					}
					counter++;
				}
				return label;
			}

			template <typename T1, typename T2>
			std::vector<Edge<T1, T2>> remove_bidirectonal(const std::vector<Edge<T1, T2>>& edges)
			{
				std::vector<Edge<T1, T2>> results;
				for (const Edge<T1, T2>& e : edges)
				{
					bool found = false;
					for (Edge<T1, T2>& e2 : results)
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

			template <typename T1, typename T2>
			std::vector<Edge<T1, T2>> remove_equal(const std::vector<Edge<T1, T2>>& edges) {
				std::vector<Edge<T1, T2>> results;
				for (const Edge<T1, T2>& e : edges) {
					bool found = false;
					for (Edge<T1, T2>& e2 : results) {
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

			template <int N, typename T1, typename T2>
			void write_to_dot_file(const std::vector<Edge<T1, T2>>& edges, const std::string& filename)
			{
				//const bool already_exists = std::filesystem::exists(filename);
				const bool already_exists = false;

				if (!already_exists) {
					std::fstream myfile(filename, std::fstream::out); // replace existing file
					if (myfile.good()) 
					{
						myfile << "digraph G {" << std::endl;
						for (const Edge<T1, T2>& edge : edges)
						{
							const std::string label = "label=\"" + to_string(edge.transitions) + "\"";
							const std::string bidirectional = (edge.bidirectional) ? " dir=both" : "";
							const std::string line = to_string_bin<N>(edge.from) + " -> " + to_string_bin<N>(edge.to) + " [" + label + bidirectional + " fontsize=10];";
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

			std::vector<Edge<BF, Trans>> all_edges;
			for (const BF bf : equiv_class)
			{
				for (const auto& pair : transformations)
				{
					const BF next_bf = transform<N>(bf, std::get<0>(pair));
					all_edges.push_back(Edge(bf, next_bf, std::get<1>(pair), false));
				}
			}

			const std::vector<Edge<BF, Trans>> edges1 = remove_equal(all_edges);
			const std::vector<Edge<BF, Trans>> edges2 = remove_bidirectonal(edges1);
			write_to_dot_file<N>(edges2, filename);
		}

		template <int N>
		void plot_npn_outgoing_edges(
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

			//const std::vector<Edge<Trans>> edges1 = remove_equal(all_edges);
			///const std::vector<Edge<Trans>> edges2 = remove_bidirectonal(edges1);


			std::set<Trans> outgoing_edges;
			for (const Edge<Trans>& e : all_edges) {
				if (e.from == class_id) {
					for (auto t : e.transitions) outgoing_edges.insert(t);
				}
			}
			std::cout << to_string_bin<N>(class_id) + " (" << outgoing_edges.size()<<  ") = ";
			for (const Trans& t : outgoing_edges) {
				std::cout << t << "; ";
			}
			std::cout << std::endl;

		}

		// plot the cube of transformations
		template <int N>
		void plot_transformation_transitions(
			const std::string& filename)
		{
			using T1 = std::string;
			using T2 = std::string;

			const Transformations<N> all_transformations = create_transformations<N, true>();
			const Transformations<N> atomic_transformations = details::create_transformations_struct<N>::value(create_descriptions<N>());

			std::vector<Edge<T1, T2>> all_edges;
			for (const std::pair<CubeI<N>, std::string>& transition1 : all_transformations)
			{
				CubeI<N> t1 = std::get<0>(transition1);
				for (const std::pair<CubeI<N>, std::string>& transition2 : all_transformations)
				{
					const CubeI<N> t2 = std::get<0>(transition2);
					const CubeI<N> t3 = transform<N>(t1, t2);

					std::string node2 = "x";
					for (const std::pair<CubeI<N>, std::string>& transition : all_transformations) {
						if (std::get<0>(transition) == t2) {
							node2 = std::get<1>(transition);
							break;
						}
					}
					std::string node3 = "x";
					for (const std::pair<CubeI<N>, std::string>& transition : all_transformations) {
						if (std::get<0>(transition) == t3) {
							node3 = std::get<1>(transition);
							break;
						}
					}

					const std::string label = std::get<1>(transition2);
					all_edges.push_back(Edge<T1, T2>(node2, node3, label, false));
				}
			}

			const std::vector<Edge<T1, T2>> edges1 = remove_equal(all_edges);
			const std::vector<Edge<T1, T2>> edges2 = remove_bidirectonal(edges1);


			write_to_dot_file<N>(edges2, filename);
		}
	}
}