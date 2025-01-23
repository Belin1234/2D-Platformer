#include "Bullet.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"
#include "EntityManager.h"


//Bullet::Bullet() : Entity(EntityType::BULLET)
//{
//	name = "bullet";
//
//	//initilize textures
//	texture = Engine::GetInstance().textures.get()->Load("Assets/Textures/Bullet.png");
//
//	texW = 12;
//	texH = 15;
//	//Load animations
//	//idle.LoadAnimations(parameters.child("animations").child("idle"));
//	//hit.LoadAnimations(parameters.child("animations").child("hit"));
//	//currentAnimation = &idle;
//	pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX(), (int)position.getY(), 3, bodyType::DYNAMIC);
//
//	// L08 TODO 7: Assign collider type
//	pbody->ctype = ColliderType::BULLET;
//
//	// Set the gravity of the body
//	/*if (!parameters.attribute("gravity").as_bool()) */
//	pbody->body->SetGravityScale(0);
//
//}
//
//Bullet::~Bullet() {}
//
//bool Bullet::Awake() {
//
//
//	return true;
//}
//
//bool Bullet::Start() {
//
//
//
//
//	return true;
//}
//
//bool Bullet::Update(float dt)
//{
//	// L08 TODO 4: Add a physics to an item - update the position of the object from the physics.  
//	b2Vec2 vel(speedX, speedY);
//	pbody->body->SetLinearVelocity(vel);
//
//	b2Transform pbodyPos = pbody->body->GetTransform();
//	position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
//	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);
//
//	SDL_Rect section;
//	section.x = 0;
//	section.y = 0;
//	section.w = texW;
//	section.h = texH;
//
//	Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &section);
//	//currentAnimation->Update();
//
//	return true;
//}
//
//bool Bullet::CleanUp()
//{
//
//	Engine::GetInstance().textures.get()->UnLoad(texture);
//	Engine::GetInstance().physics->DeletePhysBody(pbody);
//
//
//	return true;
//}
//
//void Bullet::SetPosition(Vector2D pos) {
//	
//	position.setX(pos.getX() + 32);
//	position.setY(pos.getY() + 17);
//	
//	b2Vec2 bulletPos = b2Vec2(PIXEL_TO_METERS(position.getX()), PIXEL_TO_METERS(position.getY()));
//
//	/*const b2Vec2 bulletPos = { pos.getX() + 3.0f, pos.getY() };*/
//
//	pbody->body->SetTransform(bulletPos, 0.0f);
//	//currentAnimation = &idle;
//}
//
//void Bullet::OnCollision(PhysBody* physA, PhysBody* physB) {
//	switch (physB->ctype)
//	{
//	case ColliderType::PLATFORM:
//		LOG("Collision PLATFORM");
//		// Reset the jump bool when touching the ground
//		CleanUp();
//		break;
//		//	case ColliderType::ITEM:
//		//		LOG("Collision ITEM");
//		///*		Engine::GetInstance().audio.get()->PlayFx(pickCoinFxId);
//		//		Engine::GetInstance().physics.get()->DeletePhysBody(physB);*/ // Deletes the body of the item from the physics world
//		//		break;
//	case ColliderType::ENEMY:
//		LOG("Collision ENEMY");
//		// If the player attacks the enemy is deleted 
//		physB->deleted = true;
//		Engine::GetInstance().physics.get()->DeletePhysBody(physB);
//		CleanUp();
//		break;
///*	case ColliderType::CHECKPOINT:
//		LOG("Collision CHECKPOINT");
//		Engine::GetInstance().scene.get()->SaveState();
//		*/break;
//	case ColliderType::UNKNOWN:
//		LOG("Collision UNKNOWN");
//		break;
//	default:
//		break;
//	}
//}
Bullet::Bullet() : Entity(EntityType::BULLET)
{
	name = "bullet";


}

Bullet::~Bullet() {}

bool Bullet::Awake() {


	return true;
}

