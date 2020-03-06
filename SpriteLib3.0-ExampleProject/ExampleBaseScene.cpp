#include "ExampleBaseScene.h"
#include "Utilities.h"
#include "Timer.h"
#include "EffectManager.h"


ExampleBaseScene::ExampleBaseScene(std::string name)
	: Scene(name)
{
	//Gravity
	m_gravity = b2Vec2(0.f, -75.f);
	m_physicsWorld->SetGravity(m_gravity);
}

void ExampleBaseScene::InitScene(float windowWidth, float windowHeight)
{
	////Effect stuffs
	//EffectManager::CreateEffect(EffectType::Vignette, BackEnd::GetWindowWidth(), BackEnd::GetWindowHeight());
	//VignetteEffect* effect = (VignetteEffect*)EffectManager::GetEffect(EffectManager::GetVignetteHandle());

	//effect->SetOpacity(1.f);
	//effect->SetInnerRadius(0.f);
	//effect->SetOuterRadius(0.870f);

	/*if (m_physicsWorld != NULL && m_physicsWorld->GetBodyCount() != 0)
	{
		b2Body* bodyList = m_physicsWorld->GetBodyList();

		delete m_physicsWorld;
	}*/
	if (m_isInit) {
		return;
	}
	//Dynamically allocates the register
	m_sceneReg = new entt::registry;

	//Attach the register
	ECS::AttachRegister(m_sceneReg);

	//Sets up aspect ratio for the camera
	float aspectRatio = windowWidth / windowHeight;

//Setup Main Camera entity
	{
	//Creates Camera entity
	auto entity = ECS::CreateEntity();

	//Creates new orthographic camera
	ECS::AttachComponent<Camera>(entity);
	ECS::AttachComponent<HorizontalScroll>(entity);
	ECS::AttachComponent<VerticalScroll>(entity);

	vec4 temp = vec4(-75.f, 75.f, -75.f, 75.f);
	ECS::GetComponent<Camera>(entity).SetOrthoSize(temp);
	ECS::GetComponent<Camera>(entity).SetWindowSize(vec2(float(windowWidth), float(windowHeight)));
	ECS::GetComponent<Camera>(entity).Orthographic(aspectRatio, temp.x, temp.y, temp.z, temp.w, -100.f, 100.f);

	//attaches the camera to the vertical scroll
	ECS::GetComponent<HorizontalScroll>(entity).SetCam(&ECS::GetComponent<Camera>(entity));
	ECS::GetComponent<VerticalScroll>(entity).SetCam(&ECS::GetComponent<Camera>(entity));

	//Sets up the Identifier
	unsigned int bitHolder = EntityIdentifier::CameraBit();
	ECS::SetUpIdentifier(entity, bitHolder, "Horizontal Scrolling Cam");
	ECS::SetIsMainCamera(entity, true);
	}

	//Setup new Entity Box Main Player
	{
		//Our attack animation file
		auto Move = File::LoadJSON("Hamster.json");

		//Creates entity
		auto entity = ECS::CreateEntity();

		//Main Player
		EntityIdentifier::MainPlayer(entity);

		//Add components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<PhysicsBody>(entity);
		ECS::AttachComponent<AnimationController>(entity);

		//Sets up components
		//std::string fileName = "RetiringStudios.jpg";//20 20
		std::string fileName = "Hamster.png";
		auto& animController = ECS::GetComponent<AnimationController>(entity);
		animController.InitUVs(fileName);

		animController.AddAnimation(Move["Idle"]);
		animController.AddAnimation(Move["Jump"]);
		animController.AddAnimation(Move["Left"]);
		animController.AddAnimation(Move["Right"]);

		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 112 / 4, 117 / 4, true, &animController);
		ECS::GetComponent<Sprite>(entity).SetUVs(vec2(1.f, 117.f), vec2(112.f, 1.f));
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 0.f, -10.f));

		////Set first anitmation
		//animController.SetActiveAnim(0);
		////gets first animation
		//auto& anim = animController.GetAnimation(0);

		////Makes it repeat
		//anim.SetRepeating(true);

		////Sets the time between frames
		//anim.SetSecPerFrame(0.8f);

		auto& tempSpr = ECS::GetComponent<Sprite>(entity);
		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

		//Physics body covers the entire sprite
		//ID type is player
		//Collides with Enemies and Environments
		//Moves
		float shrinkX = 0.f;
		float shrinkY = 0.f;

		b2Body* tempBody;
		b2BodyDef tempDef;
		tempDef.type = b2_dynamicBody;
		tempDef.position.Set(float32(0.f), float32(10.f));
		//tempDef.position.Set(float32(50.f), float32(5.f));

		tempBody = m_physicsWorld->CreateBody(&tempDef);

		tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
			vec2(0.f, 0.f), false);

		//Makes the box not able to rotate
		tempBody->SetFixedRotation(true);

		//Sets up the Identifier
		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit() | EntityIdentifier::AnimationBit();
		ECS::SetUpIdentifier(entity, bitHolder, "Main Box");
		ECS::SetIsMainPlayer(entity, true);
	}

	//Makes the camera focus on this entity
	ECS::GetComponent<HorizontalScroll>(EntityIdentifier::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(EntityIdentifier::MainPlayer()));
	ECS::GetComponent<VerticalScroll>(EntityIdentifier::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(EntityIdentifier::MainPlayer()));

