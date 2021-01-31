#include "ModulationSource.h"

namespace
{
	double ModulateValue(double initialValue, short modSettingsValue, double modulationInput, short min, short max, double doubleMin, double doubleMax)
	{
		double modulationAmount = modSettingsValue * modulationInput;
		short shortRange = max - min;
		double doubleRange = doubleMax - doubleMin;
		return initialValue + modulationAmount * doubleRange / shortRange;
	}

	void ModifySynthParams(SynthVoiceDoubleParams::SynthParamsType& params, const SynthVoiceModulationShortParams::SynthParamsType& modulationSettings, double modulationInput)
	{
#define PARAM(name, shortName, min, max, initial, doubleMin, doubleMax) \
params.name = ModulateValue(params.name, modulationSettings.name, modulationInput, min, max, doubleMin, doubleMax);
#include "Voice/Macros/SynthParamMacros.h"
#undef PARAM
	}

	void ModifyOscillatorParams(SynthVoiceDoubleParams::OscillatorParamsType& params, const SynthVoiceModulationShortParams::OscillatorParamsType& modulationSettings, double modulationInput)
	{
#define PARAM(name, shortName, min, max, initial, doubleMin, doubleMax) \
params.name = ModulateValue(params.name, modulationSettings.name, modulationInput, min, max, doubleMin, doubleMax);
#include "Voice/Macros/OscillatorParamMacros.h"
#undef PARAM
	}

	void ModifyEnvelopeParams(SynthVoiceDoubleParams::EnvelopeParamsType& params, const SynthVoiceModulationShortParams::EnvelopeParamsType& modulationSettings, double modulationInput)
	{
#define PARAM(name, shortName, min, max, initial, doubleMin, doubleMax) \
params.name = ModulateValue(params.name, modulationSettings.name, modulationInput, min, max, doubleMin, doubleMax);
#include "Voice/Macros/EnvelopeParamMacros.h"
#undef PARAM
	}

	void ModifyLfoParams(SynthVoiceDoubleParams::LfoParamsType& params, const SynthVoiceModulationShortParams::LfoParamsType& modulationSettings, double modulationInput)
	{
#define PARAM(name, shortName, min, max, initial, doubleMin, doubleMax) \
params.name = ModulateValue(params.name, modulationSettings.name, modulationInput, min, max, doubleMin, doubleMax);
#include "Voice/Macros/LfoParamMacros.h"
#undef PARAM
	}

	void ModifyFilterParams(SynthVoiceDoubleParams::FilterParamsType& params, const SynthVoiceModulationShortParams::FilterParamsType& modulationSettings, double modulationInput)
	{
#define PARAM(name, shortName, min, max, initial, doubleMin, doubleMax) \
params.name = ModulateValue(params.name, modulationSettings.name, modulationInput, min, max, doubleMin, doubleMax);
#include "Voice/Macros/FilterParamMacros.h"
#undef PARAM
	}

	void ModifyDelayParams(SynthVoiceDoubleParams::DelayParamsType& params, const SynthVoiceModulationShortParams::DelayParamsType& modulationSettings, double modulationInput)
	{
#define PARAM(name, shortName, min, max, initial, doubleMin, doubleMax) \
params.name = ModulateValue(params.name, modulationSettings.name, modulationInput, min, max, doubleMin, doubleMax);
#include "Voice/Macros/DelayParamMacros.h"
#undef PARAM
	}
}

ModulationSource::ModulationSource()
	: m_name("")
{
	m_modulations.SetAllToZero();
}

ModulationSource::ModulationSource(SignalModule* module, const std::string& name)
	: m_module(module),
	m_name(name)
{
	m_modulations.SetAllToZero();
}

const std::string& ModulationSource::GetName() const { return m_name; }

SynthVoiceModulationShortParams& ModulationSource::GetModulations() { return m_modulations; }

SignalModule& ModulationSource::GetModule() { return *m_module; }

void ModulationSource::ModifyParams(SynthVoiceDoubleParams& params)
{
	if (m_module == nullptr)
	{
		return;
	}

	double modulationInput = m_module->GetPreviousValue();

#define CATEGORY_INSTANCE(name, categoryName) \
Modify##categoryName##Params(params.name##Params, m_modulations.name##Params, modulationInput);
#include "Voice/Macros/SynthVoiceCategoryInstanceMacros.h"
#undef CATEGORY_INSTANCE
}

nlohmann::json ModulationSource::ToJson() const
{
	return m_modulations.ToJson();
}

void ModulationSource::LoadFromJson(const nlohmann::json& json)
{
	m_modulations.LoadFromJson(json);
}
