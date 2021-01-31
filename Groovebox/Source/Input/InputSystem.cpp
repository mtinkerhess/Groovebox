#include "InputSystem.h"

#include "UI/TextInputWidget.h"
#include "UI/Widget.h"

#include <sstream>
#include <string>
#include <locale>

using namespace sf;
using namespace std;

InputSystem* InputSystem::s_instance;

InputSystem& InputSystem::Instance()
{
	if (s_instance == nullptr)
	{
		s_instance = new InputSystem();
	}

	return *s_instance;
}

void InputSystem::Update(float deltaSeconds)
{
	if (m_currentTextInput != nullptr && !m_currentTextInput->GetIsActiveInHierarchy())
	{
		SetCurrentTextInput(nullptr);
	}
}

bool InputSystem::HandleInputEvent(Event event, Window& sfmlWindow, Widget& rootWidget)
{
	switch (event.type)
	{
	case Event::MouseButtonPressed:
		if (m_widgetCapturingCursorOnMouseDown = rootWidget.GetWidgetThatCapturesCursor(event.mouseButton.button, Vector2i(event.mouseButton.x, event.mouseButton.y)))
		{
			m_buttonWhenCapturingCursor = event.mouseButton.button;
			sfmlWindow.setMouseCursorVisible(false);
		}
		return true;
	case Event::MouseButtonReleased:
		m_widgetCapturingCursorOnMouseDown = nullptr;
		m_buttonWhenCapturingCursor = 0;
		sfmlWindow.setMouseCursorVisible(true);
		rootWidget.HandleClick(Vector2i(event.mouseButton.x, event.mouseButton.y), event.mouseButton.button);
		return true;
	case Event::MouseMoved:
	{
		Vector2i mousePosition(event.mouseMove.x, event.mouseMove.y);
		if (m_widgetCapturingCursorOnMouseDown)
		{
			Vector2i deltaMousePosition = mousePosition - m_previousMousePosition;
			m_widgetCapturingCursorOnMouseDown->CapturedCursorDragged(deltaMousePosition, m_buttonWhenCapturingCursor);

			FloatRect widgetBounds = m_widgetCapturingCursorOnMouseDown->GetGlobalBounds();
			mousePosition = Vector2i((int)(widgetBounds.left + widgetBounds.width / 2), (int)(widgetBounds.top + widgetBounds.height / 2));
			Mouse::setPosition(mousePosition, sfmlWindow);
		}
		m_previousMousePosition = mousePosition;
		return true;
	}
	case Event::TextEntered:
	{
		if (m_currentTextInput != nullptr)
		{
			wstringstream ss;
			wchar_t c = (wchar_t)event.text.unicode;
			const wstring& currentValue = m_currentTextInput->GetValue();
			ss << currentValue;

			if (c == L'\b')
			{
				wstring str = ss.str();
				if (str.length() > 0)
				{
					m_currentTextInput->SetValue(str.substr(0, str.length() - 1));
				}
			}
			else
			{
				ss << c;
				m_currentTextInput->SetValue(ss.str());
			}
		}
	}
	}

	return false;
}

void InputSystem::SetCurrentTextInput(TextInputWidget* textInput) { m_currentTextInput = textInput; }

void InputSystem::ClearCurrentTextInput() { m_currentTextInput = nullptr; }