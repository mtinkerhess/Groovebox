#pragma once

// https://en.wikipedia.org/wiki/Digital_biquad_filter
// https://webaudio.github.io/Audio-EQ-Cookbook/audio-eq-cookbook.html

#include "Constants.h"
#include "Util.h"

#include <algorithm>
#include <cmath>

class Biquad
{
protected:

	///////////////
	// Parmeters //
	///////////////

	int m_samplingFrequency;
	double m_frequency = -1.0;
	double m_pitch = -1.0;

	// Only for peak and shelf filters
	double m_gain = 1.0;

	// Or "BW" for notch or peak, or "S" for shelf.
	double m_q = -1.0;

	/////////////
	// Weights //
	/////////////

	double m_b0{};
	double m_b1{};
	double m_b2{};
	double m_a0{};
	double m_a1{};
	double m_a2{};

	///////////
	// State //
	///////////

	// Using "Transposed direct form 2" from Wikipedia

	double m_cache0{};
	double m_cache1{};

public:
	Biquad() : m_samplingFrequency() { }

	Biquad(int samplingFrequency)
		: m_samplingFrequency(samplingFrequency)
	{
	}

	double Next(double input)
	{
		double output = (input * m_b0 + m_cache0) / m_a0;
		m_cache0 = input * m_b1 + m_cache1 - output * m_a1;
		m_cache1 = input * m_b2 - output * m_a2;

		if (isnan(m_cache1) || isnan(m_cache1))
		{
			ClearState();
			return 0.0;
		}

		return output;
	}

	void ClearState()
	{
		m_cache0 = 0.0;
		m_cache1 = 0.0;
	}

	void SetParameters(double pitch, double q, double gain)
	{
		if (m_pitch == pitch && m_q == q && m_gain == gain)
		{
			return;
		}

		m_pitch = pitch;
		m_frequency = Util::ConvertPitchToFrequency(pitch);
		m_q = q;
		m_gain = gain;

		double w0 = 2.0 * Constants::PI * m_frequency / m_samplingFrequency;
		CalculateParametersInternal(w0, sin(w0), cos(w0));
	}

protected:
	virtual void CalculateParametersInternal(double w0, double sinW0, double cosW0) = 0;
};

class LpfBiquad : public Biquad
{
public:
	using Biquad::Biquad;

protected:
	virtual void CalculateParametersInternal(double w0, double sinW0, double cosW0) override
	{
		double alpha = sinW0 / (2.0 * m_q);
		m_b0 = (1.0 - cosW0) / 2.0;
		m_b1 = 1.0 - cosW0;
		m_b2 = (1.0 - cosW0) / 2.0;
		m_a0 = 1.0 + alpha;
		m_a1 = -2.0 * cosW0;
		m_a2 = 1.0 - alpha;
	}
};

class HpfBiquad : public Biquad
{
public:
	using Biquad::Biquad;

protected:
	virtual void CalculateParametersInternal(double w0, double sinW0, double cosW0) override
	{
		double alpha = sinW0 / (2.0 * m_q);
		m_b0 = (1.0 + cosW0) / 2.0;
		m_b1 = -(1.0 + cosW0);
		m_b2 = (1.0 + cosW0) / 2.0;
		m_a0 = 1.0 + alpha;
		m_a1 = -2.0 * cosW0;
		m_a2 = 1.0 - alpha;
	}
};

class HpfSkirtBiquad : public Biquad
{
public:
	using Biquad::Biquad;

protected:
	virtual void CalculateParametersInternal(double w0, double sinW0, double cosW0) override
	{
		double alpha = sinW0 / (2.0 * m_q);
		m_b0 = m_q * alpha;
		m_b1 = 0.0;
		m_b2 = -(m_q * alpha);
		m_a0 = 1.0 + alpha;
		m_a1 = -2.0 * cosW0;
		m_a2 = 1.0 - alpha;
	}
};

class HpfPeakBiquad : public Biquad
{
public:
	using Biquad::Biquad;

protected:
	virtual void CalculateParametersInternal(double w0, double sinW0, double cosW0) override
	{
		double alpha = sinW0 / (2.0 * m_q);
		m_b0 = alpha;
		m_b1 = 0.0;
		m_b2 = -alpha;
		m_a0 = 1.0 + alpha;
		m_a1 = -2.0 * cosW0;
		m_a2 = 1.0 - alpha;
	}
};

