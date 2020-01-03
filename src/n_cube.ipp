#pragma once
#include <string>
#include <array>
#include <set>
#include <iostream>		// std::cout
#include <utility>
#include <tuple>
#include <numeric>
#include <intrin.h>
#include <mutex>
#include <omp.h>
#include <atomic>
#include <fstream>
#include <time.h>
#include <filesystem>


#include "Cube.h"
#include "bf_tools.ipp"
#include "array_tools.h"


#define INOUT

namespace bf {
	constexpr unsigned long long cardinality_npn_class(int n) {
		switch (n) {
		case 1: return 2;
		case 2: return 4;
		case 3: return 14;
		case 4: return 222;
		case 5: return 616126;
		case 6: return 200253952537184;
		default: return 0;
		}
	}
}


namespace prime {

	namespace {
		constexpr int MAXN = 10000;
		int spf[MAXN];
		bool spf_filled = false;

		void sieve()
		{
			spf[1] = 1;
			for (int i = 2; i < MAXN; i++)

				// marking smallest prime factor for every 
				// number to be itself. 
				spf[i] = i;

			// separately marking spf for every even 
			// number as 2 
			for (int i = 4; i < MAXN; i += 2) {
				spf[i] = 2;
			}
			for (int i = 3; (i * i) < MAXN; i++)
			{
				// checking if i is prime 
				if (spf[i] == i)
				{
					// marking SPF for all numbers divisible by i 
					for (int j = i * i; j < MAXN; j += i) {
						// marking spf[j] if it is not  
						// previously marked 
						if (spf[j] == j) {
							spf[j] = i;
						}
					}
				}
			}
		}

	}
	// A O(log n) function returning primefactorization 
	// by dividing by smallest prime factor at every step 
	std::vector<int> get_factorization(int x)
	{
		if (!spf_filled) {
			sieve();
			spf_filled = true;
		}
		std::vector<int> ret;
		while (x != 1)
		{
			ret.push_back(spf[x]);
			x = x / spf[x];
		}
		return ret;
	}

	std::string vector_to_string(const std::vector<int>& a) {
		std::stringstream result;
		const int size = static_cast<int>(a.size());

		for (const int p : {2, 3, 5, 7, 11, 13}) {
			int count = 0;
			for (int i = 0; i < size; ++i) {
				if (a[i] == p) {
					count++;
				}
			}
			if (count > 0) {
				result << p << "^" << count << " + ";
			}
		}
		return result.str().substr(0, result.str().length() - 3);
	}
}

namespace n_cube
{
	template<int DIM>
	using CubeI = std::array<int, ((1 << DIM))>;

	template<int DIM>
	using CubeMap = std::map<CubeI<DIM>, std::string>;

	template<int DIM>
	using Transformations = std::vector<std::pair<CubeI<DIM>, std::string>>;

	using Cycle = std::array<int, 4>;

	#pragma endregion

	#pragma region to_string
	template <int N> std::string to_string(const CubeI<N>& c)
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
	template <int N> std::string to_string(const std::pair<CubeI<N>, std::string>& c)
	{
		return to_string<N>(std::get<0>(c)) + " " + std::get<1>(c);
	}
	template <int N> std::string to_string_bin(const BF bf)
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
	template <int N> std::string to_string_hex(const BF bf)
	{
		constexpr int width = ((N <= 3) ? 2 : ((N == 4) ? 4 : ((N == 5) ? 8 : 16)));
		std::stringstream stream;
		stream << std::uppercase << std::hex << std::setfill('0') << std::setw(width) << bf;
		return stream.str();
	}
	#pragma endregion

	namespace details
	{
		// count the number of bits set to one.
		constexpr int count_bits(const BF v) noexcept
		{
			int r = 0;
			for (int i = 0; i < 64; ++i) if (((v >> i) & 1) == 1) r++;
			return r;
		}

		template <int N>
		constexpr CubeI<N> init_cubeI() noexcept
		{
			constexpr int S = 1 << N;
			return array_tools::create_index_array<int, S>();
		}

#pragma region apply
		template <int N> constexpr CubeI<N> apply(const CubeI<N>& c, const CubeI<N>& ci) {
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
			return CubeI<N>();
		}

		template <int N> constexpr Cube<N> apply(const Cube<N>& c, const CubeI<N>& ci) {
			if constexpr (N == 1) {
				return Cube<N>{ std::array<bool, (1 << N)>{c[ci[0]], c[ci[1]] }};
			}
			if constexpr (N == 2) {
				return Cube<N>{ std::array<bool, (1 << N)>{c[ci[0]], c[ci[1]], c[ci[2]], c[ci[3]] }};
			}
			if constexpr (N == 3) {
				return Cube<N>{ std::array<bool, (1 << N)>{c[ci[0]], c[ci[1]], c[ci[2]], c[ci[3]], c[ci[4]], c[ci[5]], c[ci[6]], c[ci[7]] }};
			}
			if constexpr (N == 4) {
				return Cube<N>{ std::array<bool, (1 << N)>{
					c[ci[0]], c[ci[1]], c[ci[2]], c[ci[3]], c[ci[4]], c[ci[5]], c[ci[6]], c[ci[7]],
						c[ci[8]], c[ci[9]], c[ci[10]], c[ci[11]], c[ci[12]], c[ci[13]], c[ci[14]], c[ci[15]]
				}};
			}
			if constexpr (N == 5) {
				return Cube<N>{ std::array<bool, (1 << N)>{
					c[ci[0]], c[ci[1]], c[ci[2]], c[ci[3]], c[ci[4]], c[ci[5]], c[ci[6]], c[ci[7]],
						c[ci[8]], c[ci[9]], c[ci[10]], c[ci[11]], c[ci[12]], c[ci[13]], c[ci[14]], c[ci[15]],
						c[ci[16]], c[ci[17]], c[ci[18]], c[ci[19]], c[ci[20]], c[ci[21]], c[ci[22]], c[ci[23]],
						c[ci[24]], c[ci[25]], c[ci[26]], c[ci[27]], c[ci[28]], c[ci[29]], c[ci[30]], c[ci[31]]
				}};
			}
			if constexpr (N == 6) {
				return Cube<N>{ std::array<bool, (1 << N)>{
					c[ci[0]], c[ci[1]], c[ci[2]], c[ci[3]], c[ci[4]], c[ci[5]], c[ci[6]], c[ci[7]],
						c[ci[8]], c[ci[9]], c[ci[10]], c[ci[11]], c[ci[12]], c[ci[13]], c[ci[14]], c[ci[15]],
						c[ci[16]], c[ci[17]], c[ci[18]], c[ci[19]], c[ci[20]], c[ci[21]], c[ci[22]], c[ci[23]],
						c[ci[24]], c[ci[25]], c[ci[26]], c[ci[27]], c[ci[28]], c[ci[29]], c[ci[30]], c[ci[31]],
						c[ci[32]], c[ci[33]], c[ci[34]], c[ci[35]], c[ci[36]], c[ci[37]], c[ci[38]], c[ci[39]],
						c[ci[40]], c[ci[41]], c[ci[42]], c[ci[43]], c[ci[44]], c[ci[45]], c[ci[46]], c[ci[47]],
						c[ci[48]], c[ci[49]], c[ci[50]], c[ci[51]], c[ci[52]], c[ci[53]], c[ci[54]], c[ci[55]],
						c[ci[56]], c[ci[57]], c[ci[58]], c[ci[59]], c[ci[60]], c[ci[61]], c[ci[62]], c[ci[63]]
				}};
			}
		}
#pragma endregion


#pragma region functional composition

		template <int N> constexpr CubeI<N> function_composition(const CubeI<N>& a) noexcept {
			return a;
		}
		template <int N> constexpr CubeI<N> function_composition(const CubeI<N>& a0, const CubeI<N>& a1) noexcept {
			return apply<N>(a0, a1);
		}
		template <int N> constexpr CubeI<N> function_composition(const CubeI<N>& a0, const CubeI<N>& a1, const CubeI<N>& a2) noexcept {
			return apply<N>(a0, apply<N>(a1, a2));
		}
		template <int N> constexpr CubeI<N> function_composition(const CubeI<N>& a0, const CubeI<N>& a1, const CubeI<N>& a2, const CubeI<N>& a3) noexcept {
			return apply<N>(apply<N>(a0, a1), apply<N>(a2, a3));
		}

		template <int N, int M> constexpr CubeI<N> function_composition(const std::array<CubeI<N>, M>& a) noexcept {
			if constexpr (M == 1) {
				return function_composition(a[0]);
			}
			if constexpr (M == 2) {
				return function_composition(a[0], a[1]);
			}
			if constexpr (M == 3) {
				return function_composition(a[0], a[1], a[2]);
			}
			if constexpr (M == 4) {
				return function_composition(a[0], a[1], a[2], a[3]);
			}
			static_assert(fail);
		}

