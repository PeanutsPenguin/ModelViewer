#include "Resources/Scene.h"

namespace Resources
{
	void Scene::renderScene(Resources::Shader& shader, const LibMath::Mat4& pvm)
	{
		for (int i = 0; i < this->m_entities.size(); i++)
		{
			this->m_entities[i]->renderNode(shader, pvm);
		}
	}

	void Scene::insert(Resources::Entity toInsert)
	{
		this->m_entities.push_back(new Entity(toInsert)); }

	void Scene::eraseEntities() 
	{
		for (int i = 0; i < this->m_entities.size(); i++) delete this->m_entities[i];
		this->m_entities.clear();
	}

	Scene::~Scene() { this->eraseEntities(); }

}


