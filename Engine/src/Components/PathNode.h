#pragma once

#include "..\GameObject\GameObject.h"
#include "BoxCollider.h"
#include <vector>

class PathNode : public GameObject
{
public:
	PathNode();
	~PathNode();

	void OnCollision(GameObject* go) override;
	void Start() override;

	
	PathNode* previousNode;
	BoxCollider* sc;
	std::vector<PathNode*> neighbors;
	double distanceFromPrevious;
	double distanceFromTarget;
	double cost;


};