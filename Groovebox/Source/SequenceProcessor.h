#pragma once

#include "EventTypes.h"

class Sequence;
class SequencerStream;
class SynthVoiceParams;
class SequenceStep;

class SequenceProcessor
{
private:
    const Sequence& m_sequence;
    int m_track;

    int m_samplesPerSecond;
    double m_deltaSecondsPerSample;

    double m_stepInSequence;

    bool m_noteIsOn = false;
    double m_stepsLeftInCurrentNote = 0.0f;

    SequencerStream& m_sequencerStream;

    bool m_firstTick = true;

public:
    SequenceProcessor(
        const Sequence& sequence,
        int track,
        int samplesPerSecond,
        SequencerStream& sequencerStream)
        : m_sequence(sequence),
        m_track(track),
        m_samplesPerSecond(samplesPerSecond),
        m_deltaSecondsPerSample(1.0f / samplesPerSecond),
        m_stepInSequence(0.0f),
        m_sequencerStream(sequencerStream)
    {
    }

    void ProcessNextSample(const SynthVoiceParams& defaultParams, double tempo);

private:
    void StartNote(double note, double velocity, double stepLength);

    void StopNote();
};