#include "Player.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"
#include "EntityManager.h"

Player::Player() : Entity(EntityType::PLAYER)
{
	name = "Player";
}

Player::~Player() {

}

bool Player::Awake() {

	//L03: TODO 2: Initialize Player parameters
	/*position = Vector2D(0, 0);*/
	return true;
}

bool Player::Start() {

	//L03: TODO 2: Initialize Player parameters
	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
	position.setX(parameters.attribute("x").as_int());
	position.setY(parameters.attribute("y").as_int());
	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();

	//Load animations
	idle.LoadAnimations(parameters.child("animations").child("idle"));
	idleL.LoadAnimations(parameters.child("animations").child("idleL"));
	run.LoadAnimations(parameters.child("animations").child("run"));
	runL.LoadAnimations(parameters.child("animations").child("runL"));
	jump.LoadAnimations(parameters.child("animations").child("jump"));
	jumpL.LoadAnimations(parameters.child("animations").child("jumpL"));
	die.LoadAnimations(parameters.child("animations").child("die"));
	dieL.LoadAnimations(parameters.child("animations").child("dieL"));
	punch.LoadAnimations(parameters.child("animations").child("punch"));
	punchL.LoadAnimations(parameters.child("animations").child("punchL"));
	/*currentAnimation = &idle;*/

	// L08 TODO 5: Add physics to the player - initialize physics body
	pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX(), (int)position.getY(), texW / 2 - 2, bodyType::DYNAMIC);

	// L08 TODO 6: Assign player class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
	pbody->listener = this;

	// L08 TODO 7: Assign collider type
	pbody->ctype = ColliderType::PLAYER;

	// Set the gravity of the body
	if (!parameters.attribute("gravity").as_bool()) pbody->body->SetGravityScale(0);

	//initialize audio effect
	/*pickCoinFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/retro-video-game-coin-pickup-38299.ogg");*/
	deadFx = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/dead.ogg");
	punchFx = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/punch.ogg");
	levelCompletedFx = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/levelCompleted.ogg");

	// Attack desactivated
	attack = false;

	// Dead desactivated
	dead = false;

	// God Mode desactivated first
	godmode = false;

	// Starts looking to the right
	lookRight = true;

	//Initialize hitTimer
	hitTimer = 0;
	
	//Initialize timerFinish
	timerFinish = 0;

	return true;
}

bool Player::Update(float dt)
{
	// L08 TODO 5: Add physics to the player - updated player position using physics
	b2Vec2 velocity;

	if (!godmode) {

		velocity = b2Vec2(0, pbody->body->GetLinearVelocity().y);
	}

	// Attack and idle states
	if (lookRight && !dead) {

		if (attack) {
			currentAnimation = &punch;


			if (SDL_GetTicks() - hitTimer >= 700) {
				attack = false;
				currentAnimation = &idle;

			}
		}
		else {
			currentAnimation = &idle;

			if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_E) == KEY_DOWN) {
				attack = true;
				currentAnimation = &punch;
				hitTimer = SDL_GetTicks();
				Engine::GetInstance().audio.get()->PlayFx(punchFx);
			}
		}

	}
	if (!lookRight && !dead) {

		if (attack) {
			currentAnimation = &punchL;

			if (SDL_GetTicks() - hitTimer >= 700) {
				attack = false;
				currentAnimation = &idleL;

			}
		}
		else {
			currentAnimation = &idleL;

			if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_E) == KEY_DOWN) {
				attack = true;
				currentAnimation = &punchL;
				hitTimer = SDL_GetTicks();
				Engine::GetInstance().audio.get()->PlayFx(punchFx);
			}
		}
	}

	// Move left
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && !dead) {
		velocity.x = -0.17 * 16;

		lookRight = false;

		if (!lookRight) {

			currentAnimation = &runL;
		}

	}

	// Move right
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && !dead) {
		velocity.x = 0.17 * 16;

		lookRight = true;

		if (lookRight) {

			currentAnimation = &run;
		}


	}

	// Dies and respawns
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_T) == KEY_DOWN) {

		dead = true;

	}

	// After doing the dead animation respawns on the start 
	if (dead) {

		velocity = b2Vec2(0, -0.4);

		if (lookRight) {

			currentAnimation = &die;

			if (currentAnimation->loopCount > 0) {

				currentAnimation->loopCount = 0;

				Engine::GetInstance().scene.get()->LoadState();
				dead = false;

			}

		}

		else if (!lookRight) {

			currentAnimation = &dieL;

			if (currentAnimation->loopCount > 0) {

				currentAnimation->loopCount = 0;

				Engine::GetInstance().scene.get()->LoadState();
				dead = false;

			}

		}
	}


	// Jump
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && isJumping == false && !godmode && !dead) {
		// Apply an initial upward force
		pbody->body->ApplyLinearImpulseToCenter(b2Vec2(0, -jumpForce), true);
		isJumping = true;
	}

	// If the player is jumpling, we don't want to apply gravity, we use the current velocity prduced by the jump
	if (isJumping == true && !godmode && !dead)
	{
		if (lookRight) {
			currentAnimation = &jump;
		}
		else if (!lookRight) {
			currentAnimation = &jumpL;
		}

		velocity.y = pbody->body->GetLinearVelocity().y;
	}

	// Activate /Desactivate God Mode
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F10) == KEY_DOWN && !godmode) {

		godmode = true;

	}
	else if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F10) == KEY_DOWN && godmode) {

		godmode = false;
		velocity = b2Vec2(0, pbody->body->GetLinearVelocity().y);
	}

	// God Mode On
	if (godmode == true) {
		velocity = b2Vec2(0, -0.4);


		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
			velocity.x = -0.4 * 16;
		}

		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
			velocity.x = 0.4 * 16;
		}

		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
			velocity.y = 0.4 * 16;
		}
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
			velocity.y = -0.4 * 16;
		}


	}

	// Apply the velocity to the player
	pbody->body->SetLinearVelocity(velocity);

	b2Transform pbodyPos = pbody->body->GetTransform();
	position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);


	// Respawn when player falls of the map
	if (position.getY() > 768 && !godmode) {

		dead = true;
		Engine::GetInstance().audio.get()->PlayFx(deadFx);
	}

	// When player finishes the level a sfx is played and window closes automatically
	if (position.getX() > 5750) {
		if (timerFinish == 0) {
			timerFinish = SDL_GetTicks();
			Engine::GetInstance().audio.get()->PlayFx(levelCompletedFx);
		}

		if (SDL_GetTicks() - timerFinish >= 1000) {
			Engine::GetInstance().scene.get()->finishLevel = true;
		}
	}


	// Draws texture
	Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());
	currentAnimation->Update();
	return true;
}

