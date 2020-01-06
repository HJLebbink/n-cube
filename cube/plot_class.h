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

		template <int N>
		void plot_npn_classes(
			const BF class_id,
			const std::set<BF>& equiv_class,
			const std::string& filename)
		{
			const Transformations<N> transformations = create_transformations<N, true>();

			//const bool already_exists = std::filesystem::exists(filename);
			const bool already_exists = false;

			if (!already_exists) {
				std::fstream myfile(filename, std::fstream::out); // replace existing file
				if (myfile.good()) {

					std::vector<std::tuple<BF, BF, std::string, bool>> edges;
					for (const BF bf : equiv_class) 
					{
						for (const auto& pair : transformations)
						{
							bool edge_already_exists = false;
							const std::string& transform_str = std::get<1>(pair);
							const BF next_bf = transform<N>(bf, std::get<0>(pair));
							if (next_bf == bf)
							{
								edge_already_exists = true;
							} 
							else 
							{
								for (auto& p : edges) {
									if ((std::get<0>(p) == bf) && (std::get<1>(p) == next_bf)) {
										edge_already_exists = true;
										std::string& current_str = std::get<2>(p);
										if (current_str.find(transform_str) != std::string::npos) {
											std::get<2>(p) = current_str + "\\n" + transform_str;
											break;
										}
									}
									if ((std::get<0>(p) == next_bf) && (std::get<1>(p) == bf) && (std::get<2>(p) == transform_str)) {
										std::get<3>(p) = true;
										edge_already_exists = true;
									}
								}
							}
							if (!edge_already_exists) {
								edges.push_back(std::make_tuple(bf, next_bf, transform_str, false));
							}
						}
					}

					myfile << "graph G {" << std::endl;
					for (const auto& p : edges) 
					{
						const std::string arrow = (std::get<3>(p)) ? " <-> " : " -> ";

						std::string line;
						if (std::get<0>(p) == std::get<1>(p)) {
							line = to_string_bin<N>(std::get<0>(p)) + arrow + to_string_bin<N>(std::get<1>(p)) + " [ label=\" " + std::get<2>(p) + "\" shape=\"doublecircle\"];";
						}
						else {
							line = to_string_bin<N>(std::get<0>(p)) + arrow + to_string_bin<N>(std::get<1>(p)) + " [ label=\" " + std::get<2>(p) + "\"];";
						}
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
}