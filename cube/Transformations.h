#pragma once
#include <array>
#include <string>
#include <vector>

#include "reflect.h"
#include "rotate.h"


namespace cube {

	template<int DIM>
	using Transformations = std::vector<std::pair<CubeI<DIM>, std::string>>;

	template <int N>
	std::string tranformations_to_string(const Transformations<N>& transformations)
	{
		std::string result = "";
		for (const auto& pair : transformations)
		{
			if (result.length() > 0) result += " = ";
			result += std::get<1>(pair);
		}
		return result;
	}

	namespace details {

		template <int N> struct create_transformations_struct
		{
			static Transformations<N> value(const std::array<std::string, 2>& descr)
			{
				std::cout << "ERROR: create_transformations: dim=" << N << " not implemented yet" << std::endl;
				getchar();
				return Transformations<N>();
			}
		};
		template <> struct create_transformations_struct<1>
		{
			static constexpr int N = 1;
			static Transformations<N> value(const std::array<std::string, N>& descr)
			{
				const std::string d0 = descr[0];
				Transformations<N> transformations;
				transformations.push_back(std::make_pair(reflect<N>::value(0), "Ref[" + d0 + "]"));
				return transformations;
			}
			static constexpr std::array<CubeI<N>, 1> value()
			{
				constexpr auto results = std::array<CubeI<N>, 1> {
					reflect<N>::value(0)
				};
				return results;
			}
		};
		template <> struct create_transformations_struct<2>
		{
			static constexpr int N = 2;
			static Transformations<N> value(const std::array<std::string, N>& descr)
			{
				const std::string& d0 = descr[0];
				const std::string& d1 = descr[1];

				Transformations<N> transformations;
				transformations.push_back(std::make_pair(reflect<N>::value(0), "Ref[" + d0 + "]"));
				transformations.push_back(std::make_pair(reflect<N>::value(1), "Ref[" + d1 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(0, 1), "Rot[" + d0 + "," + d1 + "]"));
				return transformations;
			}
			static constexpr std::array<CubeI<N>, 3> value()
			{
				constexpr auto results = std::array<CubeI<N>, 3> {
					reflect<N>::value(0),
						reflect<N>::value(1),
						rotate<N>::value(0, 1)
				};
				return results;
			}
		};
		template <> struct create_transformations_struct<3>
		{
			static constexpr int N = 3;
			static Transformations<N> value(const std::array<std::string, N>& descr)
			{
				const std::string& d0 = descr[0];
				const std::string& d1 = descr[1];
				const std::string& d2 = descr[2];

				Transformations<N> transformations;
				transformations.push_back(std::make_pair(reflect<N>::value(0), "Ref[" + d0 + "]"));
				transformations.push_back(std::make_pair(reflect<N>::value(1), "Ref[" + d1 + "]"));
				transformations.push_back(std::make_pair(reflect<N>::value(2), "Ref[" + d2 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(0, 1), "Rot[" + d0 + "," + d1 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(0, 2), "Rot[" + d0 + "," + d2 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(1, 2), "Rot[" + d1 + "," + d2 + "]"));
				return transformations;
			}
			static constexpr std::array<CubeI<N>, 6> value()
			{
				constexpr auto results = std::array<CubeI<N>, 6> {
					reflect<N>::value(0),
						reflect<N>::value(1),
						reflect<N>::value(2),
						rotate<N>::value(0, 1),
						rotate<N>::value(0, 2),
						rotate<N>::value(1, 2)
				};
				return results;
			}
		};
		template <> struct create_transformations_struct<4>
		{
			static constexpr int N = 4;
			static Transformations<N> value(const std::array<std::string, N>& descr)
			{
				const std::string& d0 = descr[0];
				const std::string& d1 = descr[1];
				const std::string& d2 = descr[2];
				const std::string& d3 = descr[3];

				Transformations<N> transformations;
				transformations.push_back(std::make_pair(reflect<N>::value(0), "Ref[" + d0 + "]"));
				transformations.push_back(std::make_pair(reflect<N>::value(1), "Ref[" + d1 + "]"));
				transformations.push_back(std::make_pair(reflect<N>::value(2), "Ref[" + d2 + "]"));
				transformations.push_back(std::make_pair(reflect<N>::value(3), "Ref[" + d3 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(0, 1), "Rot[" + d0 + "," + d1 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(0, 2), "Rot[" + d0 + "," + d2 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(0, 3), "Rot[" + d0 + "," + d3 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(1, 2), "Rot[" + d1 + "," + d2 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(1, 3), "Rot[" + d1 + "," + d3 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(2, 3), "Rot[" + d2 + "," + d3 + "]"));
				return transformations;
			}
			static constexpr std::array<CubeI<N>, 10> value()
			{
				constexpr auto results = std::array<CubeI<N>, 10> {
					reflect<N>::value(0),
						reflect<N>::value(1),
						reflect<N>::value(2),
						reflect<N>::value(3),
						rotate<N>::value(0, 1),
						rotate<N>::value(0, 2),
						rotate<N>::value(0, 3),
						rotate<N>::value(1, 2),
						rotate<N>::value(1, 3),
						rotate<N>::value(2, 3)
				};
				return results;
			}
		};
		template <> struct create_transformations_struct<5>
		{
			static constexpr int N = 5;
			static Transformations<N> value(const std::array<std::string, N>& descr)
			{
				const std::string& d0 = descr[0];
				const std::string& d1 = descr[1];
				const std::string& d2 = descr[2];
				const std::string& d3 = descr[3];
				const std::string& d4 = descr[4];

				Transformations<N> transformations;
				transformations.push_back(std::make_pair(reflect<N>::value(0), "Ref[" + d0 + "]"));
				transformations.push_back(std::make_pair(reflect<N>::value(1), "Ref[" + d1 + "]"));
				transformations.push_back(std::make_pair(reflect<N>::value(2), "Ref[" + d2 + "]"));
				transformations.push_back(std::make_pair(reflect<N>::value(3), "Ref[" + d3 + "]"));
				transformations.push_back(std::make_pair(reflect<N>::value(4), "Ref[" + d4 + "]"));

				transformations.push_back(std::make_pair(rotate<N>::value(0, 1), "Rot[" + d0 + "," + d1 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(0, 2), "Rot[" + d0 + "," + d2 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(0, 3), "Rot[" + d0 + "," + d3 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(0, 4), "Rot[" + d0 + "," + d4 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(1, 2), "Rot[" + d1 + "," + d2 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(1, 3), "Rot[" + d1 + "," + d3 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(1, 4), "Rot[" + d1 + "," + d4 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(2, 3), "Rot[" + d2 + "," + d3 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(2, 4), "Rot[" + d2 + "," + d4 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(3, 4), "Rot[" + d3 + "," + d4 + "]"));
				return transformations;
			}
			static constexpr std::array<CubeI<N>, 15> value()
			{
				constexpr auto results = std::array<CubeI<N>, 15> {
					reflect<N>::value(0),
						reflect<N>::value(1),
						reflect<N>::value(2),
						reflect<N>::value(3),
						reflect<N>::value(4),
						rotate<N>::value(0, 1),
						rotate<N>::value(0, 2),
						rotate<N>::value(0, 3),
						rotate<N>::value(0, 4),
						rotate<N>::value(1, 2),
						rotate<N>::value(1, 3),
						rotate<N>::value(1, 4),
						rotate<N>::value(2, 3),
						rotate<N>::value(2, 4),
						rotate<N>::value(3, 4)
				};
				return results;
			}
		};
		template <> struct create_transformations_struct<6>
		{
			static constexpr int N = 6;
			static Transformations<N> value(const std::array<std::string, N>& descr)
			{
				const std::string& d0 = descr[0];
				const std::string& d1 = descr[1];
				const std::string& d2 = descr[2];
				const std::string& d3 = descr[3];
				const std::string& d4 = descr[4];
				const std::string& d5 = descr[5];

				Transformations<N> transformations;
				transformations.push_back(std::make_pair(reflect<N>::value(0), "Ref[" + d0 + "]"));
				transformations.push_back(std::make_pair(reflect<N>::value(1), "Ref[" + d1 + "]"));
				transformations.push_back(std::make_pair(reflect<N>::value(2), "Ref[" + d2 + "]"));
				transformations.push_back(std::make_pair(reflect<N>::value(3), "Ref[" + d3 + "]"));
				transformations.push_back(std::make_pair(reflect<N>::value(4), "Ref[" + d4 + "]"));
				transformations.push_back(std::make_pair(reflect<N>::value(5), "Ref[" + d5 + "]"));

				transformations.push_back(std::make_pair(rotate<N>::value(0, 1), "Rot[" + d0 + "," + d1 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(0, 2), "Rot[" + d0 + "," + d2 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(0, 3), "Rot[" + d0 + "," + d3 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(0, 4), "Rot[" + d0 + "," + d4 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(0, 5), "Rot[" + d0 + "," + d5 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(1, 2), "Rot[" + d1 + "," + d2 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(1, 3), "Rot[" + d1 + "," + d3 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(1, 4), "Rot[" + d1 + "," + d4 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(1, 5), "Rot[" + d1 + "," + d5 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(2, 3), "Rot[" + d2 + "," + d3 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(2, 4), "Rot[" + d2 + "," + d4 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(2, 5), "Rot[" + d2 + "," + d5 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(3, 4), "Rot[" + d3 + "," + d4 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(3, 5), "Rot[" + d3 + "," + d5 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(4, 5), "Rot[" + d4 + "," + d5 + "]"));
				return transformations;
			}
			static constexpr std::array<CubeI<N>, 21> value()
			{
				constexpr auto results = std::array<CubeI<N>, 21> {
					reflect<N>::value(0),
						reflect<N>::value(1),
						reflect<N>::value(2),
						reflect<N>::value(3),
						reflect<N>::value(4),
						reflect<N>::value(5),
						rotate<N>::value(0, 1),
						rotate<N>::value(0, 2),
						rotate<N>::value(0, 3),
						rotate<N>::value(0, 4),
						rotate<N>::value(0, 5),
						rotate<N>::value(1, 2),
						rotate<N>::value(1, 3),
						rotate<N>::value(1, 4),
						rotate<N>::value(1, 5),
						rotate<N>::value(2, 3),
						rotate<N>::value(2, 4),
						rotate<N>::value(2, 5),
						rotate<N>::value(3, 4),
						rotate<N>::value(3, 5),
						rotate<N>::value(4, 5)
				};
				return results;
			}
		};
		/*
				template <> struct create_transformations_struct<7>
				{
					static constexpr int N = 7;
					static Transformations<N> value(const std::array<std::string, N>& descr)
					{
						const std::string d0 = descr[0];
						const std::string d1 = descr[1];
						const std::string d2 = descr[2];
						const std::string d3 = descr[3];
						const std::string d4 = descr[4];
						const std::string d5 = descr[5];
						const std::string d6 = descr[6];

						Transformations<N> transformations;
						transformations.push_back(std::make_pair(reflect<N>::value(0), "Ref[" + d0 + "]"));
						transformations.push_back(std::make_pair(reflect<N>::value(1), "Ref[" + d1 + "]"));
						transformations.push_back(std::make_pair(reflect<N>::value(2), "Ref[" + d2 + "]"));
						transformations.push_back(std::make_pair(reflect<N>::value(3), "Ref[" + d3 + "]"));
						transformations.push_back(std::make_pair(reflect<N>::value(4), "Ref[" + d4 + "]"));
						transformations.push_back(std::make_pair(reflect<N>::value(5), "Ref[" + d5 + "]"));
						transformations.push_back(std::make_pair(reflect<N>::value(6), "Ref[" + d6 + "]"));

						transformations.push_back(std::make_pair(rotate<N>::value(0, 1), "Rot[" + d0 + "," + d1 + "]"));
						transformations.push_back(std::make_pair(rotate<N>::value(0, 2), "Rot[" + d0 + "," + d2 + "]"));
						transformations.push_back(std::make_pair(rotate<N>::value(0, 3), "Rot[" + d0 + "," + d3 + "]"));
						transformations.push_back(std::make_pair(rotate<N>::value(0, 4), "Rot[" + d0 + "," + d4 + "]"));
						transformations.push_back(std::make_pair(rotate<N>::value(0, 5), "Rot[" + d0 + "," + d5 + "]"));
						transformations.push_back(std::make_pair(rotate<N>::value(0, 6), "Rot[" + d0 + "," + d6 + "]"));

						transformations.push_back(std::make_pair(rotate<N>::value(1, 2), "Rot[" + d1 + "," + d2 + "]"));
						transformations.push_back(std::make_pair(rotate<N>::value(1, 3), "Rot[" + d1 + "," + d3 + "]"));
						transformations.push_back(std::make_pair(rotate<N>::value(1, 4), "Rot[" + d1 + "," + d4 + "]"));
						transformations.push_back(std::make_pair(rotate<N>::value(1, 5), "Rot[" + d1 + "," + d5 + "]"));
						transformations.push_back(std::make_pair(rotate<N>::value(1, 6), "Rot[" + d1 + "," + d6 + "]"));

						transformations.push_back(std::make_pair(rotate<N>::value(2, 3), "Rot[" + d2 + "," + d3 + "]"));
						transformations.push_back(std::make_pair(rotate<N>::value(2, 4), "Rot[" + d2 + "," + d4 + "]"));
						transformations.push_back(std::make_pair(rotate<N>::value(2, 5), "Rot[" + d2 + "," + d5 + "]"));
						transformations.push_back(std::make_pair(rotate<N>::value(2, 6), "Rot[" + d2 + "," + d6 + "]"));

						transformations.push_back(std::make_pair(rotate<N>::value(3, 4), "Rot[" + d3 + "," + d4 + "]"));
						transformations.push_back(std::make_pair(rotate<N>::value(3, 5), "Rot[" + d3 + "," + d5 + "]"));
						transformations.push_back(std::make_pair(rotate<N>::value(3, 6), "Rot[" + d3 + "," + d6 + "]"));

						transformations.push_back(std::make_pair(rotate<N>::value(4, 5), "Rot[" + d4 + "," + d5 + "]"));
						transformations.push_back(std::make_pair(rotate<N>::value(4, 6), "Rot[" + d4 + "," + d6 + "]"));

						transformations.push_back(std::make_pair(rotate<N>::value(5, 6), "Rot[" + d5 + "," + d6 + "]"));

						return transformations;
					}
					static constexpr std::array<CubeI<N>, 21> value()
					{
						constexpr auto results = std::array<CubeI<N>, 21> {
							reflect<N>::value(0),
								reflect<N>::value(1),
								reflect<N>::value(2),
								reflect<N>::value(3),
								reflect<N>::value(4),
								reflect<N>::value(5),
								rotate<N>::value(0, 1),
								rotate<N>::value(0, 2),
								rotate<N>::value(0, 3),
								rotate<N>::value(0, 4),
								rotate<N>::value(0, 5),
								rotate<N>::value(1, 2),
								rotate<N>::value(1, 3),
								rotate<N>::value(1, 4),
								rotate<N>::value(1, 5),
								rotate<N>::value(2, 3),
								rotate<N>::value(2, 4),
								rotate<N>::value(2, 5),
								rotate<N>::value(3, 4),
								rotate<N>::value(3, 5),
								rotate<N>::value(4, 5)
						};
						return results;
					}
				};
		*/
	}

	template <int N, bool DESCR>
	const Transformations<N> create_transformations()
	{
		if constexpr (DESCR)
		{
			return details::transitive_closure<N>(details::create_transformations_struct<N>::value(create_descriptions<N>()));
		}
		else
		{
			return details::transitive_closure<N>(details::create_transformations_struct<N>::value());
		}
	}

	template <int N>
	Transformations<N> create_transformations(const std::array<std::string, N>& descr)
	{
		return details::create_transformations_struct<N>::value(descr);
	}

	namespace {
		static std::tuple<Transformations<0>, Transformations<1>, Transformations<2>, Transformations<3>, Transformations<4>, Transformations<5>, Transformations<6>> tranformations_cache;
	}
	template <int N, bool DESCR>
	const Transformations<N>& get_transformations_from_cache()
	{
		if (std::get<N>(tranformations_cache).empty())
		{
			std::get<N>(tranformations_cache) = create_transformations<N, DESCR>();
		}
		return std::get<N>(tranformations_cache);
	}
}