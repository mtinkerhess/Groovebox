#pragma once

#include "EventTypes.h"
#include "json.hpp"
#include "ModulationSource.h"
#include "MyMath.h"
#include "SequenceProcessor.h"
#include "Modules/AmpModule.h"
#include "Modules/BackingObjectModule.h"
#include "Modules/LfoModule.h"
#include "Modules/TapeDelayModule.h"
#include "Modules/MixerModule.h"
#include "Modules/SignalModule.h"
#include "Modules/StereoSignalModule.h"
#include "Modules/SwitchableFilterModule.h"
#include "Voice/SynthVoiceParam.h"
#include "Voice/SynthVoiceValueParams.h"

#include <optional>
#include <mutex>

class SynthVoiceModule : public SignalModule
{
private:
	std::mutex m_mutex;

	int m_track;

	OscillatorBank<false> m_oscillatorBank1;
	MonoToStereoSignalModule<OscillatorModule> m_oscillatorModule1;

	OscillatorBank<false> m_oscillatorBank2;
	MonoToStereoSignalModule<OscillatorModule> m_oscillatorModule2;

	StereoSignalModule<MixerModule> m_oscillatorMixerModule;

	StereoSignalModule<SwitchableFilterModule> m_filterModule;

	StereoSignalModule<MixerModule> m_filterMixerModule;

	Envelope m_envelope1;
	EnvelopeModule m_envelopeModule1;

	Envelope m_envelope2;
	EnvelopeModule m_envelopeModule2;

	LfoModule m_lfoModule;
	
	StereoSignalModule<AmpModule> m_ampModule;

	StereoSignalModule<TapeDelayModule> m_delayModule;

	std::vector<SignalModule*> m_modules;
	
	SynthVoiceParams m_currentParams;
	SynthVoiceDoubleParams m_currentBufferParams;
	SynthVoiceDoubleParams m_previousBufferParams;
	SynthVoiceDoubleParams m_previousFrameParams;
	double m_amountIntoCurrentBuffer = 0.0f;

	const SynthVoiceOptionalParams* m_currentStepParams = nullptr;
	
	ModulationSource m_envelope1ModulationSource;
	ModulationSource m_envelope2ModulationSource;
	ModulationSource m_lfoModulationSource;

	std::vector<ModulationSource*> m_modulationSources;

	double m_leftValue = 0.0;
	double m_rightValue = 0.0;

public:
	explicit SynthVoiceModule(const std::string& name, int track)
		: SignalModule(name),
		m_track(track)
	{
		m_oscillatorBank1.Initialize();
		m_oscillatorBank2.Initialize();

		m_oscillatorModule1 = MonoToStereoSignalModule<OscillatorModule>(OscillatorModule("Osc 1", &m_oscillatorBank1));
		InitializeStereoModules(m_oscillatorModule1);

		m_oscillatorModule2 = MonoToStereoSignalModule<OscillatorModule>(OscillatorModule("Osc 2", &m_oscillatorBank2));
		InitializeStereoModules(m_oscillatorModule2);

		m_oscillatorMixerModule = StereoSignalModule<MixerModule>(MixerModule("Osc mixer"), &m_oscillatorModule1, &m_oscillatorModule2);
		InitializeStereoModules(m_oscillatorMixerModule);

		m_filterModule = StereoSignalModule<SwitchableFilterModule>(SwitchableFilterModule("Filter"), &m_oscillatorMixerModule);
		InitializeStereoModules(m_filterModule);

		m_filterMixerModule = StereoSignalModule<MixerModule>(MixerModule("Filter mixer"), &m_oscillatorMixerModule, &m_filterModule);
		InitializeStereoModules(m_filterMixerModule);

		m_envelopeModule1 = EnvelopeModule("Env 1", &m_envelope1);
		m_modules.push_back(&m_envelopeModule1);

		m_envelopeModule2 = EnvelopeModule("Env 2", &m_envelope2);
		m_modules.push_back(&m_envelopeModule2);

		m_lfoModule = LfoModule("Lfo");
		m_lfoModule.Initialize();
		m_modules.push_back(&m_lfoModule);

		m_ampModule = StereoSignalModule<AmpModule>(AmpModule("Amp"), &m_filterMixerModule);
		InitializeStereoModules(m_ampModule);

		m_delayModule = StereoSignalModule<TapeDelayModule>(TapeDelayModule("Tape delay"), &m_ampModule);
		InitializeStereoModules(m_delayModule);

		m_envelope1ModulationSource = ModulationSource(&m_envelopeModule1, "Envelope1");
		m_envelope1ModulationSource.GetModulations().SynthParams.Volume = 100;
		m_modulationSources.push_back(&m_envelope1ModulationSource);

		m_envelope2ModulationSource = ModulationSource(&m_envelopeModule2, "Envelope2");
		m_modulationSources.push_back(&m_envelope2ModulationSource);

		m_lfoModulationSource = ModulationSource(&m_lfoModule, "Lfo");
		m_modulationSources.push_back(&m_lfoModulationSource);
	}

