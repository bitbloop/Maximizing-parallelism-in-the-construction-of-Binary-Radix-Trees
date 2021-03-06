
#include "maximizing_tree_construction_parallelism\test_basic.h"
#include "maximizing_tree_construction_parallelism\test_obj_threaded.h"
#include "maximizing_tree_construction_parallelism\test_render_data.h"

#include <iostream>

int main()
{
	maximizing_tree_construction_parallelism::test_basic();
	maximizing_tree_construction_parallelism::test_obj_threaded();
	maximizing_tree_construction_parallelism::test_render_data();
	
	std::cout << std::endl << "waiting on a keypress to terminate.";
	getchar();

	return 0;
}

