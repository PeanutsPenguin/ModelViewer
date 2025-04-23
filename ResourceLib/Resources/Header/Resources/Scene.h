#pragma once 

//Project include
#include "LibMath/Matrix/Mat4x4.h"
#include "Resources/Shader.h"
#include "Resources/Entity.h"

//Std include
#include <vector>

namespace Resources
{
	class Scene
	{
	public:
		Scene(void) = default;

		Scene(const Scene& node) = default;

		Scene(Scene&& node) = default;

		void renderScene(Resources::Shader& shader, const LibMath::Mat4& pvm);

		void insert(Resources::Entity toInsert);

		void eraseEntities();

		~Scene();

		std::vector<Resources::Entity*> m_entities;
	};
}
