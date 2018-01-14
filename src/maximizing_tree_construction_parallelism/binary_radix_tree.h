#pragma once

/*-------------------------------------------------------------------------------------------------- 
MIT License

Copyright (c) 2018 Rados Jovanovic

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
--------------------------------------------------------------------------------------------------*/


// Maximizing Parallelism in the Construction of BVHs, Octrees, and k - d Trees
// http://research.nvidia.com/sites/default/files/publications/karras2012hpg_paper.pdf

// In the tree datastructure, the internal_nodes is an array of the size 2 * (n-1). Each internal node consists of two consecutive integers, onde indexing the left child, and one indexing the right.
// The most significant bit of an index of a node is reserved to denote if the number is indexing an internal node (0) or a leaf node (1).

#include <stdint.h> // uint_fast64_t

namespace binary_radix_tree
{
	/** binary radix tree datastructure. */
	class brt
	{
	public:
		brt(const int key_num);
		~brt() { delete[] internal_nodes; }
		brt(brt&& other);					// Move constructor
		brt& operator=(brt&& other);		// Move assignment operator

		int* internal_nodes;				// internal_nodes memory layout is {left_int, right_int}, single node memory size is size is sizeof(int)*2
		const int key_num;					// number of keys used for the construction of the tree
		
		// function to return the number of sizeof(int) slots in the memory the internal nodes occupy
		inline int get_internal_node_memory_size() const { return (this->key_num - 1) * 2; };
		// function to compute the number of internal nodes.
		inline int get_internal_node_num() const { return this->key_num - 1; };

	private:
		brt(const brt& other):key_num(0){};					// Private copy constructor
		brt& operator=(const brt& other) { return *this; };		// Private copy assigment operator
	};


	/** Given a sorted sequence of morton_keys, creates a binary radix tree. */
	brt create(const int key_num, const uint_fast64_t* morton_keys);

	/** std::thread::hardware_concurrency() */
	unsigned int hardware_concurrency();

	/** Given a sorted sequence of morton_keys, creates a binary radix tree using resources from several threads. */
	brt create_threaded(const int key_num, const uint_fast64_t* morton_keys, const int thread_number = hardware_concurrency());

	// -----------------------------
	/** node indexing functions */
	namespace node
	{
		template<typename T> // index to a leaf node. difference between a leaf node index and an internal node index is that the leaf's node index most significant bit is 1
		inline T make_leaf(const T& index) { return index ^ (-1 ^ index) & (1UL << ((sizeof(T) * 8 - 1))); };
		template<typename T> // index to an internal node
		inline T make_internal(const T& index) { return index; };
		template<typename T> // set internal node indices
		inline void set_internal_values(const T& index, T* internals, const T& left, const T& right) { internals[index * 2] = left; internals[index * 2 + 1] = right; };
		template<typename T> // get the left index
		inline void get_internal_left(const T& index, const T* internals, T& value, bool& leaf) {
			const auto& internal_value{ internals[index * 2] };
			leaf = internal_value >> (sizeof(T) * 8 - 1);
			value = internal_value & ~(T(1) << (sizeof(T) * 8 - 1)); // delete the last bit which tells if this is leaf or internal index
		};
		template<typename T> // get the right index
		inline void get_internal_right(const T& index, const T* internals, T& value, bool& leaf) {
			const auto& internal_value{ internals[index * 2 + 1] };
			leaf = internal_value >> (sizeof(T) * 8 - 1);
			value = internal_value & ~(T(1) << (sizeof(T) * 8 - 1)); // delete the last bit which tells if this is leaf or internal index
		};
	};

};

