#pragma once

#include <mutex>

#include <SFML/Audio.hpp>

#include "Constants.h"
#include "Voice/Envelope.h"
#include "MulticastDelegate.h"
#include "Voice/Oscillator.h"
#include "Sequence.h"
#include "SequenceProcessor.h"
#include "Voice/SynthVoiceModule.h"

#include <sstream>

class SequencerStream : public sf::SoundStream
{
private:

    MulticastDelegateWithPendingEvents<NoteStartedEvent> m_onNoteStarted;
    MulticastDelegateWithPendingEvents<NoteEndedEvent> m_onNoteEnded;
    MulticastDelegateWithPendingEvents<StepChangedEvent> m_onStepChanged;

public:

private:
    std::array<sf::Int16, Constants::SamplesPerChunk * 2> m_samples{};

    std::array<SequenceProcessor*, Constants::NumTracks> m_sequenceProcessors{};

    std::mutex m_mutex;
    int m_processingSampleIndex = 0;
    long m_previousSamplesProcessed = 0;

    std::array<SynthVoiceModule*, Constants::NumTracks> m_voiceModules{};
    bool m_applicationQuit = false;

    SynthVoiceParam m_tempo;
public:
    SynthVoiceModule& GetVoice(int index) { return *m_voiceModules[index]; }

    explicit SequencerStream(std::array<Sequence, Constants::NumTracks>& sequences)
        : m_tempo(1, 300, 128, 1.0, 300.0)
    {
        initialize(2, Constants::SamplesPerSecond);

        for (int i = 0; i < Constants::NumTracks; ++i)
        {
            m_sequenceProcessors[i] = new SequenceProcessor(
                sequences[i],
                i,
                Constants::SamplesPerSecond,
                *this);

            std::stringstream name;
            name << "Voice " << (i + 1);
            m_voiceModules[i] = new SynthVoiceModule(name.str(), i);
        }
    }

    MulticastDelegate<NoteStartedEvent>& OnNoteStarted() { return m_onNoteStarted.MulticastDelegate; }
    MulticastDelegate<NoteEndedEvent>& OnNoteEnded() { return m_onNoteEnded.MulticastDelegate; }
    MulticastDelegate<StepChangedEvent>& OnStepChanged() { return m_onStepChanged.MulticastDelegate; }

    void Update(double deltaSeconds)
	{
		double now = getPlayingOffset().asSeconds();
		m_onStepChanged.ProcessEvents(deltaSeconds, now);
		m_onNoteEnded.ProcessEvents(deltaSeconds, now);
		m_onNoteStarted.ProcessEvents(deltaSeconds, now);
    }

    void OnApplicationQuit()
    {
        std::scoped_lock lock(m_mutex);
        m_applicationQuit = true;
    }

    void NoteStarted(NoteStartedEvent event)
    {
        m_voiceModules[event.Track]->NoteStarted(event);
        m_onNoteStarted.PushEvent(event, Constants::SamplesPerSecond, m_previousSamplesProcessed, m_processingSampleIndex);
    }

    void NoteEnded(NoteEndedEvent event)
    {
        m_voiceModules[event.Track]->NoteEnded(event);
        m_onNoteEnded.PushEvent(event, Constants::SamplesPerSecond, m_previousSamplesProcessed, m_processingSampleIndex);
    }

    void StepChanged(StepChangedEvent event)
    {
        m_voiceModules[event.Track]->StepChanged(event);
        m_onStepChanged.PushEvent(event, Constants::SamplesPerSecond, m_previousSamplesProcessed, m_processingSampleIndex);
    }

    virtual bool onGetData(Chunk& data) override
    {
        std::scoped_lock lock(m_mutex);

        if (m_applicationQuit)
        {
            return false;
        }

        double deltaSecondsPerSample = (double)1 / Constants::SamplesPerSecond;

        std::array<double, Constants::SamplesPerChunk * 2> doubleSamples{};

        for (int trackIndex = 0; trackIndex < Constants::NumTracks; ++trackIndex)
        {
            SynthVoiceModule& voiceModule = *m_voiceModules[trackIndex];
            voiceModule.StartingBuffer();

            for (m_processingSampleIndex = 0; m_processingSampleIndex < Constants::SamplesPerChunk; ++m_processingSampleIndex)
            {
                m_sequenceProcessors[trackIndex]->ProcessNextSample(voiceModule.GetParams(), GetTempo());

                voiceModule.SetDirty();
                voiceModule.SetAmountIntoCurrentBuffer(m_processingSampleIndex / (double)Constants::SamplesPerChunk);

                double leftOutputValueDouble = voiceModule.GetValue(deltaSecondsPerSample);
                double rightOutputValueDouble = voiceModule.GetRightValue();

                leftOutputValueDouble = Math::Clamp(leftOutputValueDouble, -1.0, 1.0);
                rightOutputValueDouble = Math::Clamp(rightOutputValueDouble, -1.0, 1.0);

                doubleSamples[m_processingSampleIndex * 2] += leftOutputValueDouble;
                doubleSamples[m_processingSampleIndex * 2 + 1] += rightOutputValueDouble;
            }

            m_voiceModules[trackIndex]->FinishedBuffer();
        }

        for (int sampleIndex = 0; sampleIndex < Constants::SamplesPerChunk * 2; ++sampleIndex)
        {
            m_samples[sampleIndex] = (sf::Int16)(doubleSamples[sampleIndex] * Constants::MaxSampleValue / Constants::NumTracks);
        }

        data.sampleCount = Constants::SamplesPerChunk * 2;
        data.samples = &m_samples[0];
        m_previousSamplesProcessed += Constants::SamplesPerChunk;

        return true;
    }

    virtual void onSeek(sf::Time timeOffset) override
    {
    }

    double GetTempo() const { return m_tempo.DoubleValue() * 4.0; }

    SynthVoiceParam& GetTempoParam() { return m_tempo; }
};
