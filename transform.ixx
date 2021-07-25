module; 
#include <array>
#include "CubeDef.h"

export module transform;
import CubeIndex;
import array_tools;
import BF;


namespace cube {

	namespace {

		template <int N>
		[[nodiscard]] constexpr CubeI<N> apply(const CubeI<N>& c, const CubeI<N>& ci) {
			if constexpr (N == 1) {
				return CubeI<N>{ c[ci[0]], c[ci[1]] };
			}
			if constexpr (N == 2) {
				return CubeI<N>{ c[ci[0]], c[ci[1]], c[ci[2]], c[ci[3]] };
			}
			if constexpr (N == 3) {
				return CubeI<N>{ c[ci[0]], c[ci[1]], c[ci[2]], c[ci[3]], c[ci[4]], c[ci[5]], c[ci[6]], c[ci[7]] };
			}
			if constexpr (N == 4) {
				return CubeI<N>{
					c[ci[0]], c[ci[1]], c[ci[2]], c[ci[3]], c[ci[4]], c[ci[5]], c[ci[6]], c[ci[7]],
						c[ci[8]], c[ci[9]], c[ci[10]], c[ci[11]], c[ci[12]], c[ci[13]], c[ci[14]], c[ci[15]]
				};
			}
			if constexpr (N == 5) {
				return CubeI<N>{
					c[ci[0]], c[ci[1]], c[ci[2]], c[ci[3]], c[ci[4]], c[ci[5]], c[ci[6]], c[ci[7]],
						c[ci[8]], c[ci[9]], c[ci[10]], c[ci[11]], c[ci[12]], c[ci[13]], c[ci[14]], c[ci[15]],
						c[ci[16]], c[ci[17]], c[ci[18]], c[ci[19]], c[ci[20]], c[ci[21]], c[ci[22]], c[ci[23]],
						c[ci[24]], c[ci[25]], c[ci[26]], c[ci[27]], c[ci[28]], c[ci[29]], c[ci[30]], c[ci[31]]
				};
			}
			if constexpr (N == 6) {
				return CubeI<N>{
					c[ci[0]], c[ci[1]], c[ci[2]], c[ci[3]], c[ci[4]], c[ci[5]], c[ci[6]], c[ci[7]],
						c[ci[8]], c[ci[9]], c[ci[10]], c[ci[11]], c[ci[12]], c[ci[13]], c[ci[14]], c[ci[15]],
						c[ci[16]], c[ci[17]], c[ci[18]], c[ci[19]], c[ci[20]], c[ci[21]], c[ci[22]], c[ci[23]],
						c[ci[24]], c[ci[25]], c[ci[26]], c[ci[27]], c[ci[28]], c[ci[29]], c[ci[30]], c[ci[31]],
						c[ci[32]], c[ci[33]], c[ci[34]], c[ci[35]], c[ci[36]], c[ci[37]], c[ci[38]], c[ci[39]],
						c[ci[40]], c[ci[41]], c[ci[42]], c[ci[43]], c[ci[44]], c[ci[45]], c[ci[46]], c[ci[47]],
						c[ci[48]], c[ci[49]], c[ci[50]], c[ci[51]], c[ci[52]], c[ci[53]], c[ci[54]], c[ci[55]],
						c[ci[56]], c[ci[57]], c[ci[58]], c[ci[59]], c[ci[60]], c[ci[61]], c[ci[62]], c[ci[63]]
				};
			}
			//return CubeI<N>();
		}

		template <int N, int ... S>
		constexpr BF apply_private(const BF bf, const CubeI<N>& c, std::index_sequence<S...>) noexcept {
			return (... | (((bf >> c[S]) & 1) << S));
		}

		template <int N>
		constexpr BF apply(const BF bf, const CubeI<N>& c) noexcept {
			return apply_private<N>(bf, c, std::make_index_sequence<(1 << N)>());
		}
	}

#pragma region functional composition

	export template <int N> 
	[[nodiscard]] constexpr CubeI<N> function_composition(const CubeI<N>& a) noexcept {
		return a;
	}
	export template <int N> 
	[[nodiscard]] constexpr CubeI<N> function_composition(const CubeI<N>& a0, const CubeI<N>& a1) noexcept {
		return apply<N>(a0, a1);
	}
	export template <int N> 
	[[nodiscard]] constexpr CubeI<N> function_composition(const CubeI<N>& a0, const CubeI<N>& a1, const CubeI<N>& a2) noexcept {
		return apply<N>(a0, apply<N>(a1, a2));
	}
	export template <int N> 
	[[nodiscard]] constexpr CubeI<N> function_composition(const CubeI<N>& a0, const CubeI<N>& a1, const CubeI<N>& a2, const CubeI<N>& a3) noexcept {
		return apply<N>(apply<N>(a0, a1), apply<N>(a2, a3));
	}

	export template <int N, int M> 
	[[nodiscard]] constexpr CubeI<N> function_composition(const std::array<CubeI<N>, M>& a) noexcept {
		if constexpr (M == 1) {
			return function_composition(a[0]);
		}
		else if constexpr (M == 2) {
			return function_composition(a[0], a[1]);
		}
		else if constexpr (M == 3) {
			return function_composition(a[0], a[1], a[2]);
		}
		else if constexpr (M == 4) {
			return function_composition(a[0], a[1], a[2], a[3]);
		}
		static_assert(false);
	}

	void test_function_composition() {

		constexpr int N = 2;

		constexpr auto id = init_cubeI<(N)>();
		static_assert(array_tools::equal(function_composition<N>(id, id), id), "");
		//static_assert(array_tools::equal(function_composition<N>(value(1), value(1)), id), "");
		//static_assert(array_tools::equal(function_composition<N>(value(2), value(2)), id), "");
	}
#pragma endregion

	// Transform (shuffle) cube c according to the change as provided by cube ci
	export template <int N>
	[[nodiscard]] constexpr CubeI<N> transform(const CubeI<N>& c, const CubeI<N>& ci)
	{
		return apply<N>(c, ci);
	}

	// Transform (shuffle) Boolean function bf according to the change as provided by cube
	export template <int N>
	[[nodiscard]] constexpr BF transform(const BF bf, const CubeI<N>& cube) noexcept
	{
		return apply<N>(bf, cube);
	}
}