#pragma once

#include <bitset>
#include <set>
#include <map>
#include <vector>
#include <array>
#include <random>

#include <iostream>		// std::cout
#include <iomanip>		// std::setfill
#include <cassert>
#include <algorithm>	// std::find

namespace bf_tools
{
	using BF = unsigned long long;

	namespace details
	{
		template <size_t _Nvars>
		[[nodiscard]] std::bitset<(1ull << _Nvars)> random_bf() {
			constexpr size_t Nbits = 1ull << _Nvars;
			std::bitset<Nbits> result;

			if constexpr (Nbits < 8) {
				const int rand = std::rand();
				for (int k = 0; k < Nbits; ++k) result.set(k, ((rand >> k) & 1) == 1);
			}
			else
			{
				for (int j = 0; j < Nbits / 8; ++j) {
					const int rand = std::rand();
					for (int k = 0; k < 8; ++k) result.set((static_cast<long long>(j) * 8) + k, ((rand >> k) & 1) == 1);
				}
			}
			return result;
		}

		constexpr BF bf_used_vars(
			const BF bf,
			const int n_vars)
		{
			BF id1 = static_cast<BF>(-1);
			BF id2 = static_cast<BF>(-1);

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
					static_cast<void>(getchar());
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
		template<size_t S>
		constexpr std::bitset<S / 2> lower(const std::bitset<S>& a) {
			constexpr size_t S2 = S / 2;
			std::bitset<S2> result;
			for (int i = 0; i < S2; ++i) result.set(i, a.test(i));
			return result;
		}

		template<size_t S>
		constexpr std::bitset<S / 2> upper(const std::bitset<S>& a) {
			constexpr size_t S2 = S / 2;
			std::bitset<S2> result;
			for (int i = 0; i < S2; ++i) result.set(i, a.test(i + S2));
			return result;
		}

		template<size_t S>
		constexpr unsigned long long bf_used_vars(
			const std::bitset<S>& bf)
		{
			if constexpr (S == 2)
			{
				return (bf.test(1) == bf.test(0)) ? 0 : 1;
			}
			else
			{
				const auto id1 = upper(bf);
				const auto id2 = lower(bf);

				const auto vars1 = bf_used_vars(id1);
				const auto vars2 = bf_used_vars(id2);

				const unsigned long long result = (id1 == id2) ? (vars1 | vars2) : (vars1 | vars2 | (S >> 1));
				//std::cout << "bf_used_vars(bf = " << tools::to_string_binary((unsigned int)bf) << ", n_vars=" << n_vars << ")" << std::endl;
				//std::cout << "id1=" << tools::to_string_binary((unsigned char)id1) << "; id2=" << tools::to_string_binary((unsigned char)id2) << "; vars1=" << tools::to_string_binary((unsigned char)vars1) << "; vars2=" << tools::to_string_binary((unsigned char)vars2) << "; result="<< tools::to_string_binary((unsigned char)result) << std::endl;
				return result;
			}
		}

		constexpr unsigned long long bf_used_vars_2(
			const BF bf,
			const int n_vars)
		{
			BF id1 = static_cast<BF>(-1);
			BF id2 = static_cast<BF>(-1);

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
				static_cast<void>(getchar());
				return 0;
			}

			const unsigned long long vars1 = bf_used_vars_2(id1, n_vars - 1);
			const unsigned long long vars2 = bf_used_vars_2(id2, n_vars - 1);
			const unsigned long long result = (id1 == id2) ? (vars1 | vars2) : (vars1 | vars2 | (1ULL << (n_vars - 1)));
			//std::cout << "bf_used_vars(bf = " << tools::to_string_binary((unsigned int)bf) << ", n_vars=" << n_vars << ")" << std::endl;
			//std::cout << "id1=" << tools::to_string_binary((unsigned char)id1) << "; id2=" << tools::to_string_binary((unsigned char)id2) << "; vars1=" << tools::to_string_binary((unsigned char)vars1) << "; vars2=" << tools::to_string_binary((unsigned char)vars2) << "; result="<< tools::to_string_binary((unsigned char)result) << std::endl;
			return result;
		}

