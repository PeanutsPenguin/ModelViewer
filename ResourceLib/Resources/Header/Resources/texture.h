#pragma once

///Project Include
#include "Resources/Iresources.h"
#include "pch.h"

///STB Image include
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include "stb_image.h"

///Standart include
#include <string>

namespace Resources
{
	class Texture : public IResources
	{
	public:
		Texture() = default;

		Texture(std::string& fileName);

		~Texture() override = default;

		int				m_height	= 0;
		int				m_width		= 0;
		int				m_channels	= 0;
		unsigned char*	m_data;
		unsigned int	m_id		= 0;		
		unsigned int	m_index		= 0;
	};
}
