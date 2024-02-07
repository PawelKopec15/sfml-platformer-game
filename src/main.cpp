#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iomanip>
#include <iostream>
#include <locale>
#include <memory>
#include <valarray>
#include <vector>

#include "BitmapFont.hpp"
#include "Camera.hpp"
#include "CollisionAlgorithms.hpp"
#include "CollisionBody.hpp"
#include "Level.hpp"
#include "Player.hpp"
#include "TMXParser.hpp"
#include "Vector2fFunctions.hpp"

void handleSpriteInitPlayer(Player& outPlayer, sf::Texture& outTex)
{
	if (!outTex.loadFromFile("../assets/graphics/player_1.png"))
	{
		std::cerr << "Error loading player sprite texture." << std::endl;
	}
	outPlayer.setSpriteTexture(outTex);
	outPlayer.setSpriteTextureRect({0, 0, 16, 32});
	outPlayer.setSpriteOrigin({8.f, 16.f});
	outPlayer.setSpriteOffset({0.f, -10.f});

	KeyFrameAnimator<SpriteKeyType> standAnim;
	standAnim.addKeyToKeyFrameTimeline(SpriteKeyType::RECT_X, 0, 0.f);

	KeyFrameAnimator<SpriteKeyType> runAnim(270000);
	runAnim.addKeyToKeyFrameTimeline(SpriteKeyType::RECT_X, 0, 16.f);
	runAnim.addKeyToKeyFrameTimeline(SpriteKeyType::RECT_X, 90000, 32.f);
	runAnim.addKeyToKeyFrameTimeline(SpriteKeyType::RECT_X, 180000, 48.f);

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
	auto window = sf::RenderWindow{{1024u, 768u}, "Random Project 2", sf::Style::Default};
	window.setFramerateLimit(144);

	// Set the locale to support Unicode
	std::wcout.imbue(std::locale(""));

	BitmapFont fontKubasta;
	if (!fontKubasta.create("../assets/font/kubasta_regular_8.PNG", "../assets/font/kubasta_regular_8.fnt"))
		std::cerr << "Error loading kubasta regular 8 font as BitmapFont" << std::endl;

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

		if (!level.accessCamera().isInTransitionAnimation())
			player.process(delta);

		player.animate(delta);

		// Collision
		auto colRes = CollisionAlgorithms::Get().StaticTripleCollisionForHitboxEntity(level.Collision, player, {});

		// Camera
		if (level.accessCamera().isInTransitionAnimation())
		{
			level.accessCamera().transitionAnimationTick(delta, player);
		}
		else
		{
			level.accessCamera().followEntity(player, player.accessCollider().getCollisionBox().getSize().x / 2.f,
											  player.accessCollider().getCollisionBox().getSize().y / 2.f);
		}
		window.setView(level.accessCamera().getView());

		// Debug text
		std::wstring debugTextString =
			L"delta: " + std::to_wstring(delta) + L"\nFPS: " + std::to_wstring(delta > 0 ? 1000000 / delta : 0);

		sf::Vector2f debugTextPos =
			sf::Vector2f(level.accessCamera().getView().getCenter() - (level.accessCamera().getView().getSize() / 2.f) +
						 sf::Vector2f(2.f, -4.f));

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

		// window.draw(debug_info_text);
		window.draw(fontKubasta.getTextDrawable(debugTextString, debugTextPos), &fontKubasta.getFontTexture());

		window.display();
	}
}