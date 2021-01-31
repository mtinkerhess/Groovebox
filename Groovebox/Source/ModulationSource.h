#pragma once

#include "Voice/SynthVoiceValueParams.h"
#include "Voice/SynthParamGetters.h"
#include "Modules/SignalModule.h"

#include <string>

class ModulationSource
{
private:
	SignalModule* m_module = nullptr;

	// The mod matrix weights the user has chosen
	SynthVoiceModulationShortParams m_modulations;

	std::string m_name;

public:
	ModulationSource();

	ModulationSource(SignalModule* module, const std::string& name);

	const std::string& GetName() const;
	
	SynthVoiceModulationShortParams& GetModulations();
	
	SignalModule& GetModule();
	
	void ModifyParams(SynthVoiceDoubleParams& params);

	nlohmann::json ToJson() const;
	
	void LoadFromJson(const nlohmann::json& json);
};