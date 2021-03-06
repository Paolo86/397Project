#include "Skybox.h"
#include "..\Utils\AssetLoader.h"

Skybox::Skybox(CubeMap* cubemap)
{
	
	this->cubemap = cubemap;
	material.SetShader(AssetLoader::Instance().GetAsset<Shader>("Skybox"));
	material.LoadCubemap(cubemap);
	g = new GameObject("");
	
	AssetLoader::Instance().GetAsset<Model>("Cube")->PopulateGameObject(g);
	g->ApplyMaterial(material);
	g->ApplyMaterial(material, MaterialType::NOLIGHT);
	g->ApplyMaterial(material, MaterialType::COLORONLY);

	meshRenderer = g->GetComponentInChild<MeshRenderer>("cube", "MeshRenderer");
	meshRenderer->SetIsCullable(false);

	meshRenderer->AddPreRenderCallback(std::bind(&Skybox::OnPreRender, this, std::placeholders::_1, std::placeholders::_2));
	meshRenderer->AddPostRenderCallback(std::bind(&Skybox::OnPostRender, this, std::placeholders::_1, std::placeholders::_2));
	g->SetLayer(0);
	g->SetLayer(Layers::SKYBOX, 1);



}

Skybox::~Skybox()
{
	for (auto it = g->GetChildList().begin(); it != g->GetChildList().end(); it++)
	{
		delete (*it);
	}

	delete g;
}


void Skybox::OnPreRender(Camera& cam, Shader* s)
{
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

}
void Skybox::OnPostRender(Camera& cam, Shader* s)
{

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

}

void Skybox::EngineUpdate()
{

	//Logger::LogError("Got here");
	g->EngineUpdate();
}

