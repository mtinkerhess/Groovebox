#include "LabelWidget.h"

#include "ResourceMap.h"
#include "Util.h"

using namespace sf;
using namespace std;

LabelWidget::LabelWidget(Widget* parent, const wstring& text, int fontSize)
    : Widget(parent)
{
    const Font& font = Fonts::Instance().Get("Roboto");
    m_text.setFont(font);
    m_text.setCharacterSize(fontSize);
    m_text.setFillColor(Color::White);
    SetText(text);
}

void LabelWidget::SetText(const wstring& text)
{
    m_text.setString(text);
    FloatRect bounds = m_text.getLocalBounds();
    m_text.setOrigin(Vector2f(bounds.width / 2.0f, bounds.height / 2.0f));
}

const wstring LabelWidget::GetText() const
{
    return m_text.getString();
}

void LabelWidget::DrawInternal(RenderWindow& window)
{
    FloatRect globalBounds = GetGlobalBounds();
    m_text.setPosition(globalBounds.left + globalBounds.width / 2, globalBounds.top + globalBounds.height / 2);
    window.draw(m_text);
}

void LabelWidget::SetColor(Color color)
{
    m_text.setFillColor(color);
}