#include <chrono>
#include <iostream> // for cout

#include "src/timing.ipp"
#include "src/n_cube.ipp"

int main(int, char** /*argv[]*/)
{
	const auto start = std::chrono::system_clock::now();

	//n_cube::print_all_transformations<1>();
	//n_cube::print_all_transformations<2>();
	//n_cube::print_all_transformations<3>();
	//n_cube::print_all_transformations<4>();
	//n_cube::print_all_transformations<5>(); // 3840 transformations takes 1 sec
	//n_cube::print_all_transformations<6>(); // 46080 transformations takes 6 sec
	//n_cube::print_all_class_ids<1>();
	//n_cube::print_all_class_ids<2>();
	//n_cube::print_all_class_ids<3>();
	n_cube::print_all_class_ids<4>(); //222 classes; 65536 = 2^16 bfs
	//n_cube::print_all_class_ids<5>(); // takes ? ms: ? classes; 2^32 bfs
	//n_cube::test_greedy_rewrite_algorithm<3>();
	//n_cube::test_greedy_rewrite_algorithm<4>(); //takes 8 sec
	//n_cube::test_greedy_rewrite_algorithm<5>(); //takes ? sec

	//n_cube::find_greedy_rewrite_algorithms<3>();


	const auto end = std::chrono::system_clock::now();

	std::cout << "DONE: passed time: " << tools::timing::elapsed_time_str(start, end);
	std::cout << "-------------------" << std::endl;
	std::cout << "Press RETURN to finish:" << std::endl;
	getchar();
	return 0;
}