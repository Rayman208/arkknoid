#include <SFML\Graphics.hpp>
#include <Box2D\Box2D.h>
#include <SFML\Audio.hpp>

#include "ContactListener.h"

using namespace sf;

const float PtM = 0.04f;
const float MtP = 25.0f;

static int PlayerLifes = 3;

#pragma region CreateBodies

b2Body* CreateSomeSquareBody(b2World &world, float x, float y, float w, float h, b2BodyType type, char* userData)
{
	b2BodyDef bodyDef;
	b2PolygonShape polygonShape;
	b2FixtureDef fixtureDef;

	bodyDef.position = b2Vec2(x*PtM, y*PtM);
	bodyDef.type = type;

	polygonShape.SetAsBox((w / 2.0f)*PtM, (h / 2.0f)*PtM);

	fixtureDef.shape = &polygonShape;
	fixtureDef.density = 0.01f;
	fixtureDef.friction = 0.0f;
	fixtureDef.restitution = 1.0f;

	b2Body *body = world.CreateBody(&bodyDef);
	body->CreateFixture(&fixtureDef);
	body->SetUserData(userData);

	return body;
}

b2Body* CreateSomeCircleBody(b2World &world, float x, float y, float r, b2BodyType type, char* userData)
{
	b2BodyDef bodyDef;
	b2CircleShape circleShape;
	b2FixtureDef fixtureDef;

	bodyDef.position = b2Vec2(x*PtM, y*PtM);
	bodyDef.type = type;

	circleShape.m_radius = (r / 2.0) * PtM;

	fixtureDef.shape = &circleShape;
	fixtureDef.density = 0.01f;
	fixtureDef.friction = 0.0f;
	fixtureDef.restitution = 1.0f;

	b2Body *body = world.CreateBody(&bodyDef);
	body->CreateFixture(&fixtureDef);
	body->SetUserData(userData);

	return body;
}
#pragma endregion

#pragma region SpritesFunctions

void SetOriginInCenterSprite(Sprite &sprite)
{
	sprite.setOrigin
	(
		sprite.getGlobalBounds().width / 2,
		sprite.getGlobalBounds().height / 2
	);
}

float GetSpriteWidth(Sprite &sprite)
{
	return sprite.getGlobalBounds().width;
}

float GetSpriteHeight(Sprite &sprite)
{
	return sprite.getGlobalBounds().height;
}

#pragma endregion

#pragma region GameTools

bool IsWin(b2World &world)
{
	int countBricks = 0;

	for (b2Body* currentBody = world.GetBodyList(); currentBody != NULL; currentBody = currentBody->GetNext())
	{
		if ((char*)currentBody->GetUserData() == "brick")
		{
			countBricks++;
		}
	}
	if (countBricks > 0) { return false; }
	else { return true; }
}

bool IsLose()
{
	if (PlayerLifes > 0) { return false; }
	else { return true; }
}

#pragma endregion

void main()
{
	int gameResult = 0;

#pragma region Window&World
	RenderWindow window(VideoMode(800, 600), "SFML BOX2D");
	window.setFramerateLimit(60);

	b2Vec2 gravity(0.0f, 0.0f);
	b2World world(gravity);
	ContactListener ÒontactListener;
	world.SetContactListener(&ÒontactListener);

	Music music;
	music.openFromFile("music.ogg");
	music.setLoop(true);
	music.play();

	Font font;
	font.loadFromFile("arial.ttf");
	
	Text lifesText;
	lifesText.setPosition(0, window.getSize().y - 50);
	lifesText.setFillColor(Color::Red);
	lifesText.setCharacterSize(30);
	lifesText.setFont(font);
#pragma endregion

#pragma region Textures&Sprites
	Texture groundTexture, ballTexture, brickTexture;
	Sprite groundSprite, ballSprite, brickSprite;

	groundTexture.loadFromFile("ground.png");
	ballTexture.loadFromFile("ball.png");
	brickTexture.loadFromFile("brick.png");

	groundSprite.setTexture(groundTexture);
	ballSprite.setTexture(ballTexture);
	brickSprite.setTexture(brickTexture);

	SetOriginInCenterSprite(groundSprite);
	SetOriginInCenterSprite(ballSprite);
	SetOriginInCenterSprite(brickSprite);
#pragma endregion

#pragma region GameObjects
	b2Body* ground = CreateSomeSquareBody(
		world,
		window.getSize().x / 2,
		500,
		GetSpriteWidth(groundSprite),
		GetSpriteHeight(groundSprite),
		b2BodyType::b2_kinematicBody,
		"ground");

	b2Body *ball = CreateSomeCircleBody(
		world,
		window.getSize().x / 2,
		window.getSize().y / 2,
		GetSpriteWidth(ballSprite),
		b2BodyType::b2_dynamicBody,
		"ball");
#pragma endregion

#pragma region CreateWalls
	CreateSomeSquareBody(
		world,
		1,
		window.getSize().y / 2,
		1,
		window.getSize().y,
		b2BodyType::b2_staticBody,
		"wall");

	CreateSomeSquareBody(
		world,
		window.getSize().x - 1,
		window.getSize().y / 2,
		1,
		window.getSize().y,
		b2BodyType::b2_staticBody,
		"wall");

	CreateSomeSquareBody(
		world,
		window.getSize().x / 2,
		0,
		window.getSize().x,
		1,
		b2BodyType::b2_staticBody,
		"wall");

	CreateSomeSquareBody(
		world,
		window.getSize().x / 2,
		window.getSize().y - 1,
		window.getSize().x,
		1,
		b2BodyType::b2_staticBody,
		"floor");
#pragma endregion

#pragma region CreateBricks
	int countBricks = window.getSize().x / GetSpriteWidth(brickSprite);

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < countBricks; j++)
		{
			CreateSomeSquareBody(
				world,
				j*(GetSpriteWidth(brickSprite)+10)+15,
				i*(GetSpriteHeight(brickSprite)+10)+50,
				GetSpriteWidth(brickSprite),
				GetSpriteHeight(brickSprite),
				b2BodyType::b2_staticBody,
				"brick");
		}
	}
