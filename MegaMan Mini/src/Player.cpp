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
#include "Bullet.h"
#include "tracy/Tracy.hpp"

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

	// Cargar la textura del jugador desde los par�metros
	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());

	// Inicializar posici�n
	position.setX(parameters.attribute("x").as_int());
	position.setY(parameters.attribute("y").as_int());

	// Inicializar ancho y alto de la textura
	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();

	// Inicializar valores de vida y munici�n
	life = parameters.attribute("life").as_int();
	ammo = parameters.attribute("ammo").as_int();

	// Cargar animaciones desde los par�metros
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
	idleShoot.LoadAnimations(parameters.child("animations").child("idleShoot"));
	idleShootL.LoadAnimations(parameters.child("animations").child("idleShootL"));

	// Inicializar cuerpo f�sico del jugador
	pbody = Engine::GetInstance().physics.get()->CreateCircle(
		static_cast<int>(position.getX()),
		static_cast<int>(position.getY()),
		texW / 2 - 2,
		bodyType::DYNAMIC
	);

	// Asignar el listener del cuerpo f�sico
	pbody->listener = this;

	// Asignar tipo de colisi�n al cuerpo f�sico
	pbody->ctype = ColliderType::PLAYER;

	// Configurar gravedad del cuerpo si est� desactivada en los par�metros
	if (!parameters.attribute("gravity").as_bool())
	{
		pbody->body->SetGravityScale(0);
	}

	// Cargar las barras de informaci�n del jugador
	infoBars = Engine::GetInstance().textures.get()->Load("Assets/Textures/Player/infoBars.png");

	// Inicializar efectos de sonido
	deadFx = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/DeadFx.wav");
	punchFx = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/PunchFx.ogg");
	levelCompletedFx = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/LevelCompletedFx.wav");
	hurtFx = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/HurtFx.wav");
	ammoCollectedFx = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/AmmoCollectedFx.wav");
	lifeCollectedFx = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/LifeCollectedFx.wav");
	pointCollectedFx = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/PointCollectedFx.wav");
	shotFx = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/ShotFx.wav");
	hitFx = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/HitFx.wav");

	// Inicializar estados del jugador
	attack = false;
	dead = false;
	godmode = false;
	lookRight = true;

	// Inicializar variables de colisi�n
	hit = false;
	hitRepetitions = 0;
	hitTimer = 0;
	isVisible = true;
	maxHitRepetitions = 32;
	hitDuration = 50;

	// Inicializar temporizadores
	punchTimer = 0;
	shootTimer = 0.0f;
	timerFinish = 0;

	// Inicializar estados de disparo
	shoot = false;

	// Inicializar checkpoints
	checkpoints[0] = { 100,  465 };
	checkpoints[1] = { 1416, 593 };
	checkpoints[2] = { 2557, 369 };
	checkpoints[3] = { 3524, 433 };
	checkpoints[4] = { 4702, 177 };
	checkpoints[5] = { 4733, 625 };

	return true;
}

