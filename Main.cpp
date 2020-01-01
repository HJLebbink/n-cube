#include <chrono>
#include <iostream> // for cout

#include "src/timing.ipp"
#include "src/n_cube.ipp"
#include "src/array_tools.h"


int main(int, char** /*argv[]*/)
{
	const auto start = std::chrono::system_clock::now();

	//n_cube::print_all_transformations<1, true>(); // 2     = 2^1
	//n_cube::print_all_transformations<2, true>(); // 8     = 2^3
	//n_cube::print_all_transformations<3, true>(); // 48    = 2^4  * 3^1
	//n_cube::print_all_transformations<4, true>(); // 384   = 2^7  * 3^1
	//n_cube::print_all_transformations<5, false>(); // 3840  = 2^8  * 3^1 * 5^1 transformations takes 1 sec
	//n_cube::print_all_transformations<6, false>(); // 46080 = 2^10 * 3^2 * 5^1 transformations takes 4 sec
	//n_cube::print_all_transformations<7, false>(); //

	//n_cube::print_all_class_ids_with_values<1>(".\\data\\npn1.txt");
	n_cube::print_all_class_ids_with_values<2>(".\\data\\npn2.txt");
	//n_cube::print_all_class_ids_with_values<3>(".\\data\\npn3.txt");
	//n_cube::print_all_class_ids_with_values<4>(".\\data\\npn4.txt");   //222 classes; 65536 = 2^16 bfs; 220 ms
	//n_cube::print_all_class_ids_with_values<5>(".\\data\\npn5.txt");

	//n_cube::save_all_npn_classes<1>(".\\data\\npn1.txt");
	//n_cube::save_all_npn_classes<2>(".\\data\\npn2.txt");
	//n_cube::save_all_npn_classes<3>(".\\data\\npn3-b.txt");
	//n_cube::save_all_npn_classes<4>(".\\data\\npn4-b.txt"); //222 classes; 65536 = 2^16 bfs; 95 ms
	//n_cube::save_all_npn_classes<5>(".\\data\\npn5-b.txt"); // takes ? ms: ? classes; 2^32 bfs

	//n_cube::test_greedy_rewrite_algorithm<3>();
	//n_cube::test_greedy_rewrite_algorithm<4>(); //takes 8 sec
	//n_cube::test_greedy_rewrite_algorithm<5>(); //takes ? sec

	//n_cube::find_greedy_rewrite_algorithms<3>();


	const auto end = std::chrono::system_clock::now();

	std::cout << "DONE: passed time: " << tools::timing::elapsed_time_str(start, end);
	std::cout << "-------------------" << std::endl;
	std::cout << "Press RETURN to finish:" << std::endl;
	static_cast<void>(getchar());
	return 0;
}