bool Player::CleanUp()
{
	LOG("Cleanup player");
	Engine::GetInstance().textures.get()->UnLoad(texture);
	return true;
}

// L08 TODO 6: Define OnCollision function for the player. 
void Player::OnCollision(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");
		// Reset the jump bool when touching the ground
		isJumping = false;
		break;
	case ColliderType::ITEM:
		LOG("Collision ITEM");
		break;
	case ColliderType::ENEMY:
		LOG("Collision ENEMY");
		// If the player attacks the enemy is deleted 
		if (attack) {
			physB->deleted = true;
			Engine::GetInstance().physics.get()->DeletePhysBody(physB);
		}
		// When godmode is activated player is immortal
		else if (godmode) {
			dead = false;
		}
		// Player dies when a enemy hits him
		else {
			dead = true;
		}
		break;
	case ColliderType::CHECKPOINT:
		LOG("Collision CHECKPOINT");
		Engine::GetInstance().scene.get()->SaveState();
		break;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	default:
		break;
	}
}

void Player::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		LOG("End Collision PLATFORM");
		break;
	case ColliderType::ITEM:
		LOG("End Collision ITEM");
		Engine::GetInstance().audio.get()->PlayFx(pickCoinFxId);
		break;
	case ColliderType::ENEMY:
		LOG("End Collision ENEMY");
		if (attack) {

		}
		// Sounds deadFx after dying
		else if (!godmode) {
			Engine::GetInstance().audio.get()->PlayFx(deadFx);
		}
		break;
	case ColliderType::CHECKPOINT:
		LOG("End Collision CHECKPOINT");
		break;
	case ColliderType::UNKNOWN:
		LOG("End Collision UNKNOWN");
		break;
	default:
		break;
	}
}

void Player::SetPosition(Vector2D pos) {
	pos.setX(pos.getX() + texW / 2);
	pos.setY(pos.getY() + texH / 2);
	b2Vec2 bodyPos = b2Vec2(PIXEL_TO_METERS(pos.getX()), PIXEL_TO_METERS(pos.getY()));
	pbody->body->SetTransform(bodyPos, 0);
}

Vector2D Player::GetPosition() {
	b2Vec2 bodyPos = pbody->body->GetTransform().p;
	Vector2D pos = Vector2D(METERS_TO_PIXELS(bodyPos.x), METERS_TO_PIXELS(bodyPos.y));
	return pos;
}

void Player::Respawn() {

	pbody->body->SetTransform(b2Vec2(PIXEL_TO_METERS(100), PIXEL_TO_METERS(465)), 0);
}