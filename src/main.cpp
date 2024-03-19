#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iomanip>
#include <iostream>
#include <locale>
#include <memory>
#include <valarray>
#include <vector>

#include "Area2D.hpp"
#include "BitmapFont.hpp"
#include "Camera.hpp"
#include "CollisionAlgorithms.hpp"
#include "CollisionBody.hpp"
#include "GuiElement.hpp"
#include "GuiLabel.hpp"
#include "Level.hpp"
#include "NineSlice.hpp"
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

	KeyFrameAnimator<SpriteKeyType> pushAnim(800000);
	pushAnim.addKeyToKeyFrameTimeline(SpriteKeyType::RECT_X, 0, 112.f);
	pushAnim.addKeyToKeyFrameTimeline(SpriteKeyType::RECT_X, 400000, 128.f);

	outPlayer.addAnimation("Stand", standAnim);
	outPlayer.addAnimation("Run", runAnim);
	outPlayer.addAnimation("Jump", jumpAnim);
	outPlayer.addAnimation("Fall", fallAnim);
	outPlayer.addAnimation("Turn", turnAnim);
	outPlayer.addAnimation("Push", pushAnim);
}

int main()
{
	auto window = sf::RenderWindow{{1024u, 768u}, "Simple Platformer Game Project", sf::Style::Default};
	window.setFramerateLimit(144);

	// Set the locale to support Unicode
	std::wcout.imbue(std::locale(""));

	BitmapFont fontKubasta;
	if (!fontKubasta.create("../assets/font/kubasta_regular_8.PNG", "../assets/font/kubasta_regular_8.fnt"))
		std::cerr << "Error loading kubasta regular 8 font as BitmapFont" << std::endl;

	// Debug and debug GUI
	bool debugMode = false;

	NineSlice gui2;
	gui2.setTexture("../assets/graphics/ui/nineslice2.png");
	gui2.setSlicing({0, 0, 24, 24});

	GuiElement debugGui({0, 120, 42, 32}, std::make_shared<NineSlice>(gui2));
	debugGui.setLayoutManager(std::make_shared<VBoxLayoutManager>());
	debugGui.setInnerMargins(GuiMargins(4, 4, 4, 4));

	GuiLabel _debugLabelDelta({0, 96, 42, 16}, std::make_shared<BitmapFont>(fontKubasta), L"delta= ");
	auto debugLabelDelta = std::make_shared<GuiLabel>(_debugLabelDelta);
	debugGui.addChild(debugLabelDelta);

	GuiLabel _debugLabelFPS({0, 118, 42, 16}, std::make_shared<BitmapFont>(fontKubasta), L"FPS= ");
	auto debugLabelFPS = std::make_shared<GuiLabel>(_debugLabelFPS);
	debugGui.addChild(debugLabelFPS);

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

			if (event.type == sf::Event::KeyPressed && debugMode)
			{
				switch (event.key.scancode)
				{
					case sf::Keyboard::Scan::Numpad9:
						window.setFramerateLimit(0);
						break;

					case sf::Keyboard::Scan::Numpad8:
						window.setFramerateLimit(60);
						break;

					case sf::Keyboard::Scan::Numpad7:
						window.setFramerateLimit(30);
						break;

					case sf::Keyboard::Scan::Numpad5:
						window.setFramerateLimit(144);
						break;

					default:
						break;
				}
			}
			if (event.type == sf::Event::KeyPressed && event.key.scancode == sf::Keyboard::Scan::Numpad0)
				debugMode = !debugMode;
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
		{
			auto beforeMoveVec = player.getMoveVector();
			auto resVec = CollisionAlgorithms::Get().AABBWithStaticBodiesCollisionCheck(level.Collision, player, {});

			player.move(resVec);

			player.resetOnEverything();

			if (resVec.x * beforeMoveVec.x < 0.f)
			{
				player.setMoveVector({0.f, player.getMoveVector().y});

				if (resVec.x > 0.f)
					player.setOnLeftWall(true);
				else
					player.setOnRightWall(true);
			}
			if (resVec.y * beforeMoveVec.y < 0.f)
			{
				player.setMoveVector({player.getMoveVector().x, 0.f});

				if (resVec.y < 0.f)
					player.setOnFloor(true);
				else
					player.setOnCeil(true);
			}
		}

		// Camera
		if (level.accessCamera().isInTransitionAnimation())
		{
			level.accessCamera().transitionAnimationTick(delta, player);
		}
		else
		{
			level.accessCamera().followEntity(player, player.accessCollider().getRectangleShape().getSize().x / 2.f,
											  player.accessCollider().getRectangleShape().getSize().y / 2.f);
		}
		window.setView(level.accessCamera().getView());

		// Debug text
		if (debugMode)
		{
			debugLabelDelta->setText(L"delta: " + std::to_wstring(delta) + L"\nFPS: " +
									 std::to_wstring(delta > 0 ? 1000000 / delta : 0));

			debugGui.setPosition(sf::Vector2f(level.accessCamera().getView().getCenter() -
											  (level.accessCamera().getView().getSize() / 2.f)));
		}

		// ||--------------------------------------------------------------------------------||
		// ||                                     Render                                     ||
		// ||--------------------------------------------------------------------------------||

		window.clear();

		window.draw(player.getSprite());

		if (debugMode)
		{
			for (auto const& pair : level.Collision)
			{
				auto vector = pair.second;
				for (auto&& cb : vector)
				{
					window.draw(cb->getRectangleShape());
				}
			}

			window.draw(player.accessCollider().getRectangleShape());

			debugGui.render(window);
			// debugGui.print();
		}

		window.display();
	}
}