#pragma once
#include "n_cube.h"


namespace cube {

	namespace rewrite {

		template <int N> void test_greedy_rewrite_algorithm()
		{
			bool found_error = false;

			for (BF bf = 0; bf < (1ull << (1 << N)); ++bf)
			{
				const auto r1 = details::search_class_id_method0<N, true>(bf);
				const auto r2 = details::search_class_id_method1<N, true>(bf);

				const BF bf1 = std::get<0>(r1);
				const BF bf2 = std::get<0>(r2);

				if ((bf & 0xFF) == 0) std::cout << ".";

				if (bf1 != bf2)
				{
					std::cout << std::endl << "ERROR: test_greedy_rewrite_algorithm<" << N << ">:" << std::endl;
					std::cout << "bf      = " << to_string_hex<N>(bf) << " = " << to_string_bin<N>(bf) << "; number of bits set:" << details::count_bits(bf) << std::endl;
					std::cout << "closure = " << to_string_hex<N>(bf1) << " = " << to_string_bin<N>(bf1) << " " << std::get<1>(r1) << std::endl;
					std::cout << "greedy  = " << to_string_hex<N>(bf2) << " = " << to_string_bin<N>(bf2) << " " << std::get<1>(r2) << std::endl;

					found_error = true;
					getchar();
				}
			}
			if (!found_error) std::cout << std::endl << "test_greedy_rewrite_algorithm<" << N << ">: found no errors" << std::endl;
		}

		template <int N> void find_greedy_rewrite_algorithms()
		{
			std::vector<CubeI<N>> all_tranformations_vector;
			int counter = 0;
			for (const auto& t : details::get_transformations_from_cache<N, false>())
			{
				all_tranformations_vector.push_back(std::get<0>(t));
				std::cout << counter << ":" << to_string<N>(t) << std::endl;
				counter++;
			}
			const int s = static_cast<int>(all_tranformations_vector.size());
			std::cout << "number of transformations = " << s << std::endl;

			std::array<CubeI<N>, 6> selection;

			if (true)
			{
				selection[0] = all_tranformations_vector[27];
				selection[1] = all_tranformations_vector[14];
				selection[2] = all_tranformations_vector[6];
				selection[3] = all_tranformations_vector[41];
				selection[4] = all_tranformations_vector[2];
				selection[5] = all_tranformations_vector[7];

				const auto closure1 = details::transitive_closure_x<N>(selection);
				std::cout << "size " << closure1.size() << std::endl;
				for (const auto& cube : closure1)
				{
					std::cout << to_string<N>(cube) << std::endl;
				}
			}
			else
			{
				for (int i0 = 0; i0 < s; ++i0)
				{
					std::cout << "i0 = " << i0 << std::endl;

					selection[0] = all_tranformations_vector[i0];
					for (int i1 = 0; i1 < s; ++i1)
					{
						//std::cout << i1;
						selection[1] = all_tranformations_vector[i1];
						for (int i2 = i1 + 1; i2 < s; ++i2)
						{
							selection[2] = all_tranformations_vector[i2];
							for (int i3 = i2 + 1; i3 < s; ++i3)
							{
								selection[3] = all_tranformations_vector[i3];
								for (int i4 = i3 + 1; i4 < s; ++i4)
								{
									selection[4] = all_tranformations_vector[i4];
									for (int i5 = i4 + 1; i5 < s; ++i5)
									{
										selection[5] = all_tranformations_vector[i5];

										const auto closure = details::transitive_closure_x<N>(selection);
										if (closure.size() == s)
										{
											std::cout << "Found a solution!" << std::endl;
											for (const auto& cube : selection)
											{
												std::cout << to_string<N>(cube) << std::endl;
											}

										}
									}
								}
							}
						}
					}
				}
			}
		}
	
		void test()
		{
			constexpr int N = 4;
			constexpr BF bf1 = 0b0000011001110111; // original bf
			constexpr BF bf2 = 0b0000001111010111; // minimum of transitive closure
			constexpr BF bf3 = 0b0000011001110111; // greedy reduced

			const auto descr = create_descriptions<N>();
			{
				std::cout << "bf                  " << to_string_bin<N>(bf1) << std::endl;
			}
			{
				const auto pair = details::search_class_id_method0<N, true>(bf1);
				std::cout << "transitive closure: " << to_string_bin<N>(std::get<0>(pair)) << "; " << std::get<1>(pair) << std::endl;
			}
			{
				const auto pair = details::search_class_id_method1<N, true>(bf1);
				std::cout << "greedy rewrite    : " << to_string_bin<N>(std::get<0>(pair)) << "; " << std::get<1>(pair) << std::endl;
			}
			{
				const auto trans = find_transformation<N>(bf1, bf2, descr);
				std::cout << " bf1=" << to_string_bin<N>(bf1) << "; bf2=" << to_string_bin<N>(bf2);
				for (const auto& pair : trans) std::cout << to_string<N>(pair) << " ";
				std::cout << std::endl;
			}
			{
				const auto trans = find_transformation<N>(bf3, bf2, descr);
				std::cout << " bf3=" << to_string_bin<N>(bf3) << "; bf2=" << to_string_bin<N>(bf2);
				for (const auto& pair : trans) std::cout << to_string<N>(pair) << " ";
				std::cout << std::endl;
			}
		}
	}
}