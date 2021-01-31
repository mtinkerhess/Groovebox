#pragma once

#include "json.hpp"
#include "Voice/SynthVoiceOptionalParam.h"

#include <algorithm>

namespace
{
	template<typename T, typename ParamType>
	using ParamValueGetter = T(*)(const ParamType&);

	template<typename T>
	using MinMaxValueGetter = T(*)(short, short, double, double);
}

#define CLASS_NAME SynthParamsType
#define MACROS_PATH "Voice/Macros/SynthParamMacros.h"
#include "Voice/Templates/ValueParamsTypeTemplate.h"
#undef CLASS_NAME
#undef MACROS_PATH

#define CLASS_NAME OscillatorParamsType
#define MACROS_PATH "Voice/Macros/OscillatorParamMacros.h"
#include "Voice/Templates/ValueParamsTypeTemplate.h"
#undef CLASS_NAME
#undef MACROS_PATH

#define CLASS_NAME EnvelopeParamsType
#define MACROS_PATH "Voice/Macros/EnvelopeParamMacros.h"
#include "Voice/Templates/ValueParamsTypeTemplate.h"
#undef CLASS_NAME
#undef MACROS_PATH

#define CLASS_NAME LfoParamsType
#define MACROS_PATH "Voice/Macros/LfoParamMacros.h"
#include "Voice/Templates/ValueParamsTypeTemplate.h"
#undef CLASS_NAME
#undef MACROS_PATH

#define CLASS_NAME FilterParamsType
#define MACROS_PATH "Voice/Macros/FilterParamMacros.h"
#include "Voice/Templates/ValueParamsTypeTemplate.h"
#undef CLASS_NAME
#undef MACROS_PATH

#define CLASS_NAME DelayParamsType
#define MACROS_PATH "Voice/Macros/DelayParamMacros.h"
#include "Voice/Templates/ValueParamsTypeTemplate.h"
#undef CLASS_NAME
#undef MACROS_PATH

template <
	typename T,
	typename ConcreteType,
	ParamValueGetter<T, SynthVoiceParam> ParamGetter,
	ParamValueGetter<T, SynthVoiceOptionalParam> OptionalParamGetter,
	MinMaxValueGetter<T> MinValueGetter,
	MinMaxValueGetter<T> MaxValueGetter>
class SynthVoiceValueParams
{
public:
	using ValueType = T;

#define CATEGORY(name) \
using name##ParamsType = name##ParamsType<T, ParamGetter, OptionalParamGetter, MinValueGetter, MaxValueGetter>;
#include "Voice/Macros/SynthVoiceCategoryMacros.h"
#undef CATEGORY

#define CATEGORY_INSTANCE(name, category) \
category##ParamsType name##Params;
#include "Voice/Macros/SynthVoiceCategoryInstanceMacros.h"
#undef CATEGORY_INSTANCE

	SynthVoiceValueParams() { }

	explicit SynthVoiceValueParams(const SynthVoiceParams& other)
	{
#define CATEGORY_INSTANCE(name, category) \
name##Params = category##ParamsType(other.name##Params);
#include "Voice/Macros/SynthVoiceCategoryInstanceMacros.h"
#undef CATEGORY_INSTANCE
	}

	void SetAllToZero()
	{
#define CATEGORY_INSTANCE(name, category) \
name##Params.SetAllToZero();
#include "Voice/Macros/SynthVoiceCategoryInstanceMacros.h"
#undef CATEGORY_INSTANCE
	}

	void Clamp()
	{
#define CATEGORY_INSTANCE(name, category) \
name##Params.Clamp();
#include "Voice/Macros/SynthVoiceCategoryInstanceMacros.h"
#undef CATEGORY_INSTANCE
	}

