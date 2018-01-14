#include "test_basic.h"
#include "binary_radix_tree.h"
#include <iostream>

/*
The example data like in Figure 3. in the paper:
 Maximizing Parallelism in the Construction of BVHs, Octrees, and k - d Trees
 http://research.nvidia.com/sites/default/files/publications/karras2012hpg_paper.pdf

const uint64_t morton_keys[]{ 1, 2, 4, 5, 19, 24, 25, 30 }; // the 8 keys of the same binary value as in the paper
const auto brt{ binary_radix_tree::create(8, morton_keys) };

The algorithm will construct following internal node memory:
(leafs are displayed as negative values)
(the first column indexing is to denote the owner node of the memory "slots" used by individual internal nodes)

0	[0]	3
	[1]	4

1	[2]	-2147483648
	[3]	-2147483647

2	[4]	-2147483646
	[5]	-2147483645

3	[6]	1
	[7]	2

4	[8]	-2147483644
	[9]	5

5	[10] 6
	[11] -2147483641

6	[12] -2147483643
	[13] -2147483642

*/


namespace maximizing_tree_construction_parallelism
{
	void paper()
	{
		const uint64_t morton_keys[]{ 1, 2, 4, 5, 19, 24, 25, 30 }; // the 8 keys of the same value as in the paper
		const auto brt{ binary_radix_tree::create(8, morton_keys) };

		const bool allright{	brt.internal_nodes[0] == 3 && brt.internal_nodes[1] == 4 &&
								brt.internal_nodes[6] == 1 && brt.internal_nodes[7] == 2 &&
								brt.internal_nodes[9] == 5 && brt.internal_nodes[10] == 6 };		// expected values
		std::cout << "paper test " << ((allright) ? "ok." : "not ot.") << std::endl;
	}

	void paper_threaded()
	{
		const uint64_t morton_keys[]{ 1, 2, 4, 5, 19, 24, 25, 30 }; // the 8 keys of the same value as in the paper
		const auto brt{ binary_radix_tree::create_threaded(8, morton_keys) };

		const bool allright{	brt.internal_nodes[0] == 3 && brt.internal_nodes[1] == 4 &&
								brt.internal_nodes[6] == 1 && brt.internal_nodes[7] == 2 &&
								brt.internal_nodes[9] == 5 && brt.internal_nodes[10] == 6 };							// expected values
		std::cout << "threaded paper test " << ((allright) ? "ok." : "not ot.") << std::endl;
	}
	
	void test_basic()
	{
		paper();
		paper_threaded();

		return;
	}

};
