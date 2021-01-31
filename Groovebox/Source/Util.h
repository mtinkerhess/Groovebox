#pragma once

#include "Constants.h"

#include "SFML/Window.hpp"

#include<vector>

namespace Util
{
	template<typename CallbackType>
	void EraseRemove(std::vector<CallbackType>& v, const CallbackType& t)
	{
		v.erase(std::remove(v.begin(), v.end(), t), v.end());
	}

	bool IsShiftPressed();

	double ConvertPitchToFrequency(double pitch);

	// from https://dbj.org/c17-codecvt-deprecated-panic/
	// Transform any std string or string view
	// into any of the 4 the std string types,
	// Apache 2.0 (c) 2018 by DBJ.ORG
	template<typename T, typename F>
	inline T ConvertString(F str) noexcept
	{
		// note: F has to have 
		// the empty()method
		if (str.empty()) return {};
		// note: F must be able to work 
		// with std begin and end
		return { std::begin(str), std::end(str) };
		// also the above line requires, T has a constructor
		// that will take begin and end values of type F.
	};
}