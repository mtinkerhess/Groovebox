#include "TextInputWidget.h"

#include "UI/LabelWidget.h"

using namespace std;

TextInputWidget::TextInputWidget(Widget* root, int fontSize)
	: Widget(root),
	m_label(new LabelWidget(this, L"", fontSize))
{
}

const wstring TextInputWidget::GetValue() const
{
	return m_label->GetText();
}

void TextInputWidget::SetValue(const wstring& value) { m_label->SetText(value); }