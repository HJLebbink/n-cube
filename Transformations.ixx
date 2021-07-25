module;
#include <array>
#include <string>
#include <vector>
#include <set>
#include <compare>
#include <functional> //less


#include <map>
#include <bitset>
#include <iostream>		// std::cout
#include <utility> //pair
#include "CubeDef.h"

export module Transformations;
import reflect;
import rotate;
import transform;
import CubeIndex;
import array_tools;


namespace cube {

#ifndef INOUT
#define INOUT
#endif

	export template<int DIM>
	using Transf = std::vector<std::pair<CubeI<DIM>, std::string>>;

	template <int N>
	std::string tranformations_to_string(const Transf<N>& transformations)
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

		consteval int sterling_number_of_second_kind(int N) noexcept {
			switch (N) {
			case 1: return 1;
			case 2: return 3;
			case 3: return 6;
			case 4: return 10;
			case 5: return 15;
			case 6: return 21;
			case 7: return 28;
			case 8: return 36;
			case 9: return 45;
			case 10: return 55;
			default: return 0;
			}
		}


		export template <int N, int P = sterling_number_of_second_kind(N)>
		consteval std::array<CubeI<N>, P> create_transformation_Y() 
		{
			if constexpr (N == 1) {
				constexpr auto x1 = reflect<N, 0>();
				return std::array<CubeI<N>, P> {x1};
			}
			else if constexpr (N == 2) {
				// 3 basic transformatons is Sterling Number of second kind S(3,2) = 3
				constexpr auto x1 = reflect<N, 0>();
				constexpr auto x2 = reflect<N, 1>();
				constexpr auto x3 = rotate<N>(0, 1);
				return std::array<CubeI<N>, P> {x1, x2, x3};
			}
			else if constexpr (N == 3) {
				constexpr auto x1 = reflect<N, 0>();
				constexpr auto x2 = reflect<N, 1>();
				constexpr auto x3 = reflect<N, 2>();
				constexpr auto x4 = rotate<N>(0, 1);
				constexpr auto x5 = rotate<N>(0, 2);
				constexpr auto x6 = rotate<N>(1, 2);
				return std::array<CubeI<N>, P> {x1, x2, x3, x4, x5, x6};
			}
			else if constexpr (N == 4) {
				constexpr auto x1 = reflect<N, 0>();
				constexpr auto x2 = reflect<N, 1>();
				constexpr auto x3 = reflect<N, 2>();
				constexpr auto x4 = reflect<N, 3>();
				constexpr auto x5 = rotate<N>(0, 1);
				constexpr auto x6 = rotate<N>(0, 2);
				constexpr auto x7 = rotate<N>(0, 3);
				constexpr auto x8 = rotate<N>(1, 2);
				constexpr auto x9 = rotate<N>(1, 3);
				constexpr auto x10 = rotate<N>(2, 3);
				return std::array<CubeI<N>, P> {x1, x2, x3, x4, x5, x6, x7, x8, x9, x10};
			}
			else if constexpr (N == 5) {
				constexpr auto x1 = reflect<N, 0>();
				constexpr auto x2 = reflect<N, 1>();
				constexpr auto x3 = reflect<N, 2>();
				constexpr auto x4 = reflect<N, 3>();
				constexpr auto x5 = reflect<N, 4>();
				constexpr auto x6 = rotate<N>(0, 1);
				constexpr auto x7 = rotate<N>(0, 2);
				constexpr auto x8 = rotate<N>(0, 3);
				constexpr auto x9 = rotate<N>(0, 4);
				constexpr auto x10 = rotate<N>(1, 2);
				constexpr auto x11 = rotate<N>(1, 3);
				constexpr auto x12 = rotate<N>(1, 4);
				constexpr auto x13 = rotate<N>(2, 3);
				constexpr auto x14 = rotate<N>(2, 4);
				constexpr auto x15 = rotate<N>(3, 4);
				return std::array<CubeI<N>, P> {x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15};
			}
			else if constexpr (N == 6) {
				constexpr auto results = std::array<CubeI<N>, P> {
					reflect<N, 0>(),
						reflect<N, 1>(),
						reflect<N, 2>(),
						reflect<N, 3>(),
						reflect<N, 4>(),
						reflect<N, 5>(),
						rotate<N>(0, 1),
						rotate<N>(0, 2),
						rotate<N>(0, 3),
						rotate<N>(0, 4),
						rotate<N>(0, 5),
						rotate<N>(1, 2),
						rotate<N>(1, 3),
						rotate<N>(1, 4),
						rotate<N>(1, 5),
						rotate<N>(2, 3),
						rotate<N>(2, 4),
						rotate<N>(2, 5),
						rotate<N>(3, 4),
						rotate<N>(3, 5),
						rotate<N>(4, 5)
				};
				return results;
			}
			else if constexpr (N == 7) {
				constexpr auto results = std::array<CubeI<N>, P> {
					reflect<N, 0>(),
						reflect<N, 1>(),
						reflect<N, 2>(),
						reflect<N, 3>(),
						reflect<N, 4>(),
						reflect<N, 5>(),
						reflect<N, 6>(),
						rotate<N>(0, 1),
						rotate<N>(0, 2),
						rotate<N>(0, 3),
						rotate<N>(0, 4),
						rotate<N>(0, 5),
						rotate<N>(0, 6),
						rotate<N>(1, 2),
						rotate<N>(1, 3),
						rotate<N>(1, 4),
						rotate<N>(1, 5),
						rotate<N>(1, 6),
						rotate<N>(2, 3),
						rotate<N>(2, 4),
						rotate<N>(2, 5),
						rotate<N>(2, 6),
						rotate<N>(3, 4),
						rotate<N>(3, 5),
						rotate<N>(3, 6),
						rotate<N>(4, 5),
						rotate<N>(4, 6),
						rotate<N>(5, 6)
				};
				return results;
			}
		}


