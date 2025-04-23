#include "Resources/texture.h"
#include <iostream>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

namespace Resources
{
	Texture::Texture(std::string& filename)
	{
		///Actvie the texture for Multi Texturing
		glActiveTexture(this->m_index);
		///Set the if
		glGenTextures(1, &this->m_id);
		///Bind the texture
		glBindTexture(GL_TEXTURE_3D, this->m_id);

		///Parameter set to Repeat the color
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);

		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		///Load the image with the pathFile given
		stbi_set_flip_vertically_on_load(true);
		this->m_data = stbi_load(filename.c_str(), &this->m_width, &this->m_height, &this->m_channels, 0);

		const uint32_t color = this->m_channels > 3 ? GL_RGBA : GL_RGB;

		if (this->m_data)
		{
			glTexImage3D(GL_TEXTURE_3D, 0, color, this->m_width, this->m_height, 1, 0, color, GL_UNSIGNED_BYTE, this->m_data);

			glGenerateMipmap(GL_TEXTURE_3D);
		}
		else
			std::cout << "FAILED TO LOAD IMAGE" << std::endl;

		///free the image
		stbi_image_free(this->m_data);
	}
}

