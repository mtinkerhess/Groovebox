#include "ImageWidget.h"

#include "ResourceMap.h"

using namespace sf;
using namespace std;

ImageWidget::ImageWidget(Widget* parent, const string& textureName)
    : Widget(parent)
{
    SetTexture(textureName);
}

void ImageWidget::DrawInternal(RenderWindow& window)
{
    sf::FloatRect globalBounds = GetGlobalBounds();
    sf::Vector2f textureSize = sf::Vector2f(m_texture->getSize());
    m_sprite.setScale(globalBounds.width / textureSize.x, globalBounds.height / textureSize.y);
    m_sprite.setPosition(globalBounds.left, globalBounds.top);
    window.draw(m_sprite);
}

void ImageWidget::SetTexture(const string& textureName)
{
    m_texture = &Textures::Instance().Get(textureName);
    m_sprite.setTexture(*m_texture);
}