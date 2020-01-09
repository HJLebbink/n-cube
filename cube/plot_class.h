#pragma once
#include <set>
#include <string>
#include <filesystem>
#include <iostream>		// std::cout
#include <fstream>

#include "BF.h"
#include "Transformations.h"


namespace cube {
	namespace plot {

		namespace {

			struct Edge {
				BF from;
				BF to;
				std::string transition;
				bool bidirectional;

				Edge(BF from, BF to, const std::string& transition, bool bidirectional)
					: from(from), to(to), transition(transition), bidirectional(bidirectional)
				{}
			};

			std::vector<Edge> remove_bidirectonal(const std::vector<Edge>& edges)
			{
				std::vector<Edge> results;

				for (const Edge& e : edges)
				{
					bool found = false;
					for (Edge& e2 : results)
					{
						if ((e2.from == e.to) && (e2.to == e.from) && (e2.transition == e.transition))
						{	// found an existing edge that is pointing the other way around
							e2.bidirectional = true;
							found = true;
							break;
						}
					}
					if (!found) {
						results.push_back(Edge(e.from, e.to, e.transition, e.bidirectional));
					}
				}
				return results;
			}

			std::vector<Edge> remove_subsumed(const std::vector<Edge>& edges) {
				return edges;

				//if ((std::get<0>(p) == bf) && (std::get<1>(p) == next_bf))
				//{	// found an existing edge that is equal to (bf->next_bf); add the description to the existing description
				//	edge_already_exists = true;
				//	std::string& current_str = std::get<2>(p);
				//	if (current_str.find(transform_str) != std::string::npos) {
				//		std::get<2>(p) = current_str + "\\n" + transform_str;
				//		break;
				//	}
				//}

			}

			template <int N>
			void write_to_dot_file(const std::vector<Edge>& edges, const std::string& filename)
			{
				//const bool already_exists = std::filesystem::exists(filename);
				const bool already_exists = false;

				if (!already_exists) {
					std::fstream myfile(filename, std::fstream::out); // replace existing file
					if (myfile.good()) 
					{
						myfile << "digraph G {" << std::endl;
						for (const Edge& edge : edges)
						{
							const std::string label = "label=\"" + edge.transition + "\"";
							const std::string bidirectional = (edge.bidirectional) ? " dir=both" : "";
							const std::string shape = (edge.from == edge.to) ? " shape=\"doublecircle\"" : "";
							const std::string line = to_string_bin<N>(edge.from) + " -> " + to_string_bin<N>(edge.to) + " [" + label + bidirectional + shape + "];";
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

			std::vector<Edge> all_edges;
			for (const BF bf : equiv_class)
			{
				for (const auto& pair : transformations)
				{
					const BF next_bf = transform<N>(bf, std::get<0>(pair));
					if (next_bf != bf)
					{
						all_edges.push_back(Edge(bf, next_bf, transition_str, false));
					}
				}
			}

			const std::vector<Edge> edges1 = remove_subsumed(all_edges);
			const std::vector<Edge> edges2 = remove_bidirectonal(edges1);
			write_to_dot_file(edges2, filename);
		}
	}
}