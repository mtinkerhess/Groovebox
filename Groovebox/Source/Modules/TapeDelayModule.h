#pragma once

#include "Modules/MixerModule.h"
#include "Modules/SignalModule.h"
#include "Constants.h"
#include "MyMath.h"

#include <array>

template <size_t BufferLength>
class DelayLineTape
{
	static const int EraseLength = 11;

	// Data on the "tape loop".
	std::array<double, BufferLength> m_buffer{};

	// How far the "tape loop" has moved. Also the current index of the write head into the buffer.
	double m_bufferPosition = 0;

	// Position of the read head relative to the (fixed) write head.
	double m_readHeadPosition = BufferLength - 1;

public:
	DelayLineTape()
	{
	}

	double GetReadHeadPosition() const { return m_readHeadPosition; }
	void SetReadHeadPosition(double position) { m_readHeadPosition = Math::Clamp(position, 2.0, BufferLength - 2.0 - EraseLength); }

	double IncrementBuffer(double amount, double writeValue)
	{
		if (amount == 0.0)
		{
			return 0.0;
		}

		double nextBufferPosition = m_bufferPosition + amount;

		size_t bufferPositionIndex = (size_t)m_bufferPosition;
		size_t nextBufferPositionIndex = (size_t)nextBufferPosition;

		// TODO erase better
		size_t previousIndex = ((bufferPositionIndex + 2) + BufferLength) % BufferLength;

		for (int i = 0; i < EraseLength; ++i)
		{
			m_buffer[(previousIndex + i) % BufferLength] = 0.0;
		}

		if (bufferPositionIndex == nextBufferPositionIndex)
		{
			m_buffer[bufferPositionIndex] += writeValue;
		}
		else
		{
			double bufferPositionInterpolationAmount = 1.0 - (m_bufferPosition - bufferPositionIndex);
			double endBufferPositionInterpolationAmount = nextBufferPosition - nextBufferPositionIndex;

			for (size_t i = bufferPositionIndex; i <= nextBufferPositionIndex; ++i)
			{
				double fractionalMultiplier =
					i == bufferPositionIndex ? bufferPositionInterpolationAmount
					: i == nextBufferPositionIndex ? endBufferPositionInterpolationAmount
					: 1.0;

				m_buffer[i % BufferLength] += fractionalMultiplier * writeValue;
			}
		}

		double indexInBuffer = GetPositionInBuffer(m_readHeadPosition);

		size_t lowerIndex = (size_t)indexInBuffer;
		size_t upperIndex = (lowerIndex + 1) % BufferLength;
		double interpolationAmount = indexInBuffer - lowerIndex;

		double lowerValue = m_buffer[lowerIndex];
		double upperValue = m_buffer[upperIndex];

		m_bufferPosition = nextBufferPosition;
		while (m_bufferPosition >= BufferLength) { m_bufferPosition -= BufferLength; }

		return Math::LerpUnclamped(lowerValue, upperValue, interpolationAmount);
	}

private:
	double GetPositionInBuffer(double index) const
	{
		double result = m_bufferPosition - index + BufferLength;
		while (result >= BufferLength) { result -= BufferLength; }
		return result;
	}
};

class DelayLineModule : public SignalModule
{
public:
	static const int MaxDelayLengthSeconds = 1;
	static const size_t MaxDelayLengthSamples = MaxDelayLengthSeconds * Constants::SamplesPerSecond;

private:
	DelayLineTape<MaxDelayLengthSamples>* m_tape = nullptr;
	double m_tapeSpeed = 1.0;

public:
	DelayLineModule() { }

	explicit DelayLineModule(const std::string& name)
		: SignalModule(name),
		m_tape(new DelayLineTape<MaxDelayLengthSamples>)
	{
	}

	double m_time = 0.0;

	double CalculateValue(double deltaSeconds) override
	{
		double inputValue = GetInput()->GetValue(deltaSeconds);

		double newTimeIntoBuffer = m_time + deltaSeconds;
		double startSampleIndex = (m_time * Constants::SamplesPerSecond * m_tapeSpeed);
		double endSampleIndex = (newTimeIntoBuffer * Constants::SamplesPerSecond * m_tapeSpeed);
		double numSamplesToProcess = endSampleIndex - startSampleIndex;
		m_time += deltaSeconds;
		if (m_time > 1.0) { m_time -= 1.0; }
		return m_tape->IncrementBuffer(deltaSeconds * m_tapeSpeed * Constants::SamplesPerSecond, inputValue);
	}

	void SetTapeSpeed(double tapeSpeed) { m_tapeSpeed = SynthParamConstants::DelayTapeSpeed.ClampDouble(tapeSpeed); }
	
	void SetReadHeadDistance(double readHeadDistance)
	{
		m_tape->SetReadHeadPosition(SynthParamConstants::DelayHeadDistance.ClampDouble(readHeadDistance) * MaxDelayLengthSamples);
	}
};

class TapeDelayModule : public SignalModule
{
private:
	MixerModule m_panMixer;
	MixerModule m_feedbackPanMixer;
	MixerModule m_pingPongMixer;
	MixerModule m_delayLineMixer;
	DelayLineModule m_delayLine;
	MixerModule m_wetMixer;

	DelayLineModule* m_oppositeDelayLine = nullptr;
	MixerModule* m_oppositeFeedbackPanMixer = nullptr;

	bool m_initialized = false;

public:
	TapeDelayModule() { }

