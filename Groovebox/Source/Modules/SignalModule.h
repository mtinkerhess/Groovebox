#pragma once

#include "Biquad.h"
#include "Voice/SynthParamConstants.h"

#include <vector>

class SignalModule
{
private:
	std::string m_name;

	SignalModule* m_input = nullptr;
	SignalModule* m_secondInput = nullptr;

	SignalModule* m_otherChannelInput = nullptr;
	SignalModule* m_otherChannelSecondInput = nullptr;

	SignalModule* m_oppositeModule = nullptr;

	double m_value = 0.0;
	bool m_dirty = false;

	double m_previousValue = 0.0;

	std::vector<SignalModule*> m_containedModules;

	bool m_isLeftChannel = false;

protected:
	SignalModule() : m_name("") { }

	explicit SignalModule(const std::string& name, SignalModule* input = nullptr, SignalModule* secondInput = nullptr)
		: m_name(name),
		m_input(input),
		m_secondInput(secondInput)
	{
	}

public:
	virtual void Initialize() { }

	SignalModule* GetInput() { return m_input; }
	void SetInput(SignalModule* input) { m_input = input; }
	SignalModule* GetSecondInput() { return m_secondInput; }
	void SetSecondInput(SignalModule* secondInput) { m_secondInput = secondInput; }

	void SetInputs(
		SignalModule* firstInput,
		SignalModule* secondInput,
		SignalModule* otherChannelInput,
		SignalModule* otherChannelSecondInput)
	{
		m_input = firstInput;
		m_secondInput = secondInput;
		m_otherChannelInput = otherChannelInput;
		m_otherChannelSecondInput = otherChannelSecondInput;
	}

	bool GetIsLeftChannel() const { return m_isLeftChannel; }
	void SetIsLeftChannel(bool isLeftChannel) { m_isLeftChannel = isLeftChannel; }

	void SetDirty()
	{
		m_dirty = true;
		m_previousValue = m_value;

		for (SignalModule* module : m_containedModules)
		{
			module->SetDirty();
		}
	}

	double GetValue(double deltaSeconds)
	{
		if (m_dirty)
		{
			m_dirty = false;
			m_value = CalculateValue(deltaSeconds);
		}

		return m_value;
	}

	double GetPreviousValue() const { return m_previousValue; }

	virtual double CalculateValue(double deltaSeconds) = 0;

	const std::vector<SignalModule*>& GetContainedModules() const { return m_containedModules; }

	const std::string& GetName() const { return m_name; }

	void AddContainedModule(SignalModule* module) { m_containedModules.push_back(module); }

	SignalModule* GetOppositeModule() { return m_oppositeModule; }

	void SetOppositeModule(SignalModule* oppositeModule) { m_oppositeModule = oppositeModule; }
};
