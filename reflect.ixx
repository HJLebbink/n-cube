module; 
#include <iostream>
#include "CubeDef.h"


export module cube.reflect;
//import std.core;
import cube.index;
import array_tools;
import transform;


namespace cube {

	//forward
	export template <int N, int D> consteval [[nodiscard]]
	CubeI<N> reflect();

	template <int N, int D> consteval [[nodiscard]]
	CubeI<N> lift_reflect() noexcept
	{
		static_assert(D < N, "");
		using T = typename CubeI<N>::value_type;
		constexpr int N2 = N - 1;
		constexpr CubeI<N2> reflect_embedded_cube = reflect<N2, D>();
		return array_tools::concat<T>(reflect_embedded_cube, array_tools::add(reflect_embedded_cube, static_cast<T>(1 << N2)));
	}
	
	export template <int N, int D> consteval [[nodiscard]]
	CubeI<N> reflect() {
		using T = CubeI<N>::value_type;

		if constexpr (N == 1) {
			return CubeI<N>{ 1, 0 };
		}
		else if constexpr (N == 2) {
			if constexpr (D == 0) {
				constexpr CubeI<N> r0 = lift_reflect<N, 0>();
				return r0; //CubeI<N>{ 1, 0, 3, 2 };
			}
			else {
				constexpr int N2 = N - 1;
				constexpr CubeI<N2> id = init_cubeI<N2>();
				constexpr auto r1 = array_tools::concat(array_tools::add<T>(id, 1 << N2), id);
				return r1; //CubeI<N>{ 2, 3, 0, 1 };
			}
		}
		else if constexpr (N == 3) {
			if constexpr (D == 0) {
				constexpr auto r0 = lift_reflect<N, 0>();
				return r0; //CubeI<N>{ 1, 0, 3, 2, 5, 4, 7, 6 };
			}
			else if constexpr (D == 1) {
				constexpr auto r1 = lift_reflect<N, 1>();
				return r1; //CubeI<N>{ 2, 3, 0, 1, 6, 7, 4, 5 };
			}
			else {
				constexpr auto id = init_cubeI<(N - 1)>();
				constexpr auto r2 = array_tools::concat(array_tools::add<T>(id, 1 << (N - 1)), id);
				return r2; //CubeI<N>{ 4, 5, 6, 7, 0, 1, 2, 3 };
			}
		}
		else if constexpr (N == 4) {
			if constexpr (D == 0) {
				constexpr auto r0 = lift_reflect<N, 0>();
				return r0; //CubeI<N>{ 1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14 };
			}
			else if constexpr (D == 1) {
				constexpr auto r1 = lift_reflect<N, 1>();
				return r1; //CubeI<N>{ 2, 3, 0, 1, 6, 7, 4, 5, 10, 11, 8, 9, 14, 15, 12, 13 };
			}
			else if constexpr (D == 2) {
				constexpr auto r2 = lift_reflect<N, 2>();
				return r2; //CubeI<N>{ 4, 5, 6, 7, 0, 1, 2, 3, 12, 13, 14, 15, 8, 9, 10, 11 };
			}
			else {
				constexpr auto id = init_cubeI<(N - 1)>();
				constexpr auto r3 = array_tools::concat(array_tools::add<T>(id, 1 << (N - 1)), id);
				return r3; //CubeI<N>{ 8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7 };
			}
		}
		else if constexpr (N == 5) {
			if constexpr (D == 0) {
				return lift_reflect<N, 0>();
			}
			else if constexpr (D == 1) {
				return lift_reflect<N, 1>();
			}
			else if constexpr (D == 2) {
				return lift_reflect<N, 2>();
			}
			else if constexpr (D == 3) {
				return lift_reflect<N, 3>();
			}
			else {
				constexpr auto id = init_cubeI<(N - 1)>();
				return array_tools::concat(array_tools::add<T>(id, 1 << (N - 1)), id);
			}
		}
		else if constexpr (N == 6) {
			if constexpr (D == 0) {
				return lift_reflect<N, 0>();
			}
			else if constexpr (D == 1) {
				return lift_reflect<N, 1>();
			}
			else if constexpr (D == 2) {
				return lift_reflect<N, 2>();
			}
			else if constexpr (D == 3) {
				return lift_reflect<N, 3>();
			}
			else if constexpr (D == 4) {
				return lift_reflect<N, 4>();
			}
			else if constexpr (D == 5) {
				constexpr auto id = init_cubeI<(N - 1)>();
				return array_tools::concat(array_tools::add<T>(id, 1 << (N - 1)), id);
			}
		}
	}
	
	export template <int N> constexpr [[nodiscard]]
	CubeI<N> reflect(const int dim)
	{
		switch (dim) {
		case 1: return reflect<N, 1>();
		case 2: return reflect<N, 2>();
		case 3: return reflect<N, 3>();
		case 4: return reflect<N, 4>();
		case 5: return reflect<N, 5>();
		case 6: return reflect<N, 6>();
		default: return CubeI<N>();
		}
	}

	consteval void test_reflect()
	{
		{
			constexpr int N = 3;
			constexpr auto id = init_cubeI<N>();
			static_assert(array_tools::equal(function_composition<N>(reflect<N, 0>(), reflect<N, 0>()), id), "");
			static_assert(array_tools::equal(function_composition<N>(reflect<N, 1>(), reflect<N, 1>()), id), "");
			static_assert(array_tools::equal(function_composition<N>(reflect<N, 2>(), reflect<N, 2>()), id), "");
		}
		{
			constexpr int N = 4;
			constexpr auto id = init_cubeI<N>();
			static_assert(array_tools::equal(function_composition<N>(reflect<N, 0>(), reflect<N, 0>()), id), "");
			static_assert(array_tools::equal(function_composition<N>(reflect<N, 1>(), reflect<N, 1>()), id), "");
			static_assert(array_tools::equal(function_composition<N>(reflect<N, 2>(), reflect<N, 2>()), id), "");
			static_assert(array_tools::equal(function_composition<N>(reflect<N, 3>(), reflect<N, 3>()), id), "");
		}
		{
			constexpr int N = 5;
			constexpr auto id = init_cubeI<N>();
			static_assert(array_tools::equal(function_composition<N>(reflect<N, 0>(), reflect<N, 0>()), id), "");
			static_assert(array_tools::equal(function_composition<N>(reflect<N, 1>(), reflect<N, 1>()), id), "");
			static_assert(array_tools::equal(function_composition<N>(reflect<N, 2>(), reflect<N, 2>()), id), "");
			static_assert(array_tools::equal(function_composition<N>(reflect<N, 3>(), reflect<N, 3>()), id), "");
			static_assert(array_tools::equal(function_composition<N>(reflect<N, 4>(), reflect<N, 4>()), id), "");
		}
		{
			constexpr int N = 6;
			constexpr auto id = init_cubeI<N>();
			static_assert(array_tools::equal(function_composition<N>(reflect<N, 0>(), reflect<N, 0>()), id), "");
			static_assert(array_tools::equal(function_composition<N>(reflect<N, 1>(), reflect<N, 1>()), id), "");
			static_assert(array_tools::equal(function_composition<N>(reflect<N, 2>(), reflect<N, 2>()), id), "");
			static_assert(array_tools::equal(function_composition<N>(reflect<N, 3>(), reflect<N, 3>()), id), "");
			static_assert(array_tools::equal(function_composition<N>(reflect<N, 4>(), reflect<N, 4>()), id), "");
			static_assert(array_tools::equal(function_composition<N>(reflect<N, 5>(), reflect<N, 5>()), id), "");
		}
	}
}