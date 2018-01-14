
#include "binary_radix_tree.h"
#include "util/math.h"
#include <assert.h>     /* assert */
#include <thread>
#include <vector>		/* to store references to thread objects */

namespace
{
	/** length of common prefix in bits */
	template <typename T>
	inline int delta(const T * morton_keys, const int& i, const int& j)
	{
		const int number_of_bits = sizeof(T) * 8;
		for (int b = number_of_bits - 1; b >= 0; --b)
			if ((morton_keys[i] >> b) != (morton_keys[j] >> b))
				return static_cast<int>(number_of_bits - b - 1);
		return number_of_bits;
	}

	/** does basic out of bounds checking before invoking delta */
	inline int delta_safe(const int key_num, const uint_fast64_t * morton_keys, const int& i, const int& j)
	{
		/// for simplicity we define delta(j) to be -1 when j != [0, n)
		return (j < 0 || j >= key_num) ? -1 : delta(morton_keys, i, j);
	}
		
	/** process a single node. is thread safe. */
	void process_internal_node(const int key_num, const uint_fast64_t * morton_keys, const int i, binary_radix_tree::brt& brt)
	{
		// ________________________________________________________________
		// determine the direction of the range = (+1 or -1)
		const auto direction{ math::sign<int>(delta(morton_keys, i, i + 1) - delta_safe(key_num, morton_keys, i, i - 1)) };
		// sanity check. if it is zero, the set has duplicate data, and we do not resolve duplicate data in this implementation
		assert(direction != 0); 
		
		// ________________________________________________________________
		// compute the upper bound for the length of the edge

		const auto delta_min{ delta_safe(key_num, morton_keys, i, i - direction) };
		int I_max{ 2 };
		while (delta_safe(key_num, morton_keys, i, i + I_max*direction) > delta_min)
			I_max <<= 2;

		// ________________________________________________________________
		// find the other end using binary search

		int I{ 0 };
		for (int t{ I_max / 2 }; t; t /= 2)
			if (delta_safe(key_num, morton_keys, i, i + (I + t)*direction) > delta_min)
				I += t;
		const int j{ i + I * direction };

		// ________________________________________________________________
		// find the split position using binary search

		const auto delta_node{ delta_safe(key_num, morton_keys, i, j) }; // the distance of the prefix of i
		auto s{ 0 };
		
		int t{ I };
		do
		{
			t = math::divide2ceil<int>(t);
			if (delta_safe(key_num, morton_keys, i, i + (s + t)*direction) > delta_node)
				s += t;

		} while (t > 1);

		const auto split{ i + s * direction + math::min<int>(direction, 0) };

		// ________________________________________________________________
		// sanity check
		assert(delta(morton_keys, i,j) > delta_min);
		assert(delta(morton_keys, split, split + 1) == delta(morton_keys, i, j));
		assert(!(split < 0 || split+1 >= key_num));

		// ________________________________________________________________
		// output child pointers	

		const int left{ (math::min<int>(i, j) == split) ? binary_radix_tree::node::make_leaf<int>(split) : binary_radix_tree::node::make_internal<int>(split) };
		const int right{ (math::max<int>(i, j) == split + 1) ? binary_radix_tree::node::make_leaf<int>(split + 1) : binary_radix_tree::node::make_internal<int>(split + 1) };
		binary_radix_tree::node::set_internal_values<int>(i, brt.internal_nodes, left, right);
	}
};


// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------


/** given a sorted sequence of morton_keys, create a binary radix tree. */
binary_radix_tree::brt binary_radix_tree::create(const int key_num, const uint_fast64_t * morton_keys)
{
	// initialize the memory to hold the tree
	binary_radix_tree::brt tree(key_num);

	// for each internal node with index i [0, n-2] (to be launched in parallel)
	for (int i = 0; i < key_num-1; ++i)
		process_internal_node(key_num, morton_keys, i, tree);

	return tree;
}

/** given a sorted sequence of morton_keys, create a binary radix tree. */
binary_radix_tree::brt binary_radix_tree::create_threaded(const int key_num, const uint_fast64_t * morton_keys, const int thread_number)
{
	// initialize the memory to hold the tree
	binary_radix_tree::brt tree(key_num);
	// compute the number of elements a thread will cover
	const auto elements_per_thread{ math::divideceil<int>(key_num, thread_number) };
	
	const auto worker_fn = [key_num, &morton_keys, elements_per_thread, &tree](int i) {
		for (int t = i * elements_per_thread; t < math::min(key_num - 1, (i + 1)*elements_per_thread); ++t)
			process_internal_node(key_num, morton_keys, t, tree);
	};

	// Create the threads
	std::vector<std::thread> workers;
	for (int i = 0; i < thread_number; ++i)
		workers.push_back(std::thread(worker_fn, i));
	for (auto& t : workers)	t.join();	

	return tree;
}


/** */
unsigned int binary_radix_tree::hardware_concurrency()
{
	return std::thread::hardware_concurrency();
}



// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------





/** constructor */
binary_radix_tree::brt::brt(const int key_num) :
	internal_nodes(new int[(key_num - 1) * 2]),
	key_num(key_num)
{
}


/** move constructor */
binary_radix_tree::brt::brt(brt&& other)
	: internal_nodes(other.internal_nodes)
	, key_num(other.key_num)
{
	other.internal_nodes = nullptr;
	const_cast<int&>(other.key_num) = 0;
}

/* move assignment operator */
binary_radix_tree::brt & binary_radix_tree::brt::operator=(brt && other)
{
	if (this != &other) {           
		delete[] this->internal_nodes; 
		this->internal_nodes = other.internal_nodes;
		const_cast<int&>(this->key_num) = other.key_num;
		other.internal_nodes = nullptr;
		const_cast<int&>(other.key_num) = 0;
	}
	return *this;
}
