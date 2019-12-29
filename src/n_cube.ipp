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

#include "Cube.h"
#include "bf_tools.ipp"
#include "array_tools.h"


namespace bf {
	constexpr long long cardinality_npn_class(int n) {
		switch (n) {
		case 1: return 2;
		case 2: return 4;
		case 3: return 14;
		case 4: return 222;
		case 5: return 616126;
		case 6: return 200254952537184;
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
				result << count << "*" << p << " + ";
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

		#pragma region functional composition
		template <int N, int M> struct composition_struct {};
		template <int N> struct composition_struct<N, 1>
		{
			static constexpr CubeI<N> value(const std::array<CubeI<N>, 1>& a) noexcept
			{
				return a[0];
			}
		};
		template <int N> struct composition_struct<N, 2>
		{
			static constexpr CubeI<N> value(const CubeI<N>& c0, const CubeI<N>& c1) noexcept
			{
				return details::apply_struct<N>::value(c0, c1);
			}
			static constexpr CubeI<N> value(const std::array<CubeI<N>, 2>& a) noexcept
			{
				return value(a[0], a[1]);
			}
		};
		template <int N> struct composition_struct<N, 3>
		{
			static constexpr CubeI<N> value(const std::array<CubeI<N>, 3>& a) noexcept
			{
				return composition_struct<N, 2>::value(a[0], composition_struct<N, 2>::value(a[1], a[2]));
			}
		};
		template <int N> struct composition_struct<N, 4>
		{
			static constexpr CubeI<N> value(const std::array<CubeI<N>, 4>& a) noexcept
			{
				return composition_struct<N, 2>::value(composition_struct<N, 2>::value(a[0], a[1]), composition_struct<N, 2>::value(a[2], a[3]));
			}
		};

		template <int N, int M> constexpr CubeI<N> function_composition(const std::array<CubeI<N>, M>& a) noexcept
		{
			return composition_struct<N, M>::value(a);
		}
		template <int N> constexpr CubeI<N> function_composition(const CubeI<N>& c1, const CubeI<N>&c2) noexcept
		{
			return function_composition<N>(std::array<CubeI<N>, 2>{ c1, c2 });
		}
		template <int N> constexpr CubeI<N> function_composition(const CubeI<N>& c1, const CubeI<N>&c2, const CubeI<N>&c3) noexcept
		{
			return function_composition<N>(std::array<CubeI<N>, 3>{ c1, c2, c3 });
		}
		template <int N> constexpr CubeI<N> function_composition(const CubeI<N>& c1, const CubeI<N>&c2, const CubeI<N>&c3, const CubeI<N>&c4) noexcept
		{
			return function_composition<N>(std::array<CubeI<N>, 4>{ c1, c2, c3, c4 });
		}
		#pragma endregion

		#pragma region complement
		template <int N> struct complement_struct
		{
			static constexpr BF value(const BF bf)
			{
				std::cout << "ERROR: complement: dim=" << N << " not implemented yet" << std::endl;
				getchar();
				return ~bf;
			}
			static constexpr BF make_cannonical(const BF bf)
			{
				std::cout << "ERROR: make_cannonical: dim=" << N << " not implemented yet" << std::endl;
				getchar();
				return ~bf;
			}
		};
		template <> struct complement_struct<1>
		{
			static constexpr BF value(const BF bf) noexcept { return ~bf & 0b11; }
		};
		template <> struct complement_struct<2>
		{
			static constexpr BF value(const BF bf) noexcept { return ~bf & 0xF; }
		};
		template <> struct complement_struct<3>
		{
			static constexpr BF value(const BF bf) noexcept { return ~bf & 0xFF; }
		};
		template <> struct complement_struct<4>
		{
			static constexpr BF value(const BF bf) noexcept { return ~bf & 0xFFFF; }
		};
		template <> struct complement_struct<5>
		{
			static constexpr BF value(const BF bf) noexcept { return ~bf & 0xFFFFFFFF; }
		};
		template <> struct complement_struct<6>
		{
			static constexpr BF value(const BF bf) noexcept { return ~bf; }
		};
		#pragma endregion

		#pragma region transform
		template <int N> struct apply_struct
		{
			static constexpr CubeI<N> value(const CubeI<N>& c, [[maybe_unused]] const CubeI<N>& ci)
			{
				std::cout << "ERROR: apply_struct: dim=" << N << " not implemented yet" << std::endl;
				getchar();
				return c;
			}
			static constexpr CubeI<N> value(const Cube<N>& c, const CubeI<N>& ci)
			{
				std::cout << "ERROR: apply_struct: dim=" << N << " not implemented yet" << std::endl;
				getchar();
				return c;
			}
		};
		template <> struct apply_struct<1>
		{
			static constexpr int N = 1;
			static constexpr CubeI<N> value(const CubeI<N>& c, const CubeI<N>& ci) noexcept
			{
				return CubeI<N>{ c[ci[0]], c[ci[1]] };
			}
			static constexpr Cube<N> value(const Cube<N>& c, const CubeI<N>& ci) noexcept
			{
				return Cube<N>{  std::array<bool, (1 << N)>{c[ci[0]], c[ci[1]] }};
			}
		};
		template <> struct apply_struct<2>
		{
			static constexpr int N = 2;
			static constexpr CubeI<N> value(const CubeI<N>& c, const CubeI<N>& ci) noexcept
			{
				return CubeI<N>{ c[ci[0]], c[ci[1]], c[ci[2]], c[ci[3]] };
			}
			static constexpr Cube<N> value(const Cube<N>& c, const CubeI<N>& ci) noexcept
			{
				return Cube<N>{ std::array<bool, (1<<N)>{c[ci[0]], c[ci[1]], c[ci[2]], c[ci[3]] }};
			}
		};
		template <> struct apply_struct<3>
		{
			static constexpr int N = 3;
			static constexpr CubeI<N> value(const CubeI<N>& c, const CubeI<N>& ci) noexcept
			{
				return CubeI<N>{ c[ci[0]], c[ci[1]], c[ci[2]], c[ci[3]], c[ci[4]], c[ci[5]], c[ci[6]], c[ci[7]] };
			}
			static constexpr Cube<N> value(const Cube<N>& c, const CubeI<N>& ci) noexcept
			{
				return Cube<N>{ std::array<bool, (1 << N)>{c[ci[0]], c[ci[1]], c[ci[2]], c[ci[3]], c[ci[4]], c[ci[5]], c[ci[6]], c[ci[7]] }};
			}
		};
		template <> struct apply_struct<4>
		{
			static constexpr int N = 4;
			static constexpr CubeI<N> value(const CubeI<N>& c, const CubeI<N>& ci) noexcept
			{
				return CubeI<N>{
					c[ci[0]], c[ci[1]], c[ci[2]], c[ci[3]], c[ci[4]], c[ci[5]], c[ci[6]], c[ci[7]],
					c[ci[8]], c[ci[9]], c[ci[10]], c[ci[11]], c[ci[12]], c[ci[13]], c[ci[14]], c[ci[15]]
				};
			}
			static constexpr Cube<N> value(const Cube<N>& c, const CubeI<N>& ci) noexcept
			{
				return Cube<N>{ std::array<bool, (1 << N)>{
					c[ci[0]], c[ci[1]], c[ci[2]], c[ci[3]], c[ci[4]], c[ci[5]], c[ci[6]], c[ci[7]],
						c[ci[8]], c[ci[9]], c[ci[10]], c[ci[11]], c[ci[12]], c[ci[13]], c[ci[14]], c[ci[15]]
				}};
			}
			static __m128i value_opt(const __m128i c, const __m128i ci) noexcept
			{
				const __m128i shuff = _mm_shuffle_epi8(c, ci);
				return shuff;
			}
		};
		template <> struct apply_struct<5>
		{
			static constexpr int N = 5;
			static constexpr CubeI<N> value(const CubeI<N>& c, const CubeI<N>& ci) noexcept
			{
				return CubeI<N>{
					c[ci[0]], c[ci[1]], c[ci[2]], c[ci[3]], c[ci[4]], c[ci[5]], c[ci[6]], c[ci[7]],
					c[ci[8]], c[ci[9]], c[ci[10]], c[ci[11]], c[ci[12]], c[ci[13]], c[ci[14]], c[ci[15]],
					c[ci[16]], c[ci[17]], c[ci[18]], c[ci[19]], c[ci[20]], c[ci[21]], c[ci[22]], c[ci[23]],
					c[ci[24]], c[ci[25]], c[ci[26]], c[ci[27]], c[ci[28]], c[ci[29]], c[ci[30]], c[ci[31]]
				};
			}
			static constexpr Cube<N> value(const Cube<N>& c, const CubeI<N>& ci) noexcept
			{
				return Cube<N>{ std::array<bool, (1 << N)>{
					c[ci[0]], c[ci[1]], c[ci[2]], c[ci[3]], c[ci[4]], c[ci[5]], c[ci[6]], c[ci[7]],
						c[ci[8]], c[ci[9]], c[ci[10]], c[ci[11]], c[ci[12]], c[ci[13]], c[ci[14]], c[ci[15]],
						c[ci[16]], c[ci[17]], c[ci[18]], c[ci[19]], c[ci[20]], c[ci[21]], c[ci[22]], c[ci[23]],
						c[ci[24]], c[ci[25]], c[ci[26]], c[ci[27]], c[ci[28]], c[ci[29]], c[ci[30]], c[ci[31]]
				}};
			}
		};
		template <> struct apply_struct<6>
		{
			static constexpr int N = 6;
			static constexpr CubeI<N> value(const CubeI<N>& c, const CubeI<N>& ci) noexcept
			{
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
			static constexpr Cube<N> value(const Cube<N>& c, const CubeI<N>& ci) noexcept
			{
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
		};
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
		template <int N, bool DESCR> const Transformations<N>& all_transformations();

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
				const std::string d0 = descr[0];
				const std::string d1 = descr[1];

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
				const std::string d0 = descr[0];
				const std::string d1 = descr[1];
				const std::string d2 = descr[2];

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
				const std::string d0 = descr[0];
				const std::string d1 = descr[1];
				const std::string d2 = descr[2];
				const std::string d3 = descr[3];

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
				const std::string d0 = descr[0];
				const std::string d1 = descr[1];
				const std::string d2 = descr[2];
				const std::string d3 = descr[3];
				const std::string d4 = descr[4];

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
				const std::string d0 = descr[0];
				const std::string d1 = descr[1];
				const std::string d2 = descr[2];
				const std::string d3 = descr[3];
				const std::string d4 = descr[4];
				const std::string d5 = descr[5];

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
			std::set<CubeI<N>>& results)
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
			std::map<CubeI<N>, std::string>& results)
		{
			const int s = static_cast<int>(transformations.size());
			for (int i = start_index; i < s; ++i)
			{
				const CubeI<N> cube_new = details::function_composition<N>(cube, std::get<0>(transformations[i]));
				const std::string descr_new = descr + "." + std::get<1>(transformations[i]);
				results.insert(std::make_pair(cube_new, descr_new));
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
						std::get<N>(transformations_greedy_cache) = details::all_transformations<N, false>();
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
						std::get<N>(transformations_greedy_cache) = details::all_transformations<N, false>();
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
						std::get<N>(transformations_greedy_cache) = details::all_transformations<N, false>();
					}
				}
				return std::get<N>(transformations_greedy_cache);
			}
		};
		#pragma endregion

