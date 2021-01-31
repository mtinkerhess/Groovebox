#pragma once

#include "Modules/SignalModule.h"
#include "Voice/Oscillator.h"

#include <string>

class LfoModule : public SignalModule
{
private:
	OscillatorBank<true> m_bank;
	double m_amount = 1.0;
	
public:
	LfoModule() { }

	explicit LfoModule(const std::string& name)
		: SignalModule(name, nullptr)
	{
		SetFrequency(3.0);
	}

	void Initialize() override
	{
		m_bank.Initialize();
	}

	void SetFrequency(double frequency) { m_bank.SetFrequency(frequency); }

	int GetSelectedOscillatorIndex() const { return m_bank.GetSelectedOscillatorIndex(); }

	void SetSelectedOscillatorIndex(int index) { m_bank.SetSelectedOscillatorIndex(index); }

	double CalculateValue(double deltaSeconds) override
	{
		return m_bank.Next(deltaSeconds) * m_amount;
	}

	void ApplyParams(double frequency, double amount)
	{
		m_bank.SetFrequency(frequency);
		m_amount = amount;
	}
};