#pragma once

#include "UI/Widget.h"

class LabelWidget : public Widget
{
private:
    sf::Text m_text;

public:
    LabelWidget(Widget* parent, const std::wstring& text, int fontSize);

    void SetText(const std::wstring& text);
    
    const std::wstring GetText() const;

    void DrawInternal(sf::RenderWindow& window) override;

    void SetColor(sf::Color color);
};
