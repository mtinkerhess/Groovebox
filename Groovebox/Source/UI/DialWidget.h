#pragma once

#include "UI/Widget.h"
#include "Voice/SynthParamGetters.h"

class ImageWidget;
class GrooveboxWindow;
class LabelWidget;
class SequenceStep;

class DialWidget : public Widget
{
public:
    const int DragSensetivity = 20;
    const int DragSensetivityWithShift = 200;
    const int AlwaysUseSlowDraggingRange = 10;
    const std::string TextureName = "Dial";

private:
    ImageWidget* m_image = nullptr;
    LabelWidget* m_nameLabel = nullptr;
    LabelWidget* m_valueLabel = nullptr;
    LabelWidget* m_overrideValueLabel = nullptr;
    short m_value = 0;
    short m_minValue;
    short m_maxValue;
    int m_dragAccumulation = 0;
    GrooveboxWindow& m_window;
    SimpleSynthParamGetter m_simpleParamGetter;
    SynthParamGetter m_paramGetter;
    SynthOptionalParamGetter m_optionalParamGetter;
    SynthModulationShortParamGetter m_shortParamGetter;
    bool m_alwaysUseSlowDragging = false;

    bool m_isSimpleDial = false;

public:
    DialWidget(Widget* parent,
        const std::wstring& labelText,
        short value,
        short minValue,
        short maxValue,
        SynthParamGetter simpleParamGetter,
        SynthOptionalParamGetter optionalParamGetter,
        SynthModulationShortParamGetter shortParamGetter,
        GrooveboxWindow& window);

    DialWidget(
        Widget* parent,
        const std::wstring& labelText,
        SimpleSynthParamGetter paramGetter,
        GrooveboxWindow& window);

private:
    void Initialize(const std::wstring& labelText, short value);

public:
    short GetValue() const;

    void SetValue(short value);

    void UpdateOverriddenValue();

    bool IsSimpleDial() const;

    SynthVoiceParam& GetSimpleParam();

    SynthVoiceParam& GetParam(SynthVoiceParams& params);

    SynthVoiceOptionalParam& GetParam(SequenceStep& step);
    
    SynthVoiceOptionalParam& GetParam(SynthVoiceOptionalParams& params);
    
    short& GetParam(SynthVoiceModulationShortParams& params);

protected:
    bool HandleClickInternal(int button) override;

    bool CapturesCursorOnMouseDown(int button) const override;

    void CapturedCursorBegin() override;

    void CapturedCursorEnd() override;

    void CapturedCursorDragged(sf::Vector2i deltaMousePosition, int mouseButton) override;

};