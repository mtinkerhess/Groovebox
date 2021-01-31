#pragma once

#include "MulticastDelegate.h"
#include "Voice/SynthVoiceModule.h"

#include <optional>
#include <vector>

class SequenceStep
{
private:
    int m_index = -1;
    bool m_on = false;
    SynthVoiceOptionalParams m_params;

public:
    SequenceStep() { }
    SequenceStep(int index)
        : m_index(index)
    {
    }

    int GetIndex() const { return m_index; }
    bool GetOn() const { return m_on; }
    void SetOn(bool on) { m_on = on; }

    SynthVoiceOptionalParams& GetParams() { return m_params; }
    const SynthVoiceOptionalParams& GetParams() const { return m_params; }
};

class Sequence
{
private:
    std::vector<SequenceStep> m_steps;

public:
    Sequence() { }

    Sequence(int length)
    {
        for (int i = 0; i < length; ++i)
        {
            m_steps.push_back(SequenceStep(i));
        }
    }

    int GetLength() const { return m_steps.size(); }

    SequenceStep& GetStep(int stepIndex) { return m_steps[stepIndex]; }
    const SequenceStep& GetStep(int stepIndex) const { return m_steps[stepIndex]; }
};
