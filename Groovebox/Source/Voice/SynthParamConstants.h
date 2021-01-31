#pragma once

#include <string>

class SynthParamConstants
{
public:
	const std::wstring Name;
	const std::wstring ShortName;
	const short MinShortValue;
	const short MaxShortValue;
	const short InitialShortValue;
	const double MinDoubleValue;
	const double MaxDoubleValue;

private:
	SynthParamConstants(const std::wstring name, const std::wstring shortName, short min, short max, short initial, double doubleMin, double doubleMax)
		:
		Name(name),
		ShortName(shortName),
		MinShortValue(min),
		MaxShortValue(max),
		InitialShortValue(initial),
		MinDoubleValue(doubleMin),
		MaxDoubleValue(doubleMax)
	{
	}

public:

	short ClampShort(short value);
	double ClampDouble(double value);

#define PARAM(name, shortName, min, max, initial, doubleMin, doubleMax) \
static SynthParamConstants Synth##name;
#include "Voice/Macros/SynthParamMacros.h"
#undef PARAM

#define PARAM(name, shortName, min, max, initial, doubleMin, doubleMax) \
static SynthParamConstants Oscillator##name;
#include "Voice/Macros/OscillatorParamMacros.h"
#undef PARAM

#define PARAM(name, shortName, min, max, initial, doubleMin, doubleMax) \
static SynthParamConstants Filter##name;
#include "Voice/Macros/FilterParamMacros.h"
#undef PARAM

#define PARAM(name, shortName, min, max, initial, doubleMin, doubleMax) \
static SynthParamConstants Envelope##name;
#include "Voice/Macros/EnvelopeParamMacros.h"
#undef PARAM

#define PARAM(name, shortName, min, max, initial, doubleMin, doubleMax) \
static SynthParamConstants Delay##name;
#include "Voice/Macros/DelayParamMacros.h"
#undef PARAM

#define PARAM(name, shortName, min, max, initial, doubleMin, doubleMax) \
static SynthParamConstants Lfo##name;
#include "Voice/Macros/LfoParamMacros.h"
#undef PARAM
};