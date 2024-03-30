#ifndef GAME_HPP
#define GAME_HPP

#define _USE_MATH_DEFINES
#include <SFML/Graphics.hpp>
#include "Ball.hpp"
#include "Trail.hpp"
#include <iostream>
#include <random>
#include <cmath>
#include <optional>
class Game final {
    struct BallToBorderCollideResult {
        enum Type { LEFT, RIGHT, TOP, BOTTOM };
        float time;
        Type type;
    };
	sf::RenderWindow window{ { 800u, 600u }, "simpleAirHockey", sf::Style::Close };
	Ball ball, ball2;
	sf::Clock clock;
    std::mt19937 randomGen{std::random_device()()};
    std::vector<Trail> trails;
	void initializeWindow();
	void handleWindowEvents();
    static std::optional<float> BallToBallCollideTime(const Ball& ball1, const Ball& ball2, float maxTime);
    static std::optional<BallToBorderCollideResult> BallToBorderCollideTime(const Ball& ball, sf::FloatRect border, float maxTime);
	static void collideBalls(Ball& ball1, Ball& ball2);

public:
	Game();
	void update();
	void render();
	bool isActive() const;
};

Game::Game() {
	this->initializeWindow();
    ball.velocity = {0, 0};
    ball.setRadius(20);
    ball.setOrigin(ball.getRadius(), ball.getRadius());
    ball.setFillColor(sf::Color::White);
    ball.setPosition(window.getView().getCenter());

    ball2.velocity = {0, 0};
    ball2.setRadius(20);
    ball2.setOrigin(ball2.getRadius(), ball2.getRadius());
    ball2.setFillColor(sf::Color::Green);
    ball2.setPosition(static_cast<sf::Vector2f>(sf::Mouse::getPosition(window)));
	clock.restart();
}
void Game::initializeWindow() {
	window.setFramerateLimit(60);
	window.setMouseCursorVisible(false);
	window.setVerticalSyncEnabled(true);
	window.setActive();
	window.clear();
	window.display();
}
void Game::handleWindowEvents() {
	sf::Event event{};
	while (window.pollEvent(event)) {
		switch (event.type) {
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::KeyPressed:
				switch (event.key.code) {
					using sf::Keyboard;
					case Keyboard::Q:
					case Keyboard::Escape:
						window.close();
						break;
					case Keyboard::Up:
					case Keyboard::W:
						ball.velocity.y -= 5.f;
						break;
					case Keyboard::Down:
					case Keyboard::S:
						ball.velocity.y += 5.f;
						break;
					case Keyboard::Left:
					case Keyboard::A:
						ball.velocity.x -= 5.f;
						break;
					case Keyboard::Right:
					case Keyboard::D:
						ball.velocity.x += 5.f;
						break;
					case Keyboard::Space:
						ball.velocity *= 2.f;
						break;
					case Keyboard::Num0:
						ball.velocity.x = 0;
						ball.velocity.y = 0;
						break;
					case Keyboard::Backspace:
						sf::sleep(sf::milliseconds(1000));
						break;
					case Keyboard::Add:
					case Keyboard::Equal:
						ball2.setRadius(ball2.getRadius()+5.f);
						ball2.setOrigin(ball2.getRadius(), ball2.getRadius());
						break;
					case Keyboard::Dash:
					case Keyboard::Subtract:
						ball2.setRadius(ball2.getRadius()-5.f);
						ball2.setOrigin(ball2.getRadius(), ball2.getRadius());
						break;
					case Keyboard::LBracket:
						ball.setRadius(ball.getRadius()-5.f);
						ball.setOrigin(ball.getRadius(), ball.getRadius());
						break;
					case Keyboard::RBracket:
						ball.setRadius(ball.getRadius()+5.f);
						ball.setOrigin(ball.getRadius(), ball.getRadius());
						break;
					default:;
				}
				break;
			default:;
		}
	}
}
std::optional<float> Game::BallToBallCollideTime(const Ball& ball1, const Ball& ball2, const float maxTime) {
    const sf::Vector2f dPos = ball1.getPosition() - ball2.getPosition();
    const sf::Vector2f dVel = ball1.velocity - ball2.velocity;
	if (std::atan2(dPos.x*dVel.x + dPos.y*dVel.y, dPos.x*dVel.x - dPos.y*dVel.y) > 1.571) { // 1.571 rad = 90 degree
		return std::nullopt;
	}

    const float a = dVel.x*dVel.x + dVel.y*dVel.y;
    const float b = 2 * (dPos.x*dVel.x + dPos.y*dVel.y);
    const float c = dPos.x*dPos.x + dPos.y*dPos.y - std::pow(ball1.getRadius() + ball2.getRadius(), 2.f);
    const float temp = (b*b) - (4*a*c);

    if (temp < 0) return std::nullopt;

    const float sqrtTemp = std::sqrt(temp);
    const float t1 = (-b - sqrtTemp) / (2*a);
    const float t2 = (-b + sqrtTemp) / (2*a);
    if (t1 >= 0 && t2 >= 0) {
	    const float t = std::min(t1, t2);
		return (t <= maxTime) ? std::make_optional(t) : std::nullopt;
    } else if (t1 >= 0 && t1 <= maxTime) {
	    return t1;
    } else if (t2 >= 0 && t2 <= maxTime) {
	    return t2;
    }
    return std::nullopt;
}
std::optional<Game::BallToBorderCollideResult> Game::BallToBorderCollideTime(const Ball& ball, const sf::FloatRect border, const float maxTime) {
	const sf::Vector2f& pos = ball.getPosition();
	const float radius = ball.getRadius();
	const sf::Vector2f nextPos = pos + (ball.velocity * maxTime);
    std::optional<BallToBorderCollideResult> result;
    if (nextPos.x + radius > border.left + border.width) {
        const float x = border.left + border.width - radius;
		const float t = (x - pos.x) / ball.velocity.x;
		result = {t, BallToBorderCollideResult::RIGHT};
    } else if (nextPos.x - radius < border.left) {
        const float x = border.left + radius;
        const float t = (x - pos.x) / ball.velocity.x;
        result = {t, BallToBorderCollideResult::LEFT};
    }
    if (nextPos.y + radius > border.top + border.height) {
        const float y = border.top + border.height - radius;
        const float t = (y - pos.y) / ball.velocity.y;
        if (!result.has_value() || result->time > t) {
			result = {t, BallToBorderCollideResult::BOTTOM};
        }
    } else if (nextPos.y - radius < border.top) {
        const float y = border.top + radius;
        const float t = (y - pos.y) / ball.velocity.y;
        if (!result.has_value() || result->time > t) {
            result = {t, BallToBorderCollideResult::TOP};
        }
    }
    if (!result.has_value() || result->time > maxTime) return std::nullopt;
    return result;
}

