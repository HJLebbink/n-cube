module;
#include <string>
#include <array>
#include <vector>
#include <utility> //pair
#include <iostream>		// std::cout
#include "CubeDef.h"

export module reachable;
import CubeIndex;
import n_cube;
import BF;
import reflect;
import rotate;
import transform;


namespace cube {

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
			std::cout << to_string_bin<N>(a) << " -> " << to_string_bin<N>(b) << " not possible" << std::endl;
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
		return function_composition<N>(a, b);
	}

	void test_reachability_bf2() {
		constexpr int N = 2;
		constexpr CubeI<N> ref0 = details::reflect<N, 0>();
		constexpr CubeI<N> ref1 = details::reflect<N, 1>();
		constexpr CubeI<N> ref01 = o<N>(ref0, ref1);

		constexpr CubeI<N> rot01 = details::rotate<N>(0, 1);
		constexpr CubeI<N> rot10 = details::rotate<N>(1, 0);
		constexpr auto empty = std::array<CubeI<N>, 0>{};

		constexpr CubeI<N> id = init_cubeI<N>();
		constexpr CubeI<N> x0 = transform<N>(id, ref0);
		constexpr CubeI<N> x1 = transform<N>(id, ref1);
		constexpr CubeI<N> x01 = transform<N>(id, rot01);




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

	void test_reachability_bf3()
	{
		constexpr int N = 3;
		constexpr CubeI<N> id = init_cubeI<N>();

		constexpr CubeI<N> ref0 = details::reflect<N, 0>();
		constexpr CubeI<N> ref1 = details::reflect<N, 1>();
		constexpr CubeI<N> ref2 = details::reflect<N, 2>();

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

		constexpr CubeI<N> rot01 = details::rotate<N>(0, 1);
		constexpr CubeI<N> rot02 = details::rotate<N>(0, 2);
		constexpr CubeI<N> rot12 = details::rotate<N>(1, 2);
		constexpr CubeI<N> rot10 = details::rotate<N>(1, 0);
		constexpr CubeI<N> rot20 = details::rotate<N>(2, 0);
		constexpr CubeI<N> rot21 = details::rotate<N>(2, 1);

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
}