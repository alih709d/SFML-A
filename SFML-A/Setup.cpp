#include <SFML/Graphics.hpp>
#include <optional>
#include <cmath>

int main()
{
    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "SFML Test");
    window.setFramerateLimit(60);

    // Player (green circle)
    sf::CircleShape player(30.f);
    player.setFillColor(sf::Color::Green);
    player.setOrigin({ 30.f, 30.f });
    player.setPosition({ 400.f, 300.f });

    // Bouncing ball (red circle)
    sf::CircleShape ball(20.f);
    ball.setFillColor(sf::Color::Red);
    ball.setOrigin({ 20.f, 20.f });
    ball.setPosition({ 100.f, 100.f });
    sf::Vector2f ballVelocity(250.f, 180.f);

    sf::Clock clock;
    const float playerSpeed = 300.f;

    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds();

        // Events
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
            else if (const auto* key = event->getIf<sf::Event::KeyPressed>())
            {
                if (key->code == sf::Keyboard::Key::Escape)
                    window.close();
            }
        }

        // Player movement
        sf::Vector2f direction(0.f, 0.f);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) direction.x -= 1.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) direction.x += 1.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) direction.y -= 1.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) direction.y += 1.f;
        player.move(direction * playerSpeed * dt);

        // Keep player inside the window
        sf::Vector2f p = player.getPosition();
        if (p.x < 30.f)  p.x = 30.f;
        if (p.x > 770.f) p.x = 770.f;
        if (p.y < 30.f)  p.y = 30.f;
        if (p.y > 570.f) p.y = 570.f;
        player.setPosition(p);

        // Ball movement and wall bounce
        ball.move(ballVelocity * dt);
        sf::Vector2f b = ball.getPosition();
        if (b.x < 20.f || b.x > 780.f) ballVelocity.x = -ballVelocity.x;
        if (b.y < 20.f || b.y > 580.f) ballVelocity.y = -ballVelocity.y;

        // Collision check: distance between centers
        float dx = player.getPosition().x - b.x;
        float dy = player.getPosition().y - b.y;
        float distance = std::sqrt(dx * dx + dy * dy);
        if (distance < 30.f + 20.f)
            ball.setFillColor(sf::Color::Blue);
        else
            ball.setFillColor(sf::Color::Red);

        // Drawing
        window.clear(sf::Color(30, 30, 30));
        window.draw(player);
        window.draw(ball);
        window.display();
    }
}