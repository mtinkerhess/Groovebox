#include "DialWidget.h"

#include "GrooveboxWindow/GrooveboxWindow.h"
#include "Sequence.h"
#include "UI/ImageWidget.h"
#include "UI/LabelWidget.h"
#include "UI/Widget.h"
#include "Util.h"
#include "Voice/SynthParamGetters.h"

#include <sstream>

using namespace std;

DialWidget::DialWidget(Widget* parent,
    const wstring& labelText,
    short value,
    short minValue,
    short maxValue,
    SynthParamGetter paramGetter,
    SynthOptionalParamGetter optionalParamGetter,
    SynthModulationShortParamGetter shortParamGetter,
    GrooveboxWindow& window)
    : Widget(parent),
    m_minValue(minValue),
    m_maxValue(maxValue),
    m_paramGetter(paramGetter),
    m_optionalParamGetter(optionalParamGetter),
    m_shortParamGetter(shortParamGetter),
    m_window(window)
{
    Initialize(labelText, value);
}

DialWidget::DialWidget(
    Widget* parent,
    const wstring& labelText,
    SimpleSynthParamGetter simpleParamGetter,
    GrooveboxWindow& window)
    : Widget(parent),
    m_minValue(simpleParamGetter().MinValue()),
    m_maxValue(simpleParamGetter().MaxValue()),
    m_simpleParamGetter(simpleParamGetter),
    m_isSimpleDial(true),
    m_window(window)
{
    Initialize(labelText, simpleParamGetter().Value());
}

void DialWidget::Initialize(const wstring& labelText, short value)
{
    m_image = new ImageWidget(this, TextureName);

    m_nameLabel = new LabelWidget(this, labelText, 24);
    m_nameLabel->SetPointAnchor(0.5f, 0.0f);
    m_nameLabel->SetBoundsDelta(0.0f, -15.0f, 0.0f, 0.0f);

    m_valueLabel = new LabelWidget(this, L"", 24);
    m_valueLabel->SetPointAnchor(0.5f, 1.0f);
    m_valueLabel->SetBoundsDelta(0.0f, 15.0f, 0.0f, 0.0f);

    m_overrideValueLabel = new LabelWidget(this, L"", 24);
    m_overrideValueLabel->SetPointAnchor(0.5f, 1.0f);
    m_overrideValueLabel->SetBoundsDelta(0.0f, 45.0f, 0.0f, 0.0f);

    m_alwaysUseSlowDragging = m_maxValue - m_minValue <= AlwaysUseSlowDraggingRange;

    SetValue(value);
}

short DialWidget::GetValue() const { return m_value; }

void DialWidget::SetValue(short value)
{
    short oldValue = m_value;
    m_value = Math::Clamp(value, m_minValue, m_maxValue);

    wstringstream ss;
    ss << m_value;
    m_valueLabel->SetText(ss.str());
}

void DialWidget::UpdateOverriddenValue()
{
    if (m_isSimpleDial) { return; }

    std::string overriddenValueString;
    bool valueIsOverridden = m_window.ValueIsOverridden(m_optionalParamGetter, m_shortParamGetter, overriddenValueString);

    if (valueIsOverridden)
    {
        m_overrideValueLabel->SetText(Util::ConvertString<wstring>(overriddenValueString));
    }
    else
    {
        m_overrideValueLabel->SetText(L"");
    }
}

bool DialWidget::IsSimpleDial() const { return m_isSimpleDial; }

SynthVoiceParam& DialWidget::GetSimpleParam() { return m_simpleParamGetter(); }

SynthVoiceParam& DialWidget::GetParam(SynthVoiceParams& params) { return m_paramGetter(params); }

SynthVoiceOptionalParam& DialWidget::GetParam(SequenceStep& step) { return GetParam(step.GetParams()); }

SynthVoiceOptionalParam& DialWidget::GetParam(SynthVoiceOptionalParams& params) { return m_optionalParamGetter(params); }

short& DialWidget::GetParam(SynthVoiceModulationShortParams& params) { return m_shortParamGetter(params); }

bool DialWidget::HandleClickInternal(int button) { return m_window.HandleDialClicked(*this, button) || Widget::HandleClickInternal(button); }

bool DialWidget::CapturesCursorOnMouseDown(int button) const { return button == 0 || button == 1; }

void DialWidget::CapturedCursorBegin() { m_dragAccumulation = 0; }

void DialWidget::CapturedCursorEnd() { }

void DialWidget::CapturedCursorDragged(sf::Vector2i deltaMousePosition, int mouseButton)
{
    m_dragAccumulation -= deltaMousePosition.y;

    int sensetivity = m_alwaysUseSlowDragging || Util::IsShiftPressed() ? DragSensetivityWithShift : DragSensetivity;

    while (m_dragAccumulation >= sensetivity)
    {
        m_window.DialValueChanged(*this, 1, mouseButton);
        m_dragAccumulation -= sensetivity;
    }

    while (m_dragAccumulation <= -sensetivity)
    {
        m_window.DialValueChanged(*this, -1, mouseButton);
        m_dragAccumulation += sensetivity;
    }
}