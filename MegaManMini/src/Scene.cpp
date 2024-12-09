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
#include "MetallC15.h"
#include "BattonBone.h"

Scene::Scene() : Module()
{
	name = "scene";

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
	//for (pugi::xml_node itemNode = configParameters.child("entities").child("items").child("item"); itemNode; itemNode = itemNode.next_sibling("item"))
	//{
	//	Item* item = (Item*)Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM);
	//	item->SetParameters(itemNode);
	//}

	// Create a enemy using the entity manager 
	for (pugi::xml_node enemyNode = configParameters.child("entities").child("enemies").child("enemy"); enemyNode; enemyNode = enemyNode.next_sibling("enemy"))
	{
		// MetallC15 enemy creation
		if (std::string(enemyNode.attribute("name").as_string()) == "MetallC15") {

			MetallC15* metallC15 = (MetallC15*)Engine::GetInstance().entityManager->CreateEntity(EntityType::METALLC15);
			metallC15->SetParameters(enemyNode);
			metallC15List.push_back(metallC15);

		}
		
		// BattonBone enemy creation
		if (std::string(enemyNode.attribute("name").as_string()) == "BattonBone") {

			BattonBone* battonBone = (BattonBone*)Engine::GetInstance().entityManager->CreateEntity(EntityType::BATTONBONE);
			battonBone->SetParameters(enemyNode);
			battonBoneList.push_back(battonBone);

		}

	}

	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	//L06 TODO 3: Call the function to load the map. 
	Engine::GetInstance().map->Load(configParameters.child("map").attribute("path").as_string(), configParameters.child("map").attribute("name").as_string());

	// Initial Respawn activated
	initialRespawn = true;

	// Help menu initialization
	helpMenu = Engine::GetInstance().textures.get()->Load("Assets/Textures/HelpMenu.png");
	help = false;

	// Finished level detection desactivated
	finishLevel = false;

	// Music played
	Engine::GetInstance().audio.get()->PlayMusic("Assets/Audio/Music/level1Music.wav", 0.0f);

	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{
	// Respawns always on the initial point after the first step
	if (initialRespawn) {
		Engine::GetInstance().scene.get()->player->Respawn();
		initialRespawn = false;
	}

	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{
	//L03 TODO 3: Make the camera movement independent of framerate
	float camSpeed = 1;


	// Camera movement with limitations
	if (Engine::GetInstance().map.get()->WorldToMap(player->position.getX(), player->position.getY()).getX() < 20) {

		Engine::GetInstance().render.get()->camera.x = 0;

	}
	else if (Engine::GetInstance().map.get()->WorldToMap(player->position.getX(), player->position.getY()).getX() >= 20
		&& Engine::GetInstance().map.get()->WorldToMap(player->position.getX(), player->position.getY()).getX() <= 179) {

		Engine::GetInstance().render.get()->camera.x = (Engine::GetInstance().window->width / 2 - player->position.getX());

	}



	// Help Menu
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_H) == KEY_DOWN) {
		help = !help;

	}
	if (help) {

		int width, height;
		Engine::GetInstance().textures->GetSize(helpMenu, width, height);
		SDL_Rect dstRect = { 0, 0, width, height };

		// Crea la forma del rectangulo donde estara la textura
		SDL_RenderCopy(Engine::GetInstance().render->renderer, helpMenu, nullptr, &dstRect);


	}

	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;

	// F1 to respawn at the beginning of the first level
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		Engine::GetInstance().scene.get()->player->Respawn();
	
	// F3 to respawn at the beginning of the current level
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)
		Engine::GetInstance().scene.get()->player->Respawn();

	// ESC or finished the level to close the window
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN || finishLevel)
		ret = false;

	// F6 to Load
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
		LoadState();

	// F5 to Save
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		SaveState();

	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");


	Engine::GetInstance().textures->UnLoad(helpMenu);


	return true;
}

// Return the player position
Vector2D Scene::GetPlayerPosition()
{
	return player->GetPosition();
}

void Scene::LoadState() {

	pugi::xml_document loadFile;
	pugi::xml_parse_result result = loadFile.load_file("config.xml");

	if (result == NULL)
	{
		LOG("Could not load file. Pugi error: %s", result.description());
		return;
	}

	pugi::xml_node sceneNode = loadFile.child("config").child("scene");

	//Read XML and restore information

	//Player position
	Vector2D playerPos = Vector2D(sceneNode.child("entities").child("player").attribute("x").as_int(),
		sceneNode.child("entities").child("player").attribute("y").as_int());
	player->SetPosition(playerPos);

	//enemies

	//int i = 0;
	//int j = 0;

	//for (pugi::xml_node enemyNode = configParameters.child("entities").child("enemies").child("enemy"); enemyNode; enemyNode = enemyNode.next_sibling("enemy"))
	//{

	//	if (std::string(enemyNode.attribute("name").as_string()) == "MetallC15" && enemyNode.attribute("dead").as_bool() == false) {

	//		Vector2D metallC15Pos = Vector2D(enemyNode.attribute("x").as_int(), enemyNode.attribute("y").as_int());
	//		metallC15List[i]->SetPosition(metallC15Pos);
	//		i++;


	//	}

	//	if (std::string(enemyNode.attribute("name").as_string()) == "BattonBone" && enemyNode.attribute("dead").as_bool() == false) {

	//		Vector2D battonBonePos = Vector2D(enemyNode.attribute("x").as_int(), enemyNode.attribute("y").as_int());
	//		battonBoneList[j]->SetPosition(battonBonePos);
	//		j++;

	//	}

	//	if (i < metallC15List.size()) i = 0;
	//	else if (j < battonBoneList.size()) j = 0;

	//}


}

void Scene::SaveState() {

	pugi::xml_document loadFile;
	pugi::xml_parse_result result = loadFile.load_file("config.xml");

	if (result == NULL)
	{
		LOG("Could not load file. Pugi error: %s", result.description());
		return;
	}

	pugi::xml_node sceneNode = loadFile.child("config").child("scene");

	//Save info to XML 

	//Player position
	sceneNode.child("entities").child("player").attribute("x").set_value(player->GetPosition().getX());
	sceneNode.child("entities").child("player").attribute("y").set_value(player->GetPosition().getY() - 10);

	//enemies

	//int i = 0;
	//int j = 0;

	//for (pugi::xml_node enemyNode = configParameters.child("entities").child("enemies").child("enemy"); enemyNode; enemyNode = enemyNode.next_sibling("enemy"))
	//{

	//	if (std::string(enemyNode.attribute("name").as_string()) == "MetallC15" && metallC15List[i]->deleted == true) {

	//		sceneNode.child("entities").child("enemies").child("MetallC15").attribute("dead").set_value(true);
	//		i++;


	//	}

	//	if (std::string(enemyNode.attribute("name").as_string()) == "BattonBone" && battonBoneList[i]->deleted == true) {

	//		sceneNode.child("entities").child("enemies").child("BattonBone").attribute("dead").set_value(true);
	//		j++;

	//	}

	//	if (i < metallC15List.size()) i = 0;
	//	else if (j < battonBoneList.size()) j = 0;

	//}

	//Saves the modifications to the XML 
	loadFile.save_file("config.xml");
}
