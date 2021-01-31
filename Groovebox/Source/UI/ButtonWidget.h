#pragma once

#include "UI/Widget.h"

#include <string>

class ImageWidget;
class LabelWidget;

class ButtonWidget : public Widget
{
private:
    ClickHandler m_clickHandler;
    ImageWidget* m_image;
    LabelWidget* m_label;

public:
    ButtonWidget(Widget* parent, ClickHandler clickHandler, const std::wstring& text);

    ButtonWidget(Widget* parent, ClickHandler clickHandler);
    
    LabelWidget& GetLabel();

    void SetText(const std::wstring& text);

    bool HandleClickInternal(int button) override;
};
