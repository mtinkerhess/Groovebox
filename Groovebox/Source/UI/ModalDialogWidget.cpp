#include "ModalDialogWidget.h"

#include "Input/InputSystem.h"
#include "UI/ButtonWidget.h"
#include "UI/ImageWidget.h"
#include "UI/LabelWidget.h"
#include "UI/TextInputWidget.h"

using namespace std;

ModalDialogWidget::ModalDialogWidget(Widget* root)
	: Widget(root),
	m_backgroundImage(new ImageWidget(this, "Black")),
	m_messageLabel(new LabelWidget(this, L"", 24)),
	m_textInputLabel(new TextInputWidget(this, 24)),
	m_confirmationButton(new ButtonWidget(
		this,
		[this](int mouseButton) -> bool
		{
			if (mouseButton == 0)
			{
				return this->ConfirmationButtonClicked();
			}

			return false;
		},
		L"Okay"))
{
	m_messageLabel->SetPointAnchor(0.5f, 0.5f);
	m_messageLabel->SetBoundsDelta(0.0f, -200.0f, 0.0f, 0.0f);

	m_textInputLabel->SetPointAnchor(0.5f, 0.5f);
	m_textInputLabel->SetBoundsDelta(0.0f, -100.0f, 0.0f, 0.0f);

	m_confirmationButton->SetPointAnchor(0.5f, 0.5f);
	m_confirmationButton->SetBoundsDelta(-100.0f, 0.0f, 200.0f, 100.0f);

	SetBlocksClicks(true);
}

bool ModalDialogWidget::CapturesCursorOnMouseDown(int button) const
{
	return false;
}

bool ModalDialogWidget::ConfirmationButtonClicked()
{
	SetIsActive(false);
	m_onFinishedHandler(m_textInputLabel->GetValue());
	return true;
}

void ModalDialogWidget::ShowModalDialog(const std::wstring& message)
{
	ShowModalDialog(message, false);
}

void ModalDialogWidget::ShowModalDialog(const wstring& message, bool requireTextInput, std::function<void(const std::wstring&)> onFinishedHandler)
{
	m_requireTextInput = requireTextInput;
	m_onFinishedHandler = onFinishedHandler;

	m_messageLabel->SetText(message);

	if (requireTextInput)
	{
		m_textInputLabel->SetIsActive(true);
		m_textInputLabel->SetValue(L"");

		InputSystem::Instance().SetCurrentTextInput(m_textInputLabel);
	}
	else
	{
		m_textInputLabel->SetValue(L"");
		m_textInputLabel->SetIsActive(false);
	}

	SetIsActive(true);
}