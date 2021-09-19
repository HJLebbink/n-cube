module;
#include <iostream>

export module cube.gamma;
import cube.type;
import cube.reflect;
import cube.rotate;
import cube.transform;


namespace cube {

	// gamma transformations is the third BF transform (first=reflexifity; second=rotation)
	export template <int N> constexpr [[nodiscard]]
	CubeI<N> gamma(int d0, int d1, int d2)
	{
		if constexpr (N == 3) {
			constexpr CubeI<N> g012 = CubeI<N>{ 1, 3, 0, 7, 6, 4, 2, 5 };
			constexpr CubeI<N> g102 = CubeI<N>{ 2, 0, 3, 7, 5, 1, 4, 6 };
			constexpr CubeI<N> g021 = CubeI<N>{ 1, 5, 6, 2, 0, 7, 4, 3 };
			constexpr CubeI<N> g120 = CubeI<N>{ 2, 5, 6, 1, 0, 4, 7, 3 };
			constexpr CubeI<N> g201 = CubeI<N>{ 4, 0, 3, 1, 5, 7, 2, 6 };
			constexpr CubeI<N> g210 = CubeI<N>{ 4, 3, 0, 2, 6, 1, 7, 5 };

			if ((d0 == 0) && (d1 == 1) && (d2 == 2)) {
				return g012;
			}
			else if ((d0 == 1) && (d1 == 0) && (d2 == 2)) {
				return g102;
			}
			else if ((d0 == 0) && (d1 == 2) && (d2 == 1)) {
				return g021;
			}
			else if ((d0 == 1) && (d1 == 2) && (d2 == 0)) {
				return g120;
			}
			else if ((d0 == 2) && (d1 == 0) && (d2 == 1)) {
				return g201;
			}
			else if ((d0 == 2) && (d1 == 1) && (d2 == 0)) {
				return g210;
			}
		}
		else {
			static_assert(false);
		}
		return init_cubeI<N>();
	}

	template <int N, int D0, int D1, int D2> consteval
		void round_trip() {
		constexpr CubeI<N> tr = gamma<N>(D0, D1, D2);
		constexpr CubeI<N> g0 = init_cubeI<N>();
		constexpr CubeI<N> g1 = transform<N>(g0, tr);
		constexpr CubeI<N> g2 = transform<N>(g1, tr);
		constexpr CubeI<N> g3 = transform<N>(g2, tr);
		constexpr CubeI<N> g4 = transform<N>(g3, tr);
		constexpr CubeI<N> g5 = transform<N>(g4, tr);
		constexpr CubeI<N> g6 = transform<N>(g5, tr);
		constexpr CubeI<N> g7 = transform<N>(g6, tr);
		constexpr CubeI<N> g8 = transform<N>(g7, tr);
		//		static_assert(array_tools::equal(g0, g8), "round trip error");
	}


	export void test_gamma() {
		constexpr int N = 3;
		{
			round_trip<N, 0, 1, 2>();
			round_trip<N, 1, 0, 2>();
			round_trip<N, 0, 2, 1>();
			round_trip<N, 1, 2, 0>();
			round_trip<N, 2, 0, 1>();
			round_trip<N, 2, 1, 0>();
		}
		{
			constexpr CubeI<N> tr = gamma<N>(0, 2, 1);
			CubeI<N> g = init_cubeI<N>();
			for (int i = 0; i < 9; ++i) {
				std::cout << i << ": " << to_string<N>(g) << std::endl;
				g = transform<N>(g, tr);
			}
		}
	}
}