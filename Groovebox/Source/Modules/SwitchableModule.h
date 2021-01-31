#pragma once

#include "Modules/SignalModule.h"

template <typename T>
class SwitchableModule : public SignalModule
{
private:
	std::vector<T*> m_backingModules;
	int m_selectedModuleIndex = 0;

public:
	SwitchableModule() { }

	SwitchableModule(const std::string& name)
		: SignalModule(name)
	{
	}

protected:
	void Initialize(std::initializer_list<T*> modules)
	{
		m_backingModules.insert(m_backingModules.end(), modules);
	}

public:
	void SetSelectedModuleIndex(int index)
	{
		int clampedIndex = Math::Clamp(index, 0, (int)m_backingModules.size() - 1);

		if (m_selectedModuleIndex == clampedIndex)
		{
			return;
		}

		m_selectedModuleIndex = clampedIndex;

		GetSelectedModule().Reset();
	}

	T& GetSelectedModule() { return *m_backingModules[m_selectedModuleIndex]; }

	virtual double CalculateValue(double deltaSeconds) override
	{
		return GetSelectedModule().CalculateValue(deltaSeconds);
	}
};
