#include "PathFinder.h"
#include "..\Prefabs\Terrain.h"
#include "..\Event\EventDispatcher.h"
#include "..\Event\ApplicationEvents.h"
#include <ctime>
#include <list>
#include <limits>
#include <float.h>
#include <thread>
#include <future>
PathFinder& PathFinder::Instance()
{
	static PathFinder instance;
	return instance;

}

PathFinder::PathFinder()
{
	nodesQT = nullptr;
	srand((unsigned)time(0));
	EventDispatcher::Instance().SubscribeCallback<SceneChangedEvent>([this](Event* e){

		if (nodesQT != nullptr)
			nodesQT->ClearNodes();
		nodeMap.clear();
		pathNodes.clear();

		return 0;
	});
}

PathFinder::~PathFinder()
{
	if (nodesQT != nullptr)
	delete nodesQT;
}

void PathFinder::DeleteNodes()
{
	for (int i = 0; i < pathNodes.size(); i++)
	{
		delete pathNodes[i];
	}
}


glm::vec3 PathFinder::GetRandomFreeNodePosition()
{
	bool valid = false;
	glm::vec3 v;
	while (!valid)
	{

		int random = rand() % pathNodes.size() - 1;

		PathNode* pn = pathNodes[random];

		if (pn->cost == 0 && pn->transform.GetPosition().y > 750 &&
			pn->transform.GetPosition().x < Terrain::Instance().GetTerrainMaxX() - 2000 &&
			pn->transform.GetPosition().x > Terrain::Instance().GetTerrainMinX() + 2000 && 
			pn->transform.GetPosition().z < Terrain::Instance().GetTerrainMaxZ() - 2000 &&
			pn->transform.GetPosition().z > Terrain::Instance().GetTerrainMinZ() + 2000)
		{
			valid = 1;
			v = pn->transform.GetPosition();
		}
	}

	return v;



}



PathNode* PathFinder::ClosestNodeAt(int x, int y,  int z)
{
	std::unordered_set<PathNode*>& nodes = nodesQT->GameObjectsAt(x, z);

	double dist = 99999999999999999;
	PathNode* closest = nullptr;
	for (auto it = nodes.begin(); it != nodes.end(); it++)
	{
		double length = glm::length((*it)->sc->transform.GetGlobalPosition() - glm::vec3(x, y, z));
		
		if (length < dist)
		{
			dist = length;
			closest = (*it);

		}
	
	}

	if (closest == nullptr)
		Logger::LogError("Cannot find closest");

	///closest->sc->enableRender = 1;
	return closest;
}


void PathFinder::Generate(Terrain* terrain)
{
	int counterx = 0;
	for (int x = terrain->GetTerrainMinX() + 200; x < terrain->GetTerrainMaxX() - 200; x += 200)
	{
		std::vector<PathNode*> v;
		nodeMap.push_back(v);
		for (int z = terrain->GetTerrainMinZ() + 200; z < terrain->GetTerrainMaxZ() - 200; z += 200)
		{
			PathNode* pn = new PathNode();
			pn->transform.SetPosition(x, terrain->GetHeightAt(x, z), z);
			pn->transform.Update();
			pathNodes.push_back(pn);
			nodeMap[counterx].push_back(pn);

		}
		counterx++;

	}

	int x, y, z;
	terrain->GetCenter(x, y, z);

	if (nodesQT == nullptr)
	nodesQT = new QuadTree<PathNode*>(x, z, terrain->GetTerrainMaxX() - terrain->GetTerrainMinX(), terrain->GetTerrainMaxZ() - terrain->GetTerrainMinZ(),100);

}

