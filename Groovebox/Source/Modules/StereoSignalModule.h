#pragma once

#include "Modules/SignalModule.h"

struct StereoSignal
{
	double Left = 0.0;
	double Right = 0.0;

	StereoSignal() { }
	StereoSignal(double left, double right) : Left(left), Right(right) { }
};

class IStereoSignalModule
{
public:
	virtual SignalModule& GetLeftModuleForSignal() = 0;
	virtual SignalModule& GetRightModuleForSignal() = 0;
	virtual void Initialize() = 0;
	virtual std::vector<SignalModule*> GetContainedModules() = 0;
};

template <typename ModuleType>
class StereoSignalModule : public IStereoSignalModule
{
public:
	using ModuleT = ModuleType;

private:
	std::string m_name;
	ModuleType m_leftModule;
	ModuleType m_rightModule;
	bool m_initialized = false;

public:
	StereoSignalModule() { }

	StereoSignalModule(const ModuleType& module, IStereoSignalModule* firstInput = nullptr, IStereoSignalModule* secondInput = nullptr)
		: m_name(module.GetName()),
		m_leftModule(module),
		m_rightModule(module)
	{
		m_leftModule.SetIsLeftChannel(true);
		m_rightModule.SetIsLeftChannel(false);

		SignalModule* leftInput = &firstInput->GetLeftModuleForSignal();
		SignalModule* rightInput = &firstInput->GetRightModuleForSignal();

		SignalModule* secondLeftInput = nullptr;
		SignalModule* secondRightInput = nullptr;

		if (secondInput != nullptr)
		{
			secondLeftInput = &secondInput->GetLeftModuleForSignal();
			secondRightInput = &secondInput->GetRightModuleForSignal();
		}

		m_leftModule.SetInputs(leftInput, secondLeftInput, rightInput, secondRightInput);
		m_rightModule.SetInputs(rightInput, secondRightInput, leftInput, secondLeftInput);
	}

	void Initialize() override
	{
		assert(!m_initialized);
		m_initialized = true;

		m_leftModule.SetOppositeModule(&m_rightModule);
		m_rightModule.SetOppositeModule(&m_leftModule);

		m_leftModule.Initialize();
		m_rightModule.Initialize();
	}
	
	std::vector<SignalModule*> GetContainedModules() override
	{
		assert(m_initialized);
		return std::vector<SignalModule*>
		{
			&m_leftModule,
			&m_rightModule
		};
	}

	ModuleType& GetLeftModuleForSignal() override { return m_leftModule; }
	ModuleType& GetRightModuleForSignal() override { return m_rightModule; }
	ModuleType& GetLeftModuleForParams() { return GetLeftModuleForSignal(); }
	ModuleType* GetRightModuleForParams() { return &GetRightModuleForSignal(); }
};

template <typename ModuleType>
class MonoToStereoSignalModule : public IStereoSignalModule
{
public:
	using ModuleT = ModuleType;

private:
	std::string m_name;
	ModuleType m_module;
	AmpModule m_leftAmpModule{ "Left Amp" };
	AmpModule m_rightAmpModule{ "Right Amp" };
	bool m_initialized = false;

public:
	MonoToStereoSignalModule() { }

	MonoToStereoSignalModule(const ModuleType& module, SignalModule* firstInput = nullptr, SignalModule* secondInput = nullptr)
		: m_name(module.GetName()),
		m_module(module)
	{
		m_module.SetInput(firstInput);
		m_module.SetSecondInput(secondInput);
	}

	void SetPan(double pan)
	{
		if (pan == 0.0)
		{
			m_leftAmpModule.SetVolume(1.0);
			m_rightAmpModule.SetVolume(1.0);
		}
		else if (pan < 0.0)
		{
			m_leftAmpModule.SetVolume(1.0);
			m_rightAmpModule.SetVolume(1.0f + pan);
		}
		else if (pan > 0.0)
		{
			m_rightAmpModule.SetVolume(1.0);
			m_leftAmpModule.SetVolume(1.0 - pan);
		}
	}

	AmpModule& GetLeftModuleForSignal() override { return m_leftAmpModule; }
	AmpModule& GetRightModuleForSignal() override { return m_rightAmpModule; }
	ModuleType& GetLeftModuleForParams() { return m_module; }
	ModuleType* GetRightModuleForParams() { return nullptr; }

	void Initialize() override
	{
		assert(!m_initialized);

		m_leftAmpModule.SetInput(&m_module);
		m_rightAmpModule.SetInput(&m_module);

		m_initialized = true;
		m_module.Initialize();
		m_leftAmpModule.Initialize();
		m_rightAmpModule.Initialize();
	}

	std::vector<SignalModule*> GetContainedModules() override
	{
		assert(m_initialized);
		return std::vector< SignalModule*>
		{
			&m_module,
			&m_leftAmpModule,
			&m_rightAmpModule,
		};
	}
};