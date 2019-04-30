#include "Components\MeshRenderer.h"
#include "Prefabs\Axis.h"

#include "Core\MainCamera.h"
#include "MainScene.h"
#include "Core/Logger.h"
#include "Scene/SceneManager.h"

#include "Core\Window.h"
#include "Utils\AssetLoader.h"
#include "Lighting\LightingManager.h"
#include "Prefabs\Water.h"
#include "Prefabs\Terrain.h"
#include "Graphics\CubeMap.h"
#include "Graphics\SkyBox.h"
#include "Utils\GameAssetFactory.h"
#include "Core\Lua.h"
#include "GUI\GUIElements\GUIText.h"
#include "GUI\GUIElements\GUIImage.h"
#include "Components\Animator.h"

#include "GUI\GUIElements\GUIManager.h"
#include "Components\BoxCollider.h"
#include "Components\SphereCollider.h"

#include "Components\RigidBody.h"

#include "Physics\PhysicsWorld.h"

MainCamera* cam;
int luaAssetOffset = 0;
GameObject** nanosuits;
GameObject** pumpkins;
GameObject** barrels;
GameObject** crates;
GameObject* gun;
GameObject* ship;
GameObject* cabin;
PointLight* pLight;
DirectionalLight* dirLight;
Terrain* terrain;
btDefaultMotionState* motionstate;
btRigidBody *rigidBody;

GameObject* c1;
GameObject* c2;


MainScene::MainScene() : Scene("MainScene")
{

}

