#include "SequencerToggleWidget.h"

#include "GrooveboxWindow/GrooveboxWindow.h"

#include <sstream>

using namespace std;

SequencerToggleWidget::SequencerToggleWidget(
    Widget* parent,
    int step,
    GrooveboxWindow& window)
    : Widget(parent),
    m_step(step),
    m_window(window)
{
    wstringstream ss;
    ss << (step + 1);

    m_selectableButton = new SelectableButtonWidget(
        this,
        [this](int mouseButton) { return m_window.SequencerToggleClicked(m_step, mouseButton); },
        "Button_Selected",
        ss.str());

    m_stepOnPipImage = new ImageWidget(m_selectableButton, "Pip_Off");
    m_stepOnPipImage->SetAnchors(0.5f, 0.5f, 0.5f, 0.5f);
    m_stepOnPipImage->SetBoundsDelta(-10, -10, 20, 20);

    m_isCurrentStepPipImage = new ImageWidget(m_selectableButton, "Pip_Off");
    m_isCurrentStepPipImage->SetAnchors(0.5f, 0.5f, 0.5f, 0.5f);
    m_isCurrentStepPipImage->SetBoundsDelta(-10, 30, 20, 20);

    LabelWidget& label = m_selectableButton->GetButton().GetLabel();
    label.SetAnchors(0.5f, 0.0f, 0.5f, 0.0f);
    label.SetBoundsDelta(0.0f, -20.0f, 0.0f, 0.0f);
}

void SequencerToggleWidget::SetStepOnPipIsOn(bool isOn)
{
    if (m_stepOnPipIsOn == isOn)
    {
        return;
    }

    m_stepOnPipIsOn = isOn;
    m_stepOnPipImage->SetTexture(m_stepOnPipIsOn ? "Pip_On" : "Pip_Off");
}

void SequencerToggleWidget::SetIsCurrentStepPipIsOn(bool isOn)
{
    if (m_isCurrentStepPipIsOn == isOn)
    {
        return;
    }

    m_isCurrentStepPipIsOn = isOn;
    m_isCurrentStepPipImage->SetTexture(m_isCurrentStepPipIsOn ? "Pip_On" : "Pip_Off");
}