		static void test_function_composition() {

			constexpr int N = 2;

			constexpr auto id = init_cubeI<(N)>();
			static_assert(array_tools::equal(function_composition<N>(id, id), id), "");
			//static_assert(array_tools::equal(function_composition<N>(value(1), value(1)), id), "");
			//static_assert(array_tools::equal(function_composition<N>(value(2), value(2)), id), "");
		}
		#pragma endregion

	
		#pragma region reflect
		template <int N, int D> constexpr CubeI<N> lift_reflect() noexcept
		{
			static_assert(D < N, "");
			constexpr auto reflect_embedded_cube = reflect<(N - 1)>::value(D);
			return array_tools::concat(reflect_embedded_cube, array_tools::add(reflect_embedded_cube, 1 << (N - 1)));
		}
		template <int N> struct reflect
		{
			static constexpr CubeI<N> value(const Cube<N>& cube) noexcept
			{
				std::cout << "ERROR: reflect<N>: dim=" << N << " not implemented yet" << std::endl;
				getchar();
				return CubeI<N>();
			}
		};
		template <> struct reflect<1>
		{
			static constexpr int N = 1;
			static constexpr CubeI<N> value(const int dim) noexcept
			{
				if (dim == 0)
				{
					return CubeI<N>{ 1, 0 };
				}
				std::cout << "ERROR: reflect<" << N << ">::value: dim=" << dim << " does not exist." << std::endl;
				return CubeI<N>();
			}
			static constexpr CubeI<N> invalid(const int /*dim*/, const int /*var*/) noexcept
			{
				//std::cout << "ERROR: reflect<" << N << ">::invalid: dim=" << dim << " does not exist." << std::endl;
				return CubeI<N>();
			}
		};
		template <> struct reflect<2>
		{
			static constexpr int N = 2;
			static constexpr CubeI<N> value(const int dim) noexcept
			{
				constexpr auto r0 = lift_reflect<N, 0>();
				constexpr auto id = init_cubeI<(N - 1)>();
				constexpr auto r1 = array_tools::concat(array_tools::add(id, 1 << (N - 1)), id);

				switch (dim)
				{
					case 0: return r0; //CubeI<N>{ 1, 0, 3, 2 };
					case 1: return r1; //CubeI<N>{ 2, 3, 0, 1 };
				}
				std::cout << "ERROR: reflect<" << N << ">::value: dim=" << dim << " does not exist." << std::endl;
				return CubeI<N>();
			}
			// reflect over the provided dimension, yet keep var unchanged.
			static constexpr CubeI<N> invalid(const int dim, const int var) noexcept
			{
				if ((dim == 0) && (var == 0)) return CubeI<N>{1, 0, 2, 3};
				if ((dim == 0) && (var == 1)) return CubeI<N>{0, 1, 3, 2};
				if ((dim == 1) && (var == 0)) return CubeI<N>{2, 1, 0, 3};
				if ((dim == 1) && (var == 1)) return CubeI<N>{0, 3, 2, 1};

				std::cout << "ERROR: reflect<" << N << ">::invalid: dim=" << dim << " does not exist." << std::endl;
				return CubeI<N>();
			}
		};
		template <> struct reflect<3>
		{
			static constexpr int N = 3;
			static constexpr CubeI<N> value(const int dim) noexcept
			{
				constexpr auto r0 = lift_reflect<N, 0>();
				constexpr auto r1 = lift_reflect<N, 1>();
				constexpr auto id = init_cubeI<(N - 1)>();
				constexpr auto r2 = array_tools::concat(array_tools::add(id, 1 << (N - 1)), id);

				switch (dim)
				{
					case 0: return r0; //CubeI<N>{ 1, 0, 3, 2, 5, 4, 7, 6 };
					case 1: return r1; //CubeI<N>{ 2, 3, 0, 1, 6, 7, 4, 5 };
					case 2: return r2; //CubeI<N>{ 4, 5, 6, 7, 0, 1, 2, 3 };
				}
				std::cout << "ERROR: reflect<" << N << ">: dim=" << dim << " does not exist." << std::endl;
				return CubeI<N>();
			}
			// reflect over the provided dimension, yet keep var unchanged.
			static constexpr CubeI<N> invalid(const int dim, const int var) noexcept
			{
				if ((dim == 0) && (var == 0)) return CubeI<N>{1, 0, 2, 3};
				if ((dim == 0) && (var == 1)) return CubeI<N>{0, 1, 3, 2};
				if ((dim == 0) && (var == 2)) return CubeI<N>{0, 1, 3, 2};

				if ((dim == 1) && (var == 0)) return CubeI<N>{2, 1, 0, 3};
				if ((dim == 1) && (var == 1)) return CubeI<N>{0, 3, 2, 1};
				if ((dim == 1) && (var == 2)) return CubeI<N>{0, 3, 2, 1};

				if ((dim == 2) && (var == 0)) return CubeI<N>{2, 1, 0, 3};
				if ((dim == 2) && (var == 1)) return CubeI<N>{0, 3, 2, 1};
				if ((dim == 2) && (var == 2)) return CubeI<N>{0, 3, 2, 1};


				std::cout << "ERROR: reflect<" << N << ">::invalid: dim=" << dim << " does not exist." << std::endl;
				return CubeI<N>();
			}
			static void test()
			{
				constexpr auto id = init_cubeI<(N)>();
				static_assert(array_tools::equal(function_composition<N>(value(0), value(0)), id), "");
				static_assert(array_tools::equal(function_composition<N>(value(1), value(1)), id), "");
				static_assert(array_tools::equal(function_composition<N>(value(2), value(2)), id), "");
			}
		};
		template <> struct reflect<4>
		{
			static constexpr int N = 4;
			template <int D> static constexpr CubeI<N> cube4_reflect_indices()
			{
				static_assert(D >= 0, "cube_reflect_4: D must be positive.");
				static_assert(D < N, "cube_reflect_4: D1 must be smaller than 4");

				constexpr const auto a0 = index_split_1<D, 0>();
				constexpr const auto a1 = index_split_1<D, 1>();
				constexpr const auto a2 = index_split_1<D, 2>();
				constexpr const auto a3 = index_split_1<D, 3>();
				constexpr const auto a4 = index_split_1<D, 4>();
				constexpr const auto a5 = index_split_1<D, 5>();
				constexpr const auto a6 = index_split_1<D, 6>();
				constexpr const auto a7 = index_split_1<D, 7>();

				constexpr const auto b0 = array_tools::concat(a0, a1, a2, a3);
				constexpr const auto b1 = array_tools::concat(a4, a5, a6, a7);
				return array_tools::concat(b0, b1);
			}

			static constexpr CubeI<N> value(const int dim) noexcept
			{
				constexpr auto r0 = lift_reflect<N, 0>();
				constexpr auto r1 = lift_reflect<N, 1>();
				constexpr auto r2 = lift_reflect<N, 2>();
				constexpr auto id = init_cubeI<(N - 1)>();
				constexpr auto r3 = array_tools::concat(array_tools::add(id, 1 << (N - 1)), id);

				switch (dim)
				{
					case 0: return r0; //CubeI<N>{ 1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14 };
					case 1: return r1; //CubeI<N>{ 2, 3, 0, 1, 6, 7, 4, 5, 10, 11, 8, 9, 14, 15, 12, 13 };
					case 2: return r2; //CubeI<N>{ 4, 5, 6, 7, 0, 1, 2, 3, 12, 13, 14, 15, 8, 9, 10, 11 };
					case 3: return r3; //CubeI<N>{ 8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7 };
				}
				std::cout << "ERROR: reflect<" << N << ">: dim=" << dim << " does not exist." << std::endl;
				return CubeI<N>();
			}
			static void test()
			{
				constexpr auto id = init_cubeI<(N)>();
				static_assert(array_tools::equal(function_composition<N>(value(0), value(0)), id), "");
				static_assert(array_tools::equal(function_composition<N>(value(1), value(1)), id), "");
				static_assert(array_tools::equal(function_composition<N>(value(2), value(2)), id), "");
				static_assert(array_tools::equal(function_composition<N>(value(3), value(3)), id), "");
			}
		};
		template <> struct reflect<5>
		{
			static constexpr int N = 5;
			static constexpr CubeI<N> value(const int dim) noexcept
			{
				constexpr auto r0 = lift_reflect<N, 0>();
				constexpr auto r1 = lift_reflect<N, 1>();
				constexpr auto r2 = lift_reflect<N, 2>();
				constexpr auto r3 = lift_reflect<N, 3>();
				constexpr auto id = init_cubeI<(N - 1)>();
				constexpr auto r4 = array_tools::concat(array_tools::add(id, 1 << (N - 1)), id);

				switch (dim)
				{
					case 0: return r0;
					case 1: return r1;
					case 2: return r2;
					case 3: return r3;
					case 4: return r4;
				}
				std::cout << "ERROR: reflect<" << N << ">: dim=" << dim << " does not exist." << std::endl;
				return CubeI<N>();
			}
			static void test()
			{
				constexpr auto id = init_cubeI<(N)>();
				static_assert(array_tools::equal(function_composition<N>(value(0), value(0)), id), "");
				static_assert(array_tools::equal(function_composition<N>(value(1), value(1)), id), "");
				static_assert(array_tools::equal(function_composition<N>(value(2), value(2)), id), "");
				static_assert(array_tools::equal(function_composition<N>(value(3), value(3)), id), "");
				static_assert(array_tools::equal(function_composition<N>(value(4), value(4)), id), "");
			}
		};
		template <> struct reflect<6>
		{
			static constexpr int N = 6;
			static constexpr CubeI<N> value(const int dim) noexcept
			{
				constexpr auto r0 = lift_reflect<N, 0>();
				constexpr auto r1 = lift_reflect<N, 1>();
				constexpr auto r2 = lift_reflect<N, 2>();
				constexpr auto r3 = lift_reflect<N, 3>();
				constexpr auto r4 = lift_reflect<N, 4>();
				constexpr auto id = init_cubeI<(N - 1)>();
				constexpr auto r5 = array_tools::concat(array_tools::add(id, 1 << (N - 1)), id);

				switch (dim)
				{
					case 0: return r0;
					case 1: return r1;
					case 2: return r2;
					case 3: return r3;
					case 4: return r4;
					case 5: return r5;
				}
				std::cout << "ERROR: reflect<" << N << ">: dim=" << dim << " does not exist." << std::endl;
				return CubeI<N>();
			}
			static void test()
			{
				constexpr auto id = init_cubeI<(N)>();
				static_assert(array_tools::equal(function_composition<N>(value(0), value(0)), id), "");
				static_assert(array_tools::equal(function_composition<N>(value(1), value(1)), id), "");
				static_assert(array_tools::equal(function_composition<N>(value(2), value(2)), id), "");
				static_assert(array_tools::equal(function_composition<N>(value(3), value(3)), id), "");
				static_assert(array_tools::equal(function_composition<N>(value(4), value(4)), id), "");
				static_assert(array_tools::equal(function_composition<N>(value(5), value(5)), id), "");
			}
		};
		/*
		template <> struct reflect<7>
		{
			static constexpr int N = 7;
			static constexpr CubeI<N> value(const int dim)
			{
				constexpr auto r0 = lift_reflect<N, 0>();
				constexpr auto r1 = lift_reflect<N, 1>();
				constexpr auto r2 = lift_reflect<N, 2>();
				constexpr auto r3 = lift_reflect<N, 3>();
				constexpr auto r4 = lift_reflect<N, 4>();
				constexpr auto r5 = lift_reflect<N, 5>();
				constexpr auto id = init_cubeI<(N - 1)>();
				constexpr auto r6 = array_tools::concat(array_tools::add(id, 1 << (N - 1)), id);

				switch (dim)
				{
					case 0: return r0;
					case 1: return r1;
					case 2: return r2;
					case 3: return r3;
					case 4: return r4;
					case 5: return r5;
					case 6: return r6;
				}
				std::cout << "ERROR: reflect<" << N << ">: dim=" << dim << " does not exist." << std::endl;
				return CubeI<N>();
			}
			static void test()
			{
				constexpr auto id = init_cubeI<(N)>();
				static_assert(array_tools::equal(function_composition<N>(value(0), value(0)), id), "");
				static_assert(array_tools::equal(function_composition<N>(value(1), value(1)), id), "");
				static_assert(array_tools::equal(function_composition<N>(value(2), value(2)), id), "");
				static_assert(array_tools::equal(function_composition<N>(value(3), value(3)), id), "");
				static_assert(array_tools::equal(function_composition<N>(value(4), value(4)), id), "");
				static_assert(array_tools::equal(function_composition<N>(value(5), value(5)), id, "");
				static_assert(array_tools::equal(function_composition<N>(value(6), value(6)), id), "");
			}
		};
		*/
		#pragma endregion


		#pragma region rotate
		constexpr Cycle create_cycle(const int d1, const int d2) noexcept
		{
			return Cycle{ (0 << d1) | (0 << d2), (1 << d1) | (0 << d2), (1 << d1) | (1 << d2), (0 << d1) | (1 << d2) };
		}
		template <int M> constexpr int find_next_in_cycle(const std::array<Cycle, M>& cycles, const int i) noexcept
		{
			for (int j = 0; j < M; ++j)
			{
				const Cycle& c = cycles[j];
				if (c[0] == i) return c[1];
				if (c[1] == i) return c[2];
				if (c[2] == i) return c[3];
				if (c[3] == i) return c[0];
			}
			return -1;
		}

