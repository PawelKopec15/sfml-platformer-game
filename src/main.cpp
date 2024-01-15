#include <SFML/Graphics.hpp>
#include <iomanip>
#include <iostream>
#include <valarray>
#include <vector>

#include "CollisionAlgorithms.hpp"
#include "CollisionBody.hpp"
#include "Level.hpp"
#include "Player.hpp"
#include "TMXParser.hpp"
#include "Vector2fFunctions.hpp"

int main()
{
	auto window = sf::RenderWindow{{1024u, 768u}, "Random Project 2"};
	window.setFramerateLimit(144);

	sf::View actionView(sf::FloatRect(0.f, 0.f, 256.f, 192.f));
	window.setView(actionView);

	// Debug text

	sf::Font font;
	if (!font.loadFromFile("assets\\font\\PrStart.ttf"))
		return 0;

	sf::Text debug_info_text;
	debug_info_text.setFont(font);
	debug_info_text.setCharacterSize(8);
	debug_info_text.setFillColor(sf::Color::White);
	debug_info_text.setPosition(sf::Vector2f(1, 1));

	// Test entities

	Level level("leveldata/testmap1.tmx", true);

	Controls p1Controls;

	Player player(sf::Vector2f(32, 16), p1Controls);
	player.accessCollider().setColor(sf::Color(255, 0, 0, 180));

	//  ||--------------------------------------------------------------------------------||
	//  ||                                    Main loop                                   ||
	//  ||--------------------------------------------------------------------------------||

	sf::Clock clock;
	while (window.isOpen())
	{
		for (auto event = sf::Event{}; window.pollEvent(event);)
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
		}

		auto delta = clock.restart().asMicroseconds();

		// ||--------------------------------------------------------------------------------||
		// ||                                     Process                                    ||
		// ||--------------------------------------------------------------------------------||

		player.process(delta);

		// Test coloring and collision
		auto colRes = CollisionAlgorithms::Get().StaticTripleCollisionCheck(level.Collision, player, {}, true);

		if (colRes.ejectedUp)
			player.setCanJump(true);

		if (colRes.shouldResetHor && player.getNextFrameResetHor())
			player.setMoveVector(sf::Vector2f(0, player.getMoveVector().y));
		else if (colRes.shouldResetHor)
			player.setNextFrameResetHor(true);
		else if (player.getNextFrameResetHor())
			player.setNextFrameResetHor(false);

		if (colRes.shouldResetVer && player.getNextFrameResetVer())
			player.setMoveVector(sf::Vector2f(player.getMoveVector().x, 0));
		else if (colRes.shouldResetVer)
			player.setNextFrameResetVer(true);
		else if (player.getNextFrameResetVer())
			player.setNextFrameResetVer(false);

		// Debug text

		debug_info_text.setString("Elapsed time: " + std::to_string(delta) +
								  "\nFPS: " + std::to_string(1000000 / delta) +
								  "\t can_jump: " + std::to_string(player.getCanJump()));

		// ||--------------------------------------------------------------------------------||
		// ||                                     Render                                     ||
		// ||--------------------------------------------------------------------------------||

		window.clear();

		for (auto const& pair : level.Collision)
		{
			auto vector = pair.second;
			for (auto&& cb : vector)
			{
				window.draw(cb.getCollisionBox());
			}
		}

		window.draw(player.accessCollider().getCollisionBox());

		window.draw(debug_info_text);

		window.display();
	}
}