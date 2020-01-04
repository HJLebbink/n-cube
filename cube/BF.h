#pragma once
#include <string>
#include <bitset>
#include <iostream>		// std::cout

namespace cube {
	using BF = unsigned long long;
	
	template <int N> 
	std::string to_string_bin(const BF bf)
	{
		switch (N)
		{
			case 1: return std::bitset<2>(bf).to_string();
			case 2: return std::bitset<4>(bf).to_string();
			case 3: return std::bitset<8>(bf).to_string();
			case 4: return std::bitset<16>(bf).to_string();
			case 5: return std::bitset<32>(bf).to_string();
			case 6: return std::bitset<64>(bf).to_string();
			default:
				std::cout << "ERROR: to_string_bin: dim=" << N << " not implemented yet" << std::endl;
				static_cast<void>(getchar());
				return std::to_string(bf);
		}
	}
	template <int N> 
	std::string to_string_hex(const BF bf)
	{
		constexpr int width = ((N <= 3) ? 2 : ((N == 4) ? 4 : ((N == 5) ? 8 : 16)));
		std::stringstream stream;
		stream << std::uppercase << std::hex << std::setfill('0') << std::setw(width) << bf;
		return stream.str();
	}

	namespace details {
		// count the number of bits set to one.
		constexpr int count_bits(const BF v) noexcept
		{
			int r = 0;
			for (int i = 0; i < 64; ++i) if (((v >> i) & 1) == 1) r++;
			return r;
		}
	}
}
