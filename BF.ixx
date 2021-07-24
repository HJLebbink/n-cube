module;
#include <string>
#include <bitset>
#include <iostream>		// std::cout
#include <sstream>
#include <iomanip>
#include "CubeDef.h"

export module BF;
//import std.core;
import CubeIndex;


namespace cube {
	export using BF = unsigned long long;

	export template <int N>
	std::string to_string_bin(const BF bf)
	{
		return std::bitset<(1 << N)>(bf).to_string();
	}

	export template <int N>
	std::string to_string_bin(const CubeI<N>& cube)
	{
		std::string result = "";
		for (int i = 0; i < cube.size(); ++i)
		{
			result += std::to_string(static_cast<int>(cube[i]));
			if (i < (cube.size() - 1)) {
				result += "";
			}
		}
		return result;
	}

	export template <int N>
	std::string to_string_bin(const std::string& str)
	{
		return "\"" + str + "\"";
	}

	export template <int N>
	std::string to_string_hex(const BF bf)
	{
		constexpr int width = ((N <= 3) ? 2 : ((N == 4) ? 4 : ((N == 5) ? 8 : 16)));
		std::stringstream stream;
		stream << std::uppercase << std::hex << std::setfill('0') << std::setw(width) << bf;
		return stream.str();
	}

	namespace details {
		// count the number of bits set to one.
		export inline constexpr int count_bits(const BF v) noexcept
		{
			int r = 0;
			for (int i = 0; i < 64; ++i) if (((v >> i) & 1) == 1) r++;
			return r;
		}
	}
}