bool Player::Update(float dt)
{
	ZoneScoped;

	//if (Engine::GetInstance().scene.get()->pauseEntities){
	//}
	//else if ()
	// L08 TODO 5: Add physics to the player - updated player position using physics
	if (Engine::GetInstance().scene.get()->state == LEVEL1 || Engine::GetInstance().scene.get()->state == LEVEL2) {
	
		b2Vec2 velocity;

		if (!godmode) {

			velocity = b2Vec2(0, pbody->body->GetLinearVelocity().y);
		}

		// Attack and idle states
		if (lookRight && !dead) {
			if (shoot) {
				currentAnimation = &idleShoot;

				if (SDL_GetTicks() - shootTimer >= 400) {
			
					shoot = false;
					currentAnimation = &idle;
				}
			}
			else if (attack) {
				currentAnimation = &punch;


				if (SDL_GetTicks() - punchTimer >= 700) {
					attack = false;
					currentAnimation = &idle;

				}

			}
			else {
				currentAnimation = &idle;

				if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_E) == KEY_DOWN) {
					attack = true;
					currentAnimation = &punch;
					punchTimer = SDL_GetTicks();
					Engine::GetInstance().audio.get()->PlayFx(punchFx);
				}
				else if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F) == KEY_DOWN) {
					
					shoot = true;
					currentAnimation = &idleShoot;
					shootTimer = SDL_GetTicks();
					if (ammo >= 1) {
						Engine::GetInstance().scene.get()->createBullet = true;
						bullet = (Bullet*)Engine::GetInstance().entityManager->CreateEntity(EntityType::BULLET);
						bullet->SetParameters(Engine::GetInstance().scene.get()->bulletConfig);
						bullet->Start();
						bullet->speedX = 5.0f;
						bullet->SetPosition(position, lookRight);
						ammo -= 1;
						Engine::GetInstance().audio.get()->PlayFx(shotFx);
					}

				}
			}

		}
		if (!lookRight && !dead) {

			if (shoot) {
				currentAnimation = &idleShootL;

				if (SDL_GetTicks() - shootTimer >= 400) {
					
					shoot = false;
					currentAnimation = &idle;
				}
			}
			else if (attack) {
				currentAnimation = &punchL;

				if (SDL_GetTicks() - punchTimer >= 700) {
					attack = false;
					currentAnimation = &idleL;

				}

			}
			else {
				currentAnimation = &idleL;

				if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_E) == KEY_DOWN) {
					attack = true;
					currentAnimation = &punchL;
					punchTimer = SDL_GetTicks();
					Engine::GetInstance().audio.get()->PlayFx(punchFx);
				}
				else if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F) == KEY_DOWN) {
					shoot = true;
					currentAnimation = &idleShoot;
					shootTimer = SDL_GetTicks();
					if (ammo >= 1) {
						Engine::GetInstance().scene.get()->createBullet = true;
						bullet = (Bullet*)Engine::GetInstance().entityManager->CreateEntity(EntityType::BULLET);
						bullet->SetParameters(Engine::GetInstance().scene.get()->bulletConfig);
						bullet->Start();
						bullet->speedX = -5.0f;
						bullet->SetPosition(position, lookRight);
						ammo -= 1;
						Engine::GetInstance().audio.get()->PlayFx(shotFx);
					}


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
			life = 0;

		}

		// Dead animation
		if (dead) {

			velocity = b2Vec2(0, -0.4);

			if (lookRight) {


				currentAnimation = &die;


			}

			else if (!lookRight) {

				currentAnimation = &dieL;


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


		// Respawn when player falls of the map or dies
		if (position.getY() > 768 && !godmode) {
			hit = false;
			life = -4;
			Engine::GetInstance().scene.get()->LoadState();
			/*Engine::GetInstance().audio.get()->PlayFx(deadFx);*/
		}
		else if (life <= 0) {
			hit = false;
			dead = true;
			Engine::GetInstance().scene.get()->activationPreparationTimer = true;
			Engine::GetInstance().audio.get()->PlayFx(deadFx);
		}
		else if (life > MAX_LIFE) {
			life = MAX_LIFE;
		}

		if (ammo > MAX_AMMO) {
			ammo = MAX_AMMO;
		}


		// When player finishes the level a sfx is played and window closes automatically
		if (position.getX() > 5750 && Engine::GetInstance().scene.get()->state == LEVEL1) {

			Engine::GetInstance().audio.get()->PlayFx(levelCompletedFx);
			Engine::GetInstance().scene.get()->finishLevel = true;

		}


		// Hit feature
		if (hit && !dead) {
			if (hitTimer == 0) {
				// Inicia el temporizador de hit
				hitTimer = SDL_GetTicks();
			}

			// Alterna la visibilidad del personaje cada `hitDuration` milisegundos
			if (SDL_GetTicks() - hitTimer >= hitDuration) {
				isVisible = !isVisible; // Alterna entre visible e invisible
				hitRepetitions++;

				// Reinicia el temporizador para el siguiente parpadeo
				hitTimer = SDL_GetTicks();
			}

			// Termina el estado de "hit" despu�s de `maxHitRepetitions` parpadeos
			if (hitRepetitions >= maxHitRepetitions) {
				hit = false;
				hitRepetitions = 0;
				isVisible = true; // Aseg�rate de que el personaje sea visible al terminar
				hitTimer = 0;     // Reinicia el temporizador
			}
		}

		// Draws texture
		UpdateRender();
		
		currentAnimation->Update();
		
	}

	return true;
}

bool Player::CleanUp()
{
	LOG("Cleanup player");
	Engine::GetInstance().textures.get()->UnLoad(texture);
	return true;
}