		export template <int N>
			Transf<N> create_transformations_X(const std::array<std::string, N>& descr)
		{
			constexpr auto x = create_transformation_Y<N>();

			if constexpr (N == 1) {
				const std::string& d0 = descr[0];
				return Transf<N>{
					{ x[0], d0 }
				};
			}
			else if constexpr (N == 2) {
				const std::string& d0 = descr[0];
				const std::string& d1 = descr[1];
				return Transf<N>{
					{ x[0], d0 },
					{ x[1], d1 },
					{ x[2], d0 + "" + d1 },
				};
			}
			else if constexpr (N == 3) {
				const std::string& d0 = descr[0];
				const std::string& d1 = descr[1];
				const std::string& d2 = descr[2];
				return Transf<N>{
					{ x[0], d0 },
					{ x[1], d1 },
					{ x[2], d2 },
					{ x[3], d0 + "" + d1 },
					{ x[4], d0 + "" + d2 },
					{ x[5], d1 + "" + d2 },
				};
			}
			else if constexpr (N == 4) {
				const std::string& d0 = descr[0];
				const std::string& d1 = descr[1];
				const std::string& d2 = descr[2];
				const std::string& d3 = descr[3];
				return Transf<N>{
					{ x[0], d0 },
					{ x[1], d1 },
					{ x[2], d2 },
					{ x[3], d3 },
					{ x[4], d0 + "" + d1 },
					{ x[5], d0 + "" + d2 },
					{ x[6], d0 + "" + d3 },
					{ x[7], d1 + "" + d2 },
					{ x[8], d1 + "" + d3 },
					{ x[9], d2 + "" + d3 },
				};
			}
			else if constexpr (N == 5) {
				const std::string& d0 = descr[0];
				const std::string& d1 = descr[1];
				const std::string& d2 = descr[2];
				const std::string& d3 = descr[3];
				const std::string& d4 = descr[4];
				return Transf<N>{
					{ x[0], d0 },
					{ x[1], d1 },
					{ x[2], d2 },
					{ x[3], d3 },
					{ x[4], d4 },
					{ x[5], d0 + "" + d1 },
					{ x[6], d0 + "" + d2 },
					{ x[7], d0 + "" + d3 },
					{ x[8], d0 + "" + d4 },
					{ x[9], d1 + "" + d2 },
					{ x[10], d1 + "" + d3 },
					{ x[11], d1 + "" + d4 },
					{ x[12], d2 + "" + d3 },
					{ x[13], d2 + "" + d4 },
					{ x[14], d3 + "" + d4 },
				};
			}
			else if constexpr (N == 6) {
				const std::string& d0 = descr[0];
				const std::string& d1 = descr[1];
				const std::string& d2 = descr[2];
				const std::string& d3 = descr[3];
				const std::string& d4 = descr[4];
				const std::string& d5 = descr[5];
				return Transf<N>{
					{ x[0], d0 },
					{ x[1], d1 },
					{ x[2], d2 },
					{ x[3], d3 },
					{ x[4], d4 },
					{ x[5], d5 },
					{ x[6], d0 + "" + d1 },
					{ x[7], d0 + "" + d2 },
					{ x[8], d0 + "" + d3 },
					{ x[9], d0 + "" + d4 },
					{ x[10], d0 + "" + d5 },
					{ x[11], d1 + "" + d2 },
					{ x[12], d1 + "" + d3 },
					{ x[13], d1 + "" + d4 },
					{ x[14], d1 + "" + d5 },
					{ x[15], d2 + "" + d3 },
					{ x[16], d2 + "" + d4 },
					{ x[17], d2 + "" + d5 },
					{ x[18], d3 + "" + d4 },
					{ x[19], d3 + "" + d5 },
					{ x[20], d4 + "" + d5 },
				};
			}
		}

