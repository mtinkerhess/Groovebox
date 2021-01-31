#pragma once

#include "Modules/SwitchableModule.h"
#include "Modules/BiquadFilterModule.h"

class SwitchableFilterModule : public SwitchableModule<BiquadFilterModule>
{
private:
	LpfBiquad m_lpf{ Constants::SamplesPerSecond };
	LpfBiquad m_lpf2{ Constants::SamplesPerSecond };
	LpfBiquad m_lpf3{ Constants::SamplesPerSecond };
	LpfBiquad m_lpf4{ Constants::SamplesPerSecond };

	HpfBiquad m_hpf{ Constants::SamplesPerSecond };
	HpfBiquad m_hpf2{ Constants::SamplesPerSecond };
	HpfBiquad m_hpf3{ Constants::SamplesPerSecond };
	HpfBiquad m_hpf4{ Constants::SamplesPerSecond };

	HpfSkirtBiquad m_hpfSkirt{ Constants::SamplesPerSecond };
	HpfPeakBiquad m_hpfPeak{ Constants::SamplesPerSecond };
	NotchPeakBiquad m_notchPeak{ Constants::SamplesPerSecond };
	ApfBiquad m_apf{ Constants::SamplesPerSecond };
	PeakingEqBiquad m_peakingEq{ Constants::SamplesPerSecond };
	LowShelfBiquad m_lowShelf{ Constants::SamplesPerSecond };
	HighShelfBiquad m_highShelf{ Constants::SamplesPerSecond };

public:
	using SwitchableModule::SwitchableModule;

	SwitchableFilterModule() { }

	explicit SwitchableFilterModule(const std::string& name)
		: SwitchableModule<BiquadFilterModule>(name)
	{
	}

	void Initialize() override
	{
		SignalModule* input = GetInput();
		// Do this not in the ctor so that references to members
		// will stay stable (otherwise this is getting moved around
		// after ctor by operator=).
		SwitchableModule<BiquadFilterModule>::Initialize({
			new BiquadFilterModule("LPF1", input, &m_lpf),
			new BiquadFilterModule("LPF2", input, &m_lpf, &m_lpf2),
			new BiquadFilterModule("LPF3", input, &m_lpf, &m_lpf2, &m_lpf3),
			new BiquadFilterModule("LPF4", input, &m_lpf, &m_lpf2, &m_lpf3, &m_lpf4),
			new BiquadFilterModule("HPF1", input, &m_hpf),
			new BiquadFilterModule("HPF2", input, &m_hpf, &m_hpf2),
			new BiquadFilterModule("HPF3", input, &m_hpf, &m_hpf2, &m_hpf3),
			new BiquadFilterModule("HPF4", input, &m_hpf, &m_hpf2, &m_hpf3, &m_hpf4),
			new BiquadFilterModule("Skirt", input, &m_hpfSkirt),
			new BiquadFilterModule("HPF Peak", input, &m_hpfPeak),
			new BiquadFilterModule("Notch", input, &m_notchPeak),
			new BiquadFilterModule("APF", input, &m_apf),
			new BiquadFilterModule("EQ", input, &m_peakingEq),
			new BiquadFilterModule("Low Shelf", input, &m_lowShelf),
			new BiquadFilterModule("High Shelf", input, &m_highShelf)
			});
	}
};