	nlohmann::json ToJson() const
	{
		return nlohmann::json
		{
#define CATEGORY_INSTANCE(name, category) \
{ #name, name##Params.ToJson() },
#include "Voice/Macros/SynthVoiceCategoryInstanceMacros.h"
#undef CATEGORY_INSTANCE
		};
	}

	void LoadFromJson(const nlohmann::json& json)
	{
#define CATEGORY_INSTANCE(name, category) \
name##Params.LoadFromJson(json[#name]);
#include "Voice/Macros/SynthVoiceCategoryInstanceMacros.h"
#undef CATEGORY_INSTANCE
	}

	static ConcreteType Interpolate(const ConcreteType& from, const ConcreteType& to, double amount)
	{
		ConcreteType result;

#define CATEGORY_INSTANCE(name, category) \
result.name##Params = category##ParamsType::Interpolate(from.name##Params, to.name##Params, amount);
#include "Voice/Macros/SynthVoiceCategoryInstanceMacros.h"
#undef CATEGORY_INSTANCE

		return result;
	}
};

namespace
{
	double GetDoubleValueFromParam(const SynthVoiceParam& param) { return param.DoubleValue(); }
	short GetShortValueFromParam(const SynthVoiceParam& param) { return param.Value(); }
	double GetDoubleValueFromOptionalParam(const SynthVoiceOptionalParam& param) { return param.DoubleValue(); }
	short GetShortValueFromOptionalParam(const SynthVoiceOptionalParam& param) { return param.Value(); }
	short GetShortMinValue(short min, short, double, double) { return min; }
	short GetShortMaxValue(short, short max, double, double) { return max; }
	double GetDoubleMinValue(short, short, double min, double) { return min; }
	double GetDoubleMaxValue(short, short, double, double max) { return max; }

	short GetShortRangeMinValue(short min, short max, double, double) { return -(max - min); }
	short GetShortRangeMaxValue(short min, short max, double, double) { return max - min; }
}

class SynthVoiceDoubleParams
	: public SynthVoiceValueParams<
	double,
	SynthVoiceDoubleParams,
	GetDoubleValueFromParam,
	GetDoubleValueFromOptionalParam,
	GetDoubleMinValue,
	GetDoubleMaxValue>
{
public:
	SynthVoiceDoubleParams() { }

	SynthVoiceDoubleParams(const SynthVoiceParams& other)
		: SynthVoiceValueParams<
		double, 
		SynthVoiceDoubleParams, 
		GetDoubleValueFromParam, 
		GetDoubleValueFromOptionalParam, 
		GetDoubleMinValue,
		GetDoubleMaxValue>(other) { }

	SynthVoiceDoubleParams(const SynthVoiceParams& other, const SynthVoiceOptionalParams& stepParams)
	{
#define CATEGORY_INSTANCE(name, category) \
name##Params = category##ParamsType(other.name##Params, stepParams.name##Params);
#include "Voice/Macros/SynthVoiceCategoryInstanceMacros.h"
#undef CATEGORY_INSTANCE
	}
};

class SynthVoiceShortParams
	: public SynthVoiceValueParams<
	short, 
	SynthVoiceShortParams, 
	GetShortValueFromParam, 
	GetShortValueFromOptionalParam,
	GetShortMinValue,
	GetShortMaxValue>
{
public:
	SynthVoiceShortParams() { }

	SynthVoiceShortParams(const SynthVoiceParams& other)
		: SynthVoiceValueParams<
		short, 
		SynthVoiceShortParams, 
		GetShortValueFromParam, 
		GetShortValueFromOptionalParam,
		GetShortMinValue,
		GetShortMaxValue>(other) { }
};

class SynthVoiceModulationShortParams
	: public SynthVoiceValueParams<
	short,
	SynthVoiceShortParams,
	GetShortValueFromParam,
	GetShortValueFromOptionalParam,
	GetShortRangeMinValue,
	GetShortRangeMaxValue>
{
public:
	SynthVoiceModulationShortParams() { }

	SynthVoiceModulationShortParams(const SynthVoiceParams& other)
		: SynthVoiceValueParams<
		short,
		SynthVoiceShortParams,
		GetShortValueFromParam,
		GetShortValueFromOptionalParam,
		GetShortRangeMinValue,
		GetShortRangeMaxValue>(other) { }
};