		#pragma region all transformations
		static std::mutex tranformations_cache_mutex;
		static std::tuple<Transformations<0>, Transformations<1>, Transformations<2>, Transformations<3>, Transformations<4>, Transformations<5>, Transformations<6>> tranformations_cache;

		template <int N, bool DESCR> 
		const Transformations<N>& all_transformations()
		{
			if (std::get<N>(tranformations_cache).empty())
			{
				if constexpr (DESCR)
				{
					std::get<N>(tranformations_cache) = details::transitive_closure<N>(details::create_transformations_struct<N>::value(create_descriptions<N>()));
				}
				else
				{
					std::get<N>(tranformations_cache) = details::transitive_closure<N>(details::create_transformations_struct<N>::value());
				}
			}
			return std::get<N>(tranformations_cache);
		}
		#pragma endregion

	} // end namespace details


	template <int N> 
	void init_n_cube() {
		// run all_transformations once to fill the transformation cache
		const auto all_transformations = details::all_transformations<N, false>();
	}


	template <int N> std::array<std::string, N> create_descriptions()
	{
		if constexpr (N == 1) {
			return { "x0" };
		}
		if constexpr (N == 2) {
			return { "b", "a" };
		}
		if constexpr (N == 3) {
			return { "c", "b", "a" };
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
	template <int N> constexpr BF complement(const BF bf)
	{
		return details::complement_struct<N>::value(bf);
	}
	template <int N> constexpr BF complement_if_needed(const BF bf)
	{
		constexpr int N2 = (1<<N) / 2;
		return (details::count_bits(bf) <= N2) ? bf : complement<N>(bf);
	}

	#pragma region transform
	// Transform (shuffle) cube c according to the change as provided by cube ci
	template <int N> constexpr Cube<N> transform(const Cube<N>& c, const CubeI<N>& ci)
	{
		return details::apply_struct<N>::value(c, ci);
	}
	// Transform (shuffle) cube c according to the change as provided by cube ci
	template <int N> constexpr CubeI<N> transform(const CubeI<N>& c, const CubeI<N>& ci)
	{
		return details::apply_struct<N>::value(c, ci);
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
			for (const auto& pair : details::all_transformations<N, DESCR>())
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
	template <int N> constexpr std::set<BF> equiv_class(const BF bf)
	{
		const auto all_transformations = details::all_transformations<N, false>();

		std::set<BF> result;
		for (const auto& pair : all_transformations)
		{
			const CubeI<N> cube = std::get<0>(pair);
			const BF bf_new = transform<N>(bf, cube);

			result.insert(bf_new);
			result.insert(complement<N>(bf_new));
		}
		return result;
	}

	#pragma region all class ids
	namespace details
	{
		//Naive method: iterates over all bf of cardinality >=N
		template <int N> constexpr std::set<BF> all_class_ids_method0(const std::string& filename)
		{
			constexpr long long max_bf = 1ll << (1 << N);
			constexpr long long update_interval = 0x3FFFF;
			constexpr int n_threads = 12;

			const time_t start_time_sec = time(nullptr);

			std::array<std::set<BF>, n_threads> results_per_thread;
			std::array<long long, n_threads> counter_per_thread;
			counter_per_thread.fill(0);

			std::atomic<long long> global_counter = 0;
			std::mutex global_results_mutex;
			std::set<BF> global_results;

			constexpr long long total_number_npn_classes = bf::cardinality_npn_class(N);

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
					std::cout << "percentage done : " << percentage_done << "; NPN classes not found : " << (total_number_npn_classes-number_npn_classes) << "; needed_time : " << needed_time_sec << " sec = " << needed_time_sec / 60 << " min = " << needed_time_sec / (60 * 60) << " hour " << std::endl;
					
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

			if (filename != "") {
				std::ofstream myfile(filename);
				for (const BF bf : global_results) {
					myfile << bf << std::endl;
				}
			}
			return global_results;
		}
		template <int N> std::set<BF> all_class_ids_method1()
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
	template <int N> std::set<BF> all_class_ids(const std::string& filename = "")
	{
		const bool use_method0 = true;
		return (use_method0)
			? details::all_class_ids_method0<N>(filename)
			: details::all_class_ids_method1<N>();
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
		for (const auto& t : details::all_transformations<N, false>())
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
	template <int N> std::vector<std::pair<CubeI<N>, std::string>> find_transformation(const BF bf1, const BF bf2, const std::array<std::string, N>& /*descr*/)
	{
		std::vector<std::pair<CubeI<N>, std::string>> results;

		const BF bf1b = complement_if_needed<N>(bf1);
		const BF bf2b = complement_if_needed<N>(bf2);

		const Cube<N> cube1 = Cube<N>(bf1b);
		const Cube<N> cube2 = Cube<N>(bf2b);

		const auto transformations = details::all_transformations<N, true>();
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

	template <int N> void print_all_transformations()
	{
		std::cout << "Transformations obtained by transitive closure N=" << N << ":" << std::endl;
		const auto all_transformations = details::all_transformations<N, false>();

		// create a map such that the transformations are sorted
		std::map<CubeI<N>, std::string> trans2;
		for (const auto& pair : all_transformations)
		{
			trans2.insert(pair);
		}

		for (const auto& pair : trans2)
		{
			std::cout << to_string<N>(pair) << std::endl;
		}
		std::cout << "number of transformations: " << all_transformations.size() << std::endl;
	}
	template <int N> void print_all_class_ids_with_values()
	{
		int bf_counter = 0;
		int class_counter = 0;
		std::cout << "All classes (with values) for N=" << N <<":" << std::endl;

		init_n_cube<N>();
		
		const auto class_ids = all_class_ids<N>();

		for (int c = 0; c <= N; ++c)
		{
			for (const auto bf_class_id : class_ids)
			{
				const auto cardinality = bf_tools::bf_cardinality(bf_class_id, N);

				if (cardinality == c)
				{
					const auto set = equiv_class<N>(bf_class_id);
					const std::string factors = prime::vector_to_string(prime::get_factorization(static_cast<int>(set.size())));

					std::cout << "class #" << std::dec << class_counter << ": cardinality " << cardinality << ": " << to_string_hex<N>(bf_class_id) << "=" << to_string_bin<N>(bf_class_id) << ": class set size:" << set.size() << " = [" << factors << "]";
					for ([[maybe_unused]] const auto& bf : set)
					{
						//std::cout << to_string_bin<N>(bf) << " ";
						//std::cout << std::uppercase << std::hex << std::setfill('0') << std::setw(2) << bf << " ";
						bf_counter++;
					}
					std::cout << std::endl;
					class_counter++;
				}
			}
		}
		std::cout << "total number of bf:" << std::dec << bf_counter << std::endl;
	}

	template <int N> void print_all_class_ids(const std::string& filename = "")
	{
		long long bf_counter = 0;
		int class_counter = 0;
		std::cout << "All NPN classes for N=" << N <<":" << std::endl;

		init_n_cube<N>();

		const auto class_ids = all_class_ids<N>(filename);

		for (int c = 0; c <= N; ++c)
		{
			for (const auto bf_class_id : class_ids)
			{
				const auto cardinality = bf_tools::bf_cardinality(bf_class_id, N);

				if (cardinality == c)
				{
					std::cout << "class #" << std::dec << class_counter << ": cardinality " << cardinality << ": " << to_string_hex<N>(bf_class_id) << "=" << to_string_bin<N>(bf_class_id);
					std::cout << std::endl;
					class_counter++;
				}
			}
		}
		std::cout << "total number of bf:" << std::dec << bf_counter << std::endl;
	}
}
