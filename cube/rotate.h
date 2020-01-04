#pragma once
#include <array>

#include "Cube.h"
#include "CubeI.h"
#include "array_tools.h"
#include "transform.h"

namespace cube {

	namespace details {
		
		using Cycle = std::array<int, 4>;

#pragma region rotate
		constexpr Cycle create_cycle(const int d1, const int d2) noexcept
		{
			return Cycle{ (0 << d1) | (0 << d2), (1 << d1) | (0 << d2), (1 << d1) | (1 << d2), (0 << d1) | (1 << d2) };
		}
		template <int M> constexpr int find_next_in_cycle(const std::array<Cycle, M>& cycles, const int i) noexcept
		{
			for (int j = 0; j < M; ++j)
			{
				const Cycle& c = cycles[j];
				if (c[0] == i) return c[1];
				if (c[1] == i) return c[2];
				if (c[2] == i) return c[3];
				if (c[3] == i) return c[0];
			}
			return -1;
		}

		template <int N> struct rotate
		{
			static constexpr CubeI<N> value(const int dim1, const int dim2) noexcept
			{
				std::cout << "ERROR: rotate<N>: N=" << N << " not implemented yet" << std::endl;
				getchar();
				return CubeI<N>();
			}
		};
		template <> struct rotate<2>
		{
			static constexpr int N = 2;

			static constexpr CubeI<2> cycles_2_cube(const std::array<Cycle, 1>& cycles) noexcept
			{
				return CubeI<2>{
					find_next_in_cycle(cycles, 0),
						find_next_in_cycle(cycles, 1),
						find_next_in_cycle(cycles, 2),
						find_next_in_cycle(cycles, 3)
				};
			}
			static constexpr CubeI<N> value(const int d1, const int d2) noexcept
			{
				const auto cycle = create_cycle(d1, d2);
				const auto cycles = std::array<Cycle, 1> {cycle};
				return cycles_2_cube(cycles);
			}
			static void test()
			{
				constexpr auto id = init_cubeI<N>();
				constexpr auto a = value(0, 1); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
				constexpr auto b = value(1, 0); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
				static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
				static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
			}
		};
		template <> struct rotate<3>
		{
			static constexpr int N = 3;

			static constexpr std::array<int, (N-2)> fixed_dimensions(const int d1, const int d2) noexcept
			{
				const int d1a = std::min(d1, d2);
				const int d2a = std::max(d1, d2);

				if ((d1a == 0) & (d2a == 1)) return { 2 };
				if ((d1a == 0) & (d2a == 2)) return { 1 };
				if ((d1a == 1) & (d2a == 2)) return { 0 };
				return { -1 };
			}
			static constexpr CubeI<N> cycles_2_cube(const std::array<Cycle, 2>& cycles) noexcept
			{
				return CubeI<N>{
					find_next_in_cycle(cycles, 0),
						find_next_in_cycle(cycles, 1),
						find_next_in_cycle(cycles, 2),
						find_next_in_cycle(cycles, 3),

						find_next_in_cycle(cycles, 4),
						find_next_in_cycle(cycles, 5),
						find_next_in_cycle(cycles, 6),
						find_next_in_cycle(cycles, 7)
				};
			}
			static constexpr CubeI<N> value(const int d1, const int d2) noexcept
			{
				const auto cycle = create_cycle(d1, d2);
				const auto dim_fixed = fixed_dimensions(d1, d2);
				const auto cycles = std::array<Cycle, 2> {
					array_tools::add(cycle, (0 << dim_fixed[0])),
					array_tools::add(cycle, (1 << dim_fixed[0]))
				};
				return cycles_2_cube(cycles);
			}
			static void test()
			{
				constexpr auto id = init_cubeI<N>();
				constexpr auto r01 = value(0, 1);
				constexpr auto r10 = value(1, 0);
				constexpr auto r02 = value(0, 2);
				constexpr auto r20 = value(2, 0);
				constexpr auto r12 = value(1, 2);
				constexpr auto r21 = value(2, 1);

				static_assert(array_tools::equal(function_composition<N>(r01, r01, r01, r01), id), "");
				static_assert(array_tools::equal(function_composition<N>(r10, r10, r10, r10), id), "");
				static_assert(array_tools::equal(function_composition<N>(r02, r02, r02, r02), id), "");
				static_assert(array_tools::equal(function_composition<N>(r20, r20, r20, r20), id), "");
				static_assert(array_tools::equal(function_composition<N>(r12, r12, r12, r12), id), "");
				static_assert(array_tools::equal(function_composition<N>(r21, r21, r21, r21), id), "");

				static_assert(array_tools::equal(function_composition<N>(r01, r10), id), "");
				static_assert(array_tools::equal(function_composition<N>(r02, r20), id), "");
				static_assert(array_tools::equal(function_composition<N>(r12, r21), id), "");

				constexpr auto t01 = function_composition<N>(r01, r01);
				constexpr auto t10 = function_composition<N>(r10, r10);
				constexpr auto t02 = function_composition<N>(r02, r02);
				constexpr auto t20 = function_composition<N>(r20, r20);
				constexpr auto t12 = function_composition<N>(r12, r12);
				constexpr auto t21 = function_composition<N>(r21, r21);

				static_assert(array_tools::equal(t01, t10), "");
				static_assert(array_tools::equal(t02, t20), "");
				static_assert(array_tools::equal(t12, t21), "");
			}
		};
		template <> struct rotate<4>
		{
			static constexpr int N = 4;

			static constexpr std::array<int, (N - 2)> fixed_dimensions(const int d1, const int d2) noexcept
			{
				const int d1a = std::min(d1, d2);
				const int d2a = std::max(d1, d2);

				if ((d1a == 0) & (d2a == 1)) return { 2, 3 };
				if ((d1a == 0) & (d2a == 2)) return { 1, 3 };
				if ((d1a == 0) & (d2a == 3)) return { 1, 2 };
				if ((d1a == 1) & (d2a == 2)) return { 0, 3 };
				if ((d1a == 1) & (d2a == 3)) return { 0, 2 };
				if ((d1a == 2) & (d2a == 3)) return { 0, 1 };

				return { -1, -1 };
			}
			static constexpr CubeI<N> cycles_2_cube(const std::array<Cycle, 4>& cycles) noexcept
			{
				return CubeI<N>{
					find_next_in_cycle(cycles, 0),
						find_next_in_cycle(cycles, 1),
						find_next_in_cycle(cycles, 2),
						find_next_in_cycle(cycles, 3),

						find_next_in_cycle(cycles, 4),
						find_next_in_cycle(cycles, 5),
						find_next_in_cycle(cycles, 6),
						find_next_in_cycle(cycles, 7),

						find_next_in_cycle(cycles, 8),
						find_next_in_cycle(cycles, 9),
						find_next_in_cycle(cycles, 10),
						find_next_in_cycle(cycles, 11),

						find_next_in_cycle(cycles, 12),
						find_next_in_cycle(cycles, 13),
						find_next_in_cycle(cycles, 14),
						find_next_in_cycle(cycles, 15)
				};
			}
			static constexpr CubeI<N> value(const int d1, const int d2)
			{
				const auto cycle = create_cycle(d1, d2);
				const auto dim_fixed = fixed_dimensions(d1, d2);
				const auto cycles = std::array<Cycle, 4> {
					array_tools::add(cycle, (0 << dim_fixed[0]) | (0 << dim_fixed[1])),
					array_tools::add(cycle, (0 << dim_fixed[0]) | (1 << dim_fixed[1])),
					array_tools::add(cycle, (1 << dim_fixed[0]) | (0 << dim_fixed[1])),
					array_tools::add(cycle, (1 << dim_fixed[0]) | (1 << dim_fixed[1]))
				};
				return cycles_2_cube(cycles);
			}
			static void test()
			{
				constexpr auto id = init_cubeI<N>();
				{
					constexpr auto a = value(0, 1); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(1, 0); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
				{
					constexpr auto a = value(0, 2); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(2, 0); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
				{
					constexpr auto a = value(0, 3); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(3, 0); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
				{
					constexpr auto a = value(1, 2); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(2, 1); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
				{
					constexpr auto a = value(1, 3); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(3, 1); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
				{
					constexpr auto a = value(2, 3); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(3, 2); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
			}
		};
		template <> struct rotate<5>
		{
			static constexpr int N = 5;
			
			static constexpr std::array<int, (N - 2)> fixed_dimensions(const int d1, const int d2) noexcept
			{
				const int d1a = std::min(d1, d2);
				const int d2a = std::max(d1, d2);

				if ((d1a == 0) & (d2a == 1)) return { 2, 3, 4 };
				if ((d1a == 0) & (d2a == 2)) return { 1, 3, 4 };
				if ((d1a == 0) & (d2a == 3)) return { 1, 2, 4 };
				if ((d1a == 0) & (d2a == 4)) return { 1, 2, 3 };
				if ((d1a == 1) & (d2a == 2)) return { 0, 3, 4 };
				if ((d1a == 1) & (d2a == 3)) return { 0, 2, 4 };
				if ((d1a == 1) & (d2a == 4)) return { 0, 2, 3 };
				if ((d1a == 2) & (d2a == 3)) return { 0, 1, 4 };
				if ((d1a == 2) & (d2a == 4)) return { 0, 1, 3 };
				if ((d1a == 3) & (d2a == 4)) return { 0, 1, 2 };

				return { -1, -1, -1 };
			}
			static constexpr CubeI<N> cycles_2_cube(const std::array<Cycle, 8>& cycles) noexcept
			{
				return CubeI<N>{
					find_next_in_cycle(cycles, 0),
						find_next_in_cycle(cycles, 1),
						find_next_in_cycle(cycles, 2),
						find_next_in_cycle(cycles, 3),

						find_next_in_cycle(cycles, 4),
						find_next_in_cycle(cycles, 5),
						find_next_in_cycle(cycles, 6),
						find_next_in_cycle(cycles, 7),

						find_next_in_cycle(cycles, 8),
						find_next_in_cycle(cycles, 9),
						find_next_in_cycle(cycles, 10),
						find_next_in_cycle(cycles, 11),

						find_next_in_cycle(cycles, 12),
						find_next_in_cycle(cycles, 13),
						find_next_in_cycle(cycles, 14),
						find_next_in_cycle(cycles, 15),
						find_next_in_cycle(cycles, 16),
						find_next_in_cycle(cycles, 17),
						find_next_in_cycle(cycles, 18),
						find_next_in_cycle(cycles, 19),

						find_next_in_cycle(cycles, 20),
						find_next_in_cycle(cycles, 21),
						find_next_in_cycle(cycles, 22),
						find_next_in_cycle(cycles, 23),

						find_next_in_cycle(cycles, 24),
						find_next_in_cycle(cycles, 25),
						find_next_in_cycle(cycles, 26),
						find_next_in_cycle(cycles, 27),

						find_next_in_cycle(cycles, 28),
						find_next_in_cycle(cycles, 29),
						find_next_in_cycle(cycles, 30),
						find_next_in_cycle(cycles, 31)
				};
			}
			static constexpr CubeI<N> value(const int d1, const int d2) noexcept
			{
				const auto cycle = create_cycle(d1, d2);
				const auto dim_fixed = fixed_dimensions(d1, d2);
				const auto cycles = std::array<Cycle, 8> {
					array_tools::add(cycle, (0 << dim_fixed[0]) | (0 << dim_fixed[1]) | (0 << dim_fixed[2])),
					array_tools::add(cycle, (0 << dim_fixed[0]) | (0 << dim_fixed[1]) | (1 << dim_fixed[2])), 
					array_tools::add(cycle, (0 << dim_fixed[0]) | (1 << dim_fixed[1]) | (0 << dim_fixed[2])),
					array_tools::add(cycle, (0 << dim_fixed[0]) | (1 << dim_fixed[1]) | (1 << dim_fixed[2])),
					array_tools::add(cycle, (1 << dim_fixed[0]) | (0 << dim_fixed[1]) | (0 << dim_fixed[2])),
					array_tools::add(cycle, (1 << dim_fixed[0]) | (0 << dim_fixed[1]) | (1 << dim_fixed[2])),
					array_tools::add(cycle, (1 << dim_fixed[0]) | (1 << dim_fixed[1]) | (0 << dim_fixed[2])),
					array_tools::add(cycle, (1 << dim_fixed[0]) | (1 << dim_fixed[1]) | (1 << dim_fixed[2]))
				};
				return cycles_2_cube(cycles);
			}
			static void test()
			{
				constexpr auto id = init_cubeI<N>();
				{
					constexpr auto a = value(0, 1); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(1, 0); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
				{
					constexpr auto a = value(0, 2); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(2, 0); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
				{
					constexpr auto a = value(0, 3); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(3, 0); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
				{
					constexpr auto a = value(0, 4); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(4, 0); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
				{
					constexpr auto a = value(1, 2); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(2, 1); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
				{
					constexpr auto a = value(1, 3); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(3, 1); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
				{
					constexpr auto a = value(1, 4); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(4, 1); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
				{
					constexpr auto a = value(2, 3); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(3, 2); static_assert(array_tools::equal(function_composition<N>(b, b, b, b),id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
				{
					constexpr auto a = value(2, 4); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(4, 2); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
				{
					constexpr auto a = value(3, 4); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(4, 3); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
			}
		};
		template <> struct rotate<6>
		{
			static constexpr int N = 6;

			static constexpr std::array<int, (N - 2)> fixed_dimensions(const int d1, const int d2) noexcept
			{
				const int d1a = std::min(d1, d2);
				const int d2a = std::max(d1, d2);

				if ((d1a == 0) & (d2a == 1)) return { 2, 3, 4, 5 };
				if ((d1a == 0) & (d2a == 2)) return { 1, 3, 4, 5 };
				if ((d1a == 0) & (d2a == 3)) return { 1, 2, 4, 5 };
				if ((d1a == 0) & (d2a == 4)) return { 1, 2, 3, 5 };
				if ((d1a == 0) & (d2a == 5)) return { 1, 2, 3, 4 };
				if ((d1a == 1) & (d2a == 2)) return { 0, 3, 4, 5 };
				if ((d1a == 1) & (d2a == 3)) return { 0, 2, 4, 5 };
				if ((d1a == 1) & (d2a == 4)) return { 0, 2, 3, 5 };
				if ((d1a == 1) & (d2a == 5)) return { 0, 2, 3, 4 };
				if ((d1a == 2) & (d2a == 3)) return { 0, 1, 4, 5 };
				if ((d1a == 2) & (d2a == 4)) return { 0, 1, 3, 5 };
				if ((d1a == 2) & (d2a == 5)) return { 0, 1, 3, 4 };
				if ((d1a == 3) & (d2a == 4)) return { 0, 1, 2, 5 };
				if ((d1a == 3) & (d2a == 5)) return { 0, 1, 2, 4 };
				if ((d1a == 4) & (d2a == 5)) return { 0, 1, 2, 3 };

				return { -1, -1, -1, -1 };
			}
			static constexpr CubeI<N> cycles_2_cube(const std::array<Cycle, 16>& cycles) noexcept
			{
				return CubeI<N>{
					find_next_in_cycle(cycles, 0),
						find_next_in_cycle(cycles, 1),
						find_next_in_cycle(cycles, 2),
						find_next_in_cycle(cycles, 3),

						find_next_in_cycle(cycles, 4),
						find_next_in_cycle(cycles, 5),
						find_next_in_cycle(cycles, 6),
						find_next_in_cycle(cycles, 7),

						find_next_in_cycle(cycles, 8),
						find_next_in_cycle(cycles, 9),
						find_next_in_cycle(cycles, 10),
						find_next_in_cycle(cycles, 11),

						find_next_in_cycle(cycles, 12),
						find_next_in_cycle(cycles, 13),
						find_next_in_cycle(cycles, 14),
						find_next_in_cycle(cycles, 15),

						find_next_in_cycle(cycles, 16),
						find_next_in_cycle(cycles, 17),
						find_next_in_cycle(cycles, 18),
						find_next_in_cycle(cycles, 19),

						find_next_in_cycle(cycles, 20),
						find_next_in_cycle(cycles, 21),
						find_next_in_cycle(cycles, 22),
						find_next_in_cycle(cycles, 23),

						find_next_in_cycle(cycles, 24),
						find_next_in_cycle(cycles, 25),
						find_next_in_cycle(cycles, 26),
						find_next_in_cycle(cycles, 27),

						find_next_in_cycle(cycles, 28),
						find_next_in_cycle(cycles, 29),
						find_next_in_cycle(cycles, 30),
						find_next_in_cycle(cycles, 31),

						find_next_in_cycle(cycles, 32),
						find_next_in_cycle(cycles, 33),
						find_next_in_cycle(cycles, 34),
						find_next_in_cycle(cycles, 35),

						find_next_in_cycle(cycles, 36),
						find_next_in_cycle(cycles, 37),
						find_next_in_cycle(cycles, 38),
						find_next_in_cycle(cycles, 39),

						find_next_in_cycle(cycles, 40),
						find_next_in_cycle(cycles, 41),
						find_next_in_cycle(cycles, 42),
						find_next_in_cycle(cycles, 43),

						find_next_in_cycle(cycles, 44),
						find_next_in_cycle(cycles, 45),
						find_next_in_cycle(cycles, 46),
						find_next_in_cycle(cycles, 47),

						find_next_in_cycle(cycles, 48),
						find_next_in_cycle(cycles, 49),
						find_next_in_cycle(cycles, 50),
						find_next_in_cycle(cycles, 51),

						find_next_in_cycle(cycles, 52),
						find_next_in_cycle(cycles, 53),
						find_next_in_cycle(cycles, 54),
						find_next_in_cycle(cycles, 55),

						find_next_in_cycle(cycles, 56),
						find_next_in_cycle(cycles, 57),
						find_next_in_cycle(cycles, 58),
						find_next_in_cycle(cycles, 59),

						find_next_in_cycle(cycles, 60),
						find_next_in_cycle(cycles, 61),
						find_next_in_cycle(cycles, 62),
						find_next_in_cycle(cycles, 63)
				};
			}
			static constexpr CubeI<N> value(const int d1, const int d2)
			{
				const auto cycle = create_cycle(d1, d2);
				const auto dim_fixed = fixed_dimensions(d1, d2);
				const auto cycles = std::array<Cycle, 16> {
					array_tools::add(cycle, (0 << dim_fixed[0]) | (0 << dim_fixed[1]) | (0 << dim_fixed[2]) | (0 << dim_fixed[3])),
					array_tools::add(cycle, (0 << dim_fixed[0]) | (0 << dim_fixed[1]) | (0 << dim_fixed[2]) | (1 << dim_fixed[3])),
					array_tools::add(cycle, (0 << dim_fixed[0]) | (0 << dim_fixed[1]) | (1 << dim_fixed[2]) | (0 << dim_fixed[3])),
					array_tools::add(cycle, (0 << dim_fixed[0]) | (0 << dim_fixed[1]) | (1 << dim_fixed[2]) | (1 << dim_fixed[3])),
					array_tools::add(cycle, (0 << dim_fixed[0]) | (1 << dim_fixed[1]) | (0 << dim_fixed[2]) | (0 << dim_fixed[3])),
					array_tools::add(cycle, (0 << dim_fixed[0]) | (1 << dim_fixed[1]) | (0 << dim_fixed[2]) | (1 << dim_fixed[3])),
					array_tools::add(cycle, (0 << dim_fixed[0]) | (1 << dim_fixed[1]) | (1 << dim_fixed[2]) | (0 << dim_fixed[3])),
					array_tools::add(cycle, (0 << dim_fixed[0]) | (1 << dim_fixed[1]) | (1 << dim_fixed[2]) | (1 << dim_fixed[3])),
					array_tools::add(cycle, (1 << dim_fixed[0]) | (0 << dim_fixed[1]) | (0 << dim_fixed[2]) | (0 << dim_fixed[3])),
					array_tools::add(cycle, (1 << dim_fixed[0]) | (0 << dim_fixed[1]) | (0 << dim_fixed[2]) | (1 << dim_fixed[3])),
					array_tools::add(cycle, (1 << dim_fixed[0]) | (0 << dim_fixed[1]) | (1 << dim_fixed[2]) | (0 << dim_fixed[3])),
					array_tools::add(cycle, (1 << dim_fixed[0]) | (0 << dim_fixed[1]) | (1 << dim_fixed[2]) | (1 << dim_fixed[3])),
					array_tools::add(cycle, (1 << dim_fixed[0]) | (1 << dim_fixed[1]) | (0 << dim_fixed[2]) | (0 << dim_fixed[3])),
					array_tools::add(cycle, (1 << dim_fixed[0]) | (1 << dim_fixed[1]) | (0 << dim_fixed[2]) | (1 << dim_fixed[3])),
					array_tools::add(cycle, (1 << dim_fixed[0]) | (1 << dim_fixed[1]) | (1 << dim_fixed[2]) | (0 << dim_fixed[3])),
					array_tools::add(cycle, (1 << dim_fixed[0]) | (1 << dim_fixed[1]) | (1 << dim_fixed[2]) | (1 << dim_fixed[3]))
				};
				return cycles_2_cube(cycles);
			}
			static void test()
			{
				constexpr auto id = init_cubeI<N>();
				{
					constexpr auto a = value(0, 1); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(1, 0); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
				{
					constexpr auto a = value(0, 2); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(2, 0); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
				{
					constexpr auto a = value(0, 3); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(3, 0); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
				{
					constexpr auto a = value(0, 4); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(4, 0); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
				{
					constexpr auto a = value(0, 5); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(5, 0); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
				{
					constexpr auto a = value(1, 2); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(2, 1); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
				{
					constexpr auto a = value(1, 3); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(3, 1); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
				{
					constexpr auto a = value(1, 4); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(4, 1); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
				{
					constexpr auto a = value(1, 5); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(5, 1); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
				{
					constexpr auto a = value(2, 3); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(3, 2); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
				{
					constexpr auto a = value(2, 4); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(4, 2); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
				{
					constexpr auto a = value(2, 5); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(5, 2); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
				{
					constexpr auto a = value(3, 4); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(4, 3); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
				{
					constexpr auto a = value(3, 5); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(5, 3); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
				{
					constexpr auto a = value(4, 5); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(5, 4); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
			}
		};
		#pragma endregion


	}
}