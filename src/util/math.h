#pragma once

namespace math
{

	/** minimum of two numbers */
	template<typename T>
	inline T min(const T& x, const T& y)
	{
		return y ^ ((x ^ y) & -(x < y));
	}

	/** maximum of two numbers */
	template<typename T>
	inline T max(const T& x, const T& y)
	{
		return x ^ ((x ^ y) & -(x < y));
	}

	/** computes the sign of the value. returning -1, 0, 1 */
	template <typename T> 
	inline int sign(T val) {
		return (T(0) < val) - (val < T(0));
	}

	/** Integer division, rounding up */
	template <typename T>
	inline int divideceil(const T& x, const T& y) {
		return (x + y - 1) / y;
	}

	/** Integer division by two, rounding up */
	template <typename T>
	inline int divide2ceil(const T& x) {
		return (x + 1) >> 1;
	}
	
};