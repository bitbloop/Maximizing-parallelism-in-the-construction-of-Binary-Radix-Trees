#pragma once

#include <vector>
#include "binary_radix_tree.h"

namespace binary_radix_tree
{
	// -----------------------------
	// For debugging, iterates the tree and gathers info
	namespace info
	{
		/** The structure to hold the gathered information */
		struct tree_info
		{
			std::vector<int> depth_level_internal_node_count;
			std::vector<int> depth_level_leaf_node_count;
			int internal_nodes_touched;		// the number of internal nodes reached when iterating from the root
			int leaf_nodes_touched;			// the number of leaf nodes reached when iterating from the root
		};

		/** returns the maximum tree depth
			fills the vector with the number of internal nodes at each depth level.
			fills the vector with the number of leaf nodes at each depth level.
			The implementation can be more efficient. */
		int iterate_and_get_tree_depth_info(tree_info& info_out, const brt& tree, const int i = 0, const int current_depth = 0);
	};
};

