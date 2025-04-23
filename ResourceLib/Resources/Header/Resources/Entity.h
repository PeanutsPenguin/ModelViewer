#pragma once 

//Project include
#include "Resources/Model.h"
#include "LibMath/Matrix/Mat4x4.h"
#include "Resources/texture.h"
#include "Resources/Shader.h"

namespace Resources
{
	class Entity
	{
	public:
		Entity() = default;

		Entity(const Entity& toCopy) = default;

		Entity(Entity&& toMve) = default;

		Entity(Resources::Model* model);

		Entity(Resources::Model* model, Resources::Texture* first);

		void addTexture(Resources::Texture* manager);

		void renderNode(Resources::Shader& shader, const LibMath::Mat4& mvp) const;

		~Entity() = default;

		LibMath::Mat4 m_local = LibMath::Mat4::Identity();

	private:
		Resources::Model* m_model;
		std::vector<Resources::Texture*> m_texArray;
	};
}
