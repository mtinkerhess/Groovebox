#include "SelectableButtonWidget.h"

SelectableButtonWidget::SelectableButtonWidget(
    Widget* parent,
    ClickHandler clickHandler,
    const std::string& selectedImageName,
    const std::wstring& labelText)
    : Widget(parent)
{
    m_selectedImage = new ImageWidget(this, selectedImageName);
    m_selectedImage->SetBoundsDelta(-5.0f, -5.0f, 10.0f, 10.0f);
    m_selectedImage->SetIsActive(false);

    m_button = new ButtonWidget(
        this,
        clickHandler,
        labelText
    );

    m_hasParamLockImage = new ImageWidget(this, "Pip_Off");
    m_hasParamLockImage->SetAnchors(0.5f, 0.5f, 0.5f, 0.5f);
    m_hasParamLockImage->SetBoundsDelta(-10, 50, 20, 20);
}

ButtonWidget& SelectableButtonWidget::GetButton() { return *m_button; }

ImageWidget& SelectableButtonWidget::GetSelectedImage() { return *m_selectedImage; }

bool SelectableButtonWidget::GetIsSelected() const { return m_isSelected; }

void SelectableButtonWidget::SelectableButtonWidget::SetIsSelected(bool isSelected)
{
    if (m_isSelected != isSelected)
    {
        m_isSelected = isSelected;
        m_selectedImage->SetIsActive(m_isSelected);
    }
}

void SelectableButtonWidget::ToggleIsSelected()
{
    m_isSelected = !m_isSelected;
    m_selectedImage->SetIsActive(m_isSelected);
}

void SelectableButtonWidget::SetHasParamLock(bool hasParamLock)
{
    if (m_hasParamLock != hasParamLock)
    {
        m_hasParamLock = hasParamLock;
        m_hasParamLockImage->SetTexture(m_hasParamLock ? "Pip_On" : "Pip_Off");
    }
}