		template <class T1, class T2>
		[[nodiscard]] std::tuple<std::vector<T1>, std::vector<T2>> remove_redundant_vars(
			const unsigned long long bf,
			const std::vector<T1>& content,
			const std::vector<T2>& descr)
		{
			const int n_vars = static_cast<int>(content.size());
			const unsigned long long used_vars = bf_tools::details::bf_used_vars(bf, n_vars);
			const int n_used_vars = bf_tools::details::count_bits(used_vars);

			if (n_used_vars < n_vars)
			{
				std::vector<T1> content2;
				std::vector<T2> descr2;

				for (int i = 0; i < n_vars; ++i)
				{
					if (((used_vars >> i) & 1) == 1)
					{
						content2.push_back(content[i]);
						descr2.push_back(descr[i]);
					}
				}
				if (false)
				{
					std::cout << "remove_redundant_vars: bf=" << bf << "; n_vars=" << n_vars << "; n_used_vars=" << n_used_vars << std::endl;
					for (int i = 0; i < n_used_vars; ++i)
					{
						//std::cout << "remove_redundant_vars: " << descr2[i] << " = " << tools::to_string_binary(content2[i]) << std::endl;
					}
				}
				return { content2, descr2 };
			}
			else
			{
				return { content, descr };
			}
		}

		template <class T1, class T2>
		[[nodiscard]] std::tuple<std::vector<T1>, std::vector<T2>> remove_redundant_vars_2(
			const unsigned long long bf,
			const std::vector<T1>& content,
			const std::vector<T2>& descr)
		{
			const int n_vars = static_cast<int>(content.size());

			const unsigned long long used_vars = bf_tools::details::bf_used_vars(bf, n_vars);
			const int n_used_vars = bf_tools::details::count_bits(used_vars);

			if (n_used_vars < n_vars)
			{
				std::vector<T1> content2;
				std::vector<T2> descr2;

				for (int i = 0; i < n_vars; ++i)
				{
					if (((used_vars >> i) & 1) == 1)
					{
						content2.push_back(content[i]);
						descr2.push_back(descr[i]);
					}
				}
				if (true)
				{
					std::cout << "remove_redundant_vars: bf=" << bf << "; n_vars=" << n_vars << "; n_used_vars=" << n_used_vars << std::endl;
					for (int i = 0; i < n_used_vars; ++i)
					{
						std::cout << "remove_redundant_vars: " << descr2[i] << " = " << tools::to_string_binary(content2[i]) << std::endl;
					}
				}
				return { content2, descr2 };
			}
			else
			{
				return { content, descr };
			}
		} // details

	}

	// get a random boolean function for the provided number of variables _Nvars
	template <size_t _Nvars>
	[[nodiscard]] std::bitset<(1ull << _Nvars)> random_bf() {
		return details::random_bf<_Nvars>();
	}

	// get the Boolean Function cardinality.
	[[nodiscard]] constexpr int bf_cardinality(
		const BF bf,
		const int n_vars)
	{
		return details::count_bits(details::bf_used_vars(bf, n_vars));
	}

	// get the Boolean Function cardinality.
	template<size_t S>
	[[nodiscard]] constexpr int bf_cardinality(
		const std::bitset<S> bf,
		const int n_vars)
	{
		return details::count_bits(details::bf_used_vars(bf, n_vars));
	}

	template <size_t S>
	[[nodiscard]] std::bitset<S> create_truth_table_var(int i)
	{
		auto result = std::bitset<S>(); // construct with all false values
		bool b = false;
		int p = 0;

		for (int j = 0; j < (S / (1ull << i)); ++j)
		{
			for (int k = 0; k < (1 << i); ++k, ++p) result.set(p, b);
			b = !b;
		}
		return result;
	}

