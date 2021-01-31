#pragma once

#include "Constants.h"
#include "Voice/SynthParamReflection.h"
#include "Voice/SynthParamConstants.h"
#include "MyMath.h"
#include "Util.h"

#include <algorithm>
#include <cstdlib>
#include <memory>
#include <random>
#include <type_traits>

template <bool UseFrequency>
class Oscillator
{
private:
    double m_tune = 0.0;
    double m_fineTune = 0.0;

    double m_pitch = 0.0;
    double m_frequency = 0.0;

    double m_level = 1.0f;
    int m_oversampleFactor = 1;

    double m_value = 0.0;
    double m_periodTime = 0.0;

protected:
    bool m_canOversample = true;

public:
    
    template <typename = typename std::enable_if_t<!UseFrequency>>
    void SetPitch(double pitch) { m_pitch = pitch; }

    template <typename = typename std::enable_if_t<UseFrequency>>
    void SetFrequency(double frequency) { m_frequency = frequency; }

    bool GetCanOversample() const { return m_canOversample; }

    void SetParams(double tune, double fineTune, double level, int oversampleFactor)
    {
        m_tune = SynthParamConstants::OscillatorPitch.ClampDouble(tune);
        m_fineTune = SynthParamConstants::OscillatorFinePitch.ClampDouble(fineTune);
        m_level = SynthParamConstants::OscillatorLevel.ClampDouble(level);
        m_oversampleFactor = (int)SynthParamConstants::OscillatorOversampleFactor.ClampDouble(oversampleFactor);
    }

    double Next(double deltaSeconds)
    {
        m_value = 0.0f;

        double frequency = UseFrequency ? m_frequency : Util::ConvertPitchToFrequency(m_tune + m_pitch + m_fineTune / 100.0f);
        double deltaPeriod = deltaSeconds * frequency;
        double startPeriodTime = m_periodTime;

        int oversampleCount = m_canOversample ? m_oversampleFactor : 1;
        for (int oversampleIndex = 0; oversampleIndex < oversampleCount; ++oversampleIndex)
        {
            m_periodTime = startPeriodTime + deltaPeriod * ((double)oversampleIndex + 1) / oversampleCount;
            while (m_periodTime >= 1.0) { m_periodTime -= 1.0; }
            m_value += CalculateValue(m_periodTime);
        }

        m_value = (m_value / oversampleCount) * m_level;
        return m_value;
    }

    void Reset(double phase) { m_periodTime = phase; }

    virtual double GetValue() const { return m_value; }

    virtual double CalculateValue(double preiodTime) = 0;
};


template<bool UseFrequency> class SinOscillator : public Oscillator<UseFrequency> { virtual double CalculateValue(double periodTime) override { return sin(periodTime * Constants::PI * 2.0); } };
template<bool UseFrequency> class TriangleOscillator : public Oscillator<UseFrequency> { virtual double CalculateValue(double periodTime) override { return periodTime < 0.5 ? 4.0 * periodTime - 1.0 : -4.0 * periodTime + 3.0; } };
template<bool UseFrequency> class SquareOscillator : public Oscillator<UseFrequency> { virtual double CalculateValue(double periodTime) override { return periodTime < 0.5 ? -1.0 : 1.0; } };
template<bool UseFrequency> class SawOscillator : public Oscillator<UseFrequency> { virtual double CalculateValue(double periodTime) override { return periodTime * 2.0 - 1.0; } };

template<bool UseFrequency>
class NoiseOscillator : public Oscillator<UseFrequency>
{
    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution{ -1.0, 1.0 };

public:
    NoiseOscillator() 
        : Oscillator<UseFrequency>()
    {
        this->m_canOversample = false;
    }

protected:
    double CalculateValue(double periodTime) override { return distribution(generator); }
};

template<bool UseFrequency>
class OscillatorBank : public Oscillator<UseFrequency>
{
public:
    SinOscillator<UseFrequency> SinOscillator;
    TriangleOscillator<UseFrequency> TriangleOscillator;
    SquareOscillator<UseFrequency> SquareOscillator;
    SawOscillator<UseFrequency> SawOscillator;
    NoiseOscillator<UseFrequency> NoiseOscillator;

    std::vector<Oscillator<UseFrequency>*> Oscillators;

private:
    int m_selectedOscillatorIndex = 3;
    Oscillator<UseFrequency>* m_currentOscillator = nullptr;

public:

    OscillatorBank() { }

    void Initialize()
    {
        Oscillators.push_back(&SinOscillator);
        Oscillators.push_back(&TriangleOscillator);
        Oscillators.push_back(&SquareOscillator);
        Oscillators.push_back(&SawOscillator);
        Oscillators.push_back(&NoiseOscillator);

        m_currentOscillator = Oscillators[m_selectedOscillatorIndex];
    }

    // Okay for these to be non-virtual, not called polymorphically
    template <typename = typename std::enable_if_t<!UseFrequency>>
    void SetPitch(double pitch)
    {
        Oscillator<UseFrequency>::SetPitch(pitch);

        for (auto oscillator : Oscillators)
        {
            oscillator->SetPitch(pitch);
        }
    }

    template <typename = typename std::enable_if_t<UseFrequency>>
    void SetFrequency(double frequency)
    {
        Oscillator<UseFrequency>::SetFrequency(frequency);

        for (auto oscillator : Oscillators)
        {
            oscillator->SetFrequency(frequency);
        }
    }

    Oscillator<UseFrequency>* GetCurrentOscillator() { return m_currentOscillator; }

    void SetSelectedOscillatorIndex(int index)
    {
        m_selectedOscillatorIndex = index;
        m_currentOscillator = Oscillators[m_selectedOscillatorIndex];
        this->m_canOversample = m_currentOscillator->GetCanOversample();
    }

    int GetSelectedOscillatorIndex() const { return m_selectedOscillatorIndex; }

protected:
    virtual double CalculateValue(double periodTime) override
    {
        return m_currentOscillator->CalculateValue(periodTime);
    }
};