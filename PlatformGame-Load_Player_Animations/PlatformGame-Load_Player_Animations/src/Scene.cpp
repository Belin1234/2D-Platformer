#include "Engine.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "Log.h"
#include "Entity.h"
#include "EntityManager.h"
#include "Player.h"
#include "Map.h"
#include "Item.h"

Scene::Scene() : Module()
{
	name = "scene";
	img = nullptr;
	
}

// Destructor
Scene::~Scene()
{}

// Called before render is available
bool Scene::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	//L04: TODO 3b: Instantiate the player using the entity manager
	player = (Player*)Engine::GetInstance().entityManager->CreateEntity(EntityType::PLAYER);
	player->SetParameters(configParameters.child("entities").child("player"));
	
	//L08 Create a new item using the entity manager and set the position to (200, 672) to test
	Item* item = (Item*) Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM);
	item->position = Vector2D(200, 672);

	

	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	//L06 TODO 3: Call the function to load the map. 
	Engine::GetInstance().map->Load(configParameters.child("map").attribute("path").as_string(), configParameters.child("map").attribute("name").as_string());
	helpMenu = Engine::GetInstance().textures.get()->Load("Assets/Textures/HelpMenu.png");
	help = false;

	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{
	//L03 TODO 3: Make the 
	// era movement independent of framerate
	float camSpeed = 1;

	/*if(Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		Engine::GetInstance().render.get()->camera.y -= ceil(camSpeed * dt);

	if(Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		Engine::GetInstance().render.get()->camera.y += ceil(camSpeed * dt);

	if(Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		Engine::GetInstance().render.get()->camera.x -= ceil(camSpeed * dt);

	if(Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		Engine::GetInstance().render.get()->camera.x += ceil(camSpeed * dt);*/
	
	
	Engine::GetInstance().render.get()->camera.x = (Engine::GetInstance().window->width/2 - player->position.getX());
	
	
		//int width, height;
		//Engine::GetInstance().textures->GetSize(helpMenuTexture, width, height);
		//int windowWidth, windowHeight;
		//Engine::GetInstance().window->GetWindowSize(windowWidth, windowHeight);

		////Imagen en la esquina superior derecha
		//SDL_Rect dstRect = { windowWidth - width - 10, 10, width, height };
		////Imagen en el centro de la pantalla
		////SDL_Rect dstRect = { (windowWidth - width) / 2, (windowHeight - height) / 2, width, height };

		

	/*DrawText("Hola, raylib!", 190, 200, 20, DARKGRAY);*/
	/////////////////////////////////////////////////////////////////////
	// Peta por el &destRect pero no se como pasarle los parametros
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_H) == KEY_DOWN) {
		help = !help;

	}
	
	if (help) {
	
		int width, height;
		Engine::GetInstance().textures->GetSize(helpMenu, width, height);
		SDL_Rect dstRect = { 0, 0, width, height };

		SDL_RenderCopy(Engine::GetInstance().render->renderer, helpMenu, nullptr, &dstRect);
		//// Crea la forma del rectangulo donde estara la textura
		//SDL_Rect destRect = {
		//	(Engine::GetInstance().render.get()->camera.x),  // Posición x
		//	(Engine::GetInstance().render.get()->camera.y),  // Posición y
		//	(Engine::GetInstance().window.get()->width),     // Ancho
		//	(Engine::GetInstance().window.get()->height)     // Alto
		//};

		//Engine::GetInstance().render.get()->DrawTexture(helpMenu,0,0, &destRect, 0.0f, 0.0, 1,1);
	}
	//////////////////////////////////////////////////////////////////
	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;

	if(Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");

	
	Engine::GetInstance().textures->UnLoad(helpMenu);
	

	SDL_DestroyTexture(img);

	return true;
}
