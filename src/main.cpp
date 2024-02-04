#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iomanip>
#include <iostream>
#include <memory>
#include <valarray>
#include <vector>

#include "Camera.hpp"
#include "CollisionAlgorithms.hpp"
#include "CollisionBody.hpp"
#include "Level.hpp"
#include "Player.hpp"
#include "TMXParser.hpp"
#include "Vector2fFunctions.hpp"
#include "XMLParser.hpp"

void handleSpriteInitPlayer(Player& outPlayer, sf::Texture& outTex)
{
	if (!outTex.loadFromFile("../assets/graphics/player_1.png"))
	{
		// TODO sensowny error
		std::cout << "spriteerror" << std::endl;
	}
	outPlayer.setSpriteTexture(outTex);
	outPlayer.setSpriteTextureRect({0, 0, 16, 32});
	outPlayer.setSpriteOrigin({8.f, 16.f});
	outPlayer.setSpriteOffset({0.f, -10.f});

	KeyFrameAnimator<SpriteKeyType> standAnim;
	standAnim.addKeyToKeyFrameTimeline(SpriteKeyType::RECT_X, 0, 0.f);

	KeyFrameAnimator<SpriteKeyType> runAnim(300000);
	runAnim.addKeyToKeyFrameTimeline(SpriteKeyType::RECT_X, 0, 16.f);
	runAnim.addKeyToKeyFrameTimeline(SpriteKeyType::RECT_X, 100000, 32.f);
	runAnim.addKeyToKeyFrameTimeline(SpriteKeyType::RECT_X, 200000, 48.f);

	KeyFrameAnimator<SpriteKeyType> jumpAnim;
	jumpAnim.addKeyToKeyFrameTimeline(SpriteKeyType::RECT_X, 0, 64.f);

	KeyFrameAnimator<SpriteKeyType> fallAnim;
	fallAnim.addKeyToKeyFrameTimeline(SpriteKeyType::RECT_X, 0, 80.f);

	KeyFrameAnimator<SpriteKeyType> turnAnim;
	turnAnim.addKeyToKeyFrameTimeline(SpriteKeyType::RECT_X, 0, 96.f);

	outPlayer.addAnimation("Stand", standAnim);
	outPlayer.addAnimation("Run", runAnim);
	outPlayer.addAnimation("Jump", jumpAnim);
	outPlayer.addAnimation("Fall", fallAnim);
	outPlayer.addAnimation("Turn", turnAnim);
}

int main()
{
	auto window = sf::RenderWindow{{1024u, 768u}, "Random Project 2"};
	window.setFramerateLimit(144);

	// Debug text

	sf::Font font;
	if (!font.loadFromFile("assets\\font\\PrStart.ttf"))
		return 0;

	sf::Text debug_info_text;
	debug_info_text.setFont(font);
	debug_info_text.setCharacterSize(8);
	debug_info_text.setFillColor(sf::Color::White);

	// Test entities

	Level level;
	level.create("leveldata/testmap1.tmx");
	level.accessCamera().setView(sf::View(sf::FloatRect(0.f, 0.f, 256.f, 192.f)));

	Controls p1Controls;

	Player player(sf::Vector2f(32, 128), p1Controls);
	player.accessCollider().setColor(sf::Color(255, 100, 100, 120));

	sf::Texture playerTexture;
	handleSpriteInitPlayer(player, playerTexture);

	//  ||--------------------------------------------------------------------------------||
	//  ||                                    Main loop                                   ||
	//  ||--------------------------------------------------------------------------------||

	sf::Clock clock;
	while (window.isOpen())
	{
		for (auto event = sf::Event{}; window.pollEvent(event);)
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		auto delta = clock.restart().asMicroseconds();
		if (delta > 100000)
			continue;

		// ||--------------------------------------------------------------------------------||
		// ||                                     Process                                    ||
		// ||--------------------------------------------------------------------------------||

		player.process(delta);

		// Collision
		auto colRes = CollisionAlgorithms::Get().StaticTripleCollisionForHitboxEntity(level.Collision, player, {});

		// Camera
		level.accessCamera().followEntity(player, player.accessCollider().getCollisionBox().getSize().x / 2.f,
										  player.accessCollider().getCollisionBox().getSize().y / 2.f);
		window.setView(level.accessCamera().getView());

		// Debug text
		debug_info_text.setString("delta: " + std::to_string(delta) +
								  "\nFPS: " + (delta > 0 ? std::to_string(1000000 / delta) : 0));
		debug_info_text.setPosition(level.accessCamera().getView().getCenter() -
									(level.accessCamera().getView().getSize() / 2.f) + sf::Vector2f(1.f, 1.f));

		// ||--------------------------------------------------------------------------------||
		// ||                                     Render                                     ||
		// ||--------------------------------------------------------------------------------||

		window.clear();

		window.draw(player.getSprite());

		for (auto const& pair : level.Collision)
		{
			auto vector = pair.second;
			for (auto&& cb : vector)
			{
				window.draw(cb->getCollisionBox());
			}
		}

		window.draw(player.accessCollider().getCollisionBox());

		window.draw(debug_info_text);

		window.display();
	}
}