#include "Resources/Entity.h"

namespace Resources
{
	Entity::Entity(Resources::Model* model)
	{
		this->m_model = model;
	}

	Entity::Entity(Resources::Model* model, Resources::Texture* first)
	{
		this->m_model = model;

		if (first != nullptr)
			this->m_texArray.push_back(first);
	}

	void Entity::addTexture(Resources::Texture* manager)
	{
		this->m_texArray.push_back(manager);
	}

	void Entity::renderNode(Resources::Shader& shader, const LibMath::Mat4& mvp) const
	{
		///Bind the element buffer
		glBindBuffer(GL_ARRAY_BUFFER, m_model->m_elementBuffer.m_buffer);

		///For all the texture the current object has, bind them and send them in the fragment shader
		for (int i = 0; i < this->m_texArray.size(); ++i)
		{
			glActiveTexture(this->m_texArray[i]->m_index);
			glBindTexture(GL_TEXTURE_3D, this->m_texArray[i]->m_id);

			std::string name = "textures[" + std::to_string(i) + "]";
			glUniform1i(glGetUniformLocation(shader.m_shaderProgram, name.c_str()), i);
		}

		///Find the pvm matrix and apply it to every vertex
		glUniformMatrix4fv(glGetUniformLocation(shader.m_shaderProgram, "PVM"), 1, GL_FALSE, &(mvp * this->m_local)[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(shader.m_shaderProgram, "model"), 1, GL_FALSE, &this->m_local[0][0]);
		glUniform1i(glGetUniformLocation(shader.m_shaderProgram, "nbtextures"), this->m_texArray.size());

		///Bind the Vertex Array Object
		glBindVertexArray(this->m_model->m_vertexObject.m_VAO);

		///Draw everything
		glDrawElements(GL_TRIANGLES, this->m_model->m_indices.size(), GL_UNSIGNED_INT, 0);

		///Unbind 
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		///Unbind the textures
		for (int i = 0; i < this->m_texArray.size(); ++i)
		{
			glActiveTexture(this->m_texArray[i]->m_index);
			glBindTexture(GL_TEXTURE_3D, 0);
		}
	}
}
