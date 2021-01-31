#pragma once

#include "UI/ButtonWidget.h"
#include "UI/LayoutWidget.h"
#include "UI/Widget.h"

#include <string>
#include <vector>

class TabbedPanelWidget : public Widget
{
public:
	const float HeaderPanelHeight = 100.0f;
	const float TabButtonWidth = 150.0f;

private:

	LayoutWidget* m_headerPanel;
	Widget* m_contentPanelRoot;

	std::vector<ButtonWidget*> m_headerButtons;
	std::vector<Widget*> m_contentPanels;

public:
	TabbedPanelWidget(Widget* root)
		: Widget(root),
		m_headerPanel(new LayoutWidget(
			root,
			LayoutWidget::Orientation::Horizontal,
			LayoutWidget::HorizontalAlignment::Left,
			LayoutWidget::VerticalAlignment::Bottom,
			false,
			false,
			sf::Vector2f(TabButtonWidth, HeaderPanelHeight),
			sf::FloatRect())),
		m_contentPanelRoot(new Widget(root))
	{
		m_headerPanel->SetAnchors(0.0f, 0.0f, 1.0f, 0.0f);
		m_headerPanel->SetBoundsDelta(0.0f, 0.0f, 0.0f, HeaderPanelHeight);
	}

	LayoutWidget& GetHeaderPanel() { return *m_headerPanel; }
	Widget& GetContentPanelRoot() { return *m_contentPanelRoot; }
	Widget& GetContentPanel(int index) { return *m_contentPanels[index]; }

	Widget* AddPanel(const std::wstring& name)
	{
		int headerButtonIndex = m_headerButtons.size();
		
		ButtonWidget* button = new ButtonWidget(
			m_headerPanel,
			[this, headerButtonIndex](int mouseButtonIndex)
			{
				if (mouseButtonIndex != 0) { return false; }
				this->OpenContentPanel(headerButtonIndex);
				return true;
			},
			name);

		m_headerButtons.push_back(button);

		Widget* newPanel = new Widget(m_contentPanelRoot);
		newPanel->SetBoundsDelta(0.0f, HeaderPanelHeight, 0.0f, -HeaderPanelHeight);
		newPanel->SetIsActive(m_contentPanels.size() == 0); 
		m_contentPanels.push_back(newPanel);
		return newPanel;
	}

	Widget* AddButton(const std::wstring& name, std::function<void()> handler)
	{
		int headerButtonIndex = m_headerButtons.size();

		ButtonWidget* button = new ButtonWidget(
			m_headerPanel,
			[this, handler](int mouseButtonIndex)
			{
				if (mouseButtonIndex != 0) { return false; }
				handler();
				return true;
			},
			name);

		m_contentPanels.push_back(nullptr);
		m_headerButtons.push_back(button);

		return button;
	}

	void OpenContentPanel(int index)
	{
		for (size_t i = 0; i < m_contentPanels.size(); ++i)
		{
			if (m_contentPanels[i])
			{
				m_contentPanels[i]->SetIsActive(i == index);
			}
		}
	}
};