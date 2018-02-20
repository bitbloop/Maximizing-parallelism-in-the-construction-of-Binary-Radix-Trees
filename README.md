# Maximizing Parallelism in the Construction of Binary Radix Trees

The repository contains a C++ implementation of a threaded Binary Radix Tree creation, from an nVidia paper "Maximizing Parallelism in the Construction of BVHs, Octrees, and k - d Trees" (http://research.nvidia.com/sites/default/files/publications/karras2012hpg_paper.pdf)

![BRT](http://radosjovanovic.com/projects/git/brt_release.jpg)

## Basic Use

```cpp
#include "binary_radix_tree.h"
void main()
{
	const uint64_t morton_keys[]{ 1, 2, 4, 5, 19, 24, 25, 30 }; // the 8 keys of the same binary value as in the paper
	// With Serial Construction
	const auto brt_serial{ binary_radix_tree::create(8, morton_keys) };
	// With Threaded Construction
	const auto brt_threaded{ binary_radix_tree::create_threaded(8, morton_keys) };
}
```

## Summary

The main parts in this C++ implementation of the algorithm is written in the binary_radix_tree.h and binary_radix_tree.cpp files. Outside of those two files, the algorithm is using custom function to determine the sign of a variable and the standard library vector and threading functionalities.

The provided Visual Studio solution file is a project containing basic tests to check the validity of the algorithm.

### Basic tests

Tests to test the basic tree construction correctness are written in the  
src/maximizing_tree_construction_parallelism/test_basic.*  
files.  

They test the serial and threaded tree creation with the data given in the paper as an example. This will verify the correctness of the implemented tree against the example tree in the paper.

### Mesh tests

Tests to test the validity and profile the tree construction, using vertices of a mesh as keys. The tests are written in the  
src/maximizing_tree_construction_parallelism/test_obj_threaded.*  
files.

## Deployment

If distributing the built executable, put the data files in the folder <exe_folder>/maximizing_tree_construction_parallelism

## Authors

* **Rados Jovanovic** - *Initial work* - [bitbloop](https://github.com/bitbloop)

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details

## Acknowledgments

* Hat tip to everyone contributing to science!


