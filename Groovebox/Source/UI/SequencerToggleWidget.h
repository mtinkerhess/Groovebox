#pragma once

#include "UI/ButtonWidget.h"
#include "UI/ImageWidget.h"
#include "UI/LabelWidget.h"
#include "UI/SelectableButtonWidget.h"
#include "UI/Widget.h"

class GrooveboxWindow;

class SequencerToggleWidget : Widget
{
private:
    SelectableButtonWidget* m_selectableButton;
    int m_step;
    ImageWidget* m_stepOnPipImage;
    ImageWidget* m_isCurrentStepPipImage;
    GrooveboxWindow& m_window;

    bool m_stepOnPipIsOn = false;
    bool m_isCurrentStepPipIsOn = false;

public:
    SequencerToggleWidget(Widget* parent, int step, GrooveboxWindow& window);

    void SetStepOnPipIsOn(bool isOn);
    void SetIsCurrentStepPipIsOn(bool isOn);

    int GetStepIndex() { return m_step; }

    SelectableButtonWidget& GetSelectableButton() { return *m_selectableButton; }
};
