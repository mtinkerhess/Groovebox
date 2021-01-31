#pragma once

#include "UI/Widget.h"

#include <string>

class ButtonWidget;
class ImageWidget;
class LabelWidget;
class TextInputWidget;

class ModalDialogWidget : public Widget
{
private:
	ImageWidget* m_backgroundImage;
	LabelWidget* m_messageLabel;
	TextInputWidget* m_textInputLabel;
	ButtonWidget* m_confirmationButton;

	bool m_requireTextInput = false;
	std::function<void(const std::wstring&)> m_onFinishedHandler;

public:
	ModalDialogWidget(Widget* root);

	bool CapturesCursorOnMouseDown(int button) const override;

	bool ConfirmationButtonClicked();

	void ShowModalDialog(const std::wstring& message);

	void ShowModalDialog(const std::wstring& message, bool requireTextInput, std::function<void(const std::wstring&)> onFinishedHandler = [](const std::wstring&) { });
};