#pragma once
#include <array>

namespace cube {

	template <int DIM> using CubeI = std::array<int, (1 << DIM)>; // char takes 9 hours for save_all_npn_classes<5>

}