void PathFinder::Start()
{

	for (unsigned i = 0; i < pathNodes.size(); i++)
	{

		pathNodes[i]->Start();
		//pathNodes[i]->sc->enableRender = 1;
		nodesQT->AddElement(pathNodes[i], pathNodes[i]->sc->transform.GetGlobalPosition().x, pathNodes[i]->sc->transform.GetGlobalPosition().z, pathNodes[i]->sc->transform.GetGlobalScale().x, pathNodes[i]->sc->transform.GetGlobalScale().z);
	}

	int mapWidth = nodeMap.size();
	int mapHeight = nodeMap[0].size();

	for (int x = 0; x < nodeMap.size(); x++) {
		for (int y = 0; y < nodeMap[0].size(); y++) {

			if (nodeMap[x][y]->transform.GetPosition().y < 750)
				nodeMap[x][y]->cost = 5000;

			if (x > 0) {
				nodeMap[x][y]->neighbors.push_back(nodeMap[x - 1][y]);

				//Diagonals

				if (y < mapHeight - 1) {
					nodeMap[x][y]->neighbors.push_back(nodeMap[x - 1][y + 1]);
				}

				if (y > 0) {
					nodeMap[x][y]->neighbors.push_back(nodeMap[x - 1][y - 1]);
				}
			}
			if (x < mapWidth - 1) {
				nodeMap[x][y]->neighbors.push_back(nodeMap[x + 1][y]);

				//Diagonals

				if (y < mapHeight - 1) {
					nodeMap[x][y]->neighbors.push_back(nodeMap[x + 1][y + 1]);
				}
				if (y > 0) {
					nodeMap[x][y]->neighbors.push_back(nodeMap[x + 1][y - 1]);
				}
			}
			if (y > 0) {
				nodeMap[x][y]->neighbors.push_back(nodeMap[x][y - 1]);
			}
			if (y < mapHeight - 1) {
				nodeMap[x][y]->neighbors.push_back(nodeMap[x][y + 1]);
			}
		}
	}

}



std::vector<glm::vec3> PathFinder::GeneratePath(glm::vec3 start, glm::vec3 finish)
{
/*	for (int i = 0; i < pathNodes.size(); i++)
	{
		pathNodes[i]->sc->enableRender = 1;	
		//pathNodes[i]->sc->meshRenderer->GetMaterial().SetColor(0, 1, 0);
	}*/


	PathNode* startNode = ClosestNodeAt(start.x, start.y, start.z);
	PathNode* goalNode = ClosestNodeAt(finish.x, finish.y, finish.z);
	float absoluteDistance = 0;
	float previousAbsoluteDistance = 0;


	/*goalNode->sc->meshRenderer->GetMaterial().SetColor(0, 1, 1);
	for (int i = 0; i < goalNode->neighbors.size(); i++)
	{
		goalNode->neighbors[i]->sc->meshRenderer->GetMaterial().SetColor(0, 1, 1);
	}*/

	PathNode* currentNode = startNode;

	absoluteDistance = glm::length(currentNode->transform.GetPosition() - goalNode->transform.GetPosition());
	previousAbsoluteDistance = absoluteDistance;
	int quitCounter = 0;
	PathNode* shortest = nullptr;
	std::vector<glm::vec3> path;
	do
	{

		shortest = nullptr;
		double minDist = 99999999999999999;
		absoluteDistance = glm::length(currentNode->transform.GetPosition() - goalNode->transform.GetPosition());

		if (absoluteDistance > previousAbsoluteDistance)
		{
			break;
		}

	
		previousAbsoluteDistance = absoluteDistance;
		for (unsigned i = 0; i < currentNode->neighbors.size(); i++)
			currentNode->neighbors[i]->checked = false;
		
		for (unsigned i = 0; i < currentNode->neighbors.size(); i++)
		{
			if (currentNode->neighbors[i]->checked)
				continue;
		/*	if (currentNode->neighbors[i]->cost == 0)
				currentNode->neighbors[i]->sc->meshRenderer->GetMaterial().SetColor(0, 1, 1);
			else
				currentNode->neighbors[i]->sc->meshRenderer->GetMaterial().SetColor(1, 0, 0);*/

			currentNode->neighbors[i]->distanceFromPrevious = glm::length(currentNode->transform.GetPosition() - currentNode->neighbors[i]->transform.GetPosition());
			currentNode->neighbors[i]->distanceFromTarget = glm::length(goalNode->transform.GetPosition() - currentNode->neighbors[i]->transform.GetPosition());
			double total = currentNode->neighbors[i]->distanceFromPrevious + currentNode->neighbors[i]->distanceFromTarget + currentNode->neighbors[i]->cost;
			currentNode->neighbors[i]->checked = 1;
			if (total < minDist)
			{
				shortest = currentNode->neighbors[i];
				minDist = total;
			}
		}

		if (shortest != nullptr)
		{

			//shortest->sc->meshRenderer->GetMaterial().SetColor(1, 1, 1);
			//shortest->sc->enableRender = 1;
			path.push_back(shortest->transform.GetPosition());

		}
		else
			Logger::LogError("Error while getting path");

		currentNode = shortest;

	} while (shortest != goalNode && path.size() < 2);



	return path;

}