	const std::vector<ModulationSource*>& GetModulationSources() { return m_modulationSources; }

	void StartingBuffer()
	{
		m_currentBufferParams = m_currentStepParams == nullptr ? SynthVoiceDoubleParams(m_currentParams) : SynthVoiceDoubleParams(m_currentParams, *m_currentStepParams);
	}

	void FinishedBuffer()
	{
		SnapParams();
	}

	void SetAmountIntoCurrentBuffer(double amountIntoCurrentBuffer)
	{
		m_amountIntoCurrentBuffer = amountIntoCurrentBuffer;
	}

	SynthVoiceParams& GetParams()
	{
		// Technically this isn't safe since the lock is released but the caller still has access to the params.
		// But this is to guard against loading a synch voice from a file at the same time as processing the stream.
		// Processing the stream happens really often but loading from a file doesn't happen very often.
		std::scoped_lock lock(m_mutex);
		return m_currentParams;
	}
	
	void NoteStarted(NoteStartedEvent event)
	{
		m_oscillatorBank1.SetPitch((double)event.Pitch);
		m_oscillatorBank2.SetPitch((double)event.Pitch);
		m_envelope1.NoteOn();
		m_envelope2.NoteOn();

		if (m_currentParams.Oscillator1Params.GetReset().Value() > 0)
		{
			m_oscillatorBank1.Reset(m_currentParams.Oscillator1Params.GetResetPhase().DoubleValue());
		}

		if (m_currentParams.Oscillator2Params.GetReset().Value() > 0)
		{
			m_oscillatorBank2.Reset(m_currentParams.Oscillator2Params.GetResetPhase().DoubleValue());
		}
	}

	void NoteEnded(NoteEndedEvent event)
	{
		m_envelope1.NoteOff();
		m_envelope2.NoteOff();
	}

	void StepChanged(StepChangedEvent event);

	SynthVoiceDoubleParams CalculateInterpolatedParams()
	{
		return SynthVoiceDoubleParams::Interpolate(m_previousBufferParams, m_currentBufferParams, m_amountIntoCurrentBuffer);
	}

	nlohmann::json ToJson() const;

	void LoadFromJson(const nlohmann::json& json);

	double GetRightValue() const { return m_rightValue; }

protected:
	virtual double CalculateValue(double deltaSeconds) override
	{
		SynthVoiceDoubleParams params = CalculateInterpolatedParams();

		for (ModulationSource* modulationSource : m_modulationSources)
		{
			modulationSource->ModifyParams(params);
		}

		// params.Clamp();

		ApplyParams(params);

		for (auto& module : m_modules)
		{
			module->SetDirty();
		}
		
		// Proccess these to make sure their previous value is correct next sample
		for (ModulationSource* modulationSource : m_modulationSources)
		{
			modulationSource->GetModule().GetValue(deltaSeconds);
		}

		m_leftValue = m_delayModule.GetLeftModuleForSignal().GetValue(deltaSeconds);
		m_rightValue = m_delayModule.GetRightModuleForSignal().GetValue(deltaSeconds);

		return m_leftValue;
	}

private:
	void InitializeStereoModules(IStereoSignalModule& module)
	{
		module.Initialize();
		for (SignalModule* containedModule : module.GetContainedModules())
		{
			m_modules.push_back(containedModule);
		}
	}

