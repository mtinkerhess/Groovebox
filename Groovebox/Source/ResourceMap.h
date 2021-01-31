#pragma once

#include "Util.h"

#include <fstream>
#include <map>
#include <string>
#include <sstream>
#include <iostream>

#include <SFML/Graphics.hpp>

enum class ResourceMapType
{
	Texture,
	Font
};

template <typename ResourceType, ResourceMapType EnumType>
class ResourceMap
{
private:
	static ResourceMap<ResourceType, EnumType>* s_instance;

public:
	static ResourceMap<ResourceType, EnumType>& Instance()
	{
		if (s_instance == nullptr)
		{
			s_instance = new ResourceMap<ResourceType, EnumType>;
		}

		return *s_instance;
	}

private:

	std::map<std::string, ResourceType> m_map;

	ResourceMap()
	{
		std::ifstream fileStream(GetConfigPath());

		std::string line;
		while (std::getline(fileStream, line))
		{
			std::istringstream lineStream(line);

			std::string textureName;
			std::string fileName;

			if (!(lineStream >> textureName >> fileName))
			{
				std::cerr << "Error: " << line << std::endl;
			}

			ResourceType resource;
			std::stringstream resourcePath;
			resourcePath << GetAssetDir() << fileName;
			resource.loadFromFile(resourcePath.str());
			m_map[textureName] = resource;
		}
	}

public:
	const ResourceType& Get(std::string textureName) const
	{
		return m_map.find(textureName)->second;
	}

private:
	const char* GetConfigPath()
	{
		switch (EnumType)
		{
		case ResourceMapType::Texture: return "Resources\\Config\\Textures.txt";
		case ResourceMapType::Font: return "Resources\\Config\\Fonts.txt";
		}

		return nullptr;
	}

	const char* GetAssetDir()
	{
		switch (EnumType)
		{
		case ResourceMapType::Texture: return "Resources\\Textures\\";
		case ResourceMapType::Font: return "Resources\\Fonts\\";
		}

		return nullptr;
	}
};

typedef ResourceMap<sf::Texture, ResourceMapType::Texture> Textures;
typedef ResourceMap<sf::Font, ResourceMapType::Font> Fonts;