#pragma once

#include "Voice/SynthVoiceParam.h"

class SynthVoiceOptionalParam
{
protected:
	SynthVoiceParam m_param;
	bool m_hasValue = false;

public:

	SynthVoiceOptionalParam()
	{
	}

	SynthVoiceOptionalParam(short minValue, short maxValue, short defaultValue, double minFloatValue, double maxFloatValue)
		: m_param(minValue, maxValue, defaultValue, minFloatValue, maxFloatValue)
	{
	}

	bool HasValue() const { return m_hasValue; }

	short Value() const
	{
		assert(m_hasValue);
		return m_param.Value();
	}

	void SetValue(short value)
	{
		m_param.SetValue(value);
		m_hasValue = true;
	}

	void ClearValue() { m_hasValue = false; }

	double DoubleValue() const
	{
		assert(m_hasValue);
		return m_param.DoubleValue();
	}
};

class SynthVoiceOptionalParams : public TSynthVoiceParams<SynthVoiceOptionalParam> { };