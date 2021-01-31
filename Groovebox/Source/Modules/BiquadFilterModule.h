#pragma once

#include "Modules/SignalModule.h"

class BiquadFilterModule : public SignalModule
{
private:
	Biquad* m_biquad = nullptr;
	Biquad* m_biquad2 = nullptr;
	Biquad* m_biquad3 = nullptr;
	Biquad* m_biquad4 = nullptr;
	int m_order;

	double m_pitch = -1.0;
	double m_q = -1.0;
	double m_gain = -1.0;

	bool m_dirty = false;

public:
	BiquadFilterModule(const std::string& name, SignalModule* input, Biquad* biquad, Biquad* biquad2 = nullptr, Biquad* biquad3 = nullptr, Biquad* biquad4 = nullptr)
		: SignalModule(name, input),
		m_biquad(biquad),
		m_biquad2(biquad2),
		m_biquad3(biquad3),
		m_biquad4(biquad4)
	{
		m_order = m_biquad2 == nullptr ? 1
			: m_biquad3 == nullptr ? 2
			: m_biquad4 == nullptr ? 3
			: 4;
	}

	void SetParameters(double pitch, double q, double gain)
	{
		if (!m_dirty && m_pitch == pitch && m_q == q && m_gain == gain)
		{
			return;
		}

		m_pitch = pitch;
		m_q = q;
		m_gain = gain;

		double curvedQ = pow(4.0, q - 5);
		double effectiveQ = pow(curvedQ, 1.0 / m_order);
		m_biquad->SetParameters(pitch, effectiveQ, gain);
		if (m_biquad2 != nullptr) { m_biquad2->SetParameters(pitch, effectiveQ, gain); }
		if (m_biquad3 != nullptr) { m_biquad3->SetParameters(pitch, effectiveQ, gain); }
		if (m_biquad4 != nullptr) { m_biquad4->SetParameters(pitch, effectiveQ, gain); }
	}

	void Reset()
	{
		m_dirty = true;

		m_biquad->ClearState();
		if (m_biquad2 != nullptr) { m_biquad2->ClearState(); }
		if (m_biquad3 != nullptr) { m_biquad3->ClearState(); }
		if (m_biquad4 != nullptr) { m_biquad4->ClearState(); }
	}

	virtual double CalculateValue(double deltaSeconds) override
	{
		double inputValue = GetInput()->GetValue(deltaSeconds);
		double result = m_biquad->Next(inputValue);

		if (m_biquad2 == nullptr) { return result; }
		result = m_biquad2->Next(result);

		if (m_biquad3 == nullptr) { return result; }
		result = m_biquad3->Next(result);

		if (m_biquad4 == nullptr) { return result; }
		result = m_biquad4->Next(result);

		return result;
	}
};
