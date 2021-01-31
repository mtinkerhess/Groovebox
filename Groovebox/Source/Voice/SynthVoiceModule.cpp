#include "Voice/SynthVoiceModule.h"

#include "Sequence.h"
#include "Voice/SynthParamReflection.h"

using namespace nlohmann;

void SynthVoiceModule::StepChanged(StepChangedEvent event)
{
	if (!event.Step->GetOn() && !event.IsFirstStep)
	{
		return;
	}

	m_currentStepParams = &event.Step->GetParams();
	SnapParams();
}

json SynthVoiceModule::ToJson() const
{
	json modulations
	{
	};

	for (ModulationSource* modulationSource : m_modulationSources)
	{
		modulations[modulationSource->GetName()] = modulationSource->ToJson();
	}

	return json
	{
		{ "Params", SynthParamReflection::Reflection::Instance().ToJson(m_currentParams) },
		{ "Modulations", modulations }
	};
}

void SynthVoiceModule::LoadFromJson(const nlohmann::json& json)
{
	std::scoped_lock lock(m_mutex);
	SynthParamReflection::Reflection::Instance().LoadFromJson(json["Params"], m_currentParams);

	for (ModulationSource* modulationSource : m_modulationSources)
	{
		modulationSource->LoadFromJson(json["Modulations"][modulationSource->GetName()]);
	}

	SnapParams();
}