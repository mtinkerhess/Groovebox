#pragma once

#include <algorithm>
#include <cmath>

namespace Math
{
	template <typename T>
	T Clamp(T value, T min, T max)
	{
		return value < (T)min ? (T)min : value >(T)max ? (T)max : value;
	}

	template <typename T>
	T Clamp01(T value)
	{
		return value < (T)0 ? (T)0 : value >(T)1 ? (T)1 : value;
	}

	template <typename T>
	T Lerp(T from, T to, T amount)
	{
		return from + (to - from) * Clamp01(amount);
	}

	template <typename T>
	T LerpUnclamped(T from, T to, T amount)
	{
		return from + (to - from) * amount;
	}
}