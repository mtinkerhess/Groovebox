#include "Widget.h"

#include "Util.h"

Widget::Widget(Widget* parent) : m_parent(nullptr)
{
	if (parent)
	{
		parent->AddChild(this);
	}
}

void Widget::SetPointAnchor(float x, float y) { SetAnchorMin(x, y); SetAnchorMax(x, y); }

void Widget::SetAnchors(float minX, float minY, float maxX, float maxY) { SetAnchorMin(minX, minY); SetAnchorMax(maxX, maxY); }

sf::Vector2f Widget::GetAnchorMin() const { return m_anchorMin; }

void Widget::SetAnchorMin(sf::Vector2f anchorMin) { m_anchorMin = anchorMin; m_boundsHaveChanged = true; }

void Widget::SetAnchorMin(float x, float y) { return SetAnchorMin(sf::Vector2f(x, y)); }

sf::Vector2f Widget::GetAnchorMax() const { return m_anchorMax; }

void Widget::SetAnchorMax(float x, float y) { return SetAnchorMax(sf::Vector2f(x, y)); }

void Widget::SetAnchorMax(sf::Vector2f anchorMax) { m_anchorMax = anchorMax; m_boundsHaveChanged = true; }

sf::FloatRect Widget::GetBoundsDelta() const { return m_boundsDelta; }

void Widget::SetBoundsDelta(float left, float top, float width, float height) { return SetBoundsDelta(sf::FloatRect(left, top, width, height)); }

void Widget::SetBoundsDelta(sf::FloatRect bounsdDelta) { m_boundsDelta = bounsdDelta; m_boundsHaveChanged = true; }

void Widget::SetBlocksClicks(bool blocksClicks) { m_blocksClicks = blocksClicks; }

bool Widget::GetIsActive() const { return m_isActive; }

void Widget::SetIsActive(bool isActive) { m_isActive = isActive; }

bool Widget::GetIsActiveInHierarchy() const
{
	if (!m_isActive) { return false; }
	return m_parent == nullptr || m_parent->GetIsActiveInHierarchy();
}

sf::FloatRect Widget::GetGlobalBounds() const
{
	if (m_parent)
	{
		sf::FloatRect parentBounds = m_parent->GetGlobalBounds();

		sf::Vector2f anchorMin = sf::Vector2f(
			parentBounds.left + parentBounds.width * m_anchorMin.x,
			parentBounds.top + parentBounds.height * m_anchorMin.y);

		sf::Vector2f anchorMax = sf::Vector2f(
			parentBounds.left + parentBounds.width * m_anchorMax.x,
			parentBounds.top + parentBounds.height * m_anchorMax.y);

		return sf::FloatRect(
			anchorMin.x + m_boundsDelta.left,
			anchorMin.y + m_boundsDelta.top,
			anchorMax.x - anchorMin.x + m_boundsDelta.width,
			anchorMax.y - anchorMin.y + m_boundsDelta.height);
	}
	else
	{
		return m_boundsDelta;
	}
}

const std::vector<Widget*> Widget::GetChildren() const { return m_children; }

Widget& Widget::GetChild(int childIndex) { return *m_children[childIndex]; }

int Widget::GetNumChildren() const { return m_children.size(); }

void Widget::AddChild(Widget* child)
{
	child->SetParent(this);
	m_children.push_back(child);
	ChildrenChanged();
}

void Widget::RemoveChild(Widget* child)
{
	Util::EraseRemove(m_children, child);
	ChildrenChanged();
}

bool Widget::HandleClick(sf::Vector2i mousePosition, int button)
{
	if (!m_isActive)
	{
		return false;
	}

	sf::FloatRect globalBounds = GetGlobalBounds();
	bool boundsContainsMouse = globalBounds.contains(sf::Vector2f(mousePosition));

	if (m_clipsChildClickHandlers && !boundsContainsMouse)
	{
		return false;
	}

	for (auto child = m_children.rbegin(); child != m_children.rend(); ++child)
	{
		if ((*child)->HandleClick(mousePosition, button))
		{
			return true;
		}
	}

	if (boundsContainsMouse)
	{
		return HandleClickInternal(button);
	}

	return false;
}

Widget* Widget::GetWidgetThatCapturesCursor(int button, sf::Vector2i mousePosition)
{
	if (!m_isActive)
	{
		return nullptr;
	}

	sf::FloatRect globalBounds = GetGlobalBounds();
	bool boundsContainsMouse = globalBounds.contains(sf::Vector2f(mousePosition));

	if (m_clipsChildClickHandlers && !boundsContainsMouse)
	{
		return nullptr;
	}

	for (auto child = m_children.rbegin(); child != m_children.rend(); ++child)
	{
		if (Widget* result = (*child)->GetWidgetThatCapturesCursor(button, mousePosition))
		{
			return result;
		}
	}

	if (boundsContainsMouse && CapturesCursorOnMouseDown(button))
	{
		return this;
	}

	return nullptr;
}

void Widget::Draw(sf::RenderWindow& window)
{
	if (!m_isActive)
	{
		return;
	}

	DrawInternal(window);

	for (auto child : m_children)
	{
		child->Draw(window);
	}
}

void Widget::CapturedCursorBegin() { }

void Widget::CapturedCursorEnd() { }

void Widget::CapturedCursorDragged(sf::Vector2i deltaMousePosition, int mouseButton) { }

void Widget::CheckBoundsHaveChanged(bool ancestorHasChanged)
{
	if (!m_isActive)
	{
		return;
	}

	if (m_boundsHaveChanged || ancestorHasChanged)
	{
		HandleBoundsHaveChanged();
		ancestorHasChanged = true;
		m_boundsHaveChanged = false;
	}

	for (auto child : m_children)
	{
		child->CheckBoundsHaveChanged(ancestorHasChanged);
	}
}

bool Widget::CapturesCursorOnMouseDown(int button) const { return false; }

bool Widget::HandleClickInternal(int button) { return m_blocksClicks; }

void Widget::DrawInternal(sf::RenderWindow& window) { }

void Widget::ChildrenChanged() { }

void Widget::HandleBoundsHaveChanged() { }

void Widget::SetParent(Widget* parent)
{
	if (m_parent)
	{
		m_parent->RemoveChild(this);
	}

	m_parent = parent;
}