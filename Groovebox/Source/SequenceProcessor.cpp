#include "SequenceProcessor.h"

#include "Sequence.h"
#include "SequencerStream.h"
#include "Voice/SynthVoiceModule.h"

void SequenceProcessor::ProcessNextSample(const SynthVoiceParams& defaultParams, double tempo)
{
    // This all takes place on the audio thread.

    int previousStep = (int)m_stepInSequence;

    double deltaStep = m_deltaSecondsPerSample * tempo / 60.0f;
    m_stepInSequence += deltaStep;

    bool wrapped = m_stepInSequence >= m_sequence.GetLength();
    if (wrapped)
    {
        m_stepInSequence -= m_sequence.GetLength();
    }

    if (m_noteIsOn)
    {
        m_stepsLeftInCurrentNote -= deltaStep;
        if (m_stepsLeftInCurrentNote <= 0.0f)
        {
            StopNote();
        }
    }

    int currentStep = (int)m_stepInSequence;

    if (currentStep != previousStep || wrapped || m_firstTick)
    {
        m_sequencerStream.StepChanged(StepChangedEvent(m_track, &m_sequence.GetStep(currentStep), m_firstTick));

        const SequenceStep& step = m_sequence.GetStep(currentStep);

        const SynthVoiceOptionalParams& params = step.GetParams();

        if (step.GetOn())
        {
            
            double note = params.SynthParams.Note.HasValue() ? params.SynthParams.Note.DoubleValue() : defaultParams.SynthParams.Note.DoubleValue();
            double velocity = params.SynthParams.Velocity.HasValue() ? params.SynthParams.Velocity.DoubleValue() : defaultParams.SynthParams.Velocity.DoubleValue();
            double length = params.SynthParams.Length.HasValue() ? params.SynthParams.Length.DoubleValue() : defaultParams.SynthParams.Length.DoubleValue();

            StartNote(note, velocity, length);
        }
    }

    m_firstTick = false;
}

void SequenceProcessor::StartNote(double note, double velocity, double stepLength)
{
    // Takes place on the audio thread
    if (m_noteIsOn)
    {
        StopNote();
    }

    m_noteIsOn = true;
    m_stepsLeftInCurrentNote = stepLength;

    m_sequencerStream.NoteStarted(NoteStartedEvent(m_track, note, velocity));
}

void SequenceProcessor::StopNote()
{
    // Takes place on the audio thread
    m_noteIsOn = false;
    m_sequencerStream.NoteEnded(NoteEndedEvent(m_track));
}