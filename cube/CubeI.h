#pragma once
#include <string>
#include <utility> // pair

namespace cube {

	template<int DIM>
	//using CubeI = std::array<unsigned char, (1 << DIM)>; // unsigned char takes 8 hours for save_all_npn_classes<5>
	//using CubeI = std::array<char, (1 << DIM)>; // char takes 8 hours for save_all_npn_classes<5>
	using CubeI = std::array<int, (1 << DIM)>; // char takes 9 hours for save_all_npn_classes<5>

	template <int N>
	constexpr CubeI<N> init_cubeI() noexcept
	{
		constexpr int S = 1 << N;
		return array_tools::create_index_array<CubeI<N>::value_type, S>();
	}

	template <int N> 
	std::string to_string(const CubeI<N>& c)
	{
		std::string r = "";

		if (false)
		{
			if constexpr (N == 3)
			{
				r += "210:\n--------\n";

				//print cube as truth-table
				for (int i = 0; i < 8; ++i)
				{
					r += std::bitset<3>(i).to_string() + ": ";
					for (int j = 2; j >= 0; --j)
					{
						const bool bit = (((c[i] >> j) & 1) == 1);
						r += (bit) ? "1" : "0";
					}
					r += "\n";
				}
			}
			else
			{
				r += "TODO\n";
			}
		}

		if (true)
		{
			const int size = static_cast<int>(c.size());

			const std::string sep = "_";
			for (int i = 0; i < size; ++i)
			{
				r += std::to_string(c[i]);
				if (i < (size - 1)) r += sep;
			}
		}
		return r;
	}


	template <int N> 
	std::string to_string(const std::pair<CubeI<N>, std::string>& c)
	{
		return to_string<N>(std::get<0>(c)) + " " + std::get<1>(c);
	}
}