#pragma once

#include "SFML/Window.hpp"

class Widget;
class TextInputWidget;

class InputSystem
{
private:
	static InputSystem* s_instance;

	Widget* m_widgetCapturingCursorOnMouseDown = nullptr;
	int m_buttonWhenCapturingCursor = 0;
	sf::Vector2i m_previousMousePosition;

	TextInputWidget* m_currentTextInput = nullptr;

	InputSystem() = default;

public:
	static InputSystem& Instance();
	
	void Update(float deltaSeconds);
	
	bool HandleInputEvent(sf::Event event, sf::Window& sfmlWindow, Widget& rootWidget);

	void SetCurrentTextInput(TextInputWidget* textInput);

	void ClearCurrentTextInput();
};