		template <int N> struct rotate
		{
			static constexpr CubeI<N> value(const int dim1, const int dim2) noexcept
			{
				std::cout << "ERROR: rotate<N>: N=" << N << " not implemented yet" << std::endl;
				getchar();
				return CubeI<N>();
			}
		};
		template <> struct rotate<2>
		{
			static constexpr int N = 2;

			static constexpr CubeI<2> cycles_2_cube(const std::array<Cycle, 1>& cycles) noexcept
			{
				return CubeI<2>{
					find_next_in_cycle(cycles, 0),
						find_next_in_cycle(cycles, 1),
						find_next_in_cycle(cycles, 2),
						find_next_in_cycle(cycles, 3)
				};
			}
			static constexpr CubeI<N> value(const int d1, const int d2) noexcept
			{
				const auto cycle = create_cycle(d1, d2);
				const auto cycles = std::array<Cycle, 1> {cycle};
				return cycles_2_cube(cycles);
			}
			static void test()
			{
				constexpr auto id = init_cubeI<N>();
				constexpr auto a = value(0, 1); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
				constexpr auto b = value(1, 0); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
				static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
				static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
			}
		};
		template <> struct rotate<3>
		{
			static constexpr int N = 3;

			static constexpr std::array<int, (N-2)> fixed_dimensions(const int d1, const int d2) noexcept
			{
				const int d1a = std::min(d1, d2);
				const int d2a = std::max(d1, d2);

				if ((d1a == 0) & (d2a == 1)) return { 2 };
				if ((d1a == 0) & (d2a == 2)) return { 1 };
				if ((d1a == 1) & (d2a == 2)) return { 0 };
				return { -1 };
			}
			static constexpr CubeI<N> cycles_2_cube(const std::array<Cycle, 2>& cycles) noexcept
			{
				return CubeI<N>{
					find_next_in_cycle(cycles, 0),
						find_next_in_cycle(cycles, 1),
						find_next_in_cycle(cycles, 2),
						find_next_in_cycle(cycles, 3),

						find_next_in_cycle(cycles, 4),
						find_next_in_cycle(cycles, 5),
						find_next_in_cycle(cycles, 6),
						find_next_in_cycle(cycles, 7)
				};
			}
			static constexpr CubeI<N> value(const int d1, const int d2) noexcept
			{
				const auto cycle = create_cycle(d1, d2);
				const auto dim_fixed = fixed_dimensions(d1, d2);
				const auto cycles = std::array<Cycle, 2> {
					array_tools::add(cycle, (0 << dim_fixed[0])),
					array_tools::add(cycle, (1 << dim_fixed[0]))
				};
				return cycles_2_cube(cycles);
			}
			static void test()
			{
				constexpr auto id = init_cubeI<N>();
				constexpr auto r01 = value(0, 1);
				constexpr auto r10 = value(1, 0);
				constexpr auto r02 = value(0, 2);
				constexpr auto r20 = value(2, 0);
				constexpr auto r12 = value(1, 2);
				constexpr auto r21 = value(2, 1);

				static_assert(array_tools::equal(function_composition<N>(r01, r01, r01, r01), id), "");
				static_assert(array_tools::equal(function_composition<N>(r10, r10, r10, r10), id), "");
				static_assert(array_tools::equal(function_composition<N>(r02, r02, r02, r02), id), "");
				static_assert(array_tools::equal(function_composition<N>(r20, r20, r20, r20), id), "");
				static_assert(array_tools::equal(function_composition<N>(r12, r12, r12, r12), id), "");
				static_assert(array_tools::equal(function_composition<N>(r21, r21, r21, r21), id), "");

				static_assert(array_tools::equal(function_composition<N>(r01, r10), id), "");
				static_assert(array_tools::equal(function_composition<N>(r02, r20), id), "");
				static_assert(array_tools::equal(function_composition<N>(r12, r21), id), "");

				constexpr auto t01 = function_composition<N>(r01, r01);
				constexpr auto t10 = function_composition<N>(r10, r10);
				constexpr auto t02 = function_composition<N>(r02, r02);
				constexpr auto t20 = function_composition<N>(r20, r20);
				constexpr auto t12 = function_composition<N>(r12, r12);
				constexpr auto t21 = function_composition<N>(r21, r21);

				static_assert(array_tools::equal(t01, t10), "");
				static_assert(array_tools::equal(t02, t20), "");
				static_assert(array_tools::equal(t12, t21), "");
			}
		};
		template <> struct rotate<4>
		{
			static constexpr int N = 4;

			static constexpr std::array<int, (N - 2)> fixed_dimensions(const int d1, const int d2) noexcept
			{
				const int d1a = std::min(d1, d2);
				const int d2a = std::max(d1, d2);

				if ((d1a == 0) & (d2a == 1)) return { 2, 3 };
				if ((d1a == 0) & (d2a == 2)) return { 1, 3 };
				if ((d1a == 0) & (d2a == 3)) return { 1, 2 };
				if ((d1a == 1) & (d2a == 2)) return { 0, 3 };
				if ((d1a == 1) & (d2a == 3)) return { 0, 2 };
				if ((d1a == 2) & (d2a == 3)) return { 0, 1 };

				return { -1, -1 };
			}
			static constexpr CubeI<N> cycles_2_cube(const std::array<Cycle, 4>& cycles) noexcept
			{
				return CubeI<N>{
					find_next_in_cycle(cycles, 0),
						find_next_in_cycle(cycles, 1),
						find_next_in_cycle(cycles, 2),
						find_next_in_cycle(cycles, 3),

						find_next_in_cycle(cycles, 4),
						find_next_in_cycle(cycles, 5),
						find_next_in_cycle(cycles, 6),
						find_next_in_cycle(cycles, 7),

						find_next_in_cycle(cycles, 8),
						find_next_in_cycle(cycles, 9),
						find_next_in_cycle(cycles, 10),
						find_next_in_cycle(cycles, 11),

						find_next_in_cycle(cycles, 12),
						find_next_in_cycle(cycles, 13),
						find_next_in_cycle(cycles, 14),
						find_next_in_cycle(cycles, 15)
				};
			}
			static constexpr CubeI<N> value(const int d1, const int d2)
			{
				const auto cycle = create_cycle(d1, d2);
				const auto dim_fixed = fixed_dimensions(d1, d2);
				const auto cycles = std::array<Cycle, 4> {
					array_tools::add(cycle, (0 << dim_fixed[0]) | (0 << dim_fixed[1])),
					array_tools::add(cycle, (0 << dim_fixed[0]) | (1 << dim_fixed[1])),
					array_tools::add(cycle, (1 << dim_fixed[0]) | (0 << dim_fixed[1])),
					array_tools::add(cycle, (1 << dim_fixed[0]) | (1 << dim_fixed[1]))
				};
				return cycles_2_cube(cycles);
			}
			static void test()
			{
				constexpr auto id = init_cubeI<N>();
				{
					constexpr auto a = value(0, 1); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(1, 0); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
				{
					constexpr auto a = value(0, 2); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(2, 0); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
				{
					constexpr auto a = value(0, 3); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(3, 0); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
				{
					constexpr auto a = value(1, 2); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(2, 1); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
				{
					constexpr auto a = value(1, 3); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(3, 1); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
				{
					constexpr auto a = value(2, 3); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(3, 2); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
			}
		};
		template <> struct rotate<5>
		{
			static constexpr int N = 5;
			
			static constexpr std::array<int, (N - 2)> fixed_dimensions(const int d1, const int d2) noexcept
			{
				const int d1a = std::min(d1, d2);
				const int d2a = std::max(d1, d2);

				if ((d1a == 0) & (d2a == 1)) return { 2, 3, 4 };
				if ((d1a == 0) & (d2a == 2)) return { 1, 3, 4 };
				if ((d1a == 0) & (d2a == 3)) return { 1, 2, 4 };
				if ((d1a == 0) & (d2a == 4)) return { 1, 2, 3 };
				if ((d1a == 1) & (d2a == 2)) return { 0, 3, 4 };
				if ((d1a == 1) & (d2a == 3)) return { 0, 2, 4 };
				if ((d1a == 1) & (d2a == 4)) return { 0, 2, 3 };
				if ((d1a == 2) & (d2a == 3)) return { 0, 1, 4 };
				if ((d1a == 2) & (d2a == 4)) return { 0, 1, 3 };
				if ((d1a == 3) & (d2a == 4)) return { 0, 1, 2 };

				return { -1, -1, -1 };
			}
			static constexpr CubeI<N> cycles_2_cube(const std::array<Cycle, 8>& cycles) noexcept
			{
				return CubeI<N>{
					find_next_in_cycle(cycles, 0),
						find_next_in_cycle(cycles, 1),
						find_next_in_cycle(cycles, 2),
						find_next_in_cycle(cycles, 3),

						find_next_in_cycle(cycles, 4),
						find_next_in_cycle(cycles, 5),
						find_next_in_cycle(cycles, 6),
						find_next_in_cycle(cycles, 7),

						find_next_in_cycle(cycles, 8),
						find_next_in_cycle(cycles, 9),
						find_next_in_cycle(cycles, 10),
						find_next_in_cycle(cycles, 11),

						find_next_in_cycle(cycles, 12),
						find_next_in_cycle(cycles, 13),
						find_next_in_cycle(cycles, 14),
						find_next_in_cycle(cycles, 15),
						find_next_in_cycle(cycles, 16),
						find_next_in_cycle(cycles, 17),
						find_next_in_cycle(cycles, 18),
						find_next_in_cycle(cycles, 19),

						find_next_in_cycle(cycles, 20),
						find_next_in_cycle(cycles, 21),
						find_next_in_cycle(cycles, 22),
						find_next_in_cycle(cycles, 23),

						find_next_in_cycle(cycles, 24),
						find_next_in_cycle(cycles, 25),
						find_next_in_cycle(cycles, 26),
						find_next_in_cycle(cycles, 27),

						find_next_in_cycle(cycles, 28),
						find_next_in_cycle(cycles, 29),
						find_next_in_cycle(cycles, 30),
						find_next_in_cycle(cycles, 31)
				};
			}
			static constexpr CubeI<N> value(const int d1, const int d2) noexcept
			{
				const auto cycle = create_cycle(d1, d2);
				const auto dim_fixed = fixed_dimensions(d1, d2);
				const auto cycles = std::array<Cycle, 8> {
					array_tools::add(cycle, (0 << dim_fixed[0]) | (0 << dim_fixed[1]) | (0 << dim_fixed[2])),
					array_tools::add(cycle, (0 << dim_fixed[0]) | (0 << dim_fixed[1]) | (1 << dim_fixed[2])), 
					array_tools::add(cycle, (0 << dim_fixed[0]) | (1 << dim_fixed[1]) | (0 << dim_fixed[2])),
					array_tools::add(cycle, (0 << dim_fixed[0]) | (1 << dim_fixed[1]) | (1 << dim_fixed[2])),
					array_tools::add(cycle, (1 << dim_fixed[0]) | (0 << dim_fixed[1]) | (0 << dim_fixed[2])),
					array_tools::add(cycle, (1 << dim_fixed[0]) | (0 << dim_fixed[1]) | (1 << dim_fixed[2])),
					array_tools::add(cycle, (1 << dim_fixed[0]) | (1 << dim_fixed[1]) | (0 << dim_fixed[2])),
					array_tools::add(cycle, (1 << dim_fixed[0]) | (1 << dim_fixed[1]) | (1 << dim_fixed[2]))
				};
				return cycles_2_cube(cycles);
			}
			static void test()
			{
				constexpr auto id = init_cubeI<N>();
				{
					constexpr auto a = value(0, 1); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(1, 0); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
				{
					constexpr auto a = value(0, 2); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(2, 0); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
				{
					constexpr auto a = value(0, 3); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(3, 0); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
				{
					constexpr auto a = value(0, 4); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(4, 0); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
				{
					constexpr auto a = value(1, 2); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(2, 1); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
				{
					constexpr auto a = value(1, 3); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(3, 1); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
				{
					constexpr auto a = value(1, 4); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(4, 1); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
				{
					constexpr auto a = value(2, 3); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(3, 2); static_assert(array_tools::equal(function_composition<N>(b, b, b, b),id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
				{
					constexpr auto a = value(2, 4); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(4, 2); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
				{
					constexpr auto a = value(3, 4); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(4, 3); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
			}
		};
		template <> struct rotate<6>
		{
			static constexpr int N = 6;

			static constexpr std::array<int, (N - 2)> fixed_dimensions(const int d1, const int d2) noexcept
			{
				const int d1a = std::min(d1, d2);
				const int d2a = std::max(d1, d2);

				if ((d1a == 0) & (d2a == 1)) return { 2, 3, 4, 5 };
				if ((d1a == 0) & (d2a == 2)) return { 1, 3, 4, 5 };
				if ((d1a == 0) & (d2a == 3)) return { 1, 2, 4, 5 };
				if ((d1a == 0) & (d2a == 4)) return { 1, 2, 3, 5 };
				if ((d1a == 0) & (d2a == 5)) return { 1, 2, 3, 4 };
				if ((d1a == 1) & (d2a == 2)) return { 0, 3, 4, 5 };
				if ((d1a == 1) & (d2a == 3)) return { 0, 2, 4, 5 };
				if ((d1a == 1) & (d2a == 4)) return { 0, 2, 3, 5 };
				if ((d1a == 1) & (d2a == 5)) return { 0, 2, 3, 4 };
				if ((d1a == 2) & (d2a == 3)) return { 0, 1, 4, 5 };
				if ((d1a == 2) & (d2a == 4)) return { 0, 1, 3, 5 };
				if ((d1a == 2) & (d2a == 5)) return { 0, 1, 3, 4 };
				if ((d1a == 3) & (d2a == 4)) return { 0, 1, 2, 5 };
				if ((d1a == 3) & (d2a == 5)) return { 0, 1, 2, 4 };
				if ((d1a == 4) & (d2a == 5)) return { 0, 1, 2, 3 };

				return { -1, -1, -1, -1 };
			}
			static constexpr CubeI<N> cycles_2_cube(const std::array<Cycle, 16>& cycles) noexcept
			{
				return CubeI<N>{
					find_next_in_cycle(cycles, 0),
						find_next_in_cycle(cycles, 1),
						find_next_in_cycle(cycles, 2),
						find_next_in_cycle(cycles, 3),

						find_next_in_cycle(cycles, 4),
						find_next_in_cycle(cycles, 5),
						find_next_in_cycle(cycles, 6),
						find_next_in_cycle(cycles, 7),

						find_next_in_cycle(cycles, 8),
						find_next_in_cycle(cycles, 9),
						find_next_in_cycle(cycles, 10),
						find_next_in_cycle(cycles, 11),

						find_next_in_cycle(cycles, 12),
						find_next_in_cycle(cycles, 13),
						find_next_in_cycle(cycles, 14),
						find_next_in_cycle(cycles, 15),

						find_next_in_cycle(cycles, 16),
						find_next_in_cycle(cycles, 17),
						find_next_in_cycle(cycles, 18),
						find_next_in_cycle(cycles, 19),

						find_next_in_cycle(cycles, 20),
						find_next_in_cycle(cycles, 21),
						find_next_in_cycle(cycles, 22),
						find_next_in_cycle(cycles, 23),

						find_next_in_cycle(cycles, 24),
						find_next_in_cycle(cycles, 25),
						find_next_in_cycle(cycles, 26),
						find_next_in_cycle(cycles, 27),

						find_next_in_cycle(cycles, 28),
						find_next_in_cycle(cycles, 29),
						find_next_in_cycle(cycles, 30),
						find_next_in_cycle(cycles, 31),

						find_next_in_cycle(cycles, 32),
						find_next_in_cycle(cycles, 33),
						find_next_in_cycle(cycles, 34),
						find_next_in_cycle(cycles, 35),

						find_next_in_cycle(cycles, 36),
						find_next_in_cycle(cycles, 37),
						find_next_in_cycle(cycles, 38),
						find_next_in_cycle(cycles, 39),

						find_next_in_cycle(cycles, 40),
						find_next_in_cycle(cycles, 41),
						find_next_in_cycle(cycles, 42),
						find_next_in_cycle(cycles, 43),

						find_next_in_cycle(cycles, 44),
						find_next_in_cycle(cycles, 45),
						find_next_in_cycle(cycles, 46),
						find_next_in_cycle(cycles, 47),

						find_next_in_cycle(cycles, 48),
						find_next_in_cycle(cycles, 49),
						find_next_in_cycle(cycles, 50),
						find_next_in_cycle(cycles, 51),

						find_next_in_cycle(cycles, 52),
						find_next_in_cycle(cycles, 53),
						find_next_in_cycle(cycles, 54),
						find_next_in_cycle(cycles, 55),

						find_next_in_cycle(cycles, 56),
						find_next_in_cycle(cycles, 57),
						find_next_in_cycle(cycles, 58),
						find_next_in_cycle(cycles, 59),

						find_next_in_cycle(cycles, 60),
						find_next_in_cycle(cycles, 61),
						find_next_in_cycle(cycles, 62),
						find_next_in_cycle(cycles, 63)
				};
			}
			static constexpr CubeI<N> value(const int d1, const int d2)
			{
				const auto cycle = create_cycle(d1, d2);
				const auto dim_fixed = fixed_dimensions(d1, d2);
				const auto cycles = std::array<Cycle, 16> {
					array_tools::add(cycle, (0 << dim_fixed[0]) | (0 << dim_fixed[1]) | (0 << dim_fixed[2]) | (0 << dim_fixed[3])),
					array_tools::add(cycle, (0 << dim_fixed[0]) | (0 << dim_fixed[1]) | (0 << dim_fixed[2]) | (1 << dim_fixed[3])),
					array_tools::add(cycle, (0 << dim_fixed[0]) | (0 << dim_fixed[1]) | (1 << dim_fixed[2]) | (0 << dim_fixed[3])),
					array_tools::add(cycle, (0 << dim_fixed[0]) | (0 << dim_fixed[1]) | (1 << dim_fixed[2]) | (1 << dim_fixed[3])),
					array_tools::add(cycle, (0 << dim_fixed[0]) | (1 << dim_fixed[1]) | (0 << dim_fixed[2]) | (0 << dim_fixed[3])),
					array_tools::add(cycle, (0 << dim_fixed[0]) | (1 << dim_fixed[1]) | (0 << dim_fixed[2]) | (1 << dim_fixed[3])),
					array_tools::add(cycle, (0 << dim_fixed[0]) | (1 << dim_fixed[1]) | (1 << dim_fixed[2]) | (0 << dim_fixed[3])),
					array_tools::add(cycle, (0 << dim_fixed[0]) | (1 << dim_fixed[1]) | (1 << dim_fixed[2]) | (1 << dim_fixed[3])),
					array_tools::add(cycle, (1 << dim_fixed[0]) | (0 << dim_fixed[1]) | (0 << dim_fixed[2]) | (0 << dim_fixed[3])),
					array_tools::add(cycle, (1 << dim_fixed[0]) | (0 << dim_fixed[1]) | (0 << dim_fixed[2]) | (1 << dim_fixed[3])),
					array_tools::add(cycle, (1 << dim_fixed[0]) | (0 << dim_fixed[1]) | (1 << dim_fixed[2]) | (0 << dim_fixed[3])),
					array_tools::add(cycle, (1 << dim_fixed[0]) | (0 << dim_fixed[1]) | (1 << dim_fixed[2]) | (1 << dim_fixed[3])),
					array_tools::add(cycle, (1 << dim_fixed[0]) | (1 << dim_fixed[1]) | (0 << dim_fixed[2]) | (0 << dim_fixed[3])),
					array_tools::add(cycle, (1 << dim_fixed[0]) | (1 << dim_fixed[1]) | (0 << dim_fixed[2]) | (1 << dim_fixed[3])),
					array_tools::add(cycle, (1 << dim_fixed[0]) | (1 << dim_fixed[1]) | (1 << dim_fixed[2]) | (0 << dim_fixed[3])),
					array_tools::add(cycle, (1 << dim_fixed[0]) | (1 << dim_fixed[1]) | (1 << dim_fixed[2]) | (1 << dim_fixed[3]))
				};
				return cycles_2_cube(cycles);
			}
			static void test()
			{
				constexpr auto id = init_cubeI<N>();
				{
					constexpr auto a = value(0, 1); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(1, 0); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
				{
					constexpr auto a = value(0, 2); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(2, 0); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
				{
					constexpr auto a = value(0, 3); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(3, 0); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
				{
					constexpr auto a = value(0, 4); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(4, 0); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
				{
					constexpr auto a = value(0, 5); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(5, 0); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
				{
					constexpr auto a = value(1, 2); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(2, 1); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
				{
					constexpr auto a = value(1, 3); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(3, 1); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
				{
					constexpr auto a = value(1, 4); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(4, 1); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
				{
					constexpr auto a = value(1, 5); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(5, 1); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
				{
					constexpr auto a = value(2, 3); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(3, 2); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
				{
					constexpr auto a = value(2, 4); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(4, 2); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
				{
					constexpr auto a = value(2, 5); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(5, 2); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
				{
					constexpr auto a = value(3, 4); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(4, 3); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
				{
					constexpr auto a = value(3, 5); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(5, 3); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
				{
					constexpr auto a = value(4, 5); static_assert(array_tools::equal(function_composition<N>(a, a, a, a), id), "");
					constexpr auto b = value(5, 4); static_assert(array_tools::equal(function_composition<N>(b, b, b, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, b), id), "");
					static_assert(array_tools::equal(function_composition<N>(a, a), function_composition<N>(b, b)), "");
				}
			}
		};
		#pragma endregion

		// forward declaration
		template <int N, bool DESCR> const Transformations<N>& get_transformations_from_cache();

		#pragma region create_transformations

		template <int N> struct create_transformations_struct
		{
			static Transformations<N> value(const std::array<std::string, 2>& descr)
			{
				std::cout << "ERROR: create_transformations: dim=" << N << " not implemented yet" << std::endl;
				getchar();
				return Transformations<N>();
			}
		};
		template <> struct create_transformations_struct<1>
		{
			static constexpr int N = 1;
			static Transformations<N> value(const std::array<std::string, N>& descr)
			{
				const std::string d0 = descr[0];
				Transformations<N> transformations;
				transformations.push_back(std::make_pair(reflect<N>::value(0), "Ref[" + d0 + "]"));
				return transformations;
			}
			static constexpr std::array<CubeI<N>, 1> value()
			{
				constexpr auto results = std::array<CubeI<N>, 1> {
					reflect<N>::value(0)
				};
				return results;
			}
		};
		template <> struct create_transformations_struct<2>
		{
			static constexpr int N = 2;
			static Transformations<N> value(const std::array<std::string, N>& descr)
			{
				const std::string& d0 = descr[0];
				const std::string& d1 = descr[1];

				Transformations<N> transformations;
				transformations.push_back(std::make_pair(reflect<N>::value(0), "Ref[" + d0 + "]"));
				transformations.push_back(std::make_pair(reflect<N>::value(1), "Ref[" + d1 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(0, 1), "Rot[" + d0 + "," + d1 + "]"));
				return transformations;
			}
			static constexpr std::array<CubeI<N>, 3> value()
			{
				constexpr auto results = std::array<CubeI<N>, 3> {
					reflect<N>::value(0), 
					reflect<N>::value(1),
					rotate<N>::value(0, 1)
				};
				return results;
			}
		};
		template <> struct create_transformations_struct<3>
		{
			static constexpr int N = 3;
			static Transformations<N> value(const std::array<std::string, N>& descr)
			{
				const std::string& d0 = descr[0];
				const std::string& d1 = descr[1];
				const std::string& d2 = descr[2];

				Transformations<N> transformations;
				transformations.push_back(std::make_pair(reflect<N>::value(0), "Ref[" + d0 + "]"));
				transformations.push_back(std::make_pair(reflect<N>::value(1), "Ref[" + d1 + "]"));
				transformations.push_back(std::make_pair(reflect<N>::value(2), "Ref[" + d2 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(0, 1), "Rot[" + d0 + "," + d1 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(0, 2), "Rot[" + d0 + "," + d2 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(1, 2), "Rot[" + d1 + "," + d2 + "]"));
				return transformations;
			}
			static constexpr std::array<CubeI<N>, 6> value()
			{
				constexpr auto results = std::array<CubeI<N>, 6> {
					reflect<N>::value(0),
						reflect<N>::value(1),
						reflect<N>::value(2),
						rotate<N>::value(0, 1),
						rotate<N>::value(0, 2),
						rotate<N>::value(1, 2)
				};
				return results;
			}
		};
		template <> struct create_transformations_struct<4>
		{
			static constexpr int N = 4;
			static Transformations<N> value(const std::array<std::string, N>& descr)
			{
				const std::string& d0 = descr[0];
				const std::string& d1 = descr[1];
				const std::string& d2 = descr[2];
				const std::string& d3 = descr[3];

				Transformations<N> transformations;
				transformations.push_back(std::make_pair(reflect<N>::value(0), "Ref[" + d0 + "]"));
				transformations.push_back(std::make_pair(reflect<N>::value(1), "Ref[" + d1 + "]"));
				transformations.push_back(std::make_pair(reflect<N>::value(2), "Ref[" + d2 + "]"));
				transformations.push_back(std::make_pair(reflect<N>::value(3), "Ref[" + d3 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(0, 1), "Rot[" + d0 + "," + d1 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(0, 2), "Rot[" + d0 + "," + d2 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(0, 3), "Rot[" + d0 + "," + d3 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(1, 2), "Rot[" + d1 + "," + d2 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(1, 3), "Rot[" + d1 + "," + d3 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(2, 3), "Rot[" + d2 + "," + d3 + "]"));
				return transformations;
			}
			static constexpr std::array<CubeI<N>, 10> value()
			{
				constexpr auto results = std::array<CubeI<N>, 10> {
					reflect<N>::value(0),
						reflect<N>::value(1),
						reflect<N>::value(2),
						reflect<N>::value(3),
						rotate<N>::value(0, 1),
						rotate<N>::value(0, 2),
						rotate<N>::value(0, 3),
						rotate<N>::value(1, 2),
						rotate<N>::value(1, 3),
						rotate<N>::value(2, 3)
				};
				return results;
			}
		};
		template <> struct create_transformations_struct<5>
		{
			static constexpr int N = 5;
			static Transformations<N> value(const std::array<std::string, N>& descr)
			{
				const std::string& d0 = descr[0];
				const std::string& d1 = descr[1];
				const std::string& d2 = descr[2];
				const std::string& d3 = descr[3];
				const std::string& d4 = descr[4];

				Transformations<N> transformations;
				transformations.push_back(std::make_pair(reflect<N>::value(0), "Ref[" + d0 + "]"));
				transformations.push_back(std::make_pair(reflect<N>::value(1), "Ref[" + d1 + "]"));
				transformations.push_back(std::make_pair(reflect<N>::value(2), "Ref[" + d2 + "]"));
				transformations.push_back(std::make_pair(reflect<N>::value(3), "Ref[" + d3 + "]"));
				transformations.push_back(std::make_pair(reflect<N>::value(4), "Ref[" + d4 + "]"));

				transformations.push_back(std::make_pair(rotate<N>::value(0, 1), "Rot[" + d0 + "," + d1 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(0, 2), "Rot[" + d0 + "," + d2 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(0, 3), "Rot[" + d0 + "," + d3 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(0, 4), "Rot[" + d0 + "," + d4 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(1, 2), "Rot[" + d1 + "," + d2 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(1, 3), "Rot[" + d1 + "," + d3 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(1, 4), "Rot[" + d1 + "," + d4 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(2, 3), "Rot[" + d2 + "," + d3 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(2, 4), "Rot[" + d2 + "," + d4 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(3, 4), "Rot[" + d3 + "," + d4 + "]"));
				return transformations;
			}
			static constexpr std::array<CubeI<N>, 15> value()
			{
				constexpr auto results = std::array<CubeI<N>, 15> {
					reflect<N>::value(0),
						reflect<N>::value(1),
						reflect<N>::value(2),
						reflect<N>::value(3),
						reflect<N>::value(4),
						rotate<N>::value(0, 1),
						rotate<N>::value(0, 2),
						rotate<N>::value(0, 3),
						rotate<N>::value(0, 4),
						rotate<N>::value(1, 2),
						rotate<N>::value(1, 3),
						rotate<N>::value(1, 4),
						rotate<N>::value(2, 3),
						rotate<N>::value(2, 4),
						rotate<N>::value(3, 4)
				};
				return results;
			}
		};
		template <> struct create_transformations_struct<6>
		{
			static constexpr int N = 6;
			static Transformations<N> value(const std::array<std::string, N>& descr)
			{
				const std::string& d0 = descr[0];
				const std::string& d1 = descr[1];
				const std::string& d2 = descr[2];
				const std::string& d3 = descr[3];
				const std::string& d4 = descr[4];
				const std::string& d5 = descr[5];

				Transformations<N> transformations;
				transformations.push_back(std::make_pair(reflect<N>::value(0), "Ref[" + d0 + "]"));
				transformations.push_back(std::make_pair(reflect<N>::value(1), "Ref[" + d1 + "]"));
				transformations.push_back(std::make_pair(reflect<N>::value(2), "Ref[" + d2 + "]"));
				transformations.push_back(std::make_pair(reflect<N>::value(3), "Ref[" + d3 + "]"));
				transformations.push_back(std::make_pair(reflect<N>::value(4), "Ref[" + d4 + "]"));
				transformations.push_back(std::make_pair(reflect<N>::value(5), "Ref[" + d5 + "]"));

				transformations.push_back(std::make_pair(rotate<N>::value(0, 1), "Rot[" + d0 + "," + d1 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(0, 2), "Rot[" + d0 + "," + d2 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(0, 3), "Rot[" + d0 + "," + d3 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(0, 4), "Rot[" + d0 + "," + d4 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(0, 5), "Rot[" + d0 + "," + d5 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(1, 2), "Rot[" + d1 + "," + d2 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(1, 3), "Rot[" + d1 + "," + d3 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(1, 4), "Rot[" + d1 + "," + d4 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(1, 5), "Rot[" + d1 + "," + d5 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(2, 3), "Rot[" + d2 + "," + d3 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(2, 4), "Rot[" + d2 + "," + d4 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(2, 5), "Rot[" + d2 + "," + d5 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(3, 4), "Rot[" + d3 + "," + d4 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(3, 5), "Rot[" + d3 + "," + d5 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(4, 5), "Rot[" + d4 + "," + d5 + "]"));
				return transformations;
			}
			static constexpr std::array<CubeI<N>, 21> value()
			{
				constexpr auto results = std::array<CubeI<N>, 21> {
					reflect<N>::value(0),
						reflect<N>::value(1),
						reflect<N>::value(2),
						reflect<N>::value(3),
						reflect<N>::value(4),
						reflect<N>::value(5),
						rotate<N>::value(0, 1),
						rotate<N>::value(0, 2),
						rotate<N>::value(0, 3),
						rotate<N>::value(0, 4),
						rotate<N>::value(0, 5),
						rotate<N>::value(1, 2),
						rotate<N>::value(1, 3),
						rotate<N>::value(1, 4),
						rotate<N>::value(1, 5),
						rotate<N>::value(2, 3),
						rotate<N>::value(2, 4),
						rotate<N>::value(2, 5),
						rotate<N>::value(3, 4),
						rotate<N>::value(3, 5),
						rotate<N>::value(4, 5)
				};
				return results;
			}
		};
/*
		template <> struct create_transformations_struct<7>
		{
			static constexpr int N = 7;
			static Transformations<N> value(const std::array<std::string, N>& descr)
			{
				const std::string d0 = descr[0];
				const std::string d1 = descr[1];
				const std::string d2 = descr[2];
				const std::string d3 = descr[3];
				const std::string d4 = descr[4];
				const std::string d5 = descr[5];
				const std::string d6 = descr[6];

				Transformations<N> transformations;
				transformations.push_back(std::make_pair(reflect<N>::value(0), "Ref[" + d0 + "]"));
				transformations.push_back(std::make_pair(reflect<N>::value(1), "Ref[" + d1 + "]"));
				transformations.push_back(std::make_pair(reflect<N>::value(2), "Ref[" + d2 + "]"));
				transformations.push_back(std::make_pair(reflect<N>::value(3), "Ref[" + d3 + "]"));
				transformations.push_back(std::make_pair(reflect<N>::value(4), "Ref[" + d4 + "]"));
				transformations.push_back(std::make_pair(reflect<N>::value(5), "Ref[" + d5 + "]"));
				transformations.push_back(std::make_pair(reflect<N>::value(6), "Ref[" + d6 + "]"));

				transformations.push_back(std::make_pair(rotate<N>::value(0, 1), "Rot[" + d0 + "," + d1 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(0, 2), "Rot[" + d0 + "," + d2 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(0, 3), "Rot[" + d0 + "," + d3 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(0, 4), "Rot[" + d0 + "," + d4 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(0, 5), "Rot[" + d0 + "," + d5 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(0, 6), "Rot[" + d0 + "," + d6 + "]"));

				transformations.push_back(std::make_pair(rotate<N>::value(1, 2), "Rot[" + d1 + "," + d2 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(1, 3), "Rot[" + d1 + "," + d3 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(1, 4), "Rot[" + d1 + "," + d4 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(1, 5), "Rot[" + d1 + "," + d5 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(1, 6), "Rot[" + d1 + "," + d6 + "]"));

				transformations.push_back(std::make_pair(rotate<N>::value(2, 3), "Rot[" + d2 + "," + d3 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(2, 4), "Rot[" + d2 + "," + d4 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(2, 5), "Rot[" + d2 + "," + d5 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(2, 6), "Rot[" + d2 + "," + d6 + "]"));

				transformations.push_back(std::make_pair(rotate<N>::value(3, 4), "Rot[" + d3 + "," + d4 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(3, 5), "Rot[" + d3 + "," + d5 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(3, 6), "Rot[" + d3 + "," + d6 + "]"));

				transformations.push_back(std::make_pair(rotate<N>::value(4, 5), "Rot[" + d4 + "," + d5 + "]"));
				transformations.push_back(std::make_pair(rotate<N>::value(4, 6), "Rot[" + d4 + "," + d6 + "]"));

				transformations.push_back(std::make_pair(rotate<N>::value(5, 6), "Rot[" + d5 + "," + d6 + "]"));

				return transformations;
			}
			static constexpr std::array<CubeI<N>, 21> value()
			{
				constexpr auto results = std::array<CubeI<N>, 21> {
					reflect<N>::value(0),
						reflect<N>::value(1),
						reflect<N>::value(2),
						reflect<N>::value(3),
						reflect<N>::value(4),
						reflect<N>::value(5),
						rotate<N>::value(0, 1),
						rotate<N>::value(0, 2),
						rotate<N>::value(0, 3),
						rotate<N>::value(0, 4),
						rotate<N>::value(0, 5),
						rotate<N>::value(1, 2),
						rotate<N>::value(1, 3),
						rotate<N>::value(1, 4),
						rotate<N>::value(1, 5),
						rotate<N>::value(2, 3),
						rotate<N>::value(2, 4),
						rotate<N>::value(2, 5),
						rotate<N>::value(3, 4),
						rotate<N>::value(3, 5),
						rotate<N>::value(4, 5)
				};
				return results;
			}
		};
*/
#pragma endregion

		#pragma region transitive closure
		template <int N, int M> void transitive_closure_recursive_no_descriptions(
			const int start_index,
			const std::array<CubeI<N>, M>& transformations,
			const CubeI<N>& cube,
			INOUT std::set<CubeI<N>>& results)
		{
			for (int i = start_index; i < M; ++i)
			{
				const CubeI<N> cube_new = details::function_composition<N>(cube, transformations[i]);
				results.insert(cube_new);
				transitive_closure_recursive_no_descriptions<N>(i + 1, transformations, cube_new, results);
			}
		}

		template <int N> void transitive_closure_recursive_with_descriptions(
			const int start_index,
			const Transformations<N>& transformations,
			const CubeI<N>& cube,
			const std::string& descr,
			INOUT std::map<CubeI<N>, std::string>& results)
		{
			const int s = static_cast<int>(transformations.size());
			for (int i = start_index; i < s; ++i)
			{
				const CubeI<N> cube_new = details::function_composition<N>(cube, std::get<0>(transformations[i]));
				const std::string descr_new = descr + "." + std::get<1>(transformations[i]);
				
				if (results.contains(cube_new)) 
				{
					if (descr_new.length() < results.at(cube_new).length()) {
						//std::cout << "descr_new=" << descr_new << "; old=" << results.at(cube_new) << std::endl;
						results[cube_new] = descr_new;
					}
				}
				else {
					results.insert(std::make_pair(cube_new, descr_new));
				}				
				transitive_closure_recursive_with_descriptions<N>(i + 1, transformations, cube_new, descr_new, results);
			}
		}

		template <int N> static Transformations<N> transitive_closure(
			const Transformations<N>& transformations_in)
		{
			std::map<CubeI<N>, std::string> results;
			results.insert(std::make_pair(init_cubeI<N>(), ""));
			transitive_closure_recursive_with_descriptions<N>(0, transformations_in, init_cubeI<N>(), "", results);
			return Transformations<N>(results.begin(), results.end());
		}
		template <int N, int M> static Transformations<N> transitive_closure(
			const std::array<CubeI<N>, M>& transformations_in)
		{
			std::set<CubeI<N>> results;
			results.insert(init_cubeI<N>());
			transitive_closure_recursive_no_descriptions<N>(0, transformations_in, init_cubeI<N>(), results);
			Transformations<N> transformations_out;
			for (const auto& e : results) transformations_out.push_back(std::make_pair(e, "?"));
			return transformations_out;
		}

		template <int N, int M> static std::set<CubeI<N>> transitive_closure_x(
			const std::array<CubeI<N>, M>& transformations_in)
		{
			std::set<CubeI<N>> results;
			results.insert(init_cubeI<N>());
			transitive_closure_recursive_no_descriptions<N>(0, transformations_in, init_cubeI<N>(), results);
			return results;
		}
		// slow method
		template <int N, int M> static std::set<CubeI<N>> transitive_closure_y(
			const std::array<CubeI<N>, M>& transformations_in)
		{
			std::set<CubeI<N>> results;
			//results.insert(init_cubeI<N>());

			for (const auto& cube_in : transformations_in) results.insert(cube_in);

			bool changed = true;
			while (changed)
			{
				changed = false;
				for (const auto& cube_in : transformations_in)
				{
					const auto CubeI<N> cube = std::get<0>(transformations[i]);

					for (const auto& e2 : results)
					{
						const CubeI<N> cube_new = details::function_composition<N>(cube_in, cube);
						results.insert(cube_new);
						changed = true;
					}
				}
			}
			return results;
		}
		#pragma endregion

		#pragma region greedy rewrite tranformations
		static std::tuple<Transformations<0>, Transformations<1>, Transformations<2>, Transformations<3>, Transformations<4>, Transformations<5>, Transformations<6>> transformations_greedy_cache;
		template <int N> struct create_transformations_for_greedy_rewrite_struct
		{
			static Transformations<N> value([[maybe_unused]] const std::array<std::string, N>& descr)
			{
				std::cout << "ERROR: create_transformations_for_greedy_rewrite: dim=" << N << " not implemented yet" << std::endl;
				getchar();
				return Transformations<N>();
			}
		};
		template <> struct create_transformations_for_greedy_rewrite_struct<2>
		{
			static constexpr int N = 2;

			static Transformations<N> value(const std::array<std::string, N>& descr)
			{
				if (std::get<N>(transformations_greedy_cache).empty())
				{
					const std::string b = descr[0];
					const std::string a = descr[1];

					auto& transformations = std::get<N>(transformations_greedy_cache);
					transformations.push_back(std::make_pair(reflect<N>::value(0), "Ref[" + b + "]"));
					transformations.push_back(std::make_pair(reflect<N>::value(1), "Ref[" + a + "]"));
					transformations.push_back(std::make_pair(rotate<N>::value(0, 1), "Rot[" + b + "," + a + "]"));
				}
				return std::get<N>(transformations_greedy_cache);
			}
		};
		template <> struct create_transformations_for_greedy_rewrite_struct<3>
		{
			static constexpr int N = 3;
			static Transformations<N> value(const std::array<std::string, N>& descr)
			{
				if (std::get<N>(transformations_greedy_cache).empty())
				{
					const bool use_minized_transformations_set = true;
					if (use_minized_transformations_set)
					{
						const std::string a = descr[2];
						const std::string b = descr[1];
						const std::string c = descr[0];

						constexpr auto ref_a = reflect<N>::value(2);
						constexpr auto ref_b = reflect<N>::value(1);
						constexpr auto ref_c = reflect<N>::value(0);

						constexpr auto rot_cb = rotate<N>::value(0, 1);
						constexpr auto rot_ca = rotate<N>::value(0, 2);
						constexpr auto rot_ba = rotate<N>::value(1, 2);

						auto& transformations = std::get<N>(transformations_greedy_cache);
						transformations.push_back(std::make_pair(ref_a, "Ref[" + a + "]"));
						transformations.push_back(std::make_pair(ref_b, "Ref[" + b + "]"));
						transformations.push_back(std::make_pair(ref_c, "Ref[" + c + "]"));

						transformations.push_back(std::make_pair(function_composition<N>(ref_a, ref_b), "Ref[" + a + "].Ref[" + b + "]"));
						//transformations.push_back(std::make_pair(function_composition<N>(ref_b, ref_c), "Ref[" + b + "].Ref[" + c + "]"));
						//transformations.push_back(std::make_pair(function_composition<N>(ref_a, ref_c), "Ref[" + a + "].Ref[" + c + "]"));
						//transformations.push_back(std::make_pair(function_composition<N>(ref_a, ref_b, ref_c), "Ref[" + a + "].Ref[" + b + "].Ref[" + c + "]"));
						
						//transformations.push_back(std::make_pair(rot_cb, "Rot[" + c + "," + b + "]"));
						//transformations.push_back(std::make_pair(rot_ca, "Rot[" + c + "," + a + "]"));
						//transformations.push_back(std::make_pair(rot_ba, "Rot[" + b + "," + a + "]"));

						//transformations.push_back(std::make_pair(function_composition<N>(rot_cb, ref_a), "Rot[" + c + "," + b + "].Ref[" + a + "]"));
						transformations.push_back(std::make_pair(function_composition<N>(rot_cb, ref_b), "Rot[" + c + "," + b + "].Ref[" + b + "]"));
						//transformations.push_back(std::make_pair(function_composition<N>(rot_cb, ref_c), "Rot[" + c + "," + b + "].Ref[" + c + "]"));
						//transformations.push_back(std::make_pair(function_composition<N>(rot_cb, ref_a, ref_b), "Rot[" + c + "," + b + "].Ref[" + a + "].Ref[" + b + "]"));
						//transformations.push_back(std::make_pair(function_composition<N>(rot_cb, ref_a, ref_c), "Rot[" + c + "," + b + "].Ref[" + a + "].Ref[" + c + "]"));
						//transformations.push_back(std::make_pair(function_composition<N>(rot_cb, ref_b, ref_c), "Rot[" + c + "," + b + "].Ref[" + b + "].Ref[" + c + "]"));

						//transformations.push_back(std::make_pair(function_composition<N>(rot_ca, ref_a), "Rot[" + c + "," + a + "].Ref[" + a + "]"));
						//transformations.push_back(std::make_pair(function_composition<N>(rot_ca, ref_b), "Rot[" + c + "," + a + "].Ref[" + b + "]"));
						//transformations.push_back(std::make_pair(function_composition<N>(rot_ca, ref_c), "Rot[" + c + "," + a + "].Ref[" + c + "]"));
						//transformations.push_back(std::make_pair(function_composition<N>(rot_ca, ref_a, ref_b), "Rot[" + c + "," + a + "].Ref[" + a + "].Ref[" + b + "]"));
						//transformations.push_back(std::make_pair(function_composition<N>(rot_ca, ref_a, ref_c), "Rot[" + c + "," + a + "].Ref[" + a + "].Ref[" + c + "]"));
						//transformations.push_back(std::make_pair(function_composition<N>(rot_ca, ref_b, ref_c), "Rot[" + c + "," + a + "].Ref[" + b + "].Ref[" + c + "]"));

						//transformations.push_back(std::make_pair(function_composition<N>(rot_ba, ref_a), "Rot[" + b + "," + a + "].Ref[" + a + "]"));
						//transformations.push_back(std::make_pair(function_composition<N>(rot_ba, ref_b), "Rot[" + b + "," + a + "].Ref[" + b + "]"));
						//transformations.push_back(std::make_pair(function_composition<N>(rot_ba, ref_a), "Rot[" + b + "," + a + "].Ref[" + c + "]"));
						//transformations.push_back(std::make_pair(function_composition<N>(rot_ba, ref_a, ref_b), "Rot[" + b + "," + a + "].Ref[" + a + "].Ref[" + b + "]"));
						transformations.push_back(std::make_pair(function_composition<N>(rot_ba, ref_a, ref_c), "Rot[" + b + "," + a + "].Ref[" + a + "].Ref[" + c + "]"));
						//transformations.push_back(std::make_pair(function_composition<N>(rot_ba, ref_b, ref_c), "Rot[" + b + "," + a + "].Ref[" + b + "].Ref[" + c + "]"));
					
						for (const auto& pair : transformations)
						{
							std::cout << to_string<N>(pair) << std::endl;
						}
					
					}
					else
					{
						std::get<N>(transformations_greedy_cache) = details::get_transformations_from_cache<N, false>();
					}
				}
				return std::get<N>(transformations_greedy_cache);
			}
		};
		template <> struct create_transformations_for_greedy_rewrite_struct<4>
		{
			static constexpr int N = 4;
			static Transformations<N> value(const std::array<std::string, N>& /*descr*/)
			{
				if (std::get<N>(transformations_greedy_cache).empty())
				{
					const bool use_minized_transformations_set = false;
					if (use_minized_transformations_set)
					{
						// TODO: find a reduced set of transformations that is correct, for the time being take the full closure:
					}
					else
					{
						std::get<N>(transformations_greedy_cache) = details::get_transformations_from_cache<N, false>();
					}
				}
				return std::get<N>(transformations_greedy_cache);
			}
		};
		template <> struct create_transformations_for_greedy_rewrite_struct<5>
		{
			static constexpr int N = 5;
			static Transformations<N> value(const std::array<std::string, N>& /*descr*/)
			{
				if (std::get<N>(transformations_greedy_cache).empty())
				{
					const bool use_minized_transformations_set = false;
					if (use_minized_transformations_set)
					{
						// TODO: find a reduced set of transformations that is correct, for the time being take the full closure:
					}
					else
					{
						std::get<N>(transformations_greedy_cache) = details::get_transformations_from_cache<N, false>();
					}
				}
				return std::get<N>(transformations_greedy_cache);
			}
		};
		#pragma endregion

		#pragma region all transformations

		template <int N, bool DESCR> 
		const Transformations<N> create_transformations()
		{
			if constexpr (DESCR)
			{
				return details::transitive_closure<N>(details::create_transformations_struct<N>::value(create_descriptions<N>()));
			}
			else
			{
				return details::transitive_closure<N>(details::create_transformations_struct<N>::value());
			}
		}

		static std::tuple<Transformations<0>, Transformations<1>, Transformations<2>, Transformations<3>, Transformations<4>, Transformations<5>, Transformations<6>> tranformations_cache;

		template <int N, bool DESCR> 
		const Transformations<N>& get_transformations_from_cache()
		{
			if (std::get<N>(tranformations_cache).empty())
			{
				std::get<N>(tranformations_cache) = create_transformations<N, DESCR>();
			}
			return std::get<N>(tranformations_cache);
		}

		#pragma endregion
	} // end namespace details


	template <int N> 
	void init_n_cube() {
		// run all_transformations once to fill the transformation cache
		const auto get_transformations_from_cache = details::get_transformations_from_cache<N, false>();
	}


	template <int N> std::array<std::string, N> create_descriptions()
	{
		if constexpr (N == 1) {
			return { "x0" };
		}
		if constexpr (N == 2) {
			return { "0", "1" };
		}
		if constexpr (N == 3) {
			return { "0", "1", "2" };
		}
		if constexpr (N == 4) {
			return { "d", "c", "b", "a" };
		}
		if constexpr (N == 5) {
			return { "e", "d", "c", "b", "a" };
		}
		if constexpr (N == 6) {
			return { "f", "e", "d", "c", "b", "a" };
		}
		if constexpr (N == 7) {
			return { "g", "f", "e", "d", "c", "b", "a" };
		}
		return std::array<std::string, N>();
	}
	template <int N> constexpr BF complement(const BF bf) noexcept
	{
		if constexpr (N == 1) {
			return ~bf & 0b11;
		}
		if constexpr (N == 2) {
			return ~bf & 0xF;
		}
		if constexpr (N == 3) {
			return ~bf & 0xFF;
		}
		if constexpr (N == 4) {
			return ~bf & 0xFFFF;
		}
		if constexpr (N == 5) {
			return ~bf & 0xFFFFFFFF;
		}
		if constexpr (N == 6) {
			return ~bf;
		}
		return 0;
	}
	template <int N> constexpr BF complement_if_needed(const BF bf) noexcept
	{
		constexpr int N2 = (1<<N) / 2;
		return (details::count_bits(bf) <= N2) ? bf : complement<N>(bf);
	}

	#pragma region transform
	// Transform (shuffle) cube c according to the change as provided by cube ci
	template <int N> constexpr Cube<N> transform(const Cube<N>& c, const CubeI<N>& ci)
	{
		return details::apply<N>(c, ci);
	}
	// Transform (shuffle) cube c according to the change as provided by cube ci
	template <int N> constexpr CubeI<N> transform(const CubeI<N>& c, const CubeI<N>& ci)
	{
		return details::apply<N>(c, ci);
	}
	// Transform (shuffle) Boolean function bf according to the change as provided by cube
	template <int N> constexpr BF transform(const BF bf, const CubeI<N>& cube)
	{
		return transform<N>(Cube<N>(bf), cube).get_bf();
	}
	#pragma endregion

	#pragma region reflect
	template <int N> constexpr CubeI<N> reflect(const int dim)
	{
		return details::reflect<N>::value(dim);
	}
	template <int N> constexpr BF reflect(const BF bf, const int dim)
	{
		return transform<N>(Cube<N>(bf), reflect<N>(dim)).get_bf();
	}
	#pragma endregion

	#pragma region rotate
	template <int N> constexpr CubeI<N> rotate(const int dim1, const int dim2)
	{
		return details::rotate<N>::value(dim1, dim2);
	}
	template <int N> constexpr BF rotate(const BF bf, const int dim1, const int dim2)
	{
		return to_BF<N>(transform<N>(to_cube<N>(bf), rotate<N>(dim1, dim2)));
	}
	#pragma endregion

	#pragma region create transformations
	template <int N> Transformations<N> create_transformations(const std::array<std::string, N>& descr)
	{
		return details::create_transformations_struct<N>::value(descr);
	}
	template <int N, bool DESC = false> Transformations<N> create_transformations_for_greedy_rewrite()
	{
		if constexpr (DESC)
		{
			return details::create_transformations_for_greedy_rewrite_struct<N>::value(create_descriptions<N>());
		}
		else
		{
			//TODO make a greedy rewrite with no descriptions
			return details::create_transformations_for_greedy_rewrite_struct<N>::value(create_descriptions<N>());
		}
	}
	#pragma endregion

	#pragma region class id
	namespace details
	{
		//for the set of all transformations find the transformation that yields the smallest bf, which is the bf class id.
		template <int N, bool DESCR> std::tuple<BF, std::string> search_class_id_method0(const BF bf)
		{
			std::string complement_string = "";
			BF smallest_bf = bf;
			if (details::count_bits(bf) > ((1 << N) / 2))
			{
				complement_string = "COMPLEMENT ";
				smallest_bf = complement<N>(bf);
			}

			std::string transform_string = "";

			// find minimum in set
			for (const auto& pair : details::get_transformations_from_cache<N, DESCR>())
			{
				const CubeI<N> cube = std::get<0>(pair);
				const BF bf_new = transform<N>(bf, cube);

				if (bf_new < smallest_bf)
				{
					smallest_bf = bf_new;
					transform_string = to_string<N>(pair);
				}

				const BF bf_new_compl = complement<N>(bf_new);
				if (bf_new_compl < smallest_bf)
				{
					smallest_bf = bf_new_compl;
					transform_string = to_string<N>(pair);
				}
			}
			return std::make_pair(smallest_bf, complement_string + transform_string);
		}


		bool overall_decreasing(const BF a, const BF b) noexcept { return b < a; }



		//for a specific sequence of transformations, reduce the provided bf with in a greedy fashion with the tranformations.
		template <int N, bool DESCR = false> std::tuple<BF, std::string> search_class_id_method1(const BF bf)
		{
			const auto transformations = create_transformations_for_greedy_rewrite<N, DESCR>();

			std::string transform_string = "";
			BF smallest_bf = bf;
			if (details::count_bits(bf) > ((1 << N) / 2))
			{
				transform_string = "COMPLEMENT ";
				smallest_bf = complement<N>(bf);
			}

			bool changed = true;
			while (changed)
			{
				changed = false;
				{
					for (const auto& pair : transformations)
					{
						const BF bf_tmp = transform<N>(smallest_bf, std::get<0>(pair));
						const BF bf_new = std::min(bf_tmp, complement<N>(bf_tmp));

						//std::cout << "trying " << to_string<N>(pair) << "; bf_new=" << to_string_bin<N>(bf_new) << std::endl;
						if (bf_new < smallest_bf)
						{
							smallest_bf = bf_new;

							if (transform_string.length() > 0) transform_string += " + ";
							transform_string += to_string<N>(pair);
							changed = true;
						}
					}
				}
			}
			return std::make_tuple(smallest_bf, transform_string);
		}

	}

	template <int N> constexpr BF search_class_id(const BF bf)
	{
		const bool method1 = true;
		return (method1)
			? std::get<0>(details::search_class_id_method0<N, false>(bf))
			: std::get<0>(details::search_class_id_method1<N, false>(bf));
	}

	#pragma endregion 

	// Get the set of BFs that are in the equivalence class of the provided bf.
	template <int N> std::set<BF> equiv_class(const BF bf)
	{
		const Transformations<N>& get_transformations_from_cache = details::get_transformations_from_cache<N, false>();

		std::set<BF> result;
		result.insert(complement<N>(bf));

		for (const auto& pair : get_transformations_from_cache)
		{
			const CubeI<N>& cube = std::get<0>(pair);
			const BF bf_new = transform<N>(bf, cube);

			result.insert(bf_new);
			result.insert(complement<N>(bf_new));
		}
		return result;
	}

	#pragma region reachable
	namespace {

		template <int N>
		bool constexpr reachable_private_A(const BF from, const BF to, const CubeI<N>& transformation)
		{
			const BF bf_new = transform<N>(from, transformation);
			return (bf_new == to) ? true : ((complement<N>(bf_new) == to) ? true : false);
		}

		template <int N, int M, int ... S>
		bool constexpr reachable_private_X1(const BF from, const BF to, const std::array<CubeI<N>, M>& transformations, std::index_sequence<S...>)
		{
			return (complement<N>(to) == from) ? true : (... || reachable_private_A<N>(from, to, transformations[S]));
		}

		template <int N, int Q, int ... S>
		bool constexpr reachable_private_X2(const BF from, const std::array<BF, Q>& to, const CubeI<N>& transformation, std::index_sequence<S...>)
		{
			return (complement<N>(to) == from) ? true : (... || reachable_private_A<N>(from, to[S], transformation));
		}

		template <int N, int M, int Q, int ... S>
		bool constexpr reachable_private_Y(const BF from, const std::array<BF, Q>& to, const std::array<CubeI<N>, M>& transformations, std::index_sequence<S...>)
		{
			return (... && reachable_private_X1<N>(from, to[S], transformations, std::make_index_sequence<N>()));
		}
	}
	
	template <int N>
	bool constexpr reachable(const BF from, const BF to, const CubeI<N>& transformation)
	{
		return reachable_private_A<N>(from, to, transformation);
	}

	template <int N, int M>
	bool constexpr reachable(const BF from, const BF to, const std::array<CubeI<N>, M>& transformations)
	{
		return reachable_private_X1<N, M>(from, to, transformations, std::make_index_sequence<N>());
	}

	template <int N, int Q>
	bool constexpr reachable(const BF from, const std::array<BF, Q>& to, const CubeI<N>& transformation) 
	{
		return reachable_private_X2<N, Q>(from, to, transformation, std::make_index_sequence<Q>());
	}

	template <int N, int Q, int M>
	bool constexpr reachable(const BF from, const std::array<BF, Q>& to, const std::array<CubeI<N>, M>& transformations)
	{
		return reachable_private_Y<N, M, Q>(from, to, transformations, std::make_index_sequence<N>());
	}



	#pragma region all class ids
	namespace details
	{
		//Naive method: iterates over all bf of cardinality >=N
		template <int N> std::set<BF> generate_all_npn_classes_method0()
		{
			constexpr unsigned long long total_number_npn_classes = bf::cardinality_npn_class(N);
			constexpr long long max_bf = 1ll << (1 << N);
			constexpr long long update_interval = 0x3FFFF;
			constexpr int n_threads = 12;
			
			init_n_cube<N>();

			const time_t start_time_sec = time(nullptr);

			std::array<std::set<BF>, n_threads> results_per_thread;
			std::array<long long, n_threads> counter_per_thread;
			counter_per_thread.fill(0);
			std::set<BF> global_results;
			std::atomic<long long> global_counter = 0;
			std::mutex global_results_mutex;

			#pragma omp parallel for num_threads(12) default(shared)
			for (long long bf = 0; bf < max_bf; ++bf)
			{
				const BF c = search_class_id<N>(bf);
				const int thread_id = omp_get_thread_num();

				counter_per_thread[thread_id]++;
				results_per_thread[thread_id].insert(c);

				if ((counter_per_thread[thread_id] & update_interval) == 0)
				{
					global_counter++;
					const time_t passed_time_sec = time(nullptr) - start_time_sec;
					const double percentage_done = (global_counter * update_interval) / static_cast<double>(max_bf);
					const int needed_time_sec = static_cast<int>((passed_time_sec / percentage_done) - passed_time_sec);

					auto& results = results_per_thread[thread_id];
					{
						const auto lock = std::unique_lock(global_results_mutex);
						for (const BF bf2 : results) {
							global_results.insert(bf2);
						}
					}
					results.clear();
					const long long number_npn_classes = global_results.size();
					std::cout << "percentage done : " << percentage_done << "; NPN classes not found : " << (total_number_npn_classes - number_npn_classes) << "; needed_time : " << needed_time_sec << " sec = " << needed_time_sec / 60 << " min = " << needed_time_sec / (60 * 60) << " hour " << std::endl;

					if (number_npn_classes == total_number_npn_classes) {
						break;
					}
				}
			}

			for (int i = 0; i < n_threads; ++i) {
				for (const BF bf : results_per_thread[i]) {
					global_results.insert(bf);
				}
			}
			return global_results;
		}
		template <int N> std::set<BF> generate_all_npn_classes_method1()
		{
			std::set<BF> results;
			// start with the number of bits set in lowest bit positions, perform a not allowed transformations, and reduce to smallest form
			for (int i = 0; i < N; ++i)
			{
				results.insert(i);

				// for each invalid tranformation yield a bf class that is different from the class of i.

				// reflection dim 0 for var 1
			}
			return results;
		}
	}
	template <int N> std::set<BF> generate_all_npn_classes()
	{
		const bool use_method0 = true;
		return (use_method0)
			? details::generate_all_npn_classes_method0<N>()
			: details::generate_all_npn_classes_method1<N>();
	}
	#pragma endregion

	template <int N> void test_greedy_rewrite_algorithm()
	{
		bool found_error = false;

		for (BF bf = 0; bf < (1ull << (1 << N)); ++bf)
		{
			const auto r1 = details::search_class_id_method0<N>(bf, true);
			const auto r2 = details::search_class_id_method1<N>(bf, true);

			const BF bf1 = std::get<0>(r1);
			const BF bf2 = std::get<0>(r2);

			if ((bf & 0xFF) == 0) std::cout << ".";

			if (bf1 != bf2)
			{
				std::cout << std::endl << "ERROR: test_greedy_rewrite_algorithm<" << N << ">:" << std::endl;
				std::cout << "bf      = " << to_string_hex<N>(bf) << " = " << to_string_bin<N>(bf) << "; number of bits set:" << details::count_bits(bf) << std::endl;
				std::cout << "closure = " << to_string_hex<N>(bf1) << " = " << to_string_bin<N>(bf1) << " " << std::get<1>(r1) << std::endl;
				std::cout << "greedy  = " << to_string_hex<N>(bf2) << " = " << to_string_bin<N>(bf2) << " " << std::get<1>(r2) << std::endl;

				found_error = true;
				getchar();
			}
		}
		if (!found_error) std::cout << std::endl << "test_greedy_rewrite_algorithm<" << N << ">: found no errors" << std::endl;
	}

	template <int N> void find_greedy_rewrite_algorithms()
	{
		std::vector<CubeI<N>> all_tranformations_vector;
		int counter = 0;
		for (const auto& t : details::get_transformations_from_cache<N, false>())
		{
			all_tranformations_vector.push_back(std::get<0>(t));
			std::cout << counter << ":" << to_string<N>(t) << std::endl;
			counter++;
		}
		const int s = static_cast<int>(all_tranformations_vector.size());
		std::cout << "number of transformations = " << s << std::endl;

		std::array<CubeI<N>, 6> selection;

		if (true)
		{
			selection[0] = all_tranformations_vector[27];
			selection[1] = all_tranformations_vector[14];
			selection[2] = all_tranformations_vector[6];
			selection[3] = all_tranformations_vector[41];
			selection[4] = all_tranformations_vector[2];
			selection[5] = all_tranformations_vector[7];

			const auto closure1 = details::transitive_closure_x<N>(selection);
			std::cout << "size " << closure1.size() << std::endl;
			for (const auto& cube : closure1)
			{
				std::cout << to_string<N>(cube) << std::endl;
			}
		}
		else
		{
			for (int i0 = 0; i0 < s; ++i0)
			{
				std::cout << "i0 = " << i0 << std::endl;

				selection[0] = all_tranformations_vector[i0];
				for (int i1 = 0; i1 < s; ++i1)
				{
					//std::cout << i1;
					selection[1] = all_tranformations_vector[i1];
					for (int i2 = i1 + 1; i2 < s; ++i2)
					{
						selection[2] = all_tranformations_vector[i2];
						for (int i3 = i2 + 1; i3 < s; ++i3)
						{
							selection[3] = all_tranformations_vector[i3];
							for (int i4 = i3 + 1; i4 < s; ++i4)
							{
								selection[4] = all_tranformations_vector[i4];
								for (int i5 = i4 + 1; i5 < s; ++i5)
								{
									selection[5] = all_tranformations_vector[i5];

									const auto closure = details::transitive_closure_x<N>(selection);
									if (closure.size() == s)
									{
										std::cout << "Found a solution!" << std::endl;
										for (const auto& cube : selection)
										{
											std::cout << to_string<N>(cube) << std::endl;
										}

									}
								}
							}
						}
					}
				}
			}
		}
	}

	// find the transformation that when applied to bf1 yields bf2.
	template <int N> std::vector<std::pair<CubeI<N>, std::string>> find_transformation(const BF bf1, const BF bf2, [[maybe_unused]] const std::array<std::string, N>& descr)
	{
		std::vector<std::pair<CubeI<N>, std::string>> results;

		const BF bf1b = complement_if_needed<N>(bf1);
		const BF bf2b = complement_if_needed<N>(bf2);

		const Cube<N> cube1 = Cube<N>(bf1b);
		const Cube<N> cube2 = Cube<N>(bf2b);

		const auto transformations = details::get_transformations_from_cache<N, true>();
		{
			if (bf1b == bf2b) results.push_back(std::make_pair(details::init_cubeI<N>(), "Identity"));

			for (const auto& pair : transformations)
			{
				if (transform<N>(cube1, std::get<0>(pair)) == cube2)
				{
					results.push_back(pair);
				}
			}
			if (results.empty())
			{
				std::cout << "WARNING: could not find transformation" << std::endl;
				static_cast<void>(getchar());
			}
		}
		return results;
	}

	
	template <int N>
	static void print_rotation(const BF a, const BF b, const std::vector<std::pair<std::string, CubeI<N>>>& vec) 
	{
		bool none = true;
		for (const std::pair<std::string, CubeI<N>>& p : vec) {
			if (transform<N>(a, p.second) == b) {
				std::cout << to_string_bin<N>(a) << " -> " << to_string_bin<N>(b) << " with " << p.first << " " << to_string<N>(p.second) << std::endl;
				none = false;
			}
		}
		if (none) {
			std::cout << to_string_bin<N>(a) << " -> " << to_string_bin<N>(b) << " not possible"  << std::endl;
		}
	}

	template <int N, int Q>
	static void print_translations(
		const std::array<BF, Q>& bfs,
		const std::vector<std::pair<std::string, CubeI<N>>>& all_refs,
		const std::vector<std::pair<std::string, CubeI<N>>>& all_rots) 
	{
		std::cout << "===============" << std::endl;
		for (int i = 1; i < bfs.size(); ++i) print_rotation<N>(bfs[0], bfs[i], all_rots);
		std::cout << "===" << std::endl;
		for (int i = 1; i < bfs.size(); ++i) print_rotation<N>(bfs[0], bfs[i], all_refs);
	}


	template <int N> constexpr CubeI<N> o(const CubeI<N>& a, const CubeI<N>& b) {
		return details::function_composition<N>(a, b);
	}

	static void test_reachability_bf2() {
		constexpr int N = 2;
		constexpr CubeI<N> ref0 = details::reflect<N>::value(0);
		constexpr CubeI<N> ref1 = details::reflect<N>::value(1);
		constexpr CubeI<N> ref01 = o<N>(ref0, ref1);
		
		constexpr CubeI<N> rot01 = details::rotate<N>::value(0, 1);
		constexpr CubeI<N> rot10 = details::rotate<N>::value(1, 0);
		constexpr auto empty = std::array<CubeI<N>, 0>{};

		std::vector<std::pair<std::string, CubeI<N>>> all_refs;
		{
			all_refs.push_back(std::make_pair("ref0", ref0));
			all_refs.push_back(std::make_pair("ref1", ref1));
			all_refs.push_back(std::make_pair("ref01", ref01));
		}
		std::vector<std::pair<std::string, CubeI<N>>> all_rots;
		{
			all_rots.push_back(std::make_pair("rot01", rot01));
			all_rots.push_back(std::make_pair("rot10", rot10));
		}


		//npn class #0; cardinality=0: 00=0000; class size=2=(2^1)
		//equivalent BF: 0000 1111
		static_assert(reachable<N>(0b0000, 0b1111, empty));

		//===========================
		//npn class #1; cardinality=1: 03=0011; class size=4=(2^2)
		//equivalent BF: 0011 0101 1010 1100
		constexpr auto c1 = std::array<BF, 2>{0b0011, 0b0101};
		//rot
		if (true) print_translations<N>(c1, all_refs, all_rots);

		//===========================
		//npn class #2; cardinality=2: 01=0001; class size=8=(2^3)
		//equivalent BF: 0001 0010 0100 0111 1000 1011 1101 1110
		constexpr auto c2 = std::array<BF, 4>{0b0001, 0b0010, 0b0100, 0b1000};
		//ref
		if (true) print_translations<N>(c2, all_refs, all_rots);

		//===========================
		//npn class #3; cardinality=2: 06=0110; class size=2=(2^1)
		//equivalent BF: 0110 1001
	}


	static void test_reachability_bf3() 
	{
		constexpr int N = 3;
		constexpr CubeI<N> id = details::init_cubeI<N>();
		
		constexpr CubeI<N> ref0 = details::reflect<N>::value(0);
		constexpr CubeI<N> ref1 = details::reflect<N>::value(1);
		constexpr CubeI<N> ref2 = details::reflect<N>::value(2);

		constexpr CubeI<N> ref01 = o<N>(ref0, ref1);
		constexpr CubeI<N> ref02 = o<N>(ref0, ref2);
		constexpr CubeI<N> ref12 = o<N>(ref1, ref2);
		constexpr CubeI<N> ref012 = o<N>(ref0, o<N>(ref1, ref2));

/*
0_1_2_3_4_5_6_7
1_3_0_2_5_7_4_6 .Rot[0,1]
1_5_3_7_0_4_2_6 .Rot[0,2]
2_0_3_1_6_4_7_5 .Rot[1,0]
2_3_6_7_0_1_4_5 .Rot[1,2]
4_0_6_2_5_1_7_3 .Rot[2,0]
4_5_0_1_6_7_2_3 .Rot[2,1]

3_7_2_6_1_5_0_4 .Rot[0,1].Rot[0,2]
0_2_4_6_1_3_5_7 .Rot[0,1].Rot[1,2]
5_1_4_0_7_3_6_2 .Rot[0,1].Rot[2,0]
5_7_1_3_4_6_0_2 .Rot[0,1].Rot[2,1]
3_1_7_5_2_0_6_4 .Rot[0,2].Rot[1,0]
0_4_1_5_2_6_3_7 .Rot[0,2].Rot[2,1]
6_4_2_0_7_5_3_1 .Rot[1,0].Rot[2,1]
6_2_7_3_4_0_5_1 .Rot[1,2].Rot[1,0]

1_0_5_4_3_2_7_6 .Rot[0,1].Rot[1,2].Rot[2,0]
2_6_0_4_3_7_1_5 .Rot[0,1].Rot[0,2].Rot[1,2]
4_6_5_7_0_2_1_3 .Rot[1,2].Rot[1,0].Rot[2,0]
7_3_5_1_6_2_4_0 .Rot[0,1].Rot[2,0].Rot[2,1]

3_2_1_0_7_6_5_4 .Rot[0,1].Rot[0,2].Rot[1,2].Rot[2,0]
5_4_7_6_1_0_3_2 .Rot[0,1].Rot[1,2].Rot[1,0].Rot[2,0]
7_6_3_2_5_4_1_0 .Rot[0,1].Rot[0,2].Rot[1,2].Rot[2,0].Rot[2,1]
*/
		
		constexpr CubeI<N> rot01 = details::rotate<N>::value(0, 1);
		constexpr CubeI<N> rot02 = details::rotate<N>::value(0, 2);
		constexpr CubeI<N> rot12 = details::rotate<N>::value(1, 2);
		constexpr CubeI<N> rot10 = details::rotate<N>::value(1, 0);
		constexpr CubeI<N> rot20 = details::rotate<N>::value(2, 0);
		constexpr CubeI<N> rot21 = details::rotate<N>::value(2, 1);

		constexpr CubeI<N> rot01_02 = o<N>(rot01, rot02);
		constexpr CubeI<N> rot01_12 = o<N>(rot01, rot12);
		constexpr CubeI<N> rot01_20 = o<N>(rot01, rot20);
		constexpr CubeI<N> rot01_21 = o<N>(rot01, rot21);
		constexpr CubeI<N> rot02_10 = o<N>(rot02, rot10);
		constexpr CubeI<N> rot02_21 = o<N>(rot02, rot21);
		constexpr CubeI<N> rot10_21 = o<N>(rot10, rot21);
		constexpr CubeI<N> rot12_10 = o<N>(rot12, rot10);

		constexpr CubeI<N> rot01_12_20 = o<N>(rot01, o<N>(rot12, rot20));
		constexpr CubeI<N> rot01_02_12 = o<N>(rot01, o<N>(rot02, rot12));
		constexpr CubeI<N> rot12_10_20 = o<N>(rot12, o<N>(rot10, rot20));
		constexpr CubeI<N> rot01_20_21 = o<N>(rot01, o<N>(rot20, rot21));

		constexpr CubeI<N> rot01_02_12_20 = o<N>(rot01, o<N>(rot02, o<N>(rot12, rot20)));
		constexpr CubeI<N> rot01_12_10_20 = o<N>(rot01, o<N>(rot12, o<N>(rot10, rot20)));
		constexpr CubeI<N> rot01_02_12_20_21 = o<N>(rot01, o<N>(rot02, o<N>(rot12, o<N>(rot20, rot21))));

		std::vector<std::pair<std::string, CubeI<N>>> all_refs;
		{
			all_refs.push_back(std::make_pair("ref0", ref0));
			all_refs.push_back(std::make_pair("ref1", ref1));
			all_refs.push_back(std::make_pair("ref2", ref2));
			all_refs.push_back(std::make_pair("ref01", ref01));
			all_refs.push_back(std::make_pair("ref02", ref02));
			all_refs.push_back(std::make_pair("ref12", ref12));
			all_refs.push_back(std::make_pair("ref012", ref012));
		}

		std::vector<std::pair<std::string, CubeI<N>>> all_rots;
		{
			all_rots.push_back(std::make_pair("rot01", rot01));
			all_rots.push_back(std::make_pair("rot02", rot02));
			all_rots.push_back(std::make_pair("rot12", rot12));
			all_rots.push_back(std::make_pair("rot10", rot10));
			all_rots.push_back(std::make_pair("rot20", rot20));
			all_rots.push_back(std::make_pair("rot21", rot21));

			all_rots.push_back(std::make_pair("rot01_02", rot01_02));
			all_rots.push_back(std::make_pair("rot01_12", rot01_12));
			all_rots.push_back(std::make_pair("rot01_20", rot01_20));
			all_rots.push_back(std::make_pair("rot01_21", rot01_21));
			all_rots.push_back(std::make_pair("rot02_10", rot02_10));
			all_rots.push_back(std::make_pair("rot02_21", rot02_21));
			all_rots.push_back(std::make_pair("rot10_21", rot10_21));
			all_rots.push_back(std::make_pair("rot12_10", rot12_10));

			all_rots.push_back(std::make_pair("rot01_12_20", rot01_12_20));
			all_rots.push_back(std::make_pair("rot01_02_12", rot01_02_12));
			all_rots.push_back(std::make_pair("rot12_10_20", rot12_10_20));
			all_rots.push_back(std::make_pair("rot01_20_21", rot01_20_21));

			all_rots.push_back(std::make_pair("rot01_02_12_20", rot01_02_12_20));
			all_rots.push_back(std::make_pair("rot01_12_10_20", rot01_12_10_20));
			all_rots.push_back(std::make_pair("rot01_02_12_20_21", rot01_02_12_20_21));
		}
		
		constexpr auto empty = std::array<CubeI<N>, 0>{};

		//npn class #0; cardinality=0: 00=00000000; class size=2=(2^1)
		//equivalent BF: 00000000 11111111
		static_assert(reachable<N>(0b0000'0000, 0b1111'1111, empty));

		//===========================
		//npn class #1; cardinality=1: 0F=00001111; class size=6=(2^1 + 3^1)
		//equivalent BF: 00001111 00110011 01010101 10101010 11001100 11110000
		constexpr auto c1 = std::array<BF, 3>{0b0000'1111, 0b0011'0011, 0b0101'0101};
		static_assert(reachable<N>(c1[0], c1[1], rot12));
		static_assert(reachable<N>(c1[0], c1[2], rot02));
		//rot
		if (false) print_translations<N>(c1, all_refs, all_rots);

		//===========================
		//npn class #2; cardinality=2: 03=00000011; class size=24=(2^3 + 3^1)
		//equivalent BF: 00000011 00000101 00001010 00001100 00010001 00100010 00110000 00111111 01000100 01010000 01011111 01110111 10001000 10100000 10101111 10111011 11000000 11001111 11011101 11101110 11110011 11110101 11111010 11111100
		constexpr auto c2 = std::array<BF, 12>{0b0000'0011, 0b0000'0101, 0b0000'1010, 0b0000'1100, 0b0001'0001, 0b0010'0010, 0b0011'0000, 0b0100'0100, 0b0101'0000, 0b1000'1000, 0b1010'0000, 0b1100'0000};		
		static_assert(reachable<N>(c2[0], c2[1], rot01));
		static_assert(reachable<N>(c2[0], c2[2], rot10));
		static_assert(reachable<N>(c2[0], c2[3], rot21)); static_assert(reachable<N>(c2[0], c2[3], ref1));
		static_assert(reachable<N>(c2[0], c2[4], rot02));
		static_assert(reachable<N>(c2[0], c2[5], rot20));
		static_assert(reachable<N>(c2[0], c2[6], rot12)); static_assert(reachable<N>(c2[0], c2[6], ref2));
		static_assert(reachable<N>(c2[0], c2[7], rot01_21));
		static_assert(reachable<N>(c2[0], c2[8], rot01_02));
		static_assert(reachable<N>(c2[0], c2[9], rot10_21));
		static_assert(reachable<N>(c2[0], c2[10], rot12_10));
		static_assert(reachable<N>(c2[0], c2[11], rot01_02_12_20_21)); static_assert(reachable<N>(c2[0], c2[11], ref12));
		//rot
		if (false) print_translations<N>(c2, all_refs, all_rots);

		//===========================
		//npn class #3; cardinality=2: 3C=00111100; class size=6=(2^1 + 3^1)
		//equivalent BF: 00111100 01011010 01100110 10011001 10100101 11000011
		constexpr auto c3 = std::array<BF, 3>{0b00111100, 0b01011010, 0b01100110};
		static_assert(reachable<N>(c3[0], c3[1], rot01));
		static_assert(reachable<N>(c3[0], c3[2], rot02));
		//rot
		if (false) print_translations<N>(c3, all_refs, all_rots);

		//===========================
		//npn class #4; cardinality=3: 01=00000001; class size=16=(2^4)
		//equivalent BF: 00000001 00000010 00000100 00001000 00010000 00100000 01000000 01111111 10000000 10111111 11011111 11101111 11110111 11111011 11111101 11111110
		constexpr auto c4 = std::array<BF, 8>{0b0000'0001, 0b0000'0010, 0b0000'0100, 0b0000'1000, 0b0001'0000, 0b0010'0000, 0b0100'0000, 0b1000'0000};
		static_assert(reachable<N>(c4[0], c4[1], ref0));
		static_assert(reachable<N>(c4[0], c4[2], ref1));
		static_assert(reachable<N>(c4[0], c4[3], ref01));
		static_assert(reachable<N>(c4[0], c4[4], ref2));
		static_assert(reachable<N>(c4[0], c4[5], ref02));
		static_assert(reachable<N>(c4[0], c4[6], ref12));
		static_assert(reachable<N>(c4[0], c4[7], ref012));
		//rot || ref
		if (false) print_translations<N>(c4, all_refs, all_rots);

		//===========================
		//npn class #5; cardinality=3: 06=00000110; class size=24=(2^3 + 3^1)
		//equivalent BF: 00000110 00001001 00010010 00010100 00100001 00101000 01000001 01001000 01100000 01101111 01111011 01111101 10000010 10000100 10010000 10011111 10110111 10111110 11010111 11011110 11101011 11101101 11110110 11111001
		constexpr auto c5 = std::array<BF, 12>{0b00000110, 0b00001001, 0b00010010, 0b00010100, 0b00100001, 0b00101000, 0b01000001, 0b01001000, 0b01100000, 0b10000010, 0b10000100, 0b10010000};
		static_assert(reachable<N>(c5[0], c5[1], rot01));
		static_assert(reachable<N>(c5[0], c5[2], rot01_12));
		static_assert(reachable<N>(c5[0], c5[3], rot01_02));
		static_assert(reachable<N>(c5[0], c5[4], rot12));
		static_assert(reachable<N>(c5[0], c5[5], rot20));
		static_assert(reachable<N>(c5[0], c5[6], rot02));
		static_assert(reachable<N>(c5[0], c5[7], rot21));
		//rot 
		if (false) print_translations<N>(c5, all_refs, all_rots);

		//===========================
		//npn class #6; cardinality=3: 07=00000111; class size=48=(2^4 + 3^1)
		//equivalent BF: 00000111 00001011 00001101 00001110 00010011 00010101 00011111 00100011 00101010 00101111 00110001 00110010 00110111 00111011 01000101 01001100 01001111 01010001 01010100 01010111 01011101 01110000 01110011 01110101 10001010 10001100 10001111 10100010 10101000 10101011 10101110 10110000 10110011 10111010 11000100 11001000 11001101 11001110 11010000 11010101 11011100 11100000 11101010 11101100 11110001 11110010 11110100 11111000
		constexpr auto c6 = std::array<BF, 24>{0b00000111, 0b00001011, 0b00001101, 0b00001110, 0b00010011, 0b00010101, 0b00100011, 0b00101010, 0b00110001, 0b00110010, 0b01000101, 0b01001100, 0b01010001, 0b01010100, 0b01110000, 0b10001010, 0b10001100, 0b10100010, 0b10101000, 0b10110000, 0b11000100, 0b11001000, 0b11010000, 0b11100000};
		//rot && ref
		if (false) print_translations<N>(c6, all_refs, all_rots);

		//===========================
		//npn class #7; cardinality=3: 16=00010110; class size=16=(2^4)
		//equivalent BF: 00010110 00101001 01001001 01100001 01101000 01101011 01101101 01111001 10000110 10010010 10010100 10010111 10011110 10110110 11010110 11101001
		constexpr auto c7 = std::array<BF, 8>{0b00010110, 0b00101001, 0b01001001, 0b01100001, 0b01101000, 0b10000110, 0b10010010, 0b10010100};
		static_assert(reachable<N>(c7[0], c7[1], ref0));
		static_assert(reachable<N>(c7[0], c7[2], ref1));
		static_assert(reachable<N>(c7[0], c7[3], ref2));
		static_assert(reachable<N>(c7[0], c7[4], ref012));
		static_assert(reachable<N>(c7[0], c7[5], ref01));
		static_assert(reachable<N>(c7[0], c7[6], ref02));
		static_assert(reachable<N>(c7[0], c7[7], ref12));
		//rot || ref
		if (false) print_translations<N>(c7, all_refs, all_rots);

		//===========================
		//npn class #8; cardinality=3: 17=00010111; class size=8=(2^3)
		//equivalent BF: 00010111 00101011 01001101 01110001 10001110 10110010 11010100 11101000

		constexpr auto c8 = std::array<BF, 4>{0b00010111, 0b00101011, 0b01001101, 0b01110001};
		static_assert(reachable<N>(c8[0], c8[1], ref0));
		static_assert(reachable<N>(c8[0], c8[2], ref1));
		static_assert(reachable<N>(c8[0], c8[3], ref2));
		//rot || ref
		if (false) print_translations<N>(c8, all_refs, all_rots);

		//===========================
		//npn class #9; cardinality=3: 18=00011000; class size=8=(2^3)
		//equivalent BF: 00011000 00100100 01000010 01111110 10000001 10111101 11011011 11100111
		constexpr auto c9 = std::array<BF, 4>{0b00011000, 0b00100100, 0b01000010, 0b10000001};
		static_assert(reachable<N>(c9[0], c9[1], ref0));
		static_assert(reachable<N>(c9[0], c9[2], ref1));
		static_assert(reachable<N>(c9[0], c9[3], ref2));
		//rot || ref
		if (false) print_translations<N>(c9, all_refs, all_rots);
		
		//===========================
		//npn class #10; cardinality=3: 19=00011001; class size=48=(2^4 + 3^1)
		//equivalent BF: 00011001 00011010 00011100 00100101 00100110 00101100 00110100 00111000 00111101 00111110 01000011 01000110 01001010 01010010 01011000 01011011 01011110 01100010 01100100 01100111 01101110 01110110 01111010 01111100 10000011 10000101 10001001 10010001 10011000 10011011 10011101 10100001 10100100 10100111 10101101 10110101 10111001 10111100 11000001 11000010 11000111 11001011 11010011 11011001 11011010 11100011 11100101 11100110
		//both ref, rotation and complement leads to 48 permutations
		constexpr auto c10 = std::array<BF, 24>{0b00011001, 0b00011010, 0b00011100, 0b00100101, 0b00100110, 0b00101100, 0b00110100, 0b00111000, 0b01000011, 0b01000110, 0b01001010, 0b01010010, 0b01011000, 0b01100010, 0b01100100, 0b10000011, 0b10000101, 0b10001001, 0b10010001, 0b10011000, 0b10100001, 0b10100100, 0b11000001, 0b11000010};
		//rot && ref
		if (false) print_translations<N>(c10, all_refs, all_rots);

		//===========================
		//npn class #11; cardinality=3: 1B=00011011; class size=24=(2^3 + 3^1)
		//equivalent BF: 00011011 00011101 00100111 00101110 00110101 00111010 01000111 01001110 01010011 01011100 01110010 01110100 10001011 10001101 10100011 10101100 10110001 10111000 11000101 11001010 11010001 11011000 11100010 11100100
		constexpr auto c11 = std::array<BF, 12>{0b00011011, 0b00011101, 0b00100111, 0b00101110, 0b00110101, 0b00111010, 0b01000111, 0b01001110, 0b01010011, 0b01011100, 0b01110010, 0b01110100};
		// needs both ref with rot closure!

		//static_assert(reachable<N>(c11[0], c11[1], )); // needs closure of ref and rot!!
		//print_rotation<N>(c11[0], c11[1], search_space);
		static_assert(reachable<N>(c11[0], c11[2], ref0));
		static_assert(reachable<N>(c11[0], c11[3], rot10));
		static_assert(reachable<N>(c11[0], c11[4], rot02));
		//static_assert(reachable<N>(c11[0], c11[5], ));// needs closure of ref and rot!!
		static_assert(reachable<N>(c11[0], c11[6], rot01));
		static_assert(reachable<N>(c11[0], c11[7], ref1));
		//static_assert(reachable<N>(c11[0], c11[8], ));// needs closure of ref and rot!!
		static_assert(reachable<N>(c11[0], c11[9], rot01_21));
		static_assert(reachable<N>(c11[0], c11[10], rot12)); 
		static_assert(reachable<N>(c11[0], c11[10], ref02));
		//static_assert(reachable<N>(c11[0], c11[11], ));// needs closure of ref and rot!!

		//rot && ref with closure!
		if (false) print_translations<N>(c11, all_refs, all_rots);

		//===========================
		//npn class #12; cardinality=3: 1E=00011110; class size=24=(2^3 + 3^1)
		//equivalent BF: 00011110 00101101 00110110 00111001 01001011 01010110 01011001 01100011 01100101 01101010 01101100 01111000 10000111 10010011 10010101 10011010 10011100 10100110 10101001 10110100 11000110 11001001 11010010 11100001
		constexpr auto c12 = std::array<BF, 12>{0b00011110, 0b00101101, 0b00110110, 0b00111001, 0b01001011, 0b01010110, 0b01011001, 0b01100011, 0b01100101, 0b01101010, 0b01101100, 0b01111000 };

		//rot && ref
		if (true) print_translations<N>(c12, all_refs, all_rots);

		//===========================
		//npn class #13; cardinality=3: 69=01101001; class size=2=(2^1)
		//equivalent BF: 01101001 10010110
	}
	#pragma endregion


	template <int N> std::vector<std::string> impossible_transformations(const BF bf, const std::set<BF>& npn_set)
	{
		std::vector<std::string> result;
		if constexpr (N == 2) 
		{
			//constexpr auto translations_reflection = details::transitive_closure<N>({ reflect<N>::value(0), reflect<N>::value(1) });
			//constexpr auto translations_rotate = details::transitive_closure<N>(rotate<N>::value(0, 1));

			const Transformations<N>& get_transformations_from_cache = details::get_transformations_from_cache<N, false>();

			for (const BF other : npn_set) {
				if (other != bf) {

				}
			}
			return result;
		}
		return result;
	}

	void test()
	{
		constexpr int N = 4;
		constexpr BF bf1 = 0b0000011001110111; // original bf
		constexpr BF bf2 = 0b0000001111010111; // minimum of transitive closure
		constexpr BF bf3 = 0b0000011001110111; // greedy reduced

		const auto descr = create_descriptions<N>();
		{
			std::cout << "bf                  " << to_string_bin<N>(bf1) << std::endl;
		}
		{
			const auto pair = details::search_class_id_method0<N, true>(bf1);
			std::cout << "transitive closure: " << to_string_bin<N>(std::get<0>(pair)) << "; " << std::get<1>(pair) << std::endl;
		}
		{
			const auto pair = details::search_class_id_method1<N, true>(bf1);
			std::cout << "greedy rewrite    : " << to_string_bin<N>(std::get<0>(pair)) << "; " << std::get<1>(pair) << std::endl;
		}
		{
			const auto trans = find_transformation<N>(bf1, bf2, descr);
			std::cout << " bf1=" << to_string_bin<N>(bf1) << "; bf2=" << to_string_bin<N>(bf2);
			for (const auto& pair : trans) std::cout << to_string<N>(pair) << " ";
			std::cout << std::endl;
		}
		{
			const auto trans = find_transformation<N>(bf3, bf2, descr);
			std::cout << " bf3=" << to_string_bin<N>(bf3) << "; bf2=" << to_string_bin<N>(bf2);
			for (const auto& pair : trans) std::cout << to_string<N>(pair) << " ";
			std::cout << std::endl;
		}
	}

	template <int N> std::string tranformations_to_string(const std::vector<std::pair<CubeI<N>, std::string>>& transformations)
	{
		std::string result = "";
		for (const auto& pair : transformations)
		{
			if (result.length() > 0) result += " = ";
			result += std::get<1>(pair);
		}
		return result;
	}

	std::tuple<std::array<BF, 2>, std::array<std::string, 3>> boolean_expression_transform_3_2(
		const CubeI<3>& transformation,
		const std::array<BF, 2>& params,
		const std::array<std::string, 3>& descr)
	{
		constexpr int N = 3;

		constexpr auto r0 = reflect<N>(0);
		constexpr auto r1 = reflect<N>(1);
		constexpr auto r2 = reflect<N>(2);


		if (transformation == r0)
		{
			// BF3[00000001b] = BF2[0010b](a,BF2[0001b](b,c))
			// BF3[00000010b] = BF2[0010b](a,BF2[0010b](b,c))
			return { { params[0], reflect<2>(params[1], 0) }, descr };
		}
		if (transformation == r1)
		{
			// BF3[00000001b] = BF2[0010b](a,BF2[0001b](b,c))
			// BF3[00000010b] = BF2[0010b](a,BF2[0010b](b,c))
			return { { params[0], reflect<2>(params[1], 1) }, descr };
		}
		if (transformation == r2)
		{
			// BF3[00000001b] = BF2[0010b](a,BF2[0001b](b,c))
			// BF3[00000010b] = BF2[0010b](a,BF2[0010b](b,c))
			return { { reflect<2>(params[0], 1), params[1] }, descr };
		}

		std::cout << "WARNING: boolean_expression_transform_3_2: transformation " << to_string<N>(transformation) << " not implemented yet!";
		static_cast<void>(getchar());
		return { params, descr };
	}

	template <int N> std::set<BF> load_all_npn_classes(const std::string& filename) 
	{
		std::set<BF> results;
		if (std::filesystem::exists(filename)) {
			std::fstream myfile(filename, std::ios_base::in);
			BF bf;
			while (myfile >> bf)
			{
				results.insert(bf);
			}
			constexpr unsigned long long total_number_npn_classes = bf::cardinality_npn_class(N);
			if (results.size() != total_number_npn_classes) {
				std::cout << "WARNING: load_all_npn_classes the file " << filename << " has " << results.size() << " classes witch is not equal to the expected number of NPN classes " << total_number_npn_classes << std::endl;
				static_cast<void>(getchar());
			}
		}
		else {
			std::cout << "WARNING: load_all_npn_classes: filename " << filename << " does not exist" << std::endl;
		}
		return results;
	}

	template <int N> std::set<BF> load_all_npn_classes() 
	{
		if constexpr (N == 1) {
			return load_all_npn_classes<N>("C://Users//henk//Documents//Github//n-transformation//data//npn1.txt");
		}
		if constexpr (N == 2) {
			return load_all_npn_classes<N>("C://Users//henk//Documents//Github//n-transformation//data//npn2.txt");
		}
		if constexpr (N == 3) {
			return load_all_npn_classes<N>("C://Users//henk//Documents//Github//n-transformation//data//npn3.txt");
		}
		if constexpr (N == 4) {
			return load_all_npn_classes<N>("C://Users//henk//Documents//Github//n-transformation//data//npn4.txt");
		}
		if constexpr (N == 5) {
			return load_all_npn_classes<N>("C://Users//henk//Documents//Github//n-transformation//data//npn5.txt");
		}
		std::cout << "WARNING: load_all_npn_classes provided N " << N << " is not supported" << std::endl;
		return std::set<BF>();
	}

	template <int N> void save_all_npn_classes(const std::string& filename)
	{
		const std::set<BF> all_npn_classes = generate_all_npn_classes<N>(); // this may take a while...

		if (!std::filesystem::exists(filename)) {
			std::cout << "INFO: Creating new NPN class file " << filename << " with " << all_npn_classes.size() << " clasess in " << std::filesystem::current_path() << std::endl;
			std::fstream myfile(filename, std::fstream::out); // replace existing file
			if (myfile.good()) {
				for (const BF bf : all_npn_classes) {
					myfile << bf << std::endl;
				}
			}
			else {
				std::cout << "WARNING NPN class file " << filename << " not good" << std::endl;
			}
			myfile.close();
		}
		else {
			std::cout << "WARNING: NPN class file " << filename << " already exists in " << std::filesystem::current_path() << std::endl;
		}
	}

	template <int N, bool DESCR> void print_all_transformations()
	{
		std::cout << "Transformations obtained by transitive closure N=" << N << ":" << std::endl;
		const Transformations<N>& get_transformations_from_cache = details::get_transformations_from_cache<N, DESCR>();
		std::cout << "number of transformations: " << get_transformations_from_cache.size() << std::endl;

		// create a map such that the transformations are sorted
		std::map<CubeI<N>, std::string> trans2;
		for (const auto& pair : get_transformations_from_cache)
		{
			trans2.insert(pair);
		}
		for (const auto& pair : trans2)
		{
			std::cout << to_string<N>(pair) << std::endl;
		}
	}

	template <int N> void print_all_class_ids_with_values(const std::string& filename)
	{
		int class_counter = 0;
		std::cout << "All classes (with values) for N=" << N <<":" << std::endl;
		
		const std::set<BF> class_ids = load_all_npn_classes<N>(filename);

		for (int c = 0; c <= N; ++c)
		{
			for (const BF bf_class_id : class_ids)
			{
				const int cardinality = bf_tools::bf_cardinality(bf_class_id, N);

				if (cardinality == c)
				{
					const std::set<BF> set = equiv_class<N>(bf_class_id);
					const std::string factors = prime::vector_to_string(prime::get_factorization(static_cast<int>(set.size())));

					std::cout << "npn class #" << std::dec << class_counter;
					std::cout << "; cardinality=" << cardinality;
					std::cout << ": " << to_string_hex<N>(bf_class_id) << "=" << to_string_bin<N>(bf_class_id);
					std::cout << "; class size=" << set.size() << "=(" << factors << ")";
					std::cout << std::endl;

					constexpr bool show_npn_class_content = true;
					if (show_npn_class_content) 
					{
						std::cout << "equivalent BF: ";
						for (const BF equivalent_bf : set)
						{
							std::cout << to_string_bin<N>(equivalent_bf) << " ";
						}
						std::cout << std::endl;
					}

					constexpr bool show_transfomation_analysis = true;
					if (show_transfomation_analysis) 
					{
						std::cout << "transformation not possible: ";
						for (const std::string& str : impossible_transformations<N>(bf_class_id, set)) {
							std::cout << str << " ";
						}
						std::cout << std::endl;
					}

					std::cout << std::endl;
					std::cout << "===========================" << std::endl;
					class_counter++;
				}
			}
		}
	}
}
