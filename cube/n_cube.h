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

#include "CubeI.h"
#include "bf_tools.h"
#include "array_tools.h"
#include "constants.h"
#include "prime.h"
#include "BF.h"
#include "transform.h"
#include "reflect.h"
#include "rotate.h"
#include "rewrite.h"
#include "Transformations.h"

namespace cube
{
	template<int DIM>
	using CubeMap = std::map<CubeI<DIM>, std::string>;

	namespace details
	{
		#pragma region transitive closure
		template <int N, int M> void transitive_closure_recursive_no_descriptions(
			const int start_index,
			const std::array<CubeI<N>, M>& transformations,
			const CubeI<N>& cube,
			INOUT std::set<CubeI<N>>& results)
		{
			for (int i = start_index; i < M; ++i)
			{
				const CubeI<N> cube_new = function_composition<N>(cube, transformations[i]);
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
				const CubeI<N> cube_new = function_composition<N>(cube, std::get<0>(transformations[i]));
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

	} // end namespace details


	template <int N> 
	void init_n_cube() {
		// run all_transformations once to fill the transformation cache
		const auto transformations_from_cache = get_transformations_from_cache<N, false>();
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


	#pragma region reflect
	template <int N> constexpr CubeI<N> reflect(const int dim)
	{
		return details::reflect<N>::value(dim);
	}
	template <int N> constexpr BF reflect(const BF bf, const int dim)
	{
		return transform<N>(bf, reflect<N>(dim));
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
	#pragma endregion

	#pragma region class id
	namespace details
	{
		//for the set of all transformations find the transformation that yields the smallest bf, which is the bf class id.
		template <int N, bool DESCR> std::tuple<BF, std::string> search_class_id_method0(const BF bf)
		{
			std::string complement_string;
			if constexpr (DESCR) {
				complement_string = "";
			}
			BF smallest_bf = bf;
			if (details::count_bits(bf) > ((1 << N) / 2))
			{
				if constexpr (DESCR) {
					complement_string = "COMPLEMENT ";
				}
				smallest_bf = complement<N>(bf);
			}
			std::string transform_string;
			if constexpr (DESCR) {
				transform_string = "";
			}
			// find minimum in set
			for (const auto& pair : get_transformations_from_cache<N, DESCR>())
			{
				const CubeI<N> cube = std::get<0>(pair);
				const BF bf_new = transform<N>(bf, cube);

				if (bf_new < smallest_bf)
				{
					smallest_bf = bf_new;
					if constexpr (DESCR) {
						transform_string = to_string<N>(pair);
					}
				}

				const BF bf_new_compl = complement<N>(bf_new);
				if (bf_new_compl < smallest_bf)
				{
					smallest_bf = bf_new_compl;
					if constexpr (DESCR) {
						transform_string = to_string<N>(pair);
					}
				}
			}
			if constexpr (DESCR) {
				return std::make_pair(smallest_bf, complement_string + transform_string);
			}
			else {
				return std::make_pair(smallest_bf, "");
			}
		}


		bool overall_decreasing(const BF a, const BF b) noexcept { return b < a; }



		//for a specific sequence of transformations, reduce the provided bf with in a greedy fashion with the tranformations.
		template <int N, bool DESCR = false> std::tuple<BF, std::string> search_class_id_method1(const BF bf)
		{
			const auto transformations = cube::rewrite::create_transformations_for_greedy_rewrite<N, DESCR>();

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

	template <int N> constexpr BF search_npn_class(const BF bf)
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
		const Transformations<N>& transformations_from_cache = get_transformations_from_cache<N, false>();

		std::set<BF> result;
		result.insert(complement<N>(bf));

		for (const auto& pair : transformations_from_cache)
		{
			const CubeI<N>& cube = std::get<0>(pair);
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
		template <int N> std::set<BF> generate_all_npn_classes_method0()
		{
			constexpr unsigned long long total_number_npn_classes = cardinality_npn_class(N);
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
				const BF c = search_npn_class<N>(bf);
				//std::cout << "INFO: bf " << to_string_bin<N>(bf) << "; npn class " << to_string_bin<N>(c) << std::endl;

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
			constexpr unsigned long long total_number_npn_classes = cardinality_npn_class(N);
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
		const unsigned long long n_npn_classes = all_npn_classes.size();
		if (n_npn_classes != cardinality_npn_class(N)) {
			std::cout << "WARNING: number of npn classes is " << n_npn_classes << " which is not equal to the expected number " << cardinality_npn_class(N) << std::endl;
		}

		if (!std::filesystem::exists(filename)) {
			std::cout << "INFO: Creating new NPN class file " << filename << " with " << n_npn_classes << " clasess in " << std::filesystem::current_path() << std::endl;
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
				const int cardinality = cube::bf_cardinality(bf_class_id, N);

				if (cardinality == c)
				{
					const std::set<BF> set = equiv_class<N>(bf_class_id);
					const std::string factors = prime::vector_to_string(prime::get_factorization(static_cast<int>(set.size())));

					std::cout << "npn class #" << std::dec << class_counter;
					std::cout << "; cardinality=" << cardinality;
					std::cout << ": " << to_string_hex<N>(bf_class_id) << "=" << to_string_bin<N>(bf_class_id);
					std::cout << "; class size=" << set.size() << "=(" << factors << ")";
					std::cout << std::endl;

					constexpr bool show_npn_class_content = false;
					if (show_npn_class_content) 
					{
						std::cout << "equivalent BF: ";
						for (const BF equivalent_bf : set)
						{
							std::cout << to_string_bin<N>(equivalent_bf) << " ";
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
