module;
#include <set>
#include <array>
#include <string>
#include <filesystem>
#include <iostream>		// std::cout
#include <fstream>
#include <set>
#include <unordered_map>
#include <utility> // pair
#include "CubeDef.h"

export module plot_class;
import BF;
import Transformations;
import reflect;
import rotate;
import transform;
import array_tools;


namespace cube::plot {

	namespace {

		using Trans = std::string;

		template <typename T1, typename T2>
		struct Edge {
			T1 from;
			T1 to;
			std::set<T2> transitions;
			bool bidirectional;

			Edge(T1 from, T1 to, const T2& transition, bool bidirectional)
				: from(from), to(to), transitions(std::set<T2>{transition}), bidirectional(bidirectional)
			{}
			Edge(T1 from, T1 to, const std::set<T2>& transitions, bool bidirectional)
				: from(from), to(to), transitions(transitions), bidirectional(bidirectional)
			{}
		};

		// return true when a is a subset of b
		template <typename T>
		bool subset(const std::set<T>& a, const std::set<T>& b) noexcept {
			for (const T& str_a : a) {
				if (!b.contains(str_a)) return false;
			}
			return true;
		}

		template <typename T>
		std::string to_string(const std::set<T>& transitions)
		{
			std::string label = "";
			const int count = static_cast<int>(transitions.size()) - 1;

			int counter = 0;
			for (const T& t : transitions) {
				label += t;// std::to_string(t);
				if (counter < count) {
					label += "\\n";
				}
				counter++;
			}
			return label;
		}

		template <typename T1, typename T2>
		std::vector<Edge<T1, T2>> remove_bidirectonal(const std::vector<Edge<T1, T2>>& edges)
		{
			std::vector<Edge<T1, T2>> results;
			for (const Edge<T1, T2>& e : edges)
			{
				bool found = false;
				for (Edge<T1, T2>& e2 : results)
				{
					if ((e2.from == e.to) && (e2.to == e.from) && (e2.transitions == e.transitions))
					{	// found an existing edge that is pointing the other way around
						e2.bidirectional = true;
						found = true;
						break;
					}
				}
				if (!found) {
					results.push_back(e);
				}
			}
			return results;
		}

		template <typename T1, typename T2>
		std::vector<Edge<T1, T2>> remove_equal(const std::vector<Edge<T1, T2>>& edges) {
			std::vector<Edge<T1, T2>> results;
			for (const Edge<T1, T2>& e : edges) {
				bool found = false;
				for (Edge<T1, T2>& e2 : results) {
					if ((e.from == e2.from) && (e.to == e2.to) && (e.bidirectional == e2.bidirectional)) {
						for (const Trans& t : e.transitions) {
							e2.transitions.insert(t);
							found = true;
							break;
						}
					}
				}
				if (!found) {
					results.push_back(e);
				}
			}
			return results;
		}

