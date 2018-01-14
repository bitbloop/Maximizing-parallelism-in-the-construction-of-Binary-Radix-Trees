
#include "random.h"


namespace
{
	// For random number generation
	std::random_device rd;
	std::mt19937 mt(rd());
};

std::mt19937&  util::random::get_mt19937()
{
	return mt;
}