	// return the number of bits that missmatch, that is, return the number of different bits.
	template <size_t S>
	[[nodiscard]] constexpr int missmatch(const std::bitset<S>& bf1, const std::bitset<S>& bf2) {
		const auto eq = (bf1 ^ bf2);
		return static_cast<int>(eq.count());
	}

	// return the used variables. A set bit at position p indicates that the variable p is used.
	[[nodiscard]] constexpr unsigned long long bf_used_vars(const BF bf, const int n_vars) {
		return details::bf_used_vars(bf, n_vars);
	}

	// return the used variables. A set bit at position p indicates that the variable p is used.
	template <size_t S>
	[[nodiscard]] constexpr unsigned long long bf_used_vars(const std::bitset<S>& bf) {
		return details::bf_used_vars(bf);
	}

	template <class T1, class T2>
	[[nodiscard]] std::tuple<std::vector<T1>, std::vector<T2>> remove_redundant_vars(
		const unsigned long long bf,
		const std::vector<T1>& content,
		const std::vector<T2>& descr)
	{
		return details::remove_redundant_vars(bf, content, descr);
	}

	namespace test
	{
		void remove_redundant_vars_bf4_test() 
		{
			std::vector<std::string> descr = { "x0", "x1", "x2", "x3" };
			std::vector<std::bitset<16>> content = {
				create_truth_table_var<16>(0),
				create_truth_table_var<16>(1),
				create_truth_table_var<16>(2),
				create_truth_table_var<16>(3)
			};
			for (int i = 0; i < content.size(); ++i) {
				std::cout << "i=" << i << ":" << content[i].to_string() << std::endl;
			}
			std::cout << "---------------------------------" << std::endl;


			if (true)
			{
				std::vector<std::tuple<const unsigned long long, int>> test_cases = { { {0x00FF, 1}, {0x00FE, 4} } };

				for (const auto test : test_cases) {
					const auto bf = std::get<0>(test);
					const auto tup = remove_redundant_vars(bf, content, descr);
					const auto c = std::get<0>(tup);

					std::cout << "BF[" << std::bitset<16>(bf).to_string() << "]" << std::endl;
					for (int j = 0; j < c.size(); ++j) {
						std::cout << "j=" << j << ": " << c[j].to_string() << std::endl;
					}
				}
			}
			else 
			{
				constexpr int N = 4;
				constexpr int n_bf = (1 << (1 << N));
				for (int i = 0; i < n_bf; ++i) {
					const auto tup = remove_redundant_vars(i, content, descr);
					const auto c = std::get<0>(tup);

					std::cout << "BF[" << std::bitset<16>(i).to_string() << "]" << std::endl;
					for (int j = 0; j < c.size(); ++j) {
						std::cout << "j=" << j << ": " << c[j].to_string() << std::endl;
					}
				}
			}
		}
		void test_bf_used_vars_1()
		{
			for (int i = 0; i < 1000000; ++i) {
				int r = (std::rand() & 0xFF) + ((std::rand() & 0xFF) << 8) + ((std::rand() & 0xFF) << 16) + ((std::rand() & 0xFF) << 24);

				const unsigned long long result1 = bf_used_vars(r, 5);
				const unsigned long long result2 = bf_used_vars(std::bitset<32>(r));

				if (result1 != result2) std::cout << "r=" << r << "; result1 = " << std::bitset<32>(result1) << "; result2 = " << std::bitset<32>(result2) << std::endl;
			}
		}
		void test_bf_used_vars_2()
		{
			constexpr size_t Nvars = 9;

			for (int i = 0; i < 1000000; ++i) {
				const auto r = random_bf<Nvars>();
				const unsigned long long result2 = bf_used_vars(r);

				if (details::count_bits(result2) < Nvars) {
					std::cout << "bf=" << r << "; used vars = " << std::bitset<Nvars>(result2) << std::endl;
				}
			}
		}
	} // test
}
