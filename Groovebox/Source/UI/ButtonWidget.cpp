#include "ButtonWidget.h"

#include "UI/ImageWidget.h"
#include "UI/LabelWidget.h"

using namespace std;

ButtonWidget::ButtonWidget(Widget* parent, ClickHandler clickHandler, const std::wstring& text)
    : Widget(parent),
    m_clickHandler(clickHandler)
{
    m_image = new ImageWidget(this, "Button");
    m_label = new LabelWidget(this, text, 20);
    m_label->SetPointAnchor(0.5f, 0.5f);
    m_label->SetColor(sf::Color::Black);
}

ButtonWidget::ButtonWidget(Widget* parent, ClickHandler clickHandler) : ButtonWidget(parent, clickHandler, L"") { }

LabelWidget& ButtonWidget::GetLabel() { return *m_label; }

void ButtonWidget::SetText(const wstring& text) { m_label->SetText(text); }

bool ButtonWidget::HandleClickInternal(int button) { return m_clickHandler(button) || Widget::HandleClickInternal(button); }