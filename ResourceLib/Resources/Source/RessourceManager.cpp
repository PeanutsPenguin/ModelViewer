#include "Resources/ResourceManager.h"

namespace Resources
{
	void ResourcesManager::deleteResource(std::string name)
	{
		if (this->m_resources.contains(name) == true)
		{
			delete this->m_resources[name];
			this->m_resources.erase(name);
		}
	}

	void ResourcesManager::deleteArray()
	{
		for (auto& resource : ResourcesManager::m_resources)
			delete resource.second;

		ResourcesManager::m_resources.clear();
	}

	ResourcesManager::~ResourcesManager() { this->deleteArray(); }
}