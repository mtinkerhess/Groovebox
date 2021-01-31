class CLASS_NAME : public Category<SynthVoiceParams::PARAMS_TYPE_NAME, SynthVoiceOptionalParams::PARAMS_TYPE_NAME, SynthVoiceModulationShortParams::PARAMS_TYPE_NAME>
{
#define PARAM(name, shortName, min, max, initial, doubleMin, doubleMax) \
VALUE(PARAMS_TYPE_NAME, name, shortName, min, max, initial, doubleMin, doubleMax)
#include MACROS_PATH
#undef PARAM

public:
	CLASS_NAME(
		const std::string& name,
		SynthVoiceParamsGategoryGetter paramsCategoryGetter,
		SynthVoiceOptionalParamsCategoryGetter optionalParamsCategoryGetter,
		SynthVoiceModulationShortParamsCategoryGetter modulationShortParamsCategoryGetter)
		: Category(name, paramsCategoryGetter, optionalParamsCategoryGetter, modulationShortParamsCategoryGetter)
	{
#define PARAM(name, shortName, min, max, initial, doubleMin, doubleMax) \
m_values.push_back(&name);
#include MACROS_PATH
#undef PARAM
	}
};