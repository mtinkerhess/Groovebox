#pragma once

#include "UI/ButtonWidget.h"
#include "UI/ImageWidget.h"
#include "UI/Widget.h"

class SelectableButtonWidget : public Widget
{
    ButtonWidget* m_button;
    ImageWidget* m_selectedImage;
    ImageWidget* m_hasParamLockImage;

    bool m_isSelected = false;
    bool m_hasParamLock = false;

public:
    SelectableButtonWidget(
        Widget* parent,
        ClickHandler clickHandler,
        const std::string& selectedImageName,
        const std::wstring& labelText);

    ButtonWidget& GetButton();

    ImageWidget& GetSelectedImage();

    bool GetIsSelected() const;

    void SetIsSelected(bool isSelected);

    void ToggleIsSelected();

    void SetHasParamLock(bool hasParamLock);
};
