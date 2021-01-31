#pragma once

#include "Modules/SignalModule.h"

class AmpModule : public SignalModule
{
private:
	double m_volume = 0.0;

public:
	AmpModule() { }

	explicit AmpModule(const std::string& name) : SignalModule(name) { }

	void SetVolume(double volume) { m_volume = SynthParamConstants::SynthVolume.ClampDouble(volume); }

	double CalculateValue(double deltaSeconds) override
	{
		return GetInput()->GetValue(deltaSeconds) * m_volume;
	}
};
