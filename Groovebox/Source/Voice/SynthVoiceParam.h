#pragma once

#include "json.hpp"
#include "MyMath.h"
#include "Voice/TSynthVoiceParams.h"

#include <algorithm>
#include <cassert>

class SynthVoiceParam
{
protected:
	short m_value, m_minValue, m_maxValue, m_defaultValue;
	double m_minDoubleValue, m_maxDoubleValue;
	double m_modulationAmount = 0.0;

public:
	SynthVoiceParam()
		:
		m_value(0),
		m_minValue(0),
		m_maxValue(0),
		m_defaultValue(0),
		m_minDoubleValue(0.0),
		m_maxDoubleValue(0.0)
	{
	}

	SynthVoiceParam(short minValue, short maxValue, short defaultValue, double minDoubleValue, double maxDoubleValue)
		:
		m_value(defaultValue),
		m_minValue(minValue),
		m_maxValue(maxValue),
		m_defaultValue(defaultValue),
		m_minDoubleValue(minDoubleValue),
		m_maxDoubleValue(maxDoubleValue)
	{
	}

	short Value() const { return m_value; }

	short MinValue() const { return m_minValue; }
	short MaxValue() const { return m_maxValue; }

	void SetValue(short value)
	{
		m_value = Math::Clamp(value, m_minValue, m_maxValue);
	}

	void ClearModulationAmount() { m_modulationAmount = 0.0; }

	void AddModulationAmount(double modulationAmount) { m_modulationAmount += modulationAmount; }

	double DoubleValue() const
	{
		double modulatedShortValue = m_value + m_modulationAmount;
		return Math::Lerp(m_minDoubleValue, m_maxDoubleValue, (modulatedShortValue - m_minValue) / ((double)m_maxValue - m_minValue));
	}
};

class SynthVoiceParams : public TSynthVoiceParams<SynthVoiceParam> { };