
#include "info_binary_radix_tree.h"
#include "util/math.h"


int binary_radix_tree::info::iterate_and_get_tree_depth_info(tree_info& info_out, const brt& tree, const int i, const int current_depth)
{
	if (i == 0) {
		info_out.leaf_nodes_touched = 0;
		info_out.internal_nodes_touched = 0;
	}

	if (info_out.depth_level_internal_node_count.size() < current_depth + 1) { info_out.depth_level_internal_node_count.resize(current_depth + 1); info_out.depth_level_internal_node_count[current_depth] = 0; }
	if (info_out.depth_level_leaf_node_count.size() < current_depth + 2) { info_out.depth_level_leaf_node_count.resize(current_depth + 2); info_out.depth_level_leaf_node_count[current_depth + 1] = 0; }

	int valuel, valuer;
	bool leafl, leafr;
	binary_radix_tree::node::get_internal_left<int>(i, tree.internal_nodes, valuel, leafl);
	binary_radix_tree::node::get_internal_right<int>(i, tree.internal_nodes, valuer, leafr);

	info_out.depth_level_leaf_node_count[current_depth + 1] += leafl + leafr;
	info_out.depth_level_internal_node_count[current_depth] += 1;
	info_out.internal_nodes_touched += 1;
	info_out.leaf_nodes_touched += leafl + leafr;

	const int dl{ (leafl) ? 0 : iterate_and_get_tree_depth_info(info_out, tree, valuel, current_depth + 1) };
	const int dr{ (leafr) ? 0 : iterate_and_get_tree_depth_info(info_out, tree, valuer, current_depth + 1) };

	return math::max(math::max(dl, dr), current_depth);
}

