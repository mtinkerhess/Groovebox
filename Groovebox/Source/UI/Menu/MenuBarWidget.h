#pragma once

#include "UI/Widget.h"

#include <functional>

using MenuAction = std::function<void()>;

class MenuWidget;
class MenuBarItem;

class MenuBarWidget : public Widget
{
public:
	MenuBarWidget(Widget* root)
		: Widget(root)
	{
	}
};

class MenuWidget : public Widget
{
public:
	MenuWidget(MenuBarWidget* root)
		: Widget(root)
	{
	}
};

class MenuItem : public Widget
{
private:
	MenuAction m_action;

public:
	MenuItem(MenuWidget* root, MenuAction action)
		: Widget(root),
		m_action(action)
	{
	}
};