void Player::UpdateRender() {

	float scale = 3.0f; // Escala de ancho (1.5x)

	if (!Engine::GetInstance().scene.get()->help) {
		
		SDL_Rect section;
		section.x = 0;
		section.y = 0;
		section.w = 14;
		section.h = 52;
		// Life bar
		SDL_Rect lifeBarDstRect = { 15, 15, (int)(section.w * scale), (int)(section.h * scale) };
		Engine::GetInstance().render.get()->DrawTexture2(infoBars, &section, &lifeBarDstRect);

		section.x = 14;
		section.y = 32 - (32 * life / MAX_LIFE);
		section.w = 8;
		if (life <= 0) {
			section.h = 0;
		}
		else {
			section.h = 32 * life / MAX_LIFE;
		}

		// Life content
		SDL_Rect lifeContentDstRect = { 24, 24 + (32 * scale) - (section.h * scale), (int)(section.w * scale), (int)(section.h * scale) };
		/*Engine::GetInstance().render.get()->DrawTexture(infoBars, 15 + 3, 15 + 3, &section);*/
		Engine::GetInstance().render.get()->DrawTexture2(infoBars, &section, &lifeContentDstRect);

		section.x = 22;
		section.y = 0;
		section.w = 14;
		section.h = 76;
		// Ammo bar
		SDL_Rect ammoBarDstRect = { 65, 15, (int)(section.w * scale), (int)(section.h * scale) };
		//Engine::GetInstance().render.get()->DrawTexture(infoBars, 30, 15, &section);
		Engine::GetInstance().render.get()->DrawTexture2(infoBars, &section, &ammoBarDstRect);

		section.x = 36;
		section.y = 56 - (56 * ammo / MAX_AMMO);
		section.w = 8;
		if (life <= 0) {
			section.h = 0;
		}
		else {
			section.h = 56 * ammo / MAX_AMMO;
		}

		// Ammo content
		SDL_Rect ammoContentDstRect = { 74,  24 + (56 * scale) - (section.h * scale), (int)(section.w * scale), (int)(section.h * scale) };
		//Engine::GetInstance().render.get()->DrawTexture(infoBars, 30 + 3, 30 + 3, &section);
		Engine::GetInstance().render.get()->DrawTexture2(infoBars, &section, &ammoContentDstRect);
	}



	// Mega Man
	if (isVisible) {
		Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());
	}

	
	printf("X: %f\n", position.getX());
	printf("Y: %f\n", position.getY());
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
	case ColliderType::AMMO_CHARGE:
		LOG("Collision AMMO CHARGE");
		Engine::GetInstance().audio.get()->PlayFx(ammoCollectedFx);
		ammo += 2;
		Engine::GetInstance().scene.get()->scorePoints += 50;
		break;
	case ColliderType::LIFE_CHARGE:
		LOG("Collision AMMO CHARGE");

		Engine::GetInstance().audio.get()->PlayFx(lifeCollectedFx);
		life += 4;
		Engine::GetInstance().scene.get()->scorePoints += 50;
		break;
	case ColliderType::POINTS_CONTAINER:
		LOG("Collision POINTS CONTAINER");

		Engine::GetInstance().audio.get()->PlayFx(pointCollectedFx);
		Engine::GetInstance().scene.get()->scorePoints += 100;
		break;
	case ColliderType::ENEMY:
		LOG("Collision ENEMY");
		// If the player attacks the enemy is deleted 
		if (attack) {
			Engine::GetInstance().scene.get()->scorePoints += 300;
			Engine::GetInstance().audio.get()->PlayFx(hitFx);

		}
		// When godmode is activated player is immortal
		else if (godmode) {
			dead = false;
		}
		else if (life >= 0 && !hit){
			hit = true;
			life -= 4;
			Engine::GetInstance().audio.get()->PlayFx(hurtFx);
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
	//case ColliderType::ITEM:
	//	LOG("End Collision ITEM");
	//	//Engine::GetInstance().audio.get()->PlayFx(pickCoinFxId);
	//	break;
	case ColliderType::ENEMY:
		LOG("End Collision ENEMY");
		if (attack) {

		}
		// Hurt
		else if (!godmode) {
			Engine::GetInstance().audio.get()->PlayFx(hurtFx);
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

void Player::MoveToNextCheckpoint() {
	
	float diff = 0.0f;
	int indexMin = 0;
	float minDiff = 1000000000.0f;
	for (int i = 0; i < 6; ++i) {
		diff = checkpoints[i].getX() - (position.getX()+ texW/2) ;
		if (diff > (texW / 2) && diff < minDiff) {
			minDiff = diff;
			indexMin = i;
		}
		else if (diff <= 0 && i == 5) {
			pbody->body->SetTransform(b2Vec2(PIXEL_TO_METERS(checkpoints[0].getX()), PIXEL_TO_METERS(checkpoints[0].getY())), 0);
		}
	}

	pbody->body->SetTransform(b2Vec2(PIXEL_TO_METERS(checkpoints[indexMin].getX()), PIXEL_TO_METERS(checkpoints[indexMin].getY())), 0);
}
