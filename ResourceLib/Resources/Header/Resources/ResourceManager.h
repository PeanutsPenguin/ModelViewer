#pragma once

//Standard include
#include <unordered_map>
#include <string>
#include <memory>

//Project include
#include "Resources/Iresources.h"
#include "pch.h"

namespace Resources
{
	class ResourcesManager
	{
	public:
		ResourcesManager(void) = default;

		template <typename _Type>
		_Type* createRessource();

		template <typename _Type>	
		_Type* createRessource(std::string name);

		template <typename _Type>
		_Type* addResource(_Type* toAdd, std::string name);

		template <typename _Type>
		_Type* getRessource(const char* name);

		void deleteResource(std::string name);

		void deleteArray();

		~ResourcesManager();

	private:
		std::unordered_map<std::string, IResources*> m_resources;
	};
}

#include "ResourcesManager.hpp"