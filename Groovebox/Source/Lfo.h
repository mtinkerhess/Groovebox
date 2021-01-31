#pragma once

class LFO
{
private:
    double m_periodTime{};
    double m_frequency{};
    double m_value{};

public:
    void SetFrequency(double frequency)
    {
        m_frequency = frequency;
    }

    double Next(double deltaSeconds)
    {
        m_periodTime += deltaSeconds * m_frequency;
        while (m_periodTime >= 1.0f)
        {
            m_periodTime -= 1.0f;
        }

        m_value = CalculateValue(m_periodTime);

        return m_value;
    }

    double GetValue() const { return m_value; }

protected:
    virtual double CalculateValue(double periodTime) const = 0;
};

class SawLFO : LFO
{
protected:
    virtual double CalculateValue(double periodTime) const override
    {
        return periodTime * 2.0f - 1.0f;
    }
};

class TriangleLFO : public LFO
{
protected:
    virtual double CalculateValue(double periodTime) const override
    {
        if (periodTime < 0.5f)
        {
            return periodTime * 4.0f - 1.0f;
        }
        else
        {
            return periodTime * -4.0f + 3.0f;
        }
    }
};
