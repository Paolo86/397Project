#pragma once
#include "Scene/Scene.h"


class TestScene2 : public Scene
{
public:
	TestScene2();
	virtual void LoadAssets() override;
	virtual void UnloadAssets() override;
	virtual void ExitScene() override;
	virtual void Initialize() override;
	virtual void LogicUpdate() override;
	virtual void EngineUpdate() override;
	virtual void LateUpdate() override;


};