#pragma once

namespace n_cube {
	template<int DIM>
	using CubeI = std::array<int, ((1 << DIM))>;


	template <int N>
	constexpr CubeI<N> init_cubeI() noexcept
	{
		constexpr int S = 1 << N;
		return array_tools::create_index_array<int, S>();
	}
}