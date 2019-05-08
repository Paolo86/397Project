#include "MainCamera.h"
#include "Timer.h"
#include "..\Prefabs\Terrain.h"

namespace {

	const float MIN_SPEED = 0;
	const float MAX_SPEED = 1000;
	float ORIGINAL_SPEED = 500;
	float counter = 0;
	GranadeLauncher* gn;
}

MainCamera::MainCamera() : CameraPerspective(60.0f, Window::Instance().GetAspectRatio(), 0.1f, 10000.0f)
{
	this->SetName("Main Camera");
	_movementSpeed = 20;
	_rotationSpeed = 20;
	_isTopView = false;
	_intendedDir = glm::vec3(0, 0, 0);
	/*sphereCollider = new SphereCollider();
	AddComponent(sphereCollider);
	sphereCollider->transform.SetScale(10, 10, 10);
	sphereCollider->meshRenderer->SetIsCullable(0);*/

	boxCollider = new BoxCollider();
	AddComponent(boxCollider); // Needs to be added first and modified later. I know, messy
	boxCollider->transform.SetScale(10, 60, 10);
	boxCollider->meshRenderer->SetIsCullable(0);
	boxCollider->ResetCollisionLayer();
	boxCollider->AddCollisionLayer(CollisionLayers::PLAYER);
	boxCollider->AddCollideAgainstLayer(CollisionLayers::OBSTACLE);
	boxCollider->AddCollideAgainstLayer(CollisionLayers::ENEMY);

	//boxCollider->enableRender = 1;
	boxCollider->transform.parent = nullptr;
	//boxCollider->AddCollideAgainstLayer(CollisionLayers::PATHNODE);

	gn = new GranadeLauncher();
	gn->transform.SetScale(0.01, 0.01, 0.01);
	gn->transform.SetPosition(-0.899999, -1.96, 3.68);

	CameraPerspective* gunCam = new CameraPerspective(60.0f, Window::Instance().GetAspectRatio(), 0.1f, 10000.0f);
	gunCam->RemoveAllMaskLayers();
	gunCam->AddLayerMask(Layers::GUN);
	gunCam->SetDepth(10);
	gunCam->SetIsStatic(0);
	gunCam->AddChild(gn);
	AddChild(gunCam);

	



	boxCollider->collisionCallback = [this](GameObject* go){
		_movementSpeed = 0;
	};
}



void MainCamera::Update()
{
	_intendedDir.x = 0;
	_intendedDir.y = 0;
	_intendedDir.z = 0;

	//Logger::LogInfo(gn->transform.ToString());


	UpdateControls();
	CameraPerspective::Update();

	if (!IsTopView())
	{
		float h = Terrain::Instance().GetHeightAt(transform.GetPosition().x, transform.GetPosition().z);
		transform.SetPosition(transform.GetPosition().x, h + 60, transform.GetPosition().z);

		// Limit camera position within terrain
		if (transform.GetPosition().x > Terrain::Instance().GetTerrainMaxX() - 50)
			transform.SetPosition(Terrain::Instance().GetTerrainMaxX() - 50, transform.GetPosition().y, transform.GetPosition().z);
		else if (transform.GetPosition().x < Terrain::Instance().GetTerrainMinX() + 50)
			transform.SetPosition(Terrain::Instance().GetTerrainMinX() + 50, transform.GetPosition().y, transform.GetPosition().z);

		if (transform.GetPosition().z > Terrain::Instance().GetTerrainMaxZ() - 50)
			transform.SetPosition(transform.GetPosition().x, transform.GetPosition().y, Terrain::Instance().GetTerrainMaxZ() - 50);
		if (transform.GetPosition().z < Terrain::Instance().GetTerrainMinZ() + 50)
			transform.SetPosition(transform.GetPosition().x, transform.GetPosition().y, Terrain::Instance().GetTerrainMinZ() + 50);
	}

	else
	{
		int x, y, z;
		Terrain::Instance().GetCenter(x, y, z);
		transform.LookAt(x, y, z);
	}


}