void MainScene::LoadAssets() {

	AssetLoader::Instance().LoadModel("Assets\\Models\\Nanosuit\\nanosuit.obj");
	AssetLoader::Instance().LoadModel("Assets\\Models\\Pumpkin\\pumpkin.obj");
	AssetLoader::Instance().LoadModel("Assets\\Models\\Barrel\\barrel.obj");
	AssetLoader::Instance().LoadModel("Assets\\Models\\Crate\\crate.obj");
	AssetLoader::Instance().LoadModel("Assets\\Models\\Gun\\gun.obj");
	AssetLoader::Instance().LoadModel("Assets\\Models\\Ship\\ship.obj");
	AssetLoader::Instance().LoadModel("Assets\\Models\\Cabin\\cabin.fbx");	

	AssetLoader::Instance().LoadTexture("Assets\\Textures\\manual.png");
	//AssetLoader::Instance().LoadModel("Assets\\Models\\Wolf\\wolf.fbx");
	AssetLoader::Instance().LoadModel("Assets\\Models\\Spider\\spider_3.fbx",0);

	AssetLoader::Instance().LoadTexture("Assets\\Models\\Spider\\textures\\Spinnen_Bein_tex_COLOR_.jpg");

	AssetLoader::Instance().LoadTexture("Assets\\Textures\\wood.jpg");
	AssetLoader::Instance().LoadTexture("Assets\\Textures\\crate_diffuse.tga");
	AssetLoader::Instance().LoadTexture("Assets\\Textures\\crate_normal.tga");
	AssetLoader::Instance().LoadTexture("Assets\\Textures\\crate_specular.tga");
	AssetLoader::Instance().LoadTexture("Assets\\Textures\\shipTexture.png");
	AssetLoader::Instance().LoadTexture("Assets\\Textures\\cabin_diffuse.png");
	AssetLoader::Instance().LoadTexture("Assets\\Textures\\cabin_normal.png");	
}
void MainScene::UnloadAssets() {
	AssetLoader::Instance().Unload<Model>();
	AssetLoader::Instance().Unload<Texture2D>();

}
void MainScene::QuitScene() {
	Logger::LogError("Scene asset clean up");

	Scene::QuitScene();

}
void MainScene::Initialize() {

	Lua::RunLua("Assets\\Scripts\\Level1.lua");
	gContactAddedCallback = PhysicsWorld::CollisionCallback;
	skybox = new Skybox(AssetLoader::Instance().GetAsset<CubeMap>("SunSet"));

	Timer::SetDisplayFPS(true);
	
	//nanosuit = (GameObject*)GameAssetFactory::Instance().Create("Model", "Nanosuit");
	//GameObject* n2 = (GameObject*)GameAssetFactory::Instance().Create("Model", "Cabin");
	manual = new GUIImage("manualImage",AssetLoader::Instance().GetAsset<Texture2D>("manual"), 10, 10, 80, 80, 1);
	manual->isActive = 0;
	GUIManager::Instance().AddGUIObject(manual);

	

	//Lights
	LightManager::Instance().SetAmbientLight(0.5f, 0.5f, 0.2f);

	dirLight = new DirectionalLight();
	dirLight->SetDiffuseColor(1, 1, 1);
	dirLight->transform.SetRotation(45, 117, 0);
	dirLight->SetIntensity(0.9f);
	dirLight->SetDiffuseColor(1.0, 1.0, 0.8);

	DirectionalLight* dirLight2 = new DirectionalLight(false);
	dirLight2->SetDiffuseColor(1, 1, 1);
	dirLight2->transform.SetRotation(90, -120, 0);
	dirLight2->SetIntensity(0.5f);

	pLight = new PointLight();
	pLight->SetDiffuseColor(1, 1, 1);
	pLight->transform.Translate(-15, 10, -15);
	pLight->SetIntensity(50.0f);


	// Uncomment this to force a wood material!
	Material mat_wood;
	mat_wood.SetShader(AssetLoader::Instance().GetAsset<Shader>("DefaultStatic"));

	mat_wood.Loadtexture(AssetLoader::Instance().GetAsset<Texture2D>("wood"));
	mat_wood.LoadCubemap(&skybox->GetCubeMap());

	mat_wood.LoadFloat("shininess", 1000.0f);
	mat_wood.LoadFloat("reflectivness", 1.0);

	Material mat_crate;
	mat_crate.SetShader(AssetLoader::Instance().GetAsset<Shader>("DefaultStaticNormalMap"));
	mat_crate.Loadtexture(AssetLoader::Instance().GetAsset<Texture2D>("crate_diffuse"));
	mat_crate.Loadtexture(AssetLoader::Instance().GetAsset<Texture2D>("crate_normal"), TextureUniform::NORMAL0);


	Material mat_ship;
	mat_ship.SetShader(AssetLoader::Instance().GetAsset<Shader>("DefaultStatic"));
	mat_ship.Loadtexture(AssetLoader::Instance().GetAsset<Texture2D>("shipTexture"));
	mat_ship.LoadCubemap(&skybox->GetCubeMap());

	mat_ship.LoadFloat("shininess", 1000.0f);
	mat_ship.LoadFloat("reflectivness", 0.5f);

	Material mat_cabin;
	mat_cabin.SetShader(AssetLoader::Instance().GetAsset<Shader>("DefaultStaticNormalMap"));
	mat_cabin.Loadtexture(AssetLoader::Instance().GetAsset<Texture2D>("cabin_diffuse"));
	mat_cabin.Loadtexture(AssetLoader::Instance().GetAsset<Texture2D>("cabin_normal"),TextureUniform::NORMAL0);


	//Terrain
	terrain = new Terrain(256);
	terrain->ApplyHeightMap("Assets\\Textures\\hm1.jpg");

	terrain->transform.SetScale(20 ,600, 20);
	terrain->transform.Translate(0, 0, 0);

	//GameObjects
	cam = (MainCamera*)Lua::GetCreatedAsset(0);
	luaAssetOffset++;
	cam->transform.SetPosition(Lua::GetFloatFromStack("camX"), Lua::GetFloatFromStack("camY"), Lua::GetFloatFromStack("camZ"));
	cam->transform.SetRotation(Lua::GetFloatFromStack("camRotX"), Lua::GetFloatFromStack("camRotY"), Lua::GetFloatFromStack("camRotZ"));
	cam->SetMovementSpeed(500);
	cam->RemoveLayerMask(Layers::GUI);


	Water* w = (Water*)Lua::GetCreatedAsset(1);
	luaAssetOffset++;
	w->meshRenderer->GetMaterial().LoadCubemap(&skybox->GetCubeMap());
	nanosuits = new GameObject*[Lua::GetIntFromStack("npc_nanosuits")];
	for (int i = 0; i < Lua::GetIntFromStack("npc_nanosuits"); i++)
	{
		nanosuits[i] = (GameObject*)Lua::GetCreatedAsset(i + luaAssetOffset);
		AddGameObject(nanosuits[i]);
		nanosuits[i]->transform.SetScale(Lua::GetFloatFromStack("nanosuitScale"), Lua::GetFloatFromStack("nanosuitScale"), Lua::GetFloatFromStack("nanosuitScale"));
		float posX = Lua::GetFloatFromStack("nanosuit" + std::to_string(i + 1) + "X");
		float posY = Lua::GetFloatFromStack("nanosuit" + std::to_string(i + 1) + "Y");
		float posZ = Lua::GetFloatFromStack("nanosuit" + std::to_string(i + 1) + "Z");
		nanosuits[i]->transform.SetPosition(posX, terrain->GetHeightAt(posX, posZ) + posY, posZ);
	}
	luaAssetOffset += Lua::GetIntFromStack("npc_nanosuits");



	pumpkins = new GameObject*[Lua::GetIntFromStack("npc_pumpkins")];
	for (int i = 0; i < Lua::GetIntFromStack("npc_pumpkins"); i++)
	{
		pumpkins[i] = (GameObject*)Lua::GetCreatedAsset(i + luaAssetOffset);
		AddGameObject(pumpkins[i]);
		pumpkins[i]->transform.SetScale(Lua::GetFloatFromStack("pumpkinScale"), Lua::GetFloatFromStack("pumpkinScale"), Lua::GetFloatFromStack("pumpkinScale"));
		float posX = Lua::GetFloatFromStack("pumpkin" + std::to_string(i + 1) + "X");
		float posY = Lua::GetFloatFromStack("pumpkin" + std::to_string(i + 1) + "Y");
		float posZ = Lua::GetFloatFromStack("pumpkin" + std::to_string(i + 1) + "Z");
		pumpkins[i]->transform.SetPosition(posX, terrain->GetHeightAt(posX, posZ) + posY, posZ);
	}
	luaAssetOffset += Lua::GetIntFromStack("npc_pumpkins");

	barrels = new GameObject*[Lua::GetIntFromStack("prop_barrels")];
	for (int i = 0; i < Lua::GetIntFromStack("prop_barrels"); i++)
	{
		barrels[i] = (GameObject*)Lua::GetCreatedAsset(i + luaAssetOffset);
		AddGameObject(barrels[i]);
		barrels[i]->transform.SetScale(Lua::GetFloatFromStack("barrelScale"), Lua::GetFloatFromStack("barrelScale"), Lua::GetFloatFromStack("barrelScale"));
		float posX = Lua::GetFloatFromStack("barrel" + std::to_string(i + 1) + "X");
		float posY = Lua::GetFloatFromStack("barrel" + std::to_string(i + 1) + "Y");
		float posZ = Lua::GetFloatFromStack("barrel" + std::to_string(i + 1) + "Z");
		barrels[i]->transform.SetPosition(posX, terrain->GetHeightAt(posX, posZ) + posY, posZ);
	}
	luaAssetOffset += Lua::GetIntFromStack("prop_barrels");

	crates = new GameObject*[Lua::GetIntFromStack("prop_crates")];
	for (int i = 0; i < Lua::GetIntFromStack("prop_crates"); i++)
	{
		crates[i] = (GameObject*)Lua::GetCreatedAsset(i + luaAssetOffset);
		crates[i]->ApplyMaterial(mat_crate);
		AddGameObject(crates[i]);
		crates[i]->transform.SetScale(Lua::GetFloatFromStack("crateScale"), Lua::GetFloatFromStack("crateScale"), Lua::GetFloatFromStack("crateScale"));
		float posX = Lua::GetFloatFromStack("crate" + std::to_string(i + 1) + "X");
		float posY = Lua::GetFloatFromStack("crate" + std::to_string(i + 1) + "Y");
		float posZ = Lua::GetFloatFromStack("crate" + std::to_string(i + 1) + "Z");
		crates[i]->transform.SetPosition(posX, terrain->GetHeightAt(posX, posZ) + posY, posZ);
	}
	luaAssetOffset += Lua::GetIntFromStack("prop_crates");

	gun = (GameObject*)Lua::GetCreatedAsset(luaAssetOffset);
	AddGameObject(gun);
	gun->transform.SetScale(Lua::GetFloatFromStack("gunScale"), Lua::GetFloatFromStack("gunScale"), Lua::GetFloatFromStack("gunScale"));
	gun->transform.SetPosition(Lua::GetFloatFromStack("gunX"), terrain->GetHeightAt(Lua::GetFloatFromStack("gunX"), Lua::GetFloatFromStack("gunZ")) + Lua::GetFloatFromStack("gunY"), Lua::GetFloatFromStack("gunZ"));
	gun->transform.SetRotation(0, 0, 90);
	luaAssetOffset++;

	ship = (GameObject*)Lua::GetCreatedAsset(luaAssetOffset);
	ship->ApplyMaterial(mat_ship);
	AddGameObject(ship);
	ship->transform.SetScale(Lua::GetFloatFromStack("shipScale"), Lua::GetFloatFromStack("shipScale"), Lua::GetFloatFromStack("shipScale"));
	ship->transform.SetPosition(Lua::GetFloatFromStack("shipX"), terrain->GetHeightAt(Lua::GetFloatFromStack("shipX"), Lua::GetFloatFromStack("shipZ")) + Lua::GetFloatFromStack("shipY"), Lua::GetFloatFromStack("shipZ"));
	luaAssetOffset++;

	cabin = (GameObject*)Lua::GetCreatedAsset(luaAssetOffset);
	cabin->ApplyMaterial(mat_cabin);
	AddGameObject(cabin);
	cabin->transform.SetScale(Lua::GetFloatFromStack("cabinScale"), Lua::GetFloatFromStack("cabinScale"), Lua::GetFloatFromStack("cabinScale"));
	cabin->transform.SetPosition(Lua::GetFloatFromStack("cabinX"), terrain->GetHeightAt(Lua::GetFloatFromStack("cabinX"), Lua::GetFloatFromStack("cabinZ")) + Lua::GetFloatFromStack("cabinY"), Lua::GetFloatFromStack("cabinZ"));
	cabin->transform.SetRotation(-90, 0, 0);
	luaAssetOffset++;
	int x, y, z;
	terrain->GetCenter(x, y, z);
	cam->transform.SetPosition(x, y, z);
	PhysicsWorld::Instance().InitializeQuadtree(x, z, terrain->GetTerrainMaxX() - terrain->GetTerrainMinX(), terrain->GetTerrainMaxZ() - terrain->GetTerrainMinZ());
	
	/*GameObject* woof = AssetLoader::Instance().GetAsset<Model>("Wolf")->CreateGameObject();
	woof->transform.SetPosition(cam->transform.GetPosition().x + 80, 240, cam->transform.GetPosition().z + 200);
	AddGameObject(woof);*/

	Material spiderMat;
	spiderMat.Loadtexture(AssetLoader::Instance().GetAsset<Texture2D>("Spinnen_Bein_tex_COLOR_"));
	spiderMat.SetShader(AssetLoader::Instance().GetAsset<Shader>("DefaultAnimated"));

	GameObject* woof = AssetLoader::Instance().GetAsset<Model>("Spider")->CreateGameObject();
	woof->transform.SetPosition(cam->transform.GetPosition().x + 80, 240, cam->transform.GetPosition().z + 200);
	woof->GetComponent<Animator>("Animator")->SetCurrentAnimation(0);
	woof->ApplyMaterial(spiderMat);
	woof->PrintHierarchy();
	
	AddGameObject(woof);

	c1 = AssetLoader::Instance().GetAsset<Model>("Crate")->CreateGameObject();
	c1->transform.SetPosition(cam->transform.GetPosition().x+20,400, cam->transform.GetPosition().z + 400);

	c1->transform.SetScale(3, 3, 3);
	c1->transform.RotateBy(-45.0f,0,1,0);

	c1->AddComponent(new BoxCollider());
	c1->GetComponent<BoxCollider>("BoxCollider")->transform.SetScale(12, 12, 12);
	c1->GetComponent<BoxCollider>("BoxCollider")->transform.SetPosition(0, 7, 0);
	c1->GetComponent<BoxCollider>("BoxCollider")->collisionCallback = [](GameObject* go){
		//Logger::LogInfo("C1 colliding");
	};

	c2 = AssetLoader::Instance().GetAsset<Model>("Crate")->CreateGameObject();
	c2->transform.SetPosition(cam->transform.GetPosition().x+20 , 400, cam->transform.GetPosition().z + 200);
	c2->transform.SetScale(3,3,3);
	c2->AddComponent(new BoxCollider());
	c2->GetComponent<BoxCollider>("BoxCollider")->transform.SetScale(12, 12, 12);
	c2->GetComponent<BoxCollider>("BoxCollider")->transform.SetPosition(0, 7, 0);


	w->transform.SetPosition(x, 50, z);
	w->transform.SetScale(3000, 3000, 1);
	AddGameObject(c2);
	AddGameObject(c1);
	AddGameObject(w);


	AddGameObject(dirLight);
	AddGameObject(dirLight2);
	AddGameObject(pLight);
	AddGameObject(terrain);
	AddGameObject(terrain);
	AddGameObject(cam);



	Lua::CloseLua();

	cam->transform.SetRotation(0, 0, 0);		
	cam->transform.Update();


}
void MainScene::LogicUpdate() {
	PhysicsWorld::Instance().FillQuadtree();
	//c1->GetComponent<RigidBody>("RigidBody")->btrb->translate(btVector3(0, -1, 0));
	/*glm::vec3 toCam = glm::vec3(cam->transform.GetPosition().x, nanosuit->transform.GetPosition().y, cam->transform.GetPosition().z) - nanosuit->transform.GetPosition();
	float yAngle = glm::degrees(glm::angle(nanosuit->transform.GetLocalFront(),glm::normalize(toCam)));
	glm::vec3 cross = glm::normalize(glm::cross(nanosuit->transform.GetLocalFront(), glm::normalize(toCam)));
	int s = glm::sign(cross.y);

	
	glm::vec3 np = nanosuit->transform.GetPosition();
	np += nanosuit->transform.GetLocalFront() * 0.5f;
	float y = terrain->GetHeightAt(np.x, np.z);
	nanosuit->transform.SetPosition(np.x, y, np.z);*/

	PhysicsWorld::Instance().Update(Timer::GetDeltaS());
	//Logger::LogInfo("GameObj at camera", PhysicsWorld::Instance().quadtree->GameObjectInQuadrant(cam->transform.GetGlobalPosition().x, cam->transform.GetGlobalPosition().z));

	//c1->transform.RotateBy(0.5f,0,1,0);
	c1->transform.Translate(0, 0, -0.2f);
//	Logger::LogInfo("C1", c1->transform.RotationQuatToString());
//	Logger::LogInfo("C2", c2->transform.RotationQuatToString());


	//c1->transform.SetPosition(c1->transform.GetGlobalPosition() + c1->transform.GetLocalFront() * 0.2f);



	
	if (!cam->IsTopView())
	{
		float h = terrain->GetHeightAt(cam->transform.GetPosition().x, cam->transform.GetPosition().z);
		cam->transform.SetPosition(cam->transform.GetPosition().x, h + 30, cam->transform.GetPosition().z);

		// Limit camera position within terrain
		if (cam->transform.GetPosition().x > terrain->GetTerrainMaxX() - 50)
			cam->transform.SetPosition(terrain->GetTerrainMaxX() - 50, cam->transform.GetPosition().y, cam->transform.GetPosition().z);
		else if (cam->transform.GetPosition().x < terrain->GetTerrainMinX() + 50)
			cam->transform.SetPosition(terrain->GetTerrainMinX() + 50, cam->transform.GetPosition().y, cam->transform.GetPosition().z);

		if (cam->transform.GetPosition().z > terrain->GetTerrainMaxZ() - 50)
			cam->transform.SetPosition(cam->transform.GetPosition().x, cam->transform.GetPosition().y, terrain->GetTerrainMaxZ() - 50);
		if (cam->transform.GetPosition().z < terrain->GetTerrainMinZ() + 50)
			cam->transform.SetPosition(cam->transform.GetPosition().x, cam->transform.GetPosition().y, terrain->GetTerrainMinZ() + 50);
	}

	else
	{
		int x, y, z;
		terrain->GetCenter(x, y, z);
		cam->transform.LookAt(x, y, z);
	}	

	if (Input::GetKeyPressed(GLFW_KEY_M))
		manual->isActive = !manual->isActive;


	if (Input::GetKeyPressed(GLFW_KEY_ESCAPE) || Input::GetKeyPressed(GLFW_KEY_X))
		SceneManager::Instance().LoadNewScene("ExitScene");


	Scene::LogicUpdate(); //Must be last statement!
}