class NotchPeakBiquad : public Biquad
{
public:
	using Biquad::Biquad;

protected:
	virtual void CalculateParametersInternal(double w0, double sinW0, double cosW0) override
	{
		double alpha = sinW0 * sinh((log(2.0) / 2.0) * m_q * (w0 / sinW0));
		m_b0 = 1.0;
		m_b1 = -2.0 * cosW0;
		m_b2 = 1.0;
		m_a0 = 1.0 + alpha;
		m_a1 = -2.0 * cosW0;
		m_a2 = 1.0 - alpha;
	}
};

class ApfBiquad : public Biquad
{
public:
	using Biquad::Biquad;

protected:
	virtual void CalculateParametersInternal(double w0, double sinW0, double cosW0) override
	{
		double alpha = sinW0 / (2.0 * m_q);
		m_b0 = 1.0 - alpha;
		m_b1 = -2.0 * cosW0;
		m_b2 = 1.0 + alpha;
		m_a0 = 1.0 + alpha;
		m_a1 = -2.0 * cosW0;
		m_a2 = 1.0 - alpha;
	}
};

class PeakingEqBiquad : public Biquad
{
public:
	using Biquad::Biquad;

protected:
	virtual void CalculateParametersInternal(double w0, double sinW0, double cosW0) override
	{
		double a = pow(10.0, m_gain / 40.0);
		double alpha = sinW0 * sinh((log(2.0) / 2.0) * m_q * (w0 / sinW0));
		m_b0 = 1.0 + a * alpha;
		m_b1 = -2.0 * cosW0;
		m_b2 = 1.0 - a * alpha;
		m_a0 = 1.0 + a / alpha;
		m_a1 = -2.0 * cosW0;
		m_a2 = 1.0 - a / alpha;
	}
};

class LowShelfBiquad : public Biquad
{
public:
	using Biquad::Biquad;

protected:
	virtual void CalculateParametersInternal(double w0, double sinW0, double cosW0) override
	{
		double a = pow(10.0, m_gain / 40.0);
		double alpha = (sinW0 / 2.0) * sqrt((a + 1.0 / a) * (1.0 / m_q - 1.0) + 2.0);
		double sqrtA = sqrt(a);
		m_b0 = a * ((a + 1.0) - (a - 1.0) * cosW0 + 2.0 * sqrtA * alpha);
		m_b1 = 2.0 * a * ((a - 1.0) - (a + 1.0) * cosW0);
		m_b2 = a * ((a + 1.0) - (a - 1.0) * cosW0 - 2.0 * sqrtA * alpha);
		m_a0 = (a + 1.0) + (a - 1.0) * cosW0 + 2.0 * sqrtA * alpha;
		m_a1 = -2.0 * ((a - 1.0) + (a + 1) * cosW0);
		m_a2 = (a + 1.0) + (a - 1.0) * cosW0 - 2.0 * sqrtA * alpha;
	}
};

class HighShelfBiquad : public Biquad
{
public:
	using Biquad::Biquad;

protected:
	virtual void CalculateParametersInternal(double w0, double sinW0, double cosW0) override
	{
		double a = pow(10.0, m_gain / 40.0);
		double alpha = (sinW0 / 2.0) * sqrt((a + 1.0 / a) * (1.0 / m_q - 1.0) + 2.0);
		double sqrtA = sqrt(a);
		m_b0 = a * ((a + 1.0) + (a - 1.0) * cosW0 + 2.0 * sqrtA * alpha);
		m_b1 = -2.0 * a * ((a - 1.0) + (a + 1.0) * cosW0);
		m_b2 = a * ((a + 1.0) + (a - 1.0) * cosW0 - 2.0 * sqrtA * alpha);
		m_a0 = (a + 1.0) - (a - 1.0) * cosW0 + 2.0 * sqrtA * alpha;
		m_a1 = 2.0 * ((a - 1.0) - (a + 1) * cosW0);
		m_a2 = (a + 1.0) - (a - 1.0) * cosW0 - 2.0 * sqrtA * alpha;
	}
};