void MainCamera::UpdateControls()
{
	
	//Handle rotation
		if (!_isTopView)
		{
			if (counter != 0) // Super hack to fix the camera going weird
			{

			this->transform.RotateBy(Input::GetDeltaMousePosX() * Timer::GetDeltaS() * GetRotationSpeed(), glm::vec3(0, 1, 0));
			this->transform.RotateBy(Input::GetDeltaMousePosY() * Timer::GetDeltaS() * GetRotationSpeed(), transform.GetLocalRight());		
	
			if (glm::dot(transform.GetLocalFront(), glm::vec3(0, -1, 0)) > 0.8 || glm::dot(transform.GetLocalFront(), glm::vec3(0, 1, 0)) > 0.8)
				this->transform.RotateBy(Input::GetDeltaMousePosY() * Timer::GetDeltaS() * GetRotationSpeed(), -transform.GetLocalRight());
			}
			
		}
		else
		{
			//transform.Translate(0, GetMovementSpeed() * Input::GetDeltaMousePosY() * Timer::GetDeltaS(),0);
		}

		//Handle forward and backward movement
		if (Input::GetKeyDown(GLFW_KEY_W) == true && Input::GetKeyDown(GLFW_KEY_S) == false)
		{
			this->transform.SetPosition(this->transform.GetPosition() + (GetMovementSpeed() * Timer::GetDeltaS() * this->transform.GetLocalFront()));
			_intendedDir += transform.GetLocalFront();
		}
		else if (Input::GetKeyDown(GLFW_KEY_S) == true && Input::GetKeyDown(GLFW_KEY_W) == false)
		{
			this->transform.SetPosition(this->transform.GetPosition() - (GetMovementSpeed() * Timer::GetDeltaS() * this->transform.GetLocalFront()));
			_intendedDir -= transform.GetLocalFront();

		}

		//Handle side-to-side movement
		if (Input::GetKeyDown(GLFW_KEY_D) == true && Input::GetKeyDown(GLFW_KEY_A) == false)
		{
			this->transform.SetPosition(this->transform.GetPosition() + (GetMovementSpeed() * Timer::GetDeltaS() * this->transform.GetLocalRight()));
			_intendedDir += transform.GetLocalRight();

		}
		else if (Input::GetKeyDown(GLFW_KEY_A) == true && Input::GetKeyDown(GLFW_KEY_D) == false)
		{
			this->transform.SetPosition(this->transform.GetPosition() - (GetMovementSpeed() * Timer::GetDeltaS() * this->transform.GetLocalRight()));
			_intendedDir -= transform.GetLocalRight();

		}
		
		if (glm::length2(_intendedDir) != 0)
			_intendedDir = glm::normalize(_intendedDir);

		boxCollider->transform.SetPosition( transform.GetGlobalPosition() +  _intendedDir * 50.0f);


		//Logger::LogInfo(boxCollider->transform.ToString());
		//Logger::LogInfo(Maths::Vec3ToString(_intendedDir));


	//Handle up and down movement
	/*if (Input::GetKeyDown(GLFW_KEY_R) == true && Input::GetKeyDown(GLFW_KEY_F) == false)
	{
		this->transform.SetPosition(this->transform.GetPosition() + (GetMovementSpeed() * Timer::GetDeltaS() * this->transform.GetLocalUp()));
	}
	else if (Input::GetKeyDown(GLFW_KEY_F) == true && Input::GetKeyDown(GLFW_KEY_R) == false)
	{
		this->transform.SetPosition(this->transform.GetPosition() - (GetMovementSpeed() * Timer::GetDeltaS() * this->transform.GetLocalUp()));
	}*/

	if (Input::GetKeyPressed(GLFW_KEY_V))
	{
		_isTopView = !_isTopView;
		transform.SetPosition(0, 2000, 0);

	}


	if (Input::GetKeyDown(GLFW_KEY_T) == true)
		Input::SetCursorMode("normal");
	else if (Input::GetKeyDown(GLFW_KEY_Y) == true)
		Input::SetCursorMode("disabled");

	/*if (Input::GetKeyDown(GLFW_KEY_KP_ADD))
		_movementSpeed+=10;
	else if (Input::GetKeyDown(GLFW_KEY_KP_SUBTRACT))
		_movementSpeed-=10;*/

	_movementSpeed = glm::clamp(_movementSpeed, MIN_SPEED, MAX_SPEED);

	counter = 1;
	//Logger::LogInfo(transform.ToString());
}

void MainCamera::LateUpdate()
{
	_movementSpeed = ORIGINAL_SPEED;
}


void MainCamera::SetMovementSpeed(float speed)
{
	_movementSpeed = speed;
}

void MainCamera::SetRotationSpeed(float speed)
{
	_rotationSpeed = speed;
}

float MainCamera::GetMovementSpeed() const
{
	return _movementSpeed;
}

float MainCamera::GetRotationSpeed() const
{
	return _rotationSpeed;
}