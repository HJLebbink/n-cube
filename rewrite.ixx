module;
#include <string>
#include <vector>
#include <tuple>
#include <iostream>		// std::cout



export module rewrite;
import BF;
import cube.reflect;
import cube.rotate;
import Transformations;
import transform;
import n_cube;

namespace cube {

	export template <int N> [[nodiscard]]
	std::array<std::string, N> create_descriptions()
	{
		if constexpr (N == 1) {
			return { "x0" };
		}
		else if constexpr (N == 2) {
			return { "0", "1" };
		}
		else if constexpr (N == 3) {
			return { "0", "1", "2" };
		}
		else if constexpr (N == 4) {
			return { "d", "c", "b", "a" };
		}
		else if constexpr (N == 5) {
			return { "e", "d", "c", "b", "a" };
		}
		else if constexpr (N == 6) {
			return { "f", "e", "d", "c", "b", "a" };
		}
		else if constexpr (N == 7) {
			return { "g", "f", "e", "d", "c", "b", "a" };
		}
		//return std::array<std::string, N>();
	}


	namespace rewrite {

		template <int N> void find_greedy_rewrite_algorithms()
		{
			std::vector<CubeI<N>> all_tranformations_vector;
			int counter = 0;
			for (const auto& t : get_transformations_from_cache<N, false>())
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

		namespace details {
#pragma region greedy rewrite tranformations
			static std::tuple<Transf<0>, Transf<1>, Transf<2>, Transf<3>, Transf<4>, Transf<5>, Transf<6>> transformations_greedy_cache;
			template <int N> struct create_transformations_for_greedy_rewrite_struct
			{
				static Transf<N> value([[maybe_unused]] const std::array<std::string, N>& descr)
				{
					std::cout << "ERROR: create_transformations_for_greedy_rewrite: dim=" << N << " not implemented yet" << std::endl;
					getchar();
					return Transformations<N>();
				}
			};
			template <> struct create_transformations_for_greedy_rewrite_struct<2>
			{
				static constexpr int N = 2;

				static Transf<N> value(const std::array<std::string, N>& descr)
				{
					if (std::get<N>(transformations_greedy_cache).empty())
					{
						const std::string b = descr[0];
						const std::string a = descr[1];

						auto& transformations = std::get<N>(transformations_greedy_cache);
						transformations.push_back(std::make_pair(cube::reflect<N, 0>(), "Ref[" + b + "]"));
						transformations.push_back(std::make_pair(cube::reflect<N, 1>(), "Ref[" + a + "]"));
						transformations.push_back(std::make_pair(cube::rotate<N>(0, 1), "Rot[" + b + "," + a + "]"));
					}
					return std::get<N>(transformations_greedy_cache);
				}
			};
			template <> struct create_transformations_for_greedy_rewrite_struct<3>
			{
				static constexpr int N = 3;
				static Transf<N> value(const std::array<std::string, N>& descr)
				{
					if (std::get<N>(transformations_greedy_cache).empty())
					{
						const bool use_minized_transformations_set = true;
						if (use_minized_transformations_set)
						{
							const std::string a = descr[2];
							const std::string b = descr[1];
							const std::string c = descr[0];

							constexpr auto ref_a = cube::reflect<N, 2>();
							constexpr auto ref_b = cube::reflect<N, 1>();
							constexpr auto ref_c = cube::reflect<N, 0>();

							constexpr auto rot_cb = cube::rotate<N>(0, 1);
							constexpr auto rot_ca = cube::rotate<N>(0, 2);
							constexpr auto rot_ba = cube::rotate<N>(1, 2);

							auto& transformations = std::get<N>(transformations_greedy_cache);
							transformations.push_back(std::make_pair(ref_a, "Ref[" + a + "]"));
							transformations.push_back(std::make_pair(ref_b, "Ref[" + b + "]"));
							transformations.push_back(std::make_pair(ref_c, "Ref[" + c + "]"));

							transformations.push_back(std::make_pair(function_composition<N>(ref_a, ref_b), "Ref[" + a + "].Ref[" + b + "]"));
							//transformations.push_back(std::make_pair(function_composition<N>(ref_b, ref_c), "Ref[" + b + "].Ref[" + c + "]"));
							//transformations.push_back(std::make_pair(function_composition<N>(ref_a, ref_c), "Ref[" + a + "].Ref[" + c + "]"));
							//transformations.push_back(std::make_pair(function_composition<N>(ref_a, ref_b, ref_c), "Ref[" + a + "].Ref[" + b + "].Ref[" + c + "]"));

							//transformations.push_back(std::make_pair(rot_cb, "Rot[" + c + "," + b + "]"));
							//transformations.push_back(std::make_pair(rot_ca, "Rot[" + c + "," + a + "]"));
							//transformations.push_back(std::make_pair(rot_ba, "Rot[" + b + "," + a + "]"));

							//transformations.push_back(std::make_pair(function_composition<N>(rot_cb, ref_a), "Rot[" + c + "," + b + "].Ref[" + a + "]"));
							transformations.push_back(std::make_pair(function_composition<N>(rot_cb, ref_b), "Rot[" + c + "," + b + "].Ref[" + b + "]"));
							//transformations.push_back(std::make_pair(function_composition<N>(rot_cb, ref_c), "Rot[" + c + "," + b + "].Ref[" + c + "]"));
							//transformations.push_back(std::make_pair(function_composition<N>(rot_cb, ref_a, ref_b), "Rot[" + c + "," + b + "].Ref[" + a + "].Ref[" + b + "]"));
							//transformations.push_back(std::make_pair(function_composition<N>(rot_cb, ref_a, ref_c), "Rot[" + c + "," + b + "].Ref[" + a + "].Ref[" + c + "]"));
							//transformations.push_back(std::make_pair(function_composition<N>(rot_cb, ref_b, ref_c), "Rot[" + c + "," + b + "].Ref[" + b + "].Ref[" + c + "]"));

							//transformations.push_back(std::make_pair(function_composition<N>(rot_ca, ref_a), "Rot[" + c + "," + a + "].Ref[" + a + "]"));
							//transformations.push_back(std::make_pair(function_composition<N>(rot_ca, ref_b), "Rot[" + c + "," + a + "].Ref[" + b + "]"));
							//transformations.push_back(std::make_pair(function_composition<N>(rot_ca, ref_c), "Rot[" + c + "," + a + "].Ref[" + c + "]"));
							//transformations.push_back(std::make_pair(function_composition<N>(rot_ca, ref_a, ref_b), "Rot[" + c + "," + a + "].Ref[" + a + "].Ref[" + b + "]"));
							//transformations.push_back(std::make_pair(function_composition<N>(rot_ca, ref_a, ref_c), "Rot[" + c + "," + a + "].Ref[" + a + "].Ref[" + c + "]"));
							//transformations.push_back(std::make_pair(function_composition<N>(rot_ca, ref_b, ref_c), "Rot[" + c + "," + a + "].Ref[" + b + "].Ref[" + c + "]"));

							//transformations.push_back(std::make_pair(function_composition<N>(rot_ba, ref_a), "Rot[" + b + "," + a + "].Ref[" + a + "]"));
							//transformations.push_back(std::make_pair(function_composition<N>(rot_ba, ref_b), "Rot[" + b + "," + a + "].Ref[" + b + "]"));
							//transformations.push_back(std::make_pair(function_composition<N>(rot_ba, ref_a), "Rot[" + b + "," + a + "].Ref[" + c + "]"));
							//transformations.push_back(std::make_pair(function_composition<N>(rot_ba, ref_a, ref_b), "Rot[" + b + "," + a + "].Ref[" + a + "].Ref[" + b + "]"));
							transformations.push_back(std::make_pair(function_composition<N>(rot_ba, ref_a, ref_c), "Rot[" + b + "," + a + "].Ref[" + a + "].Ref[" + c + "]"));
							//transformations.push_back(std::make_pair(function_composition<N>(rot_ba, ref_b, ref_c), "Rot[" + b + "," + a + "].Ref[" + b + "].Ref[" + c + "]"));

							for (const auto& pair : transformations)
							{
								std::cout << to_string<N>(pair) << std::endl;
							}

						}
						else
						{
							std::get<N>(transformations_greedy_cache) = cube::get_transformations_from_cache<N, false>();
						}
					}
					return std::get<N>(transformations_greedy_cache);
				}
			};
			template <> struct create_transformations_for_greedy_rewrite_struct<4>
			{
				static constexpr int N = 4;
				static Transf<N> value(const std::array<std::string, N>& /*descr*/)
				{
					if (std::get<N>(transformations_greedy_cache).empty())
					{
						const bool use_minized_transformations_set = false;
						if (use_minized_transformations_set)
						{
							// TODO: find a reduced set of transformations that is correct, for the time being take the full closure:
						}
						else
						{
							std::get<N>(transformations_greedy_cache) = cube::get_transformations_from_cache<N, false>();
						}
					}
					return std::get<N>(transformations_greedy_cache);
				}
			};
			template <> struct create_transformations_for_greedy_rewrite_struct<5>
			{
				static constexpr int N = 5;
				static Transf<N> value(const std::array<std::string, N>& /*descr*/)
				{
					if (std::get<N>(transformations_greedy_cache).empty())
					{
						const bool use_minized_transformations_set = false;
						if (use_minized_transformations_set)
						{
							// TODO: find a reduced set of transformations that is correct, for the time being take the full closure:
						}
						else
						{
							std::get<N>(transformations_greedy_cache) = cube::get_transformations_from_cache<N, false>();
						}
					}
					return std::get<N>(transformations_greedy_cache);
				}
			};
#pragma endregion
		}

		template <int N, bool DESC = false> Transf<N> create_transformations_for_greedy_rewrite()
		{
			if constexpr (DESC)
			{
				return details::create_transformations_for_greedy_rewrite_struct<N>::value(create_descriptions<N>());
			}
			else
			{
				//TODO make a greedy rewrite with no descriptions
				return details::create_transformations_for_greedy_rewrite_struct<N>::value(create_descriptions<N>());
			}
		}

		// find the transformation that when applied to bf1 yields bf2.
		template <int N> std::vector<std::pair<CubeI<N>, std::string>> find_transformation(const BF bf1, const BF bf2, [[maybe_unused]] const std::array<std::string, N>& descr)
		{
			std::vector<std::pair<CubeI<N>, std::string>> results;

			const BF bf1b = complement_if_needed<N>(bf1);
			const BF bf2b = complement_if_needed<N>(bf2);

			const auto transformations = get_transformations_from_cache<N, true>();
			{
				if (bf1b == bf2b) results.push_back(std::make_pair(init_cubeI<N>(), "Identity"));

				for (const auto& pair : transformations)
				{
					if (transform<N>(bf1b, std::get<0>(pair)) == bf2b)
					{
						results.push_back(pair);
					}
				}
				if (results.empty())
				{
					std::cout << "WARNING: could not find transformation" << std::endl;
					static_cast<void>(getchar());
				}
			}
			return results;
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
				//const auto pair = cube::details::search_class_id_method0<N, true>(bf1);
				//std::cout << "transitive closure: " << to_string_bin<N>(std::get<0>(pair)) << "; " << std::get<1>(pair) << std::endl;
			}
			{
				//const auto pair = cube::details::search_class_id_method1<N, true>(bf1);
				//std::cout << "greedy rewrite    : " << to_string_bin<N>(std::get<0>(pair)) << "; " << std::get<1>(pair) << std::endl;
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