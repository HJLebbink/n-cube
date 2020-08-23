#include <chrono>
#include <iostream> // for cout

#include "cube/timing.h"
#include "cube/n_cube.h"
#include "cube/array_tools.h"

#include "cube/rewrite.h"
#include "cube/plot_class.h"
#include "cube/reachable.h"

int main(int, char** /*argv[]*/)
{
	const auto start = std::chrono::system_clock::now();

	//cube::print_all_transformations<1, true>(); // 2     = 2^1
	//cube::print_all_transformations<2, true>(); // 8     = 2^3
	//cube::print_all_transformations<3, true>(); // 48    = 2^4  * 3^1
	//cube::print_all_transformations<4, true>(); // 384   = 2^7  * 3^1
	//cube::print_all_transformations<5, false>(); // 3840  = 2^8  * 3^1 * 5^1 transformations takes 1 sec
	//cube::print_all_transformations<6, false>(); // 46080 = 2^10 * 3^2 * 5^1 transformations takes 4 sec
	//cube::print_all_transformations<7, false>(); //

	//cube::print_all_class_ids_with_values<1>(".\\data\\npn1.txt");
	//cube::print_all_class_ids_with_values<2>(".\\data\\npn2.txt");
	//cube::print_all_class_ids_with_values<3>(".\\data\\npn3.txt");	//14 class; 
	//cube::print_all_class_ids_with_values<4>(".\\data\\npn4.txt");   //222 classes; 65536 = 2^16 bfs; 220 ms
	//cube::print_all_class_ids_with_values<5>(".\\data\\npn5.txt");

	//cube::test_reachability_bf2();
	//cube::test_reachability_bf3();

	cube::test_create_transformations();

	if (false) {
		constexpr int N = 3;
		for (const cube::BF npn_class : cube::load_all_npn_classes<N>()) {
			const std::string filename = ".\\data\\class_" + std::to_string(N) + "_" + cube::to_string_bin<N>(npn_class) + ".dot";
			std::cout << filename << std::endl;
			cube::plot::plot_npn_classes<N>(cube::equiv_class<N>(npn_class), filename);
			//cube::plot::plot_npn_outgoing_edges<N>(npn_class, cube::equiv_class<N>(npn_class), filename);
		}
	}
		
	if (false) {
		constexpr int N = 1;
		cube::plot::plot_transformation_transitions<N>(".\\data\\transitions_" + std::to_string(N) + ".dot");
	}
	if (false) {
		constexpr int N = 2;
		cube::plot::plot_transformation_transitions<N>(".\\data\\transitions_" + std::to_string(N) + ".dot");
	}
	if (false) {
		constexpr int N = 3;
		cube::plot::plot_transformation_transitions<N>(".\\data\\transitions_" + std::to_string(N) + ".dot");
	}

	//cube::save_all_npn_classes<1>(".\\data\\npn1-b.txt");
	//cube::save_all_npn_classes<2>(".\\data\\npn2-b.txt");
	//cube::save_all_npn_classes<3>(".\\data\\npn3-b.txt");
	cube::save_all_npn_classes<4>(".\\data\\npn4-b.txt"); //222 classes; 65536 = 2^16 bfs; 95 ms
	//cube::save_all_npn_classes<5>(".\\data\\npn5-b.txt"); // takes ? ms: ? classes; 2^32 bfs

	//cube::rewrite::test_greedy_rewrite_algorithm<3>();
	//cube::rewrite::test_greedy_rewrite_algorithm<4>(); //takes 8 sec
	//cube::rewrite::test_greedy_rewrite_algorithm<5>(); //takes ? sec

	//cube::find_greedy_rewrite_algorithms<3>();


	const auto end = std::chrono::system_clock::now();

	std::cout << "DONE: passed time: " << tools::timing::elapsed_time_str(start, end);
	std::cout << "-------------------" << std::endl;
	std::cout << "Press RETURN to finish:" << std::endl;
	static_cast<void>(getchar());
	return 0;
}