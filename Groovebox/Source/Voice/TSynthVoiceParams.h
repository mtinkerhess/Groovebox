#pragma once


template <typename T>
class TSynthParams
{
public:
#define PARAM(name, shortName, min, max, initial, doubleMin, doubleMax) \
T name { min, max, initial, doubleMin, doubleMax }; \
T& Get##name() { return name; } \
const T& Get##name() const { return name; }
#include "Voice/Macros/SynthParamMacros.h"
#undef PARAM
};

template <typename T>
class TOscillatorParams
{
public:
#define PARAM(name, shortName, min, max, initial, doubleMin, doubleMax) \
T name { min, max, initial, doubleMin, doubleMax }; \
T& Get##name() { return name; } \
const T& Get##name() const { return name; }
#include "Voice/Macros/OscillatorParamMacros.h"
#undef PARAM
};

template <typename T>
class TEnvelopeParams
{
public:
#define PARAM(name, shortName, min, max, initial, doubleMin, doubleMax) \
T name { min, max, initial, doubleMin, doubleMax }; \
T& Get##name() { return name; } \
const T& Get##name() const { return name; }
#include "Voice/Macros/EnvelopeParamMacros.h"
#undef PARAM
};

template <typename T>
class TLfoParams
{
public:
#define PARAM(name, shortName, min, max, initial, doubleMin, doubleMax) \
T name { min, max, initial, doubleMin, doubleMax }; \
T& Get##name() { return name; } \
const T& Get##name() const { return name; }
#include "Voice/Macros/LfoParamMacros.h"
#undef PARAM
};

template <typename T>
class TFilterParams
{
public:
#define PARAM(name, shortName, min, max, initial, doubleMin, doubleMax) \
T name { min, max, initial, doubleMin, doubleMax }; \
T& Get##name() { return name; } \
const T& Get##name() const { return name; }
#include "Voice/Macros/FilterParamMacros.h"
#undef PARAM
};

template <typename T>
class TDelayParams
{
public:
#define PARAM(name, shortName, min, max, initial, doubleMin, doubleMax) \
T name { min, max, initial, doubleMin, doubleMax }; \
T& Get##name() { return name; } \
const T& Get##name() const { return name; }
#include "Voice/Macros/DelayParamMacros.h"
#undef PARAM
};

template <typename T>
class TSynthVoiceParams
{
public:
	using ParamType = T;

#define CATEGORY(name) \
using name##ParamsType = T##name##Params<ParamType>;
#include "Voice/Macros/SynthVoiceCategoryMacros.h"
#undef CATEGORY

#define CATEGORY_INSTANCE(name, categoryName) \
categoryName##ParamsType name##Params;
#include "Voice/Macros/SynthVoiceCategoryInstanceMacros.h"
#undef CATEGORY_INSTANCE

	nlohmann::json ToJson() const
	{
		return nlohmann::json
		{
#define CATEGORY_INSTANCE(name, categoryName) \
{ #name, name##Params.ToJson() },
#include "Voice/Macros/SynthVoiceCategoryInstanceMacros.h"
#undef CATEGORY_INSTANCE
		};
	}

	void LoadFromJson(const nlohmann::json& json)
	{
#define CATEGORY_INSTANCE(name, categoryName) \
name##Params->LoadFromJson(json[#name]);
#include "Voice/Macros/SynthVoiceCategoryInstanceMacros.h"
#undef CATEGORY_INSTANCE
	}
};
