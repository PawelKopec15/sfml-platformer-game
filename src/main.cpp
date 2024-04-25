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
#include "Inventory.hpp"
#include "Level.hpp"
#include "Player.hpp"
#include "TMXParser.hpp"
#include "Vector2Functions.hpp"

void handleSpriteInitPlayer(Player& outPlayer, sf::Texture& outTex)
{
	if (!outTex.loadFromFile("assets/graphics/player_1.png"))
	{
		std::cerr << "Error loading player sprite texture" << std::endl;
	}
	outPlayer.setSpriteTexture(outTex);
	outPlayer.setSpriteTextureRect({0, 0, 16, 32});
	outPlayer.setSpriteOrigin({8.f, 16.f});
	outPlayer.setSpriteOffset({0.f, -9.f});

	KeyFrameAnimator<AnimatedSprite::KeyType> standAnim;
	standAnim.addKeyToKeyFrameTimeline(AnimatedSprite::KeyType::RECT_X, 0, 0.f);

	KeyFrameAnimator<AnimatedSprite::KeyType> runAnim(270000);
	runAnim.addKeyToKeyFrameTimeline(AnimatedSprite::KeyType::RECT_X, 0, 16.f);
	runAnim.addKeyToKeyFrameTimeline(AnimatedSprite::KeyType::RECT_X, 90000, 32.f);
	runAnim.addKeyToKeyFrameTimeline(AnimatedSprite::KeyType::RECT_X, 180000, 48.f);

	KeyFrameAnimator<AnimatedSprite::KeyType> jumpAnim;
	jumpAnim.addKeyToKeyFrameTimeline(AnimatedSprite::KeyType::RECT_X, 0, 64.f);

	KeyFrameAnimator<AnimatedSprite::KeyType> fallAnim;
	fallAnim.addKeyToKeyFrameTimeline(AnimatedSprite::KeyType::RECT_X, 0, 80.f);

	KeyFrameAnimator<AnimatedSprite::KeyType> turnAnim;
	turnAnim.addKeyToKeyFrameTimeline(AnimatedSprite::KeyType::RECT_X, 0, 96.f);

	KeyFrameAnimator<AnimatedSprite::KeyType> pushAnim(800000);
	pushAnim.addKeyToKeyFrameTimeline(AnimatedSprite::KeyType::RECT_X, 0, 112.f);
	pushAnim.addKeyToKeyFrameTimeline(AnimatedSprite::KeyType::RECT_X, 400000, 128.f);

	outPlayer.addAnimation("Stand", standAnim);
	outPlayer.addAnimation("Run", runAnim);
	outPlayer.addAnimation("Jump", jumpAnim);
	outPlayer.addAnimation("Fall", fallAnim);
	outPlayer.addAnimation("Turn", turnAnim);
	outPlayer.addAnimation("Push", pushAnim);
}

AnimatedSprite createCoinSprite(sf::Texture& coinTexture)
{
	AnimatedSprite toRet;

	if (!coinTexture.loadFromFile("assets/graphics/items_1.png"))
	{
		std::cerr << "Error loading coin sprite texture" << std::endl;
	}

	toRet.setTexture(coinTexture);
	toRet.setTextureRect({0, 0, 16, 16});

	KeyFrameAnimator<AnimatedSprite::KeyType> anim(500000);
	anim.addKeyToKeyFrameTimeline(AnimatedSprite::KeyType::RECT_X, 0, 0.f);
	anim.addKeyToKeyFrameTimeline(AnimatedSprite::KeyType::RECT_X, 125000, 16.f);
	anim.addKeyToKeyFrameTimeline(AnimatedSprite::KeyType::RECT_X, 250000, 32.f);
	anim.addKeyToKeyFrameTimeline(AnimatedSprite::KeyType::RECT_X, 375000, 48.f);

	toRet.addAnimation("Default", anim);

	return toRet;
}

