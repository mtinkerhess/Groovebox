#pragma once

#include "UI/Widget.h"

#include <string>

class LabelWidget;

class TextInputWidget : public Widget
{
private:
	LabelWidget* m_label;

public:
	TextInputWidget(Widget* root, int fontSize);

	const std::wstring GetValue() const;

	void SetValue(const std::wstring& value);
};