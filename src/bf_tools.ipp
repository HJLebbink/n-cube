#pragma once

#include <bitset>
#include <set>
#include <map>
#include <vector>
#include <array>

#include <iostream>		// std::cout
#include <iomanip>		// std::setfill
#include <cassert>
#include <algorithm>	// std::find

namespace bf_tools
{
	using BF = unsigned long long;

	namespace details
	{
		constexpr BF bf_used_vars(
			const BF bf,
			const int n_vars)
		{
			BF id1 = -1;
			BF id2 = -1;

			switch (n_vars)
			{
				case 1:
					id1 = (bf >> 1) & 0x1;
					id2 = bf & 0x1;
					return (id1 == id2) ? 0 : 1 << 0;
				case 2:
					id1 = (bf >> 2) & 0x3;
					id2 = bf & 0x3;
					break;
				case 3:
					id1 = (bf >> 4) & 0xF;
					id2 = bf & 0xF;
					break;
				case 4:
					id1 = (bf >> 8) & 0xFF;
					id2 = bf & 0xFF;
					break;
				case 5:
					id1 = (bf >> 16) & 0xFFFF;
					id2 = bf & 0xFFFF;
					break;
				case 6:
					id1 = (bf >> 32) & 0xFFFFFFFF;
					id2 = bf & 0xFFFFFFFF;
					break;
				default:
					std::cout << "ERROR: bf_used_vars not implemented yet." << std::endl;
					getchar();
					return 0;
			}

			const BF vars1 = bf_used_vars(id1, n_vars - 1);
			const BF vars2 = bf_used_vars(id2, n_vars - 1);
			const BF result = (id1 == id2) ? (vars1 | vars2) : (vars1 | vars2 | (1ULL << (n_vars - 1)));
			//std::cout << "bf_used_vars(bf = " << tools::to_string_binary((unsigned int)bf) << ", n_vars=" << n_vars << ")" << std::endl;
			//std::cout << "id1=" << tools::to_string_binary((unsigned char)id1) << "; id2=" << tools::to_string_binary((unsigned char)id2) << "; vars1=" << tools::to_string_binary((unsigned char)vars1) << "; vars2=" << tools::to_string_binary((unsigned char)vars2) << "; result="<< tools::to_string_binary((unsigned char)result) << std::endl;
			return result;
		}

		// count the number of bits set to one.
		constexpr int count_bits(const BF v)
		{
			int r = 0;
			for (int i = 0; i < 64; ++i) if (((v >> i) & 1) == 1) r++;
			return r;
		}
	}

	// get the Boolean Function cardinality.
	constexpr int bf_cardinality(
		const BF bf,
		const int n_vars)
	{
		return details::count_bits(details::bf_used_vars(bf, n_vars));
	}
}
