#include "..\pch.h"
#include "Model.h"
#include "..\GameObject\GameObject.h"
#include "..\Components\MeshRenderer.h"
#include "..\Utils\AssetLoader.h"

Model::Model()
{
	rootNode = NULL;
}

Model::~Model()
{
	auto it = allNodes.begin();

	for (; it != allNodes.end(); it++)
		delete it->second;

	for (int i = 0; i < allTextures.size(); i++)
		delete allTextures[i];

	auto itm = allMeshes.begin();
	for (; itm != allMeshes.end(); itm++)
		delete itm->second;


	allNodes.clear();
	allTextures.clear();
	allMeshes.clear();
	allMaterials.clear();

}



std::vector<Mesh> Model::GetMeshes()
{
	std::vector<Mesh> v;

	for (int i = 0; i < allMeshes.size(); i++)
	{
		v.push_back(*allMeshes[i]);

	}
	return v;

}




GameObject* Model::CreateGameObject()
{
	GameObject* e = new GameObject(name);

	for (int i = 0; i < allMeshes.size(); i++)
	{
		Mesh* m = new Mesh(*allMeshes[i]);

		MeshRenderer* mr = new MeshRenderer(m, allMaterials[i]);
		Material mat;
		mat.SetShader(AssetLoader::Instance().GetAsset<Shader>("ColorOnly"));
		//simplifiedMaterial.LoadVec3("color", 0.3f,0.3f,0.3f);
	//	simplifiedMaterial.SetShader(AssetLoader::Instance().GetAsset<Shader>("StaticModelSimplifiedShader"));
		//mr->SetMaterial(*simplifiedMaterial, MaterialType::SIMPLIFIED);

		mr->SetMaterial(mat);


		GameObject* c = new GameObject(meshesNames[i]);
		c->AddComponent(mr);
		e->AddChild(c);

	}
	return e;
}