		/*
				template <> struct create_transformations_struct<7>
				{
					static constexpr int N = 7;
					static Transf<N> value(const std::array<std::string, N>& descr)
					{
						const std::string d0 = descr[0];
						const std::string d1 = descr[1];
						const std::string d2 = descr[2];
						const std::string d3 = descr[3];
						const std::string d4 = descr[4];
						const std::string d5 = descr[5];
						const std::string d6 = descr[6];

						Transf<N> transformations;
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
					static consteval std::array<CubeI<N>, 21> value()
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
	namespace details
	{
		template <int N>
		struct Cmp {
			[[nodiscard]] bool operator()(const CubeI<N>& left, const CubeI<N>& right) const {
				return array_tools::lesseq<1<<N>(left, right);
			}
		};

		template <int N>
		using CubeSet = std::set<CubeI<N>, Cmp<N>>;

		template <int N>
		using CubeMap = std::map<CubeI<N>, std::string, Cmp<N>>;


#pragma region transitive closure
		template <int N, int M> void transitive_closure_recursive_no_descriptions(
			const int start_index,
			const std::array<CubeI<N>, M>& transformations,
			const CubeI<N>& cube,
			INOUT CubeSet<N>& results)
		{
			for (int i = start_index; i < M; ++i)
			{
				const CubeI<N> cube_new = function_composition<N>(cube, transformations[i]);
				results.insert(cube_new);
				transitive_closure_recursive_no_descriptions<N>(i + 1, transformations, cube_new, INOUT results);
			}
		}

		template <int N> void transitive_closure_recursive_with_descriptions(
			const int start_index,
			const Transf<N>& transformations,
			const CubeI<N>& cube,
			const std::string& descr,
			INOUT CubeMap<N>& results)
		{
			const int s = static_cast<int>(transformations.size());
			for (int i = start_index; i < s; ++i)
			{
				const CubeI<N> cube_new = function_composition<N>(cube, std::get<0>(transformations[i]));
				const std::string descr_new = (descr.empty()) ? std::get<1>(transformations[i]) : (descr + "." + std::get<1>(transformations[i]));

				if (results.contains(cube_new))
				{
					if (descr_new.length() < results.at(cube_new).length()) {
						//std::cout << "descr_new=" << descr_new << "; old=" << results.at(cube_new) << std::endl;
						results[cube_new] = descr_new;
					}
				}
				else {
					results.insert(std::make_pair(cube_new, descr_new));
				}
				transitive_closure_recursive_with_descriptions<N>(i + 1, transformations, cube_new, descr_new, results);
			}
		}

		template <int N> static Transf<N> transitive_closure(
			const Transf<N>& transformations_in)
		{
			CubeMap<N> results;
			results.insert({ init_cubeI<N>(), "id" });
			transitive_closure_recursive_with_descriptions<N>(0, transformations_in, init_cubeI<N>(), "", INOUT results);
			return Transf<N>(results.begin(), results.end());
		}
		template <int N, int M> static Transf<N> transitive_closure(
			const std::array<CubeI<N>, M>& transformations_in)
		{
			CubeSet<N> results;
			results.insert(init_cubeI<N>());
			transitive_closure_recursive_no_descriptions<N>(0, transformations_in, init_cubeI<N>(), INOUT results);
			Transf<N> transformations_out;
			for (const CubeI<N>& e : results) transformations_out.push_back({ e, "?" });
			return transformations_out;
		}

		export template <int N, int M> static CubeSet<N> transitive_closure_x(
			const std::array<CubeI<N>, M>& transformations_in)
		{
			CubeSet<N> results{ init_cubeI<N>() };
			transitive_closure_recursive_no_descriptions<N>(0, transformations_in, init_cubeI<N>(), INOUT results);
			return results;
		}
		// slow method
		template <int N, int M> static CubeSet<N> transitive_closure_y(
			const std::array<CubeI<N>, M>& transformations_in)
		{
			CubeSet<N> results;

			//for (const auto& cube_in : transformations_in) results.insert(cube_in);

			//bool changed = true;
			//while (changed)
			//{
			//	changed = false;
			//	for (const auto& cube_in : transformations_in)
			//	{
			//		const CubeI<N> cube = std::get<0>(transformations[i]);

			//		for (const auto& e2 : results)
			//		{
			//			const CubeI<N> cube_new = details::function_composition<N>(cube_in, cube);
			//			results.insert(cube_new);
			//			changed = true;
			//		}
			//	}
			//}
			return results;
		}
#pragma endregion

	} // end namespace details


	export template <int N, bool DESCR>
	const Transf<N> create_transformations()
	{
		if constexpr (DESCR)
		{
			const auto x = details::create_transformations_X<N>(create_descriptions<N>());
			return cube::details::transitive_closure<N>(x);
		}
		else
		{
			constexpr auto x = details::create_transformation_Y<N>();
			return cube::details::transitive_closure<N>(x);
		}
	}

	namespace {
		static std::tuple<Transf<0>, Transf<1>, Transf<2>, Transf<3>, Transf<4>, Transf<5>, Transf<6>> tranformations_cache;
	}

	export template <int N, bool DESCR>
	const Transf<N>& get_transformations_from_cache()
	{
		if (std::get<N>(tranformations_cache).empty())
		{
			std::get<N>(tranformations_cache) = create_transformations<N, DESCR>();
		}
		return std::get<N>(tranformations_cache);
	}


	consteval CubeI<2> create_transformations_N2(int trans_dim) noexcept
	{
		constexpr int N = 2;
		constexpr CubeI<N> t1 = reflect<N, 0>();
		constexpr CubeI<N> t2 = reflect<N, 1>();
		constexpr CubeI<N> t3 = rotate<N>(0, 1);

		// rotation r01 is the dual of rotation sr10
		static_assert(array_tools::equal(transform<N>(t3, rotate<N>(1, 0)), init_cubeI<N>()));

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
		constexpr CubeI<N> t1 = reflect<N, 0>();
		constexpr CubeI<N> t2 = reflect<N, 1>();
		constexpr CubeI<N> t3 = reflect<N, 2>();
		constexpr CubeI<N> t4 = rotate<N>(0, 1);
		constexpr CubeI<N> t5 = rotate<N>(0, 2);
		constexpr CubeI<N> t6 = rotate<N>(1, 2);

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
		constexpr CubeI<N> t1 = reflect<N, 0>();
		constexpr CubeI<N> t2 = reflect<N, 1>();
		constexpr CubeI<N> t3 = reflect<N, 2>();
		constexpr CubeI<N> t4 = reflect<N, 3>();
		constexpr CubeI<N> t5 = rotate<N>(0, 1);
		constexpr CubeI<N> t6 = rotate<N>(0, 2);
		constexpr CubeI<N> t7 = rotate<N>(0, 3);
		constexpr CubeI<N> t8 = rotate<N>(1, 2);
		constexpr CubeI<N> t9 = rotate<N>(1, 3);
		constexpr CubeI<N> t10 = rotate<N>(2, 3);

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
	export void test_create_transformations()
	{
		if (true) {
			constexpr auto method2 = create_all_transformations_N2();
		}

		if (true)
		{
			constexpr int N = 3;
			std::vector<std::pair<CubeI<N>, std::vector<int>>> transformations_vector;

			std::cout << "000-001 = ref 0  = " << to_string<N>(reflect<N, 0>()) << std::endl;
			std::cout << "000-010 = ref 1  = " << to_string<N>(reflect<N, 1>()) << std::endl;
			std::cout << "000-100 = ref 2  = " << to_string<N>(reflect<N, 2>()) << std::endl;
			std::cout << "001-000 = rot 01 = " << to_string<N>(rotate<N>(0, 1)) << std::endl;
			std::cout << "010-000 = rot 02 = " << to_string<N>(rotate<N>(0, 2)) << std::endl;
			std::cout << "100-000 = rot 12 = " << to_string<N>(rotate<N>(1, 2)) << std::endl;

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

			std::cout << "000000-0001 = ref 0  = " << to_string<N>(reflect<N, 0>()) << std::endl;
			std::cout << "000000-0010 = ref 1  = " << to_string<N>(reflect<N, 1>()) << std::endl;
			std::cout << "000000-0100 = ref 2  = " << to_string<N>(reflect<N, 2>()) << std::endl;
			std::cout << "000000-1000 = ref 3  = " << to_string<N>(reflect<N, 3>()) << std::endl;
			std::cout << "000001-0000 = rot 01 = " << to_string<N>(rotate<N>(0, 1)) << std::endl;
			std::cout << "000010-0000 = rot 02 = " << to_string<N>(rotate<N>(0, 2)) << std::endl;
			std::cout << "000100-0000 = rot 03 = " << to_string<N>(rotate<N>(0, 3)) << std::endl;
			std::cout << "001000-0000 = rot 12 = " << to_string<N>(rotate<N>(1, 2)) << std::endl;
			std::cout << "010000-0000 = rot 13 = " << to_string<N>(rotate<N>(1, 3)) << std::endl;
			std::cout << "100000-0000 = rot 23 = " << to_string<N>(rotate<N>(2, 3)) << std::endl;

			constexpr CubeI<N> t5 = rotate<N>(0, 1);
			constexpr CubeI<N> t6 = rotate<N>(0, 2);
			constexpr CubeI<N> t7 = rotate<N>(0, 3);
			constexpr CubeI<N> t8 = rotate<N>(1, 2);
			constexpr CubeI<N> t9 = rotate<N>(1, 3);
			constexpr CubeI<N> t10 = rotate<N>(2, 3);

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