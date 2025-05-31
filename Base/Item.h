#pragma once
#include "../Core/Texture.h"
#include "GameObject.h"
#include <string>

enum class ItemType
{
	NormalItem = 0,
	ItemWithEntityData = 1,
	Ammo = 2,
	Weapon = 3,
};

struct ItemDescription
{
	std::wstring PrototypeName = L"ItemProto";
	std::wstring InternalName = L"Item";
	std::wstring Tooltip = L"Đây là một dụng cụ.";
	tn::Texture* Icon = nullptr;
	unsigned int StackSize = 1;

	ItemType Type = ItemType::NormalItem;
	struct
	{
		float Range = 1.0f; //cm
		float Recharge = 0.0f; //seconda
	} WeaponClass;
	struct
	{
		std::wstring ReferenceEntityProtoName = L"";
	} EntityDataClass;
};

class InventoryItem
{
public:
	ItemDescription* BaseItem = nullptr;
	bool HasAltText = false;
	const wchar_t* AltText = L"";
	unsigned int Size = 0;
private:
	unsigned int MaxStackSize = 1;
	bool Active = true;
};