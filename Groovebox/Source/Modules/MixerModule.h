#pragma once

#include "Modules/SignalModule.h"

class MixerModule : public SignalModule
{
private:
	double m_firstInputLevel = 0.5;
	double m_secondInputLevel = 0.5;

public:
	MixerModule() { }

	MixerModule(const std::string& name)
		: SignalModule(name)
	{
	}

	void SetBalance(double balance)
	{
		SetSecondInputLevel(balance);
		SetFirstInputLevel(1.0 - balance);
	}

	void SetFirstInputLevel(double level) { m_firstInputLevel = Math::Clamp01(level); }

	void SetSecondInputLevel(double level) { m_secondInputLevel = Math::Clamp01(level); }

	void SetFirstInput(SignalModule* input) { SetInput(input); }

	virtual double CalculateValue(double deltaSeconds) override
	{
		return GetInput()->GetValue(deltaSeconds) * m_firstInputLevel + GetSecondInput()->GetValue(deltaSeconds) * m_secondInputLevel;
	}
};