#pragma endregion

	while (window.isOpen()==true)
	{
#pragma region Events
		Event evt;
		while (window.pollEvent(evt) == true)
		{
			if (evt.type == Event::Closed ||
				evt.type == Event::KeyPressed &&
				evt.key.code == Keyboard::Key::Escape)
			{
				window.close();
			}
			if (evt.type == Event::KeyPressed &&
				evt.key.code == Keyboard::Key::Right &&
				ground->GetPosition().x*MtP < window.getSize().x)
			{
				ground->SetLinearVelocity(b2Vec2(10.0f, 0));
			}
			if (evt.type == Event::KeyPressed &&
				evt.key.code == Keyboard::Key::Left &&
				ground->GetPosition().x*MtP > 0)
			{
				ground->SetLinearVelocity(b2Vec2(-10.0f, 0));
			}
			if (evt.type == Event::KeyPressed &&
				evt.key.code == Keyboard::Key::Down)
			{
				ground->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
			}
			if (evt.type == Event::KeyPressed &&
				evt.key.code == Keyboard::Key::Space)
			{
				ball->SetLinearVelocity(b2Vec2(10.0f, 10.0f));
			}
		}
#pragma endregion
		
#pragma region Physics
		world.Step(1.0 / 60.0f, 8, 3);
		if (ground->GetPosition().x*MtP < 0 || ground->GetPosition().x*MtP > window.getSize().x)
		{
			ground->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
		}

		for (b2Body* currentBody = world.GetBodyList(); currentBody != NULL; currentBody = currentBody->GetNext())
		{
			if ((char*)currentBody->GetUserData() == "delete")
			{
				world.DestroyBody(currentBody);
				break;
			}
		}
#pragma endregion
		
#pragma region Drawing		
		window.clear(Color::White);

		lifesText.setString(L"∆ËÁÌË: " + to_string(PlayerLifes));
		window.draw(lifesText);

		for (b2Body* currentBody = world.GetBodyList(); currentBody != NULL; currentBody = currentBody->GetNext())
		{
			if (currentBody->GetUserData() == "ground")
			{
				groundSprite.setPosition
				(
					currentBody->GetPosition().x*MtP,
					currentBody->GetPosition().y*MtP
				);
				window.draw(groundSprite);
			}
			if (currentBody->GetUserData() == "brick")
			{
				brickSprite.setPosition
				(
					currentBody->GetPosition().x*MtP,
					currentBody->GetPosition().y*MtP
				);
				window.draw(brickSprite);
			}
			if (currentBody->GetUserData() == "ball")
			{
				ballSprite.setPosition
				(
					currentBody->GetPosition().x*MtP,
					currentBody->GetPosition().y*MtP
				);
				ballSprite.setRotation(currentBody->GetAngle() * 180 / b2_pi);
				window.draw(ballSprite);
			}
		}
		window.display();

		if (IsWin(world) == true) 
		{
			gameResult = 1;
		}
		else if(IsLose()==true)
		{
			gameResult = -1;
		}
		if (gameResult != 0) { break; }
#pragma endregion
	}

	lifesText.setPosition(100, 100);
	music.stop();

	while (window.isOpen()==true)
	{
		Event evt;
		while (window.pollEvent(evt)==true)
		{
			if (evt.type == Event::Closed ||
				evt.type == Event::KeyPressed &&
				evt.key.code == Keyboard::Key::Escape)
			{
				window.close();
			}
		}
		window.clear(Color::White);

		if (gameResult == 1)
		{
			lifesText.setString(L"œÓ·Â‰‡");
		}
		else
		{
			lifesText.setString(L"œÓ‡ÊÂÌËÂ");
		}
		window.draw(lifesText);
		window.display();
	}
}
