#pragma once
#include "Resources/ResourceManager.h"

namespace Resources
{
	template<typename _Type>
	_Type* ResourcesManager::createRessource()
	{
		_Type* _data = new _Type();

		return _data;
	}

	template<typename _Type>
	_Type* ResourcesManager::createRessource(std::string name)
	{	
		if (this->m_resources.contains(name))
		{
			return (_Type*)this->m_resources[name];
		}

		_Type* _data = new _Type(name);
		
		this->m_resources[name] = _data;

		return _data;
	}

	template<typename _Type>
	inline _Type* ResourcesManager::addResource(_Type* toAdd, std::string name)
	{
		if (this->m_resources[name] != nullptr)
		{
			printf("ALREADY EXISTING");
			return this->m_resources[name];
		}

		this->m_resources[name] = toAdd;

		return toAdd;
	}

	template<typename _Type>
	_Type* ResourcesManager::getRessource(const char* name)
	{	
		if (this->m_resources.contains(name))
		{
			return this->m_resources[name];
		}
		else return nullptr;
	}
}
