#pragma once
#include "GameObject.h"
#include "../Core/Texture.h"
#include "Entities.h"

class MapEditor
{
public: 
	Program* Program = nullptr;
	Scene* TargetScene = nullptr;
	size_t Width = 0.0f;
	size_t Height = 0.0f;
	tn::Texture* DefaultTexture = nullptr;
	std::vector<tn::Texture*> ListOfTextures;
	std::vector<EntityPrototype>* ReferenceListOfEntityPrototypesPtr = nullptr;
	size_t TextureIDSelected = 0;
	size_t EntityIDSelected = 0;

	MapEditor();
	~MapEditor();

	void SaveMap();
	void CreateMap();
};