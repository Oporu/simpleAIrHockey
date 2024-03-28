#ifndef BALL_HPP
#define BALL_HPP

#include <SFML/Graphics.hpp>
class Ball final : public sf::CircleShape {
public:
    Ball() : sf::CircleShape() {
    }
    sf::Vector2f velocity;
};
#endif // !BALL_HPP
