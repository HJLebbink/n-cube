#pragma once
#include "CubeI.h"
#include "array_tools.h"
#include "transform.h"


namespace cube {
	namespace details {
				
#pragma region reflect
		template <int N, int D> constexpr CubeI<N> lift_reflect() noexcept
		{
			static_assert(D < N, "");
			using T = typename CubeI<N>::value_type;
			constexpr int N2 = N - 1;
			constexpr CubeI<N2> reflect_embedded_cube = reflect<N2>::value(D);
			return array_tools::concat<T>(reflect_embedded_cube, array_tools::add(reflect_embedded_cube, static_cast<T>(1 << N2)));
		}
		template <int N> struct reflect
		{
		};

		template <> struct reflect<1>
		{
			static constexpr int N = 1;
			static constexpr CubeI<N> value(const int dim) noexcept
			{
				if (dim == 0)
				{
					return CubeI<N>{ 1, 0 };
				}
				std::cout << "ERROR: reflect<" << N << ">::value: dim=" << dim << " does not exist." << std::endl;
				return CubeI<N>();
			}
			static constexpr CubeI<N> invalid(const int /*dim*/, const int /*var*/) noexcept
			{
				//std::cout << "ERROR: reflect<" << N << ">::invalid: dim=" << dim << " does not exist." << std::endl;
				return CubeI<N>();
			}
		};
		template <> struct reflect<2>
		{
			static constexpr int N = 2;
			static constexpr CubeI<N> value(const int dim) noexcept
			{
				using T = CubeI<N>::value_type;
				constexpr int N2 = N - 1;
				constexpr CubeI<N> r0 = lift_reflect<N, 0>();
				constexpr CubeI<N2> id = init_cubeI<N2>();
				constexpr auto r1 = array_tools::concat(array_tools::add<T>(id, 1 << N2), id);

				switch (dim)
				{
					case 0: return r0; //CubeI<N>{ 1, 0, 3, 2 };
					case 1: return r1; //CubeI<N>{ 2, 3, 0, 1 };
				}
				std::cout << "ERROR: reflect<" << N << ">::value: dim=" << dim << " does not exist." << std::endl;
				return CubeI<N>();
			}
			// reflect over the provided dimension, yet keep var unchanged.
			static constexpr CubeI<N> invalid(const int dim, const int var) noexcept
			{
				if ((dim == 0) && (var == 0)) return CubeI<N>{1, 0, 2, 3};
				if ((dim == 0) && (var == 1)) return CubeI<N>{0, 1, 3, 2};
				if ((dim == 1) && (var == 0)) return CubeI<N>{2, 1, 0, 3};
				if ((dim == 1) && (var == 1)) return CubeI<N>{0, 3, 2, 1};

				std::cout << "ERROR: reflect<" << N << ">::invalid: dim=" << dim << " does not exist." << std::endl;
				return CubeI<N>();
			}
		};
		template <> struct reflect<3>
		{
			static constexpr int N = 3;
			static constexpr CubeI<N> value(const int dim) noexcept
			{
				using T = CubeI<N>::value_type;
				constexpr auto r0 = lift_reflect<N, 0>();
				constexpr auto r1 = lift_reflect<N, 1>();
				constexpr auto id = init_cubeI<(N - 1)>();
				constexpr auto r2 = array_tools::concat(array_tools::add<T>(id, 1 << (N - 1)), id);

				switch (dim)
				{
					case 0: return r0; //CubeI<N>{ 1, 0, 3, 2, 5, 4, 7, 6 };
					case 1: return r1; //CubeI<N>{ 2, 3, 0, 1, 6, 7, 4, 5 };
					case 2: return r2; //CubeI<N>{ 4, 5, 6, 7, 0, 1, 2, 3 };
				}
				std::cout << "ERROR: reflect<" << N << ">: dim=" << dim << " does not exist." << std::endl;
				return CubeI<N>();
			}
			// reflect over the provided dimension, yet keep var unchanged.
			static constexpr CubeI<N> invalid(const int dim, const int var) noexcept
			{
				if ((dim == 0) && (var == 0)) return CubeI<N>{1, 0, 2, 3};
				if ((dim == 0) && (var == 1)) return CubeI<N>{0, 1, 3, 2};
				if ((dim == 0) && (var == 2)) return CubeI<N>{0, 1, 3, 2};

				if ((dim == 1) && (var == 0)) return CubeI<N>{2, 1, 0, 3};
				if ((dim == 1) && (var == 1)) return CubeI<N>{0, 3, 2, 1};
				if ((dim == 1) && (var == 2)) return CubeI<N>{0, 3, 2, 1};

				if ((dim == 2) && (var == 0)) return CubeI<N>{2, 1, 0, 3};
				if ((dim == 2) && (var == 1)) return CubeI<N>{0, 3, 2, 1};
				if ((dim == 2) && (var == 2)) return CubeI<N>{0, 3, 2, 1};


				std::cout << "ERROR: reflect<" << N << ">::invalid: dim=" << dim << " does not exist." << std::endl;
				return CubeI<N>();
			}
			static void test()
			{
				constexpr auto id = init_cubeI<(N)>();
				static_assert(array_tools::equal(function_composition<N>(value(0), value(0)), id), "");
				static_assert(array_tools::equal(function_composition<N>(value(1), value(1)), id), "");
				static_assert(array_tools::equal(function_composition<N>(value(2), value(2)), id), "");
			}
		};
		template <> struct reflect<4>
		{
			static constexpr int N = 4;
			template <int D> static constexpr CubeI<N> cube4_reflect_indices()
			{
				static_assert(D >= 0, "cube_reflect_4: D must be positive.");
				static_assert(D < N, "cube_reflect_4: D1 must be smaller than 4");

				constexpr const auto a0 = index_split_1<D, 0>();
				constexpr const auto a1 = index_split_1<D, 1>();
				constexpr const auto a2 = index_split_1<D, 2>();
				constexpr const auto a3 = index_split_1<D, 3>();
				constexpr const auto a4 = index_split_1<D, 4>();
				constexpr const auto a5 = index_split_1<D, 5>();
				constexpr const auto a6 = index_split_1<D, 6>();
				constexpr const auto a7 = index_split_1<D, 7>();

				constexpr const auto b0 = array_tools::concat(a0, a1, a2, a3);
				constexpr const auto b1 = array_tools::concat(a4, a5, a6, a7);
				return array_tools::concat(b0, b1);
			}

			static constexpr CubeI<N> value(const int dim) noexcept
			{
				using T = CubeI<N>::value_type;
				constexpr auto r0 = lift_reflect<N, 0>();
				constexpr auto r1 = lift_reflect<N, 1>();
				constexpr auto r2 = lift_reflect<N, 2>();
				constexpr auto id = init_cubeI<(N - 1)>();
				constexpr auto r3 = array_tools::concat(array_tools::add<T>(id, 1 << (N - 1)), id);

				switch (dim)
				{
					case 0: return r0; //CubeI<N>{ 1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14 };
					case 1: return r1; //CubeI<N>{ 2, 3, 0, 1, 6, 7, 4, 5, 10, 11, 8, 9, 14, 15, 12, 13 };
					case 2: return r2; //CubeI<N>{ 4, 5, 6, 7, 0, 1, 2, 3, 12, 13, 14, 15, 8, 9, 10, 11 };
					case 3: return r3; //CubeI<N>{ 8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7 };
				}
				std::cout << "ERROR: reflect<" << N << ">: dim=" << dim << " does not exist." << std::endl;
				return CubeI<N>();
			}
			static void test()
			{
				constexpr auto id = init_cubeI<(N)>();
				static_assert(array_tools::equal(function_composition<N>(value(0), value(0)), id), "");
				static_assert(array_tools::equal(function_composition<N>(value(1), value(1)), id), "");
				static_assert(array_tools::equal(function_composition<N>(value(2), value(2)), id), "");
				static_assert(array_tools::equal(function_composition<N>(value(3), value(3)), id), "");
			}
		};
		template <> struct reflect<5>
		{
			static constexpr int N = 5;
			static constexpr CubeI<N> value(const int dim) noexcept
			{
				using T = CubeI<N>::value_type;
				constexpr auto r0 = lift_reflect<N, 0>();
				constexpr auto r1 = lift_reflect<N, 1>();
				constexpr auto r2 = lift_reflect<N, 2>();
				constexpr auto r3 = lift_reflect<N, 3>();
				constexpr auto id = init_cubeI<(N - 1)>();
				constexpr auto r4 = array_tools::concat(array_tools::add<T>(id, 1 << (N - 1)), id);

				switch (dim)
				{
					case 0: return r0;
					case 1: return r1;
					case 2: return r2;
					case 3: return r3;
					case 4: return r4;
				}
				std::cout << "ERROR: reflect<" << N << ">: dim=" << dim << " does not exist." << std::endl;
				return CubeI<N>();
			}
			static void test()
			{
				constexpr auto id = init_cubeI<(N)>();
				static_assert(array_tools::equal(function_composition<N>(value(0), value(0)), id), "");
				static_assert(array_tools::equal(function_composition<N>(value(1), value(1)), id), "");
				static_assert(array_tools::equal(function_composition<N>(value(2), value(2)), id), "");
				static_assert(array_tools::equal(function_composition<N>(value(3), value(3)), id), "");
				static_assert(array_tools::equal(function_composition<N>(value(4), value(4)), id), "");
			}
		};
		template <> struct reflect<6>
		{
			static constexpr int N = 6;
			static constexpr CubeI<N> value(const int dim) noexcept
			{
				using T = CubeI<N>::value_type;
				constexpr auto r0 = lift_reflect<N, 0>();
				constexpr auto r1 = lift_reflect<N, 1>();
				constexpr auto r2 = lift_reflect<N, 2>();
				constexpr auto r3 = lift_reflect<N, 3>();
				constexpr auto r4 = lift_reflect<N, 4>();
				constexpr auto id = init_cubeI<(N - 1)>();
				constexpr auto r5 = array_tools::concat(array_tools::add<T>(id, 1 << (N - 1)), id);

				switch (dim)
				{
					case 0: return r0;
					case 1: return r1;
					case 2: return r2;
					case 3: return r3;
					case 4: return r4;
					case 5: return r5;
				}
				std::cout << "ERROR: reflect<" << N << ">: dim=" << dim << " does not exist." << std::endl;
				return CubeI<N>();
			}
			static void test()
			{
				constexpr auto id = init_cubeI<(N)>();
				static_assert(array_tools::equal(function_composition<N>(value(0), value(0)), id), "");
				static_assert(array_tools::equal(function_composition<N>(value(1), value(1)), id), "");
				static_assert(array_tools::equal(function_composition<N>(value(2), value(2)), id), "");
				static_assert(array_tools::equal(function_composition<N>(value(3), value(3)), id), "");
				static_assert(array_tools::equal(function_composition<N>(value(4), value(4)), id), "");
				static_assert(array_tools::equal(function_composition<N>(value(5), value(5)), id), "");
			}
		};
		/*
		template <> struct reflect<7>
		{
			static constexpr int N = 7;
			static constexpr CubeI<N> value(const int dim)
			{
				constexpr auto r0 = lift_reflect<N, 0>();
				constexpr auto r1 = lift_reflect<N, 1>();
				constexpr auto r2 = lift_reflect<N, 2>();
				constexpr auto r3 = lift_reflect<N, 3>();
				constexpr auto r4 = lift_reflect<N, 4>();
				constexpr auto r5 = lift_reflect<N, 5>();
				constexpr auto id = init_cubeI<(N - 1)>();
				constexpr auto r6 = array_tools::concat(array_tools::add(id, 1 << (N - 1)), id);

				switch (dim)
				{
					case 0: return r0;
					case 1: return r1;
					case 2: return r2;
					case 3: return r3;
					case 4: return r4;
					case 5: return r5;
					case 6: return r6;
				}
				std::cout << "ERROR: reflect<" << N << ">: dim=" << dim << " does not exist." << std::endl;
				return CubeI<N>();
			}
			static void test()
			{
				constexpr auto id = init_cubeI<(N)>();
				static_assert(array_tools::equal(function_composition<N>(value(0), value(0)), id), "");
				static_assert(array_tools::equal(function_composition<N>(value(1), value(1)), id), "");
				static_assert(array_tools::equal(function_composition<N>(value(2), value(2)), id), "");
				static_assert(array_tools::equal(function_composition<N>(value(3), value(3)), id), "");
				static_assert(array_tools::equal(function_composition<N>(value(4), value(4)), id), "");
				static_assert(array_tools::equal(function_composition<N>(value(5), value(5)), id, "");
				static_assert(array_tools::equal(function_composition<N>(value(6), value(6)), id), "");
			}
		};
		*/
		#pragma endregion
	}
}