void Game::update() {
	this->handleWindowEvents();
	float t = static_cast<float>(clock.restart().asMilliseconds());
    const sf::Vector2f ball2_EndPos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));
    trails.emplace_back(ball2.getPosition(), ball2_EndPos, sf::Color::Green);
    ball2.velocity = (ball2_EndPos - ball2.getPosition()) / t;
    int updateCount = 0;
    while (t > 0) {
        if (updateCount++ == 100) {
            return;
        }
		struct Collide {
			enum Type { BALL_TO_BALL, BALL_TO_BORDER };
			float time;
			Type type;
		};
        std::optional<Collide> collide;
        const std::optional<float> ballToBallCollideTime = BallToBallCollideTime(ball, ball2, t);
        if (ballToBallCollideTime.has_value() && ballToBallCollideTime.value() > 0.0001f) {
			collide = {ballToBallCollideTime.value(), Collide::BALL_TO_BALL};
        }
        const std::optional<BallToBorderCollideResult> ballToBorderCollideResult = BallToBorderCollideTime(ball, {{0.f, 0.f},static_cast<sf::Vector2f>(window.getSize())}, collide.has_value() ? collide->time : t);

        if (ballToBorderCollideResult.has_value()) {
			collide = {ballToBorderCollideResult->time, Collide::BALL_TO_BORDER};
        }

        if (collide) {
			collide->time -= 0.00001f;
            t -= collide->time;
            sf::Vector2f ballNextPos = ball.getPosition() + (ball.velocity*collide->time);
            sf::Vector2f ball2NextPos = ball2.getPosition() + (ball2.velocity*collide->time);
            trails.emplace_back(ball2.getPosition(), ball2NextPos, sf::Color::Green);

            switch (collide->type) {
                case Collide::BALL_TO_BORDER:
                    trails.emplace_back(ball.getPosition(), ballNextPos, sf::Color::Yellow);
                    switch (ballToBorderCollideResult->type) {
                        case BallToBorderCollideResult::BOTTOM:
                        case BallToBorderCollideResult::TOP:
                            ball.velocity.y *= -1;
                            break;
                        case BallToBorderCollideResult::LEFT:
                        case BallToBorderCollideResult::RIGHT:
                            ball.velocity.x *= -1;
                            break;
                        default:;
                    }
                    break;
                case Collide::BALL_TO_BALL:
                    trails.emplace_back(ball.getPosition(), ballNextPos, sf::Color::Red);
		            collideBalls(ball,ball2);
                    break;
                default:;
            }
            ball.setPosition(ballNextPos);
            ball2.setPosition(ball2NextPos);
        } else {
            const sf::Vector2f ballNextPos = ball.getPosition() + (ball.velocity * t);
            trails.emplace_back(ball.getPosition(), ballNextPos, sf::Color::White);
            ball.setPosition(ballNextPos);
//            t = 0;
	        break;
        }
    }
    ball2.setPosition(ball2_EndPos);
    ball.velocity *= 0.975f;
}
void Game::render() {
	this->window.clear();
    for (Trail& trail : trails)
        window.draw(trail);
    this->window.draw(ball);
    this->window.draw(ball2);
	this->window.display();
    trails.clear();
}
inline bool Game::isActive() const {
	return window.isOpen();
}

void Game::collideBalls(Ball &ball1, Ball &ball2) {
	const sf::Vector2f dPos = ball1.getPosition() - ball2.getPosition();
	const sf::Vector2f dVel = ball1.velocity - ball2.velocity;

	const sf::Vector2f pushToBall2 = dPos / std::sqrt((dPos.x*dPos.x)+(dPos.y*dPos.y)) * std::sqrt(dVel.x*dVel.x + dVel.y*dVel.y);
	ball1.velocity += (pushToBall2)*1.5f;

}

#endif