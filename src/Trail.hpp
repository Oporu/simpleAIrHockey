#ifndef TRAIL_HPP
#define TRAIL_HPP
#define _USE_MATH_DEFINES
#include <cmath>
#include <SFML/Graphics.hpp>

class Trail : public sf::Drawable {
public:
	Trail(const sf::Vector2f& from, const sf::Vector2f& to, const sf::Color& color=sf::Color::White);
	Trail(const sf::Vector2f& from, const sf::Vector2f& to, const sf::Color& color, const sf::Vector2f& offset);

    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
private:
	sf::RectangleShape shape;
};
Trail::Trail(const sf::Vector2f& from, const sf::Vector2f& to, const sf::Color& color) {
	const sf::Vector2f delta = to - from;
	const auto rotation = (float) (std::atan2(delta.y, delta.x) * 180 / M_PI);
	const float length = std::sqrt(delta.x * delta.x + delta.y * delta.y);
	
	this->shape.setPosition(from);
	this->shape.setFillColor(color);
	
	this->shape.setSize({length, 5.f});
	this->shape.setRotation(rotation);
	this->shape.setOrigin(0.f, this->shape.getSize().y / 2.0f);
}
Trail::Trail(const sf::Vector2f& from, const sf::Vector2f& to, const sf::Color& color, const sf::Vector2f& offset) {
	const sf::Vector2f delta = to - from;
	const auto rotation = (float) (std::atan2(delta.y, delta.x) * 180 / M_PI);
	const float length = std::sqrt(delta.x * delta.x + delta.y * delta.y);
	
	this->shape.setPosition(from + offset);
	this->shape.setFillColor(color);
	
	this->shape.setSize({length, 2.f});
	this->shape.setRotation(rotation);
	this->shape.setOrigin(0.f, this->shape.getSize().y / 2.0f);
}

void Trail::draw(sf::RenderTarget &target, sf::RenderStates states) const {
    target.draw(this->shape);
}

#endif // !TRAIL_HPP