int main()
{
	auto window = sf::RenderWindow{{1024u, 768u}, "Platform Game", sf::Style::Default};
	window.setFramerateLimit(144);

	// Set the locale to support Unicode
	std::wcout.imbue(std::locale(""));

	BitmapFont fontKubasta;
	if (!fontKubasta.create("assets/font/kubasta_regular_8.PNG", "assets/font/kubasta_regular_8.fnt"))
		std::cerr << "Error loading kubasta regular 8 font as BitmapFont" << std::endl;

	// Debug mode
	bool debugMode = false;

	// Test entities

	sf::Texture coinTexture;
	Level level(createCoinSprite(coinTexture));
	level.create("leveldata/testmap1.tmx", false);
	level.accessCamera().setView(sf::View(sf::FloatRect(0.f, 0.f, 256.f, 192.f)));

	sf::Texture levelTiles;
	if (!levelTiles.loadFromFile("assets/graphics/tiles_1.png"))
		std::cerr << "Failed loading tiles_1.png" << std::endl;

	Controls p1Controls;

	Player player(sf::Vector2f(32, 128), p1Controls);
	player.accessCollider().setColor(sf::Color(255, 100, 100, 120));

	sf::Texture playerTexture;
	handleSpriteInitPlayer(player, playerTexture);

	Inventory inventory;
	inventory.addCollectBox(player.getCollectBox());

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

			// Handling debug mode toggles
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

		// Coins
		for (auto&& coin : level.Collectables)
		{
			coin.animate(delta);
		}
		inventory.checkIfCollectedAnything(level.Collectables);

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

		auto hudText = L"GEMS: " + std::to_wstring(inventory.getInventoryState().coins);

		auto debugText =
			L"delta: " + std::to_wstring(delta) + L"\nFPS: " + std::to_wstring(delta > 0 ? 1000000 / delta : 0);

		auto textPos = level.accessCamera().getView().getCenter() - (level.accessCamera().getView().getSize() / 2.f) +
					   sf::Vector2f(2.f, -2.f);

		// ||--------------------------------------------------------------------------------||
		// ||                                     Render                                     ||
		// ||--------------------------------------------------------------------------------||

		window.clear(debugMode ? sf::Color::Black : level.getBackgroundColor());

		// Drawing tiles, backgrounds and collectables
		// ugly placeholder code
		// TODO get rid of it
		sf::Sprite tile(levelTiles);
		{
			auto set = level.Background.gatherFromChunks();
			for (auto&& cb : set)
			{
				auto id     = cb->getTileId();
				int texLeft = id % (uint32_t)(levelTiles.getSize().x / (uint32_t)cb->getSize().x) * cb->getSize().x;
				int texTop  = id / (uint32_t)(levelTiles.getSize().x / (uint32_t)cb->getSize().x) * cb->getSize().y;

				tile.setTextureRect(sf::IntRect(texLeft, texTop, (int)cb->getSize().x, (int)cb->getSize().y));
				tile.setPosition(cb->getPosition());
				window.draw(tile);
			}
		}
		{
			auto set = level.Collision.gatherFromChunks();
			for (auto&& cb : set)
			{
				auto id     = cb->getTileId();
				int texLeft = id % (uint32_t)(levelTiles.getSize().x / (uint32_t)cb->getSize().x) * cb->getSize().x;
				int texTop  = id / (uint32_t)(levelTiles.getSize().x / (uint32_t)cb->getSize().x) * cb->getSize().y;

				tile.setTextureRect(sf::IntRect(texLeft, texTop, (int)cb->getSize().x, (int)cb->getSize().y));
				tile.setPosition(cb->getPosition());
				window.draw(tile);
			}
		}
		{
			for (auto&& coin : level.Collectables)
			{
				window.draw(coin.getSprite());
			}
		}

		window.draw(player.getSprite());

		if (debugMode)
		{
			auto set = level.Collision.gatherFromChunks();
			for (auto&& cb : set)
				window.draw(cb->getRectangleShape());

			for(auto&& coin : level.Collectables)
				window.draw(coin.accessCollectArea().getRectangleShape());
			
			window.draw(player.accessCollider().getRectangleShape());
			window.draw(player.getCollectBox()->getRectangleShape());

			window.draw(fontKubasta.getTextDrawable(debugText, textPos).first, &fontKubasta.getFontTexture());
		}
		else
		{
			window.draw(fontKubasta.getTextDrawable(hudText, textPos).first, &fontKubasta.getFontTexture());
		}

		window.display();
	}
}