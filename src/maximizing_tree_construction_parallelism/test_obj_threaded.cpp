

#include "test_obj_threaded.h"
#include <math.h>       /* floor */
#include "util\timer.h"
#include "util\loader_obj_nogl.h"
#include <morton.h>
#include "binary_radix_tree.h"
#include "info_binary_radix_tree.h"
#include <cassert>
#include <algorithm>
#include <iostream>

/** */
void maximizing_tree_construction_parallelism::test_obj_threaded(const char* obj_mesh_filename)
{
	// ****************************************************************
	// Data preparation
	// ****************************************************************

	// ________________________________________________________________
	// load the data

	loader_obj_nogl::obj *mesh{ loader_obj_nogl::obj_create(obj_mesh_filename) };
	assert(obj_num_vert(mesh));
	const auto data_num{ obj_num_vert(mesh) };

	auto* data_xyz{ new float[data_num * 3] };
	for (auto vi = 0; vi < obj_num_vert(mesh); ++vi)
		obj_get_vert_v(mesh, vi, &data_xyz[vi * 3]);

	// ________________________________________________________________
	// Print the number of vertices
	std::cout << "Loaded " << obj_mesh_filename << "." << std::endl << "Using " << data_num << " vertices as keys." << std::endl;

	// ________________________________________________________________
	// get the information to convert the data to integer values with the range [0, 2^21}

	float min[3]{ data_xyz[0], data_xyz[1], data_xyz[2] };
	float max[3]{ data_xyz[0], data_xyz[1], data_xyz[2] };
	for (int i = 0; i < data_num; ++i)
		for (auto c = 0; c < 3; ++c) {
			min[c] = (min[c] > data_xyz[i * 3 + c]) ? data_xyz[i * 3 + c] : min[c];
			max[c] = (max[c] < data_xyz[i * 3 + c]) ? data_xyz[i * 3 + c] : max[c];
		}
	const float range[3]{ max[0] - min[0], max[1] - min[1] , max[2] - min[2] };
	const float invrangetimesbound[3]{ ((2 << 20) - 1) / range[0], ((2 << 20) - 1) / range[1], ((2 << 20) - 1) / range[2] };  // (pow(2,21)-1) / range

	// ________________________________________________________________
	// shift and normalize the coordinate values to be in the range [0, 2^21}, convert them to 21 bit integers, save into an integer array

	auto* idata_xyz{ new uint_fast32_t[data_num * 3] };
	for (int i = 0; i < data_num; ++i)
		for (auto c = 0; c < 3; ++c) {
			const auto& data_component = data_xyz[i * 3 + c];
			idata_xyz[i * 3 + c] = static_cast<uint_fast32_t>(floor((data_component - min[c])*invrangetimesbound[c]));
		}

	// ________________________________________________________________
	// convert the integer data into morton codes, these are keys

	//auto* morton_data{ new uint_fast64_t[data_num] };
	std::vector<uint_fast64_t> morton_data(data_num);
	for (int i = 0; i < data_num; ++i)
		morton_data[i] = morton3D_64_encode(idata_xyz[i * 3 + 0], idata_xyz[i * 3 + 1], idata_xyz[i * 3 + 2]);

	// ________________________________________________________________
	// sort the sequence of keys

	std::sort(morton_data.begin(), morton_data.end());

	// ________________________________________________________________
	// clean up

	loader_obj_nogl::obj_delete(mesh);
	delete[] data_xyz;
	delete[] idata_xyz;
	




	// ****************************************************************
	// Tree construction
	// ****************************************************************


	// ________________________________________________________________
	// a basic check to see if we have a link to all keys, and that it doesn't have circular linking
	binary_radix_tree::info::tree_info info;
	binary_radix_tree::info::iterate_and_get_tree_depth_info(info, binary_radix_tree::create(data_num, morton_data.data()));
	std::cout << "Serial tree construction appears to create a " << 
		((info.internal_nodes_touched == data_num - 1 && info.leaf_nodes_touched == data_num) ?"valid":"invalid") 
		<< " binary tree." << std::endl;

	binary_radix_tree::info::tree_info info_threaded;
	binary_radix_tree::info::iterate_and_get_tree_depth_info(info_threaded, binary_radix_tree::create_threaded(data_num, morton_data.data()));
	std::cout << "Parallel tree construction appears to create a" <<
		((info_threaded.internal_nodes_touched == data_num - 1 && info_threaded.leaf_nodes_touched == data_num) ? " valid" : "n invalid")
		<< " binary tree." << std::endl;

	// ________________________________________________________________
	// profile the creation of the binary radix trees.
	const int test_num{ 5 };
	{
		// serial profiling
		for (auto i = 0; i < test_num; ++i)
		{
			math::time::Timer tmr;
			tmr.reset();
			const auto start{ tmr.elapsed() };

			const auto brt{ binary_radix_tree::create(data_num, morton_data.data()) };

			const auto end{ tmr.elapsed() };
			std::cout << "binary radix tree serial construction done in \t" << end - start << " s.";
			std::cout << std::endl;
		}
	}
	{
		// threaded profiling
		for (auto i = 0; i < test_num; ++i)
		{
			math::time::Timer tmr;
			tmr.reset();
			const auto start{ tmr.elapsed() };;

			const auto brt{ binary_radix_tree::create_threaded(data_num, morton_data.data()) };

			const auto end{ tmr.elapsed() };
			std::cout << "binary radix tree threaded construction done in \t" << end - start << " s.";
			std::cout << std::endl;
		}
	}

}