		template <int N, typename T1, typename T2>
		void write_to_dot_file(const std::vector<Edge<T1, T2>>& edges, const std::string& filename)
		{
			//const bool already_exists = std::filesystem::exists(filename);
			const bool already_exists = false;

			if (!already_exists) {
				std::fstream myfile(filename, std::fstream::out); // replace existing file
				if (myfile.good())
				{
					myfile << "digraph G {" << std::endl;
					for (const Edge<T1, T2>& edge : edges)
					{
						const std::string label = "label=\"" + to_string(edge.transitions) + "\"";
						const std::string bidirectional = (edge.bidirectional) ? " dir=both" : "";
						const std::string line = to_string_bin<N>(edge.from) + " -> " + to_string_bin<N>(edge.to) + " [" + label + bidirectional + " fontsize=10];";
						std::cout << line << std::endl;
						myfile << line << std::endl;
					}
					myfile << "}" << std::endl;
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
	}

	export template <int N>
	void plot_npn_classes(
		const std::set<BF>& equiv_class,
		const std::string& filename)
	{
		const Transf<N> transformations = create_transformations<N, true>();
		std::vector<std::pair<CubeI<N>, int>> transformations2;

		int transformation_id = 0;
		for (auto& pair : transformations) {
			std::cout << transformation_id << " = " << cube::to_string<N>(std::get<0>(pair)) << " = " << std::get<1>(pair) << std::endl;
			transformations2.push_back(std::make_pair(std::get<0>(pair), transformation_id));
			transformation_id++;
		}

		std::vector<Edge<BF, Trans>> all_edges;
		for (const BF bf : equiv_class)
		{
			for (const auto& pair : transformations)
			{
				const BF next_bf = transform<N>(bf, std::get<0>(pair));
				all_edges.push_back(Edge(bf, next_bf, std::get<1>(pair), false));
			}
		}

		const std::vector<Edge<BF, Trans>> edges1 = remove_equal(all_edges);
		const std::vector<Edge<BF, Trans>> edges2 = remove_bidirectonal(edges1);
		write_to_dot_file<N>(edges2, filename);
	}

	template <int N>
	void plot_npn_outgoing_edges(
		const BF class_id,
		const std::set<BF>& equiv_class,
		const std::string& filename)
	{
		const Transformations<N> transformations = create_transformations<N, true>();
		std::vector<std::pair<CubeI<N>, int>> transformations2;

		int transformation_id = 0;
		for (auto& pair : transformations) {
			std::cout << transformation_id << " = " << cube::to_string<N>(std::get<0>(pair)) << " = " << std::get<1>(pair) << std::endl;
			transformations2.push_back(std::make_pair(std::get<0>(pair), transformation_id));
			transformation_id++;
		}

		std::vector<Edge<BF, Trans>> all_edges;
		for (const BF bf : equiv_class)
		{
			for (const auto& pair : transformations2)
			{
				const BF next_bf = transform<N>(bf, std::get<0>(pair));
				all_edges.push_back(Edge(bf, next_bf, std::get<1>(pair), false));
			}
		}

		//const std::vector<Edge<Trans>> edges1 = remove_equal(all_edges);
		///const std::vector<Edge<Trans>> edges2 = remove_bidirectonal(edges1);


		std::set<Trans> outgoing_edges;
		for (const Edge<BF, Trans>& e : all_edges) {
			if (e.from == class_id) {
				for (auto t : e.transitions) outgoing_edges.insert(t);
			}
		}
		std::cout << to_string_bin<N>(class_id) + " (" << outgoing_edges.size() << ") = ";
		for (const Trans& t : outgoing_edges) {
			std::cout << t << "; ";
		}
		std::cout << std::endl;

	}


	void tranformations_partitions2()
	{
		constexpr int N = 2;
		constexpr CubeI<N> r0 = details::reflect<N>::valueY<0>(); //1032
		constexpr CubeI<N> r1 = details::reflect<N>::valueY<1>(); //2301
		constexpr CubeI<N> r01 = details::rotate<N>::valueX<0, 1>(); //1302
		constexpr CubeI<N> r10 = details::rotate<N>::valueX<1, 0>(); //2031 

		{	// size 2 (3 of 3 tranformations are idempotent) 
			constexpr BF c0 = 0b0000;
			static_assert(transform<N>(c0, r0) == c0); // r0 does nothing
			static_assert(transform<N>(c0, r1) == c0);
			static_assert(transform<N>(c0, r10) == c0);
			static_assert(transform<N>(c0, r01) == c0);
		}
		{	// size 4 (2 of 3 tranformations are idempotent)
			constexpr BF c1 = 0b0011;
			static_assert(transform<N>(c1, r0) == c1); // r0 does nothing
			static_assert(transform<N>(c1, r1) == 0b1100); // equal to negation
			static_assert(transform<N>(c1, r01) == 0b0101);
			static_assert(transform<N>(c1, r10) == 0b1010);
		}
		{	// size 8 (1 of 3 tranformations are idempotent) 
			constexpr BF c2 = 0b0001;
			static_assert(transform<N>(c2, r0) == 0b0010);
			static_assert(transform<N>(c2, r10) == 0b0010);

			static_assert(transform<N>(c2, r1) == 0b0100);
			static_assert(transform<N>(c2, r01) == 0b0100);

		//	static_assert(transform<N>(c2, apply<N>(r0, r1)) == 0b1000);
		//	static_assert(transform<N>(c2, apply<N>(r0, r10)) == 0b1000);
		//	static_assert(transform<N>(c2, apply<N>(r1, r01)) == 0b1000);
		}
		{	// size 2 (3 of 3 tranformations are idempotent) 
			constexpr BF c3 = 0b0110;
			static_assert(transform<N>(c3, r0) == 0b1001); // equal to negation
			static_assert(transform<N>(c3, r1) == 0b1001);
			static_assert(transform<N>(c3, r10) == 0b1001);
			static_assert(transform<N>(c3, r01) == 0b1001);
		}
	}

	void tranformations_partitions3()
	{
		constexpr int N = 3;
		constexpr CubeI<N> r0 = details::reflect<N>::valueY<0>();
		constexpr CubeI<N> r1 = details::reflect<N>::valueY<1>();
		constexpr CubeI<N> r2 = details::reflect<N>::valueY<2>();
		constexpr CubeI<N> r01 = details::rotate<N>::valueX<0, 1>();
		constexpr CubeI<N> r02 = details::rotate<N>::valueX<0, 2>();
		constexpr CubeI<N> r12 = details::rotate<N>::valueX<1, 2>();

		constexpr CubeI<N> r10 = details::rotate<N>::valueX<1, 0>();
		constexpr CubeI<N> r20 = details::rotate<N>::valueX<2, 0>();
		constexpr CubeI<N> r21 = details::rotate<N>::valueX<2, 1>();

		constexpr CubeI<N> id = init_cubeI<N>();
		constexpr CubeI<N> a = transform<N>(id, r0);

		//TODO
		/*
		0
		1
		3
		6
		7
		15
		22
		23
		24
		25
		27
		30
		60
		105
*/
		{	// size ? (4 of 6 tranformations are idempotent) 
			constexpr BF c3 = 0b0011'1100; //60

			static_assert(transform<N>(c3, r0) == c3); // idempotent
			static_assert(transform<N>(c3, r1) == 0b1100'0011);  // equal to negation
			static_assert(transform<N>(c3, r2) == 0b1100'0011);  // equal to negation

			static_assert(transform<N>(c3, r01) == 0b0101'1010); // new 1
			static_assert(transform<N>(c3, r02) == 0b0110'0110); // new 2
			static_assert(transform<N>(c3, r12) == 0b1100'0011); // equal to negation

			static_assert(transform<N>(c3, r10) == 0b1010'0101); // negation of new 1
			static_assert(transform<N>(c3, r20) == 0b1001'1001); // negation of new 2
			static_assert(transform<N>(c3, r21) == 0b1100'0011); // equal to negation

			constexpr BF x = transform<N>(c3, r20);


		}
		{	// size 2 (6 of 6 tranformations are idempotent) 
			constexpr BF c3 = 0b0110'1001;
			static_assert(transform<N>(c3, r0) == 0b1001'0110); // equal to negation
			static_assert(transform<N>(c3, r1) == 0b1001'0110);
			static_assert(transform<N>(c3, r2) == 0b1001'0110);
			static_assert(transform<N>(c3, r01) == 0b1001'0110);
			static_assert(transform<N>(c3, r02) == 0b1001'0110);
			static_assert(transform<N>(c3, r12) == 0b1001'0110);

			static_assert(transform<N>(c3, r10) == 0b1001'0110);
			static_assert(transform<N>(c3, r20) == 0b1001'0110);
			static_assert(transform<N>(c3, r21) == 0b1001'0110);
		}

	}

	void rot2_tranformations() {
		constexpr int N = 2;
		constexpr CubeI<N> r0 = details::reflect<N>::valueY<0>();
		constexpr CubeI<N> r1 = details::reflect<N>::valueY<1>();

		constexpr CubeI<N> r0o1 = transform<N>(r0, r1);

		constexpr CubeI<N> r01 = details::rotate<N>::valueX<0, 1>();
		constexpr CubeI<N> r10 = details::rotate<N>::valueX<1, 0>();


		constexpr CubeI<N> id = init_cubeI<N>();

		constexpr CubeI<N> x0_0 = id;                      // 0123
		constexpr CubeI<N> x0_1 = transform<N>(x0_0, r01); // 1302
		constexpr CubeI<N> x0_2 = transform<N>(x0_1, r01); // 3210
		constexpr CubeI<N> x0_3 = transform<N>(x0_2, r01); // 2031

		constexpr CubeI<N> x1_0 = transform<N>(x0_0, r0); // 1031
		constexpr CubeI<N> x1_1 = transform<N>(x0_1, r0); // 3120
		constexpr CubeI<N> x1_2 = transform<N>(x0_2, r0); // 2301
		constexpr CubeI<N> x1_3 = transform<N>(x0_3, r0); // 0213

		constexpr CubeI<N> x2_0 = transform<N>(x1_0, r10); // 0213
		constexpr CubeI<N> x2_1 = transform<N>(x1_1, r0); //
		constexpr CubeI<N> x2_2 = transform<N>(x1_2, r0); //
		constexpr CubeI<N> x2_3 = transform<N>(x1_3, r0); //



//			static_assert(array_tools::equal(x2_))
	}

	void rot3_tranformations() {
		constexpr int N = 3;
		constexpr CubeI<N> r0 = details::reflect<N>::valueY<0>();
		constexpr CubeI<N> r1 = details::reflect<N>::valueY<1>();
		constexpr CubeI<N> r2 = details::reflect<N>::valueY<2>();

		constexpr CubeI<N> r0o1 = transform<N>(r0, r1);
		constexpr CubeI<N> r0o2 = transform<N>(r0, r2);
		constexpr CubeI<N> r1o2 = transform<N>(r1, r2);

		constexpr CubeI<N> r01 = details::rotate<N>::valueX<0, 1>();
		constexpr CubeI<N> r02 = details::rotate<N>::valueX<0, 2>();
		constexpr CubeI<N> r12 = details::rotate<N>::valueX<1, 2>();

		constexpr CubeI<N> r10 = details::rotate<N>::valueX<1, 0>();
		constexpr CubeI<N> r20 = details::rotate<N>::valueX<2, 0>();
		constexpr CubeI<N> r21 = details::rotate<N>::valueX<2, 1>();


		constexpr CubeI<N> id = init_cubeI<N>();

		constexpr CubeI<N> x0_0 = id;                      // 01234567
		constexpr CubeI<N> x0_1 = transform<N>(x0_0, r01); // 13025646
		constexpr CubeI<N> x0_2 = transform<N>(x0_1, r01); // 32107654
		constexpr CubeI<N> x0_3 = transform<N>(x0_2, r01); // 20316475
		static_assert(array_tools::equal(x0_0, transform<N>(x0_3, r01)));

		constexpr CubeI<N> x1_0 = id;                      // 01234567
		constexpr CubeI<N> x1_1 = transform<N>(x1_0, r02); // 15370426
		constexpr CubeI<N> x1_2 = transform<N>(x1_1, r02); // 54761032
		constexpr CubeI<N> x1_3 = transform<N>(x1_2, r02); // 40625173
		static_assert(array_tools::equal(x1_0, transform<N>(x1_3, r02)));

		constexpr CubeI<N> x2_0 = id;                      // 01234567
		constexpr CubeI<N> x2_1 = transform<N>(x2_0, r12); // 23670145
		constexpr CubeI<N> x2_2 = transform<N>(x2_1, r12); // 67452301
		constexpr CubeI<N> x2_3 = transform<N>(x2_2, r12); // 45016723
		static_assert(array_tools::equal(x2_0, transform<N>(x2_3, r12)));

		//			static_assert(array_tools::equal(x2_2, transform<N>(id, r1)));		
		//			constexpr CubeI<N> y2_1 = transform<N>(x2_1, ); 
		//			constexpr CubeI<N> y2_1 = transform<N>(x2_1, r0o1); 
	}


	// plot the cube of transformations
	export template <int N>
	void plot_transformation_transitions(
		const std::string& filename)
	{
		using T1 = std::string;
		using T2 = std::string;

		const Transf<N> all_transformations = create_transformations<N, true>();
		const Transf<N> atomic_transformations = details::create_transformations_X<N>(create_descriptions<N>());

		std::vector<Edge<T1, T2>> all_edges;
		for (const std::pair<CubeI<N>, std::string>& transition1 : all_transformations)
		{
			const CubeI<N> node1 = std::get<0>(transition1);
			for (const std::pair<CubeI<N>, std::string>& transition2 : all_transformations)
			{
				const CubeI<N> t2 = transform<N>(node1, std::get<0>(transition2));

				std::string node2_name = "??";
				for (const std::pair<CubeI<N>, std::string>& transition : all_transformations) {
					if (array_tools::equal(std::get<0>(transition), t2)) {
						node2_name = std::get<1>(transition);
						break;
					}
				}
				const std::string node1_name = std::get<1>(transition1);
				const std::string label = std::get<1>(transition2);
				all_edges.push_back(Edge<T1, T2>(node1_name, node2_name, label, false));
			}
		}

		const std::vector<Edge<T1, T2>> edges1 = remove_equal(all_edges);
		const std::vector<Edge<T1, T2>> edges2 = remove_bidirectonal(edges1);


		write_to_dot_file<N>(edges2, filename);
	}
}