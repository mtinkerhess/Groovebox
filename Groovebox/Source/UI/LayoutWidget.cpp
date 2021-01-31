#include "LayoutWidget.h"

#include <cassert>

LayoutWidget::LayoutWidget(
	Widget* root,
	Orientation orientation,
	HorizontalAlignment horizontalAlignment,
	VerticalAlignment verticalAlignment,
	bool usePointAnchorsVertical,
	bool usePointAnchorsHorizontal,
	sf::Vector2f childSize,
	sf::FloatRect m_childBoundsDelta)
	:
	Widget(root),
	m_orientation(orientation),
	m_horizontalAlignment(horizontalAlignment),
	m_verticalAlignment(verticalAlignment),
	m_usePointAnchorsVertical(usePointAnchorsVertical),
	m_usePointAnchorsHorizontal(usePointAnchorsHorizontal),
	m_childSize(childSize),
	m_childBoundsDelta(m_childBoundsDelta)
{
}

void LayoutWidget::ChildrenChanged()
{
	UpdateChildPositions();
}

void LayoutWidget::HandleBoundsHaveChanged()
{
	UpdateChildPositions();
}

void LayoutWidget::UpdateChildPositions()
{
	int numChildren = GetNumChildren();

	sf::FloatRect globalBounds = GetGlobalBounds();
	sf::Vector2f childAnchorSize(m_childSize.x / globalBounds.width, m_childSize.y / globalBounds.height);

	for (int i = 0; i < numChildren; ++i)
	{
		float anchorMinX, anchorMaxX, anchorMinY, anchorMaxY;
		CalculateChildAnchorMinAndMaxX(i, numChildren, childAnchorSize.x, anchorMinX, anchorMaxX);
		CalculateChildAnchorMinAndMaxY(i, numChildren, childAnchorSize.y, anchorMinY, anchorMaxY);

		if (m_usePointAnchorsHorizontal)
		{
			anchorMinX = anchorMaxX = (anchorMinX + anchorMaxX) / 2.0f;
		}

		if (m_usePointAnchorsVertical)
		{
			anchorMinY = anchorMaxY = (anchorMinY + anchorMaxY) / 2.0f;
		}

		Widget& child = GetChild(i);
		child.SetAnchors(anchorMinX, anchorMinY, anchorMaxX, anchorMaxY);
		child.SetBoundsDelta(m_childBoundsDelta);
	}
}

void LayoutWidget::CalculateChildAnchorMinAndMaxX(int childIndex, int numChildren, float childAnchorWidth, float& anchorMinX, float& anchorMaxX)
{
	if (m_orientation == Orientation::Vertical)
	{
		childIndex = 0;
		numChildren = 1;
	}

	switch (m_horizontalAlignment)
	{
	case HorizontalAlignment::Left:
		anchorMinX = childIndex * childAnchorWidth;
		anchorMaxX = (childIndex + 1.0f) * childAnchorWidth;
		break;
	case HorizontalAlignment::Center:
		anchorMinX = (childIndex * childAnchorWidth + 1.0f - (numChildren - childIndex) * childAnchorWidth) / 2.0f;
		anchorMaxX = ((childIndex + 1.0f) * childAnchorWidth + 1.0f - (numChildren - childIndex - 1) * childAnchorWidth) / 2.0f;
		break;
	case HorizontalAlignment::Right:
		anchorMinX = 1.0f - (numChildren - childIndex) * childAnchorWidth;
		anchorMaxX = 1.0f - (numChildren - childIndex - 1) * childAnchorWidth;
		break;
	case HorizontalAlignment::Expand:
		anchorMinX = childIndex / (float)numChildren;
		anchorMaxX = (childIndex + 1.0f) / (float)numChildren;
		break;
	default:
		assert(false);
		break;
	}
}

void LayoutWidget::CalculateChildAnchorMinAndMaxY(int childIndex, int numChildren, float childAnchorHeight, float& anchorMinY, float& anchorMaxY)
{
	if (m_orientation == Orientation::Horizontal)
	{
		childIndex = 0;
		numChildren = 1;
	}

	switch (m_verticalAlignment)
	{
	case VerticalAlignment::Top:
		anchorMinY = childIndex * childAnchorHeight;
		anchorMaxY = (childIndex + 1.0f) * childAnchorHeight;
		break;
	case VerticalAlignment::Middle:
		anchorMinY = (childIndex * childAnchorHeight + 1.0f - (numChildren - childIndex) * childAnchorHeight) / 2.0f;
		anchorMaxY = ((childIndex + 1.0f) * childAnchorHeight + 1.0f - (numChildren - childIndex - 1) * childAnchorHeight) / 2.0f;
		break;
	case VerticalAlignment::Bottom:
		anchorMinY = 1.0f - (numChildren - childIndex) * childAnchorHeight;
		anchorMaxY = 1.0f - (numChildren - childIndex - 1) * childAnchorHeight;
		break;
	case VerticalAlignment::Expand:
		anchorMinY = childIndex / (float)numChildren;
		anchorMaxY = (childIndex + 1.0f) / (float)numChildren;
		break;
	default:
		assert(false);
		break;
	}
}