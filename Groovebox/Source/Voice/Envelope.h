#pragma once

#include "MyMath.h"
#include "Voice/SynthVoiceValueParams.h"

#include <cmath>

class Envelope
{
private:
	enum class Phase
	{
		Attack,
		Decay,
		Sustain,
		Release,
		Off
	};

	double m_attack = 0.0;
	double m_decay = 0.0;
	double m_sustain = 0.0;
	double m_release = 0.0;
	double m_amount = 0.0;

	Phase m_phase = Phase::Off;

	bool m_noteIsOn = false;
	double m_valueAtLastTransition = 0.0f;

	// Time in current phase from 0-1
	double m_time = 0.0f;

	double m_value = 0.0f;
	double m_previousValue = 0.0f;

	bool m_retrigger = false;

public:
	double GetAttack() const { return m_attack; }
	void SetAttack(double attack) { m_attack = attack; }
	double GetDecay() const { return m_decay; }
	void SetDecay(double decay) { m_decay = decay; }
	double GetSustain() const { return m_sustain; }
	void SetSustain(double sustain) { m_sustain = sustain; }
	double GetRelease() const { return m_release; }
	void SetRelease(double release) { m_release = release; }
	double GetAmount() const { return m_amount; }
	void SetAmount(double value) { m_amount = value; }
	bool GetRetrigger() const { return m_retrigger; }
	void SetRetrugger(bool retrigger) { m_retrigger = retrigger; }

	void NoteOn()
	{
		m_noteIsOn = true;
		m_phase = Phase::Attack;
		m_time = 0.0f;

		if (m_retrigger)
		{
			UpdateValue(0.0f);
		}

		m_valueAtLastTransition = m_value;
	}

	void NoteOff()
	{
		m_noteIsOn = false;
		m_phase = Phase::Release;
		m_time = 0.0f;
		m_valueAtLastTransition = m_value;
	}

	double Next(double deltaSeconds)
	{
		switch (m_phase)
		{
		case Phase::Attack:
			m_time += m_attack == 0.0f ? 1.0f : deltaSeconds / m_attack;
			break;
		case Phase::Decay:
			m_time += m_decay == 0.0f ? 1.0f : deltaSeconds / m_decay;
			break;
		case Phase::Sustain:
			break;
		case Phase::Release:
			m_time += m_release == 0.0f ? 1.0f : deltaSeconds / m_release;
			break;
		case Phase::Off:
			break;
		}

		if (m_time >= 1.0f)
		{
			NextPhase();
		}

		UpdateValue(deltaSeconds);
		return m_value * m_amount;
	}

	double GetValue() const { return m_value * m_amount; }

	void ApplyParams(SynthVoiceDoubleParams::EnvelopeParamsType params)
	{
		m_attack = params.Attack;
		m_decay = params.Decay;
		m_sustain = params.Sustain;
		m_release = params.Release;
		m_amount = params.Amount;
	}

private:
	void NextPhase()
	{
		if (m_phase < Phase::Off)
		{
			m_phase = (Phase)((int)m_phase + 1);
		}

		if (m_phase == Phase::Decay)
		{
			m_value = 1.0f;
		}

		m_time = 0.0f;
	}

	void UpdateValue(double deltaSeconds)
	{
		m_previousValue = m_value;
		m_value = CalculateValue(deltaSeconds);
	}

	double CalculateValue(double deltaSeconds)
	{
		switch (m_phase)
		{
		case Phase::Attack:
			return Math::Lerp(m_valueAtLastTransition, 1.0, m_time);
			break;
		case Phase::Decay:
		{
			double timeLeftInPhase = 1.0f - m_time;
			double secondsLeftInPhase = m_decay * timeLeftInPhase;
			return Math::Lerp(m_previousValue, m_sustain, deltaSeconds / secondsLeftInPhase);
		}
			break;
		case Phase::Sustain:
			return m_sustain;
			break;
		case Phase::Release:
			return Math::Lerp(m_valueAtLastTransition, 0.0, m_time);
			break;
		case Phase::Off:
			return 0.0f;
			break;
		}

		return 0.0f;
	}
};