//Ground
	{
		//creates entity
		auto entity = ECS::CreateEntity();

		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<PhysicsBody>(entity);
		ECS::AttachComponent<Transform>(entity);

		std::string filename = "treesV1.png";

		ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 800, 10);
		ECS::GetComponent<Transform>(entity).SetPosition(-50.f, -50.f, -10.f);

		auto& tempSpr = ECS::GetComponent<Sprite>(entity);
		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

		float shrinkX = 0.f;
		float shrinkY = 0.f;

		b2Body* tempBody;
		b2BodyDef tempDef;
		tempDef.type = b2_staticBody;
		tempDef.position.Set(float32(-50.f), float32(0.f));

		tempBody = m_physicsWorld->CreateBody(&tempDef);

		tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
			vec2(0.f, 0.f), false);
	
		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit()|EntityIdentifier::PhysicsBit();
		ECS::SetUpIdentifier(entity, bitHolder, "ground test");

	}

	//Creates entity
	auto entity = ECS::CreateEntity();

	//Add components
	ECS::AttachComponent<Sprite>(entity);
	ECS::AttachComponent<Transform>(entity);

	//Sets up components
	//std::string fileName = "treesV1.png";
	std::string fileName = "winter.png";
	//ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 100 * 5, 45 * 5);
	ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 100 * 8, 45 * 5);
	ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 0.f, -10.f));

	//Sets up the Identifier
	unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit();
	ECS::SetUpIdentifier(entity, bitHolder, "Winter");
	m_isInit = true;
}


	

void ExampleBaseScene::Update()
{

	Aiming(m_mouseGL);

	AdjustScrollOffset();

	//RainbowBackground();
}



/*void ExampleBaseScene::RainbowBackground()
{
	//Makes clear color smoothly blend between two colors, going back and forth.
	m_clearColor = Util::Lerp<vec4>(m_clearColor1, m_clearColor2, m_lerpVal);

	if (m_lerpVal >= 1.f)
	{
		vec4 temp = m_clearColor2;

		m_clearColor2 = m_clearColor1;
		m_clearColor1 = temp;

		m_lerpVal = 0.f;
	}

	m_lerpVal += Timer::deltaTime / m_repeatTime;
}
*/

void ExampleBaseScene::AdjustScrollOffset()
{
	float maxSizeX = ECS::GetComponent<Camera>(EntityIdentifier::MainCamera()).GetOrthoSize().y;
	float maxSizeY = ECS::GetComponent<Camera>(EntityIdentifier::MainCamera()).GetOrthoSize().w;

	float playerHalfSize = ECS::GetComponent<Sprite>(EntityIdentifier::MainPlayer()).GetWidth() / 2.f;

	ECS::GetComponent<HorizontalScroll>(EntityIdentifier::MainCamera()).SetOffset((maxSizeX * BackEnd::GetAspectRatio()) - playerHalfSize);
	ECS::GetComponent<VerticalScroll>(EntityIdentifier::MainCamera()).SetOffset(maxSizeY - playerHalfSize);

}

void ExampleBaseScene::Aiming(vec2 mousePosition)
{
	vec3 playerLoc = ECS::GetComponent<Transform>(EntityIdentifier::MainPlayer()).GetPosition();

	vec2 convertedMouse = mousePosition;


	//Rotates character around center point
	/*float dx = playerLoc.x - convertedMouse.x;
	float dy = playerLoc.y - convertedMouse.y;

	float playerDir = (atan2(dy, dx));

	if (ECS::GetComponent<EntityIdentifier>(EntityIdentifier::MainPlayer()).GetIdentifier()& EntityIdentifier::PhysicsBit())
	{
		auto body = ECS::GetComponent<PhysicsBody>(EntityIdentifier::MainPlayer()).GetBody();
		body->SetTransform(body->GetPosition(), playerDir - Transform::ToRadians(90.f));
	}
	else
	{
		ECS::GetComponent<Transform>(EntityIdentifier::MainPlayer()).SetRotationAngleZ(playerDir - Transform::ToRadians(90.f));
	}*/
}

void ExampleBaseScene::KeyboardHold()
{
	auto& animController = ECS::GetComponent<AnimationController>(1);

	if (Input::GetKey(Key::S))
	{
		animController.SetActiveAnim(0);
	}
	if (Input::GetKey(Key::W))
	{
		animController.SetActiveAnim(1);
	}
	if (Input::GetKey(Key::A))
	{
		animController.SetActiveAnim(2);
	}
	if (Input::GetKey(Key::D))
	{
		animController.SetActiveAnim(3);
	}

	if (ECS::GetComponent<EntityIdentifier>(EntityIdentifier::MainPlayer()).GetIdentifier()& EntityIdentifier::PhysicsBit())
	{
		auto body = ECS::GetComponent<PhysicsBody>(EntityIdentifier::MainPlayer()).GetBody();

		b2Vec2 vel = body->GetLinearVelocity();

		if (Input::GetKeyDown(Key::W)) {
			vel.y = 100;//upwards - don't change x velocity
			body->SetLinearVelocity(vel);
		}
		if (Input::GetKey(Key::A))
		{
			vel.x = -25;//upwards - don't change x velocity
			body->SetLinearVelocity(vel);
		}
		if (Input::GetKey(Key::D))
		{
			vel.x = 25;//upwards - don't change x velocity
			body->SetLinearVelocity(vel);
		}
		//dash right E
		if (Input::GetKeyDown(Key::E))
		{
			animController.SetActiveAnim(3);
			vel.x = 50;//upwards - don't change x velocity
			body->SetLinearVelocity(vel);
		}

		
	}

	

	
}

void ExampleBaseScene::MouseMotion(SDL_MouseMotionEvent evnt)
{
	m_mouseGL = Util::ConvertToGL(m_sceneReg, vec2(float(evnt.x), float(evnt.y)));
}

void ExampleBaseScene::MouseClick(SDL_MouseButtonEvent evnt)
{
}
