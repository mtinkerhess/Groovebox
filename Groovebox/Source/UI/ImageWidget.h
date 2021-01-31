#pragma once

#include "UI/Widget.h"

#include <string>

class ImageWidget : public Widget
{
private:
    const sf::Texture* m_texture;
    sf::Sprite m_sprite;

public:
    ImageWidget(Widget* parent, const std::string& textureName);

    void DrawInternal(sf::RenderWindow& window) override;
    
    void SetTexture(const std::string& textureName);
};
