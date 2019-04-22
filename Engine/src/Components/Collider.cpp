#include "Collider.h"

void Collider::EngineUpdate()
{
	if (enableRender)
	{
		meshRenderer->EngineUpdate();
	}

	transform.Update();
}

glm::vec3 Collider::GlobalTranslationFromGameObject()
{
	return glm::vec3(transform.GetPosition().x * transform.GetScale().x * _parent->transform.GetScale().x,
		transform.GetPosition().y * transform.GetScale().y * _parent->transform.GetScale().y,
		transform.GetPosition().z * transform.GetScale().z * _parent->transform.GetScale().z );
}


void Collider::OnAttach(GameObject* go)
{
	InitializeMeshRenderer();
	transform.parent = &go->transform;
	meshRenderer->transform = &transform;
	meshRenderer->SetParent(go);

	meshRenderer->AddPreRenderCallback([](Camera& cam, Shader* s){		
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_CULL_FACE);	
	});


	meshRenderer->AddPostRenderCallback([](Camera& cam, Shader* s){
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_CULL_FACE);
	});
}