	void SnapParams()
	{
		m_previousBufferParams = m_currentBufferParams = SynthVoiceDoubleParams(m_currentParams, *m_currentStepParams);
	}

	void ApplyFilterParams(SwitchableFilterModule& filterModule, const SynthVoiceDoubleParams::FilterParamsType& params)
	{
		filterModule.SetSelectedModuleIndex((int)params.Type);
		filterModule.GetSelectedModule().SetParameters(params.Pitch, params.Q, params.Gain);
	}

	void ApplyFilterMixerParams(MixerModule& filterMixerModule, const SynthVoiceDoubleParams::SynthParamsType& params)
	{
		filterMixerModule.SetBalance(params.FilterMix);
	}

	void ApplyOscillatorParams(OscillatorModule& oscillatorModule, const SynthVoiceDoubleParams::OscillatorParamsType& params)
	{
		oscillatorModule.GetBackingObject().SetParams(
			params.Pitch,
			params.FinePitch,
			params.Level,
			(int)params.OversampleFactor);
	}

	void ApplyAmpParams(AmpModule& ampModule, const SynthVoiceDoubleParams::SynthParamsType& params)
	{
		ampModule.SetVolume(params.Volume);
	}

	void ApplyDelayParams(TapeDelayModule& delayModule, const SynthVoiceDoubleParams::DelayParamsType& params)
	{
		delayModule.SetParams(
			params.Send,
			params.Feedback,
			params.WetLevel,
			params.DryLevel,
			params.TapeSpeed,
			params.HeadDistance,
			params.Pan,
			params.PingPong);
	}

	void ApplyParams(const SynthVoiceDoubleParams& params)
	{
		m_envelope1.ApplyParams(params.Envelope1Params);
		m_envelope2.ApplyParams(params.Envelope2Params);

		int lfoIndex = (int)params.LfoParams.Type;

		if (lfoIndex != m_lfoModule.GetSelectedOscillatorIndex())
		{
			m_lfoModule.SetSelectedOscillatorIndex(lfoIndex);
		}

		m_lfoModule.ApplyParams(params.LfoParams.Frequency, params.LfoParams.Amount);

#define APPLY(function, module, paramsName) \
function(module.GetLeftModuleForParams(), params.paramsName); \
if (decltype(module)::ModuleT* rightModule = module.GetRightModuleForParams()) \
{ \
	function(*rightModule, params.paramsName); \
}

		APPLY(ApplyFilterParams, m_filterModule, FilterParams);

		APPLY(ApplyFilterMixerParams, m_filterMixerModule, SynthParams);

		int oscillator1Index = (int)params.Oscillator1Params.Type;

		if (oscillator1Index != m_oscillatorBank1.GetSelectedOscillatorIndex())
		{
			m_oscillatorBank1.SetSelectedOscillatorIndex(oscillator1Index);
		}

		APPLY(ApplyOscillatorParams, m_oscillatorModule1, Oscillator1Params);
		m_oscillatorModule1.SetPan(params.Oscillator1Params.Pan);

		int oscillator2Index = (int)params.Oscillator2Params.Type;

		if (oscillator2Index != m_oscillatorBank2.GetSelectedOscillatorIndex())
		{
			m_oscillatorBank2.SetSelectedOscillatorIndex(oscillator2Index);
		}

		APPLY(ApplyOscillatorParams, m_oscillatorModule2, Oscillator2Params);
		m_oscillatorModule2.SetPan(params.Oscillator2Params.Pan);

		APPLY(ApplyAmpParams, m_ampModule, SynthParams);

		APPLY(ApplyDelayParams, m_delayModule, DelayParams);

#undef APPLY
	}
};