bool Bullet::Start() {

	//initilize textures
	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());

	position.setX(parameters.attribute("x").as_int());
	position.setY(parameters.attribute("y").as_int());
	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();

	//Load animations
	idle.LoadAnimations(parameters.child("animations").child("idle"));
	idleL.LoadAnimations(parameters.child("animations").child("idleL"));
	hit.LoadAnimations(parameters.child("animations").child("hit"));
	hitL.LoadAnimations(parameters.child("animations").child("hitL"));
	currentAnimation = &idle;
	
	pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX(), (int)position.getY(), texW, bodyType::DYNAMIC);

	// L08 TODO 7: Assign collider type
	pbody->ctype = ColliderType::BULLET;

	// Set the gravity of the body
	if (!parameters.attribute("gravity").as_bool()) pbody->body->SetGravityScale(0);

	pbody->listener = this;

	hitFx = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/HitFx.wav");

	speedX = 0.0f;
	speedY = 0.0f;

	isHitting = false;

	return true;
}

bool Bullet::Update(float dt)
{
	//if (pbody->deleted) {
	//	return true;
	//}
	//// L08 TODO 4: Add a physics to an item - update the position of the object from the physics.  
	//if (isHitting) {
	//	if (lookRight) {
	//		currentAnimation = &hit;

	//	}
	//	else if (!lookRight) {
	//		currentAnimation = &hitL;

	//	}
	//	
	//	if (hit.HasFinished() || hitL.HasFinished()) {
	//		CleanUp();

	//	}
	//	Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());
	//	currentAnimation->Update();
	//	return true;
	//}
	//else if (!isHitting) {

		if (lookRight) {

			b2Vec2 vel(-speedX, speedY);
			pbody->body->SetLinearVelocity(vel);
			currentAnimation = &idle;
		}
		else {
			b2Vec2 vel(speedX, speedY);
			pbody->body->SetLinearVelocity(vel);
			currentAnimation = &idleL;
		}




		b2Transform pbodyPos = pbody->body->GetTransform();
		position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texW);
		position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH);

		Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());
		currentAnimation->Update();



		return true;
	//}

}

bool Bullet::CleanUp()
{

	Engine::GetInstance().textures.get()->UnLoad(texture);
	Engine::GetInstance().physics->DeletePhysBody(pbody);
	

	return true;
}

void Bullet::SetPosition(Vector2D pos, bool look) {
	if (look) {
		position.setX(pos.getX() + 32);
		position.setY(pos.getY() + 17);
	}
	else {
		position.setX(pos.getX() - 32);
		position.setY(pos.getY() + 17);
	}

	b2Vec2 bodyPos = b2Vec2(PIXEL_TO_METERS(position.getX()), PIXEL_TO_METERS(position.getY()));
	pbody->body->SetTransform(bodyPos, 0);
}

void Bullet::OnCollision(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");
		if (lookRight) {
			currentAnimation = &hit;
		}
		else {
			currentAnimation = &hitL;
		}
		Engine::GetInstance().audio.get()->PlayFx(hitFx);
		// Dibuja y actualiza la animación hasta que termine
		while (!currentAnimation->HasFinished()) {
			Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());
			currentAnimation->Update();

			// Opcional: espera un pequeño intervalo para permitir la animación
			SDL_Delay(16); // Pausa de ~16ms (aproximadamente 60 FPS)
		}
		CleanUp();

		break;
	case ColliderType::ENEMY:
		LOG("Collision ENEMY");
		//Engine::GetInstance().physics.get()->DeletePhysBody(physB);
		if (lookRight) {
			currentAnimation = &hit;
		}
		else {
			currentAnimation = &hitL;
		}
		Engine::GetInstance().audio.get()->PlayFx(hitFx);
		// Dibuja y actualiza la animación hasta que termine
		while (!currentAnimation->HasFinished()) {
			Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());
			currentAnimation->Update();

			// Opcional: espera un pequeño intervalo para permitir la animación
			SDL_Delay(16); // Pausa de ~16ms (aproximadamente 60 FPS)
		}
		CleanUp();

		break;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	default:
		break;
	}
}
