#pragma once
#include <array>
#include <string>
#include <vector>
#include <bitset>

#include "reflect.h"
#include "rotate.h"
#include "transform.h"

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
				transformations.push_back(std::make_pair(reflect<N>::value(0), d0));
				transformations.push_back(std::make_pair(reflect<N>::value(1), d1));
					transformations.push_back(std::make_pair(rotate<N>::value(0, 1), d0 + "" + d1));
				return transformations;
			}
			static constexpr std::array<CubeI<N>, 3> value()
			{
				// 3 basic transformatons is Sterling Number of second kind S(3,2) = 3

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
				transformations.push_back(std::make_pair(reflect<N>::value(0), d0));
				transformations.push_back(std::make_pair(reflect<N>::value(1), d1 ));
				transformations.push_back(std::make_pair(reflect<N>::value(2), d2 ));
				transformations.push_back(std::make_pair(rotate<N>::value(0, 1), d0 + "" + d1));
				transformations.push_back(std::make_pair(rotate<N>::value(0, 2), d0 + "" + d2));
				transformations.push_back(std::make_pair(rotate<N>::value(1, 2), d1 + "" + d2));
				return transformations;
			}
			static constexpr std::array<CubeI<N>, 6> value()
			{
				// 6 basic transformatons is Sterling Number of second kind S(4,3) = 6

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
				// 10 basic transformatons is Sterling Number of second kind S(5,4) = 10

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
				// 15 basic transformatons is Sterling Number of second kind S(6,5) = 15

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
				// 21 basic transformatons is Sterling Number of second kind S(7,6) = 21

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


	constexpr CubeI<2> create_transformations_N2(int trans_dim) noexcept
	{
		constexpr int N = 2;
		constexpr CubeI<N> t1 = details::reflect<N>::value(0);
		constexpr CubeI<N> t2 = details::reflect<N>::value(1);
		constexpr CubeI<N> t3 = details::rotate<N>::value(0, 1);

		// rotation r01 is the dual of rotation sr10
		static_assert(array_tools::equal(transform<N>(t3, details::rotate<N>::value(1, 0)), init_cubeI<N>()));

		// reflections are commutative
		static_assert(array_tools::equal(transform<N>(t1, t2), transform<N>(t2, t1)));

		// rotations are not commutative!
		static_assert(!array_tools::equal(transform<N>(t1, t3), transform<N>(t3, t1)));
		static_assert(!array_tools::equal(transform<N>(t2, t3), transform<N>(t3, t2)));

		//constexpr CubeI<N> x1 = transform<N>(t1, t3);
		//constexpr CubeI<N> x2 = transform<N>(t3, t1);
		
		static_assert(!array_tools::equal(transform<N>(t3, t3), t3));
		static_assert(array_tools::equal(transform<N>(transform<N>(t3, t3), t3), transform<N>(t3, transform<N>(t3, t3))));

		//constexpr CubeI<N> x3 = transform<N>(t3, t3);
		//constexpr CubeI<N> x4 = transform<N>(t3, x3);
		//constexpr CubeI<N> x5 = transform<N>(x3, t3);

		constexpr CubeI<N> r0 = init_cubeI<N>();
		const CubeI<N> r1 = ((trans_dim >> 0) & 1) ? transform<N>(r0, t1) : r0;
		const CubeI<N> r2 = ((trans_dim >> 1) & 1) ? transform<N>(r1, t2) : r1;
		const CubeI<N> r3 = ((trans_dim >> 2) & 1) ? transform<N>(r2, t3) : r2;
		return r3;
	}
	constexpr CubeI<3> create_transformations_N3(int trans_dim) noexcept
	{
		constexpr int N = 3;
		constexpr CubeI<N> t1 = details::reflect<N>::value(0);
		constexpr CubeI<N> t2 = details::reflect<N>::value(1);
		constexpr CubeI<N> t3 = details::reflect<N>::value(2);
		constexpr CubeI<N> t4 = details::rotate<N>::value(0, 1);
		constexpr CubeI<N> t5 = details::rotate<N>::value(0, 2);
		constexpr CubeI<N> t6 = details::rotate<N>::value(1, 2);

		// reflections are commutative
		static_assert(array_tools::equal(transform<N>(t1, t2), transform<N>(t2, t1)));
		static_assert(array_tools::equal(transform<N>(t2, t3), transform<N>(t3, t2)));
		static_assert(array_tools::equal(transform<N>(t1, t3), transform<N>(t3, t1)));

		// rotations are not commutative!
		static_assert(!array_tools::equal(transform<N>(t1, t4), transform<N>(t4, t1)));
		static_assert(!array_tools::equal(transform<N>(t2, t4), transform<N>(t4, t2)));
		static_assert(!array_tools::equal(transform<N>(t3, t4), transform<N>(t3, t2)));


		// some rotations conflate!
		static_assert(array_tools::equal(transform<N>(t4, t5), transform<N>(t5, t6)));
		static_assert(array_tools::equal(t5, transform<N>(t1, transform<N>(t2, transform<N>(t4, transform<N>(t5, t6))))));




		constexpr CubeI<N> r0 = init_cubeI<N>();
		const CubeI<N> r1 = ((trans_dim >> 0) & 1) ? transform<N>(r0, t1) : r0;
		const CubeI<N> r2 = ((trans_dim >> 1) & 1) ? transform<N>(r1, t2) : r1;
		const CubeI<N> r3 = ((trans_dim >> 2) & 1) ? transform<N>(r2, t3) : r2;
		const CubeI<N> r4 = ((trans_dim >> 3) & 1) ? transform<N>(r3, t4) : r3;
		const CubeI<N> r5 = ((trans_dim >> 4) & 1) ? transform<N>(r4, t5) : r4;
		const CubeI<N> r6 = ((trans_dim >> 5) & 1) ? transform<N>(r5, t6) : r5;
		return r6;
	}
	
	constexpr CubeI<4> create_transformations_N4(int trans_dim) noexcept
	{
		constexpr int N = 4;
		constexpr CubeI<N> t1 = details::reflect<N>::value(0);
		constexpr CubeI<N> t2 = details::reflect<N>::value(1);
		constexpr CubeI<N> t3 = details::reflect<N>::value(2);
		constexpr CubeI<N> t4 = details::reflect<N>::value(3);
		constexpr CubeI<N> t5 = details::rotate<N>::value(0, 1);
		constexpr CubeI<N> t6 = details::rotate<N>::value(0, 2);
		constexpr CubeI<N> t7 = details::rotate<N>::value(0, 3);
		constexpr CubeI<N> t8 = details::rotate<N>::value(1, 2);
		constexpr CubeI<N> t9 = details::rotate<N>::value(1, 3);
		constexpr CubeI<N> t10 = details::rotate<N>::value(2, 3);

		constexpr CubeI<N> r0 = init_cubeI<N>();
		const CubeI<N> r1 = ((trans_dim >> 0) & 1) ? transform<N>(r0, t1) : r0;
		const CubeI<N> r2 = ((trans_dim >> 1) & 1) ? transform<N>(r1, t2) : r1;
		const CubeI<N> r3 = ((trans_dim >> 2) & 1) ? transform<N>(r2, t3) : r2;
		const CubeI<N> r4 = ((trans_dim >> 3) & 1) ? transform<N>(r3, t4) : r3;
		const CubeI<N> r5 = ((trans_dim >> 4) & 1) ? transform<N>(r4, t5) : r4;
		const CubeI<N> r6 = ((trans_dim >> 5) & 1) ? transform<N>(r5, t6) : r5;
		const CubeI<N> r7 = ((trans_dim >> 6) & 1) ? transform<N>(r6, t7) : r6;
		const CubeI<N> r8 = ((trans_dim >> 7) & 1) ? transform<N>(r7, t8) : r7;
		const CubeI<N> r9 = ((trans_dim >> 8) & 1) ? transform<N>(r8, t9) : r8;
		const CubeI<N> r10 = ((trans_dim >> 9) & 1) ? transform<N>(r9, t10) : r9;
		return r10;
	}


	constexpr std::array<CubeI<2>, 8> create_all_transformations_N2() noexcept
	{
		constexpr auto result = std::array<CubeI<2>, 8> {
			create_transformations_N2(0b000),
				create_transformations_N2(0b001),
				create_transformations_N2(0b010),
				create_transformations_N2(0b011),
				create_transformations_N2(0b100),
				create_transformations_N2(0b101),
				create_transformations_N2(0b110),
				create_transformations_N2(0b111)
		};
		return result;
	}
	void test_create_transformations()
	{
		if (true) {
			constexpr auto method2 = create_all_transformations_N2();
		}

		if (true)
		{
			constexpr int N = 3;
			std::vector<std::pair<CubeI<N>, std::vector<int>>> transformations_vector;

			std::cout << "000-001 = ref 0  = " << to_string<N>(details::reflect<N>::value(0)) << std::endl;
			std::cout << "000-010 = ref 1  = " << to_string<N>(details::reflect<N>::value(1)) << std::endl;
			std::cout << "000-100 = ref 2  = " << to_string<N>(details::reflect<N>::value(2)) << std::endl;
			std::cout << "001-000 = rot 01 = " << to_string<N>(details::rotate<N>::value(0, 1)) << std::endl;
			std::cout << "010-000 = rot 02 = " << to_string<N>(details::rotate<N>::value(0, 2)) << std::endl;
			std::cout << "100-000 = rot 12 = " << to_string<N>(details::rotate<N>::value(1, 2)) << std::endl;

			for (int i = 0; i < (1 << 6); ++i) {
				const CubeI<N> t = create_transformations_N3(i);

				bool contains = false;
				for (auto& e : transformations_vector) if (e.first == t) {
					contains = true;
					e.second.push_back(i);
					break;
				}
				if (!contains) {
					transformations_vector.push_back(std::make_pair(t, std::vector<int>{ i }));
				}
			}

			int counter = 0;
			for (const auto& e : transformations_vector) {
				std::cout << counter << ": ";
				std::cout << to_string<N>(e.first) << ": ";
				for (const int i : e.second) {
					std::cout << std::bitset<6>(i).to_string() << " ";
				}
				std::cout << std::endl;
				counter++;
			}
		}
	
		if (false) 
		{
			constexpr int N = 4;
			std::vector<std::pair<CubeI<N>, std::vector<int>>> transformations_vector;

			std::cout << "000000-0001 = ref 0  = " << to_string<N>(details::reflect<N>::value(0)) << std::endl;
			std::cout << "000000-0010 = ref 1  = " << to_string<N>(details::reflect<N>::value(1)) << std::endl;
			std::cout << "000000-0100 = ref 2  = " << to_string<N>(details::reflect<N>::value(2)) << std::endl;
			std::cout << "000000-1000 = ref 3  = " << to_string<N>(details::reflect<N>::value(3)) << std::endl;
			std::cout << "000001-0000 = rot 01 = " << to_string<N>(details::rotate<N>::value(0, 1)) << std::endl;
			std::cout << "000010-0000 = rot 02 = " << to_string<N>(details::rotate<N>::value(0, 2)) << std::endl;
			std::cout << "000100-0000 = rot 03 = " << to_string<N>(details::rotate<N>::value(0, 3)) << std::endl;
			std::cout << "001000-0000 = rot 12 = " << to_string<N>(details::rotate<N>::value(1, 2)) << std::endl;
			std::cout << "010000-0000 = rot 13 = " << to_string<N>(details::rotate<N>::value(1, 3)) << std::endl;
			std::cout << "100000-0000 = rot 23 = " << to_string<N>(details::rotate<N>::value(2, 3)) << std::endl;

			constexpr CubeI<N> t5 = details::rotate<N>::value(0, 1);
			constexpr CubeI<N> t6 = details::rotate<N>::value(0, 2);
			constexpr CubeI<N> t7 = details::rotate<N>::value(0, 3);
			constexpr CubeI<N> t8 = details::rotate<N>::value(1, 2);
			constexpr CubeI<N> t9 = details::rotate<N>::value(1, 3);
			constexpr CubeI<N> t10 = details::rotate<N>::value(2, 3);

			for (int i = 0; i < (1 << 10); ++i) {
				const CubeI<N> t = create_transformations_N4(i);

				bool contains = false;
				for (auto& e : transformations_vector) if (e.first == t) {
					contains = true;
					e.second.push_back(i);
					break;
				}
				if (!contains) {
					transformations_vector.push_back(std::make_pair(t, std::vector<int>{ i }));
				}
			}

			int counter = 0;
			for (const auto& e : transformations_vector) {
				std::cout << counter << ": ";
				std::cout << to_string<N>(e.first) << ": ";
				for (const int i : e.second) {
					std::cout << std::bitset<10>(i).to_string() << " ";
				}
				std::cout << std::endl;
				counter++;
			}
		}
	}
}