#pragma once
#include "../Core/Array.h"
#include "../Core/Physics.h"
#include "GameObject.h"
#include <string>

struct EntityAttribute
{
	std::wstring AttributeName = L"Attribute";
	std::wstring PrimaryAttributeType = L"integer";

	long Integer;
	float Float = 0.0f;
	bool Boolean = false;
	std::wstring String = L"";
	void* Data = nullptr;
	EntityAttribute* SubAttributes = nullptr;
};

struct EntityPrototype
{
	std::wstring EntityType = L"";
	std::wstring PrototypeName = L"";
	std::wstring InternalName = L"";
	tn::Texture* Icon = nullptr;
	tn::NormalArray<std::wstring> Flags = {
		L"placeable-neutral", L"player-creation"
	};
	tn::NormalArray<tn::Texture*> Sprites = { nullptr };
	struct
	{
		float Time = 0; //Seconds
		tn::NormalArray<std::wstring> Result = {
			
		};
	} Minable;
	tn::Vector2 SpriteSize = { 10,10 };
	tn::Rect CollisionBox = { 0,0,10,10 };
	tn::Rect SelectionBox = { 0,0,10,10 };
	unsigned int DimensionGrid[2] = { 1,1 };
	float MaxHealth = 100.0f;


	tn::NormalArray<EntityAttribute> Attributes = {};
};