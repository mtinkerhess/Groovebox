
template <
	typename T,
	ParamValueGetter<T, SynthVoiceParam> ParamGetter,
	ParamValueGetter<T, SynthVoiceOptionalParam> OptionalParamGetter,
	MinMaxValueGetter<T> MinValueGetter,
	MinMaxValueGetter<T> MaxValueGetter>
	class CLASS_NAME
{
public:
#define PARAM(name, shortName, min, max, initial, doubleMin, doubleMax) \
T name{};
#include MACROS_PATH
#undef PARAM

	CLASS_NAME() { }

	explicit CLASS_NAME(const SynthVoiceParams::CLASS_NAME& other)
	{
#define PARAM(name, shortName, min, max, initial, doubleMin, doubleMax) \
name = ParamGetter(other.name);
#include MACROS_PATH
#undef PARAM
	}

	CLASS_NAME(const SynthVoiceParams::CLASS_NAME& other, const SynthVoiceOptionalParams::CLASS_NAME& stepParams)
	{
#define PARAM(name, shortName, min, max, initial, doubleMin, doubleMax) \
if (!stepParams.name.HasValue()) \
{ \
	name = ParamGetter(other.name); \
} \
else \
{ \
	name = OptionalParamGetter(stepParams.name); \
}
#include MACROS_PATH
#undef PARAM
	}

	void SetAllToZero()
	{
#define PARAM(name, shortName, min, max, initial, doubleMin, doubleMax) \
name = (decltype(name))0;
#include MACROS_PATH
#undef PARAM
	}

	void Clamp()
	{
#define PARAM(name, shortName, min, max, initial, doubleMin, doubleMax) \
name = Math::Clamp(name, MinValueGetter(min, max, doubleMin, doubleMax), MaxValueGetter(min, max, doubleMin, doubleMax));
#include MACROS_PATH
#undef PARAM
	}

	nlohmann::json ToJson() const
	{
		return nlohmann::json
		{
#define PARAM(name, shortName, min, max, initial, doubleMin, doubleMax) \
{ #name, name },
#include MACROS_PATH
#undef PARAM
		};
	}

	void LoadFromJson(const nlohmann::json& json)
	{
#define PARAM(name, shortName, min, max, initial, doubleMin, doubleMax) \
name = json[#name].get<short>();
#include MACROS_PATH
#undef PARAM
	}

	static CLASS_NAME Interpolate(
		const CLASS_NAME& from,
		const CLASS_NAME& to,
		double amount)
	{
		CLASS_NAME result;

#define PARAM(name, shortName, min, max, initial, doubleMin, doubleMax) \
result.name = Math::Lerp(from.name, to.name, amount);
#include MACROS_PATH
#undef PARAM

		return result;
	}
};
