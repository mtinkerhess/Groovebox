#pragma once

#include <SFML/Graphics.hpp>

#include <functional>

class Widget
{
public:
    using ClickHandler = std::function<bool(int)>;

private:
    Widget* m_parent;
    sf::Vector2f m_anchorMin{ 0.0f, 0.0f };
    sf::Vector2f m_anchorMax{ 1.0f, 1.0f };
    sf::FloatRect m_boundsDelta{ 0.0f, 0.0f, 0.0f, 0.0f };
    std::vector<Widget*> m_children;
    bool m_clipsChildClickHandlers = false;
    bool m_blocksClicks = false;
    bool m_isActive = true;
    bool m_boundsHaveChanged = false;

public:
    explicit Widget(Widget* parent);

    void SetPointAnchor(float x, float y);

    void SetAnchors(float minX, float minY, float maxX, float maxY);
    
    sf::Vector2f GetAnchorMin() const;
    
    void SetAnchorMin(sf::Vector2f anchorMin);
    
    void SetAnchorMin(float x, float y);
    
    sf::Vector2f GetAnchorMax() const;
    
    void SetAnchorMax(float x, float y);
    
    void SetAnchorMax(sf::Vector2f anchorMax);
    
    sf::FloatRect GetBoundsDelta() const;
    
    void SetBoundsDelta(float left, float top, float width, float height);
    
    void SetBoundsDelta(sf::FloatRect bounsdDelta);

    void SetBlocksClicks(bool blocksClicks);

    bool GetIsActive() const;
    
    void SetIsActive(bool isActive);

    bool GetIsActiveInHierarchy() const;

    sf::FloatRect GetGlobalBounds() const;

    const std::vector<Widget*> GetChildren() const;
    
    Widget& GetChild(int childIndex);
    
    int GetNumChildren() const;

    void AddChild(Widget* child);

    void RemoveChild(Widget* child);

    bool HandleClick(sf::Vector2i mousePosition, int button);

    Widget* GetWidgetThatCapturesCursor(int button, sf::Vector2i mousePosition);

    void Draw(sf::RenderWindow& window);

    virtual void CapturedCursorBegin();
    
    virtual void CapturedCursorEnd();
    
    virtual void CapturedCursorDragged(sf::Vector2i deltaMousePosition, int mouseButton);

    void CheckBoundsHaveChanged(bool ancestorHasChanged = false);

protected:
    virtual bool CapturesCursorOnMouseDown(int button) const;
    
    virtual bool HandleClickInternal(int button);
    
    virtual void DrawInternal(sf::RenderWindow& window);
    
    virtual void ChildrenChanged();
    
    virtual void HandleBoundsHaveChanged();

private:
    void SetParent(Widget* parent);
};