	explicit TapeDelayModule(const std::string& name)
		: SignalModule(name)
	{
	}

	void Initialize() override
	{
		m_initialized = true;

		m_panMixer = MixerModule("Pan mixer");
		m_feedbackPanMixer = MixerModule("Feedback pan mixer");
		m_pingPongMixer = MixerModule("Ping pong mixer");
		m_delayLineMixer = MixerModule("Delay line mixer");
		m_delayLine = DelayLineModule("Delay line");
		m_wetMixer = MixerModule("Wet mixer");

		m_panMixer.SetFirstInput(GetInput());
		m_panMixer.SetSecondInput(GetOppositeModule()->GetInput());
		m_panMixer.SetBalance(0.0); // Pan

		m_feedbackPanMixer.SetFirstInput(&m_delayLine);
		m_feedbackPanMixer.SetFirstInputLevel(1.0); // Ping pong
		m_feedbackPanMixer.SetSecondInputLevel(0.0); // Ping pong

		m_pingPongMixer.SetFirstInput(&m_feedbackPanMixer);
		m_pingPongMixer.SetBalance(0.0); // Ping pong

		m_delayLineMixer.SetFirstInput(&m_panMixer);
		m_delayLineMixer.SetSecondInput(&m_pingPongMixer);
		m_delayLineMixer.SetFirstInputLevel(1.0); // Send
		m_delayLineMixer.SetSecondInputLevel(0.5); // Feedback

		m_delayLine.SetInput(&m_delayLineMixer);

		m_wetMixer.SetFirstInput(GetInput());
		m_wetMixer.SetSecondInput(&m_delayLine);
		m_wetMixer.SetFirstInputLevel(1.0); // Dry
		m_wetMixer.SetSecondInputLevel(0.0); // Wet

		AddContainedModule(&m_panMixer);
		AddContainedModule(&m_pingPongMixer);
		AddContainedModule(&m_feedbackPanMixer);
		AddContainedModule(&m_delayLineMixer);
		AddContainedModule(&m_delayLine);
		AddContainedModule(&m_wetMixer);

		CheckHookUpOppositeDelayLine();
		static_cast<TapeDelayModule*>(GetOppositeModule())->SetOppositeDelayLine(&m_delayLine, &m_feedbackPanMixer);
	}

	double CalculateValue(double deltaSeconds) override
	{
		return m_wetMixer.GetValue(deltaSeconds);
	}

	void GetPanLevels(double relativePan, double& panInputLevel, double& oppositePanInputLevel)
	{
		// How much of my input's signal should I use, because of panning?
		// This should be 1 if pan is >= 0,
		// and 0.5 if pan is all the way to our side.
		panInputLevel = Math::Clamp01(1.0 + relativePan / 2.0);

		// How much of my opposite's should I use, because of panning?
		// This should be 0 if pan is >= 0,
		// and 0.5 if pan is all the way on our side.
		oppositePanInputLevel = Math::Clamp01(-relativePan / 2.0);
	}

	void SetParams(
		double send,
		double feedback,
		double wetLevel,
		double dryLevel,
		double tapeSpeed,
		double headDistance,
		double pan,
		double pingPong)
	{
		// Relative to whether this is the left channel or right channel.
		double relativePan = GetIsLeftChannel() ? pan : -pan;

		// How loud should the input into this channel be, because of panning?
		double panMixerLevel = Math::Clamp01(1.0 - relativePan);
		double oppositePanMixerLevel = Math::Clamp01(1.0 + relativePan);

		double panInputLevel, oppositePanInputLevel;
		GetPanLevels(relativePan, panInputLevel, oppositePanInputLevel);

		m_panMixer.SetFirstInputLevel(panInputLevel * panMixerLevel);
		m_panMixer.SetSecondInputLevel(oppositePanInputLevel * panMixerLevel);

		bool flipFeedbackPan = pingPong > 0.5;
		if (flipFeedbackPan)
		{
			m_feedbackPanMixer.SetFirstInputLevel(1.0);
			m_feedbackPanMixer.SetSecondInputLevel(0.0);
		}
		else
		{
			m_feedbackPanMixer.SetFirstInputLevel(panMixerLevel);
			m_feedbackPanMixer.SetSecondInputLevel(oppositePanInputLevel * panMixerLevel);
		}

		m_pingPongMixer.SetBalance(pingPong); // Ping pong

		m_delayLineMixer.SetFirstInputLevel(send);
		m_delayLineMixer.SetSecondInputLevel(feedback);

		m_wetMixer.SetFirstInputLevel(dryLevel);
		m_wetMixer.SetSecondInputLevel(wetLevel);

		m_delayLine.SetTapeSpeed(tapeSpeed);
		m_delayLine.SetReadHeadDistance(headDistance);
	}

	void SetOppositeDelayLine(DelayLineModule* oppositeDelayLine, MixerModule* oppositeFeedbackPanMixer)
	{
		m_oppositeDelayLine = oppositeDelayLine;
		m_oppositeFeedbackPanMixer = oppositeFeedbackPanMixer;
		CheckHookUpOppositeDelayLine();
	}

private:
	void CheckHookUpOppositeDelayLine()
	{
		if (m_initialized && m_oppositeDelayLine != nullptr)
		{
			m_feedbackPanMixer.SetSecondInput(m_oppositeDelayLine);
			m_pingPongMixer.SetSecondInput(m_oppositeFeedbackPanMixer);
		}
	}
};