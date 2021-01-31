#pragma once

#include "UI/Widget.h"

class LayoutWidget : public Widget
{
public:
	enum class Orientation : unsigned char
	{
		Vertical,
		Horizontal
	};

	enum class HorizontalAlignment : unsigned char
	{
		Left,
		Center,
		Right,
		Expand
	};

	enum class VerticalAlignment : unsigned char
	{
		Top,
		Middle,
		Bottom,
		Expand
	};

private:

	Orientation m_orientation;
	HorizontalAlignment m_horizontalAlignment;
	VerticalAlignment m_verticalAlignment;

	// If these are true, set the child anchors to "point" anchors
	// in the middle of where you would set them otherwise.
	bool m_usePointAnchorsVertical = false;
	bool m_usePointAnchorsHorizontal = false;

	// If alignment is not expand (per dimension),
	// use m_childSize as a fraction of this's global bounds
	// to calculate the anchors of the children.
	sf::Vector2f m_childSize;

	sf::FloatRect m_childBoundsDelta;

public:
	LayoutWidget(
		Widget* root,
		Orientation orientation,
		HorizontalAlignment horizontalAlignment,
		VerticalAlignment verticalAlignment,
		bool usePointAnchorsVertical,
		bool usePointAnchorsHorizontal,
		sf::Vector2f childSize,
		sf::FloatRect m_childBoundsDelta);

protected:
	void ChildrenChanged() override;

	void HandleBoundsHaveChanged() override;

private:
	void UpdateChildPositions();

	void CalculateChildAnchorMinAndMaxX(int childIndex, int numChildren, float childAnchorWidth, float& anchorMinX, float& anchorMaxX);

	void CalculateChildAnchorMinAndMaxY(int childIndex, int numChildren, float childAnchorHeight, float& anchorMinY, float& anchorMaxY);
};