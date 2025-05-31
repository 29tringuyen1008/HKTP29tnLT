#pragma once

enum class ObjectClass
{
	Object = 0,
	ProgramObject = 1,
	Scene = 2,
	Mouse = 3,
	Folder = 4,
	SceneFolder = 5,
	GUIFolder = 6,
	AssetFolder = 7,
	TexturePackage = 8,
	SoundObject = 9,
	SoundObjectBuffer = 10,
	Sprite = 11,
	UI_Frame = 12,
	DraggableFrame = 13,
	MessageBoxUI = 14,
	ScrollingFrame = 15,
	TextLabel = 16,
	TextButton = 17,
	Entity = 18,
	TerrainFolder = 19,
	Workspace = 20,
	ImageLabel = 21,
	Collider2D = 22,
	ImageButton = 23,
};

constexpr const wchar_t* ClassesByString[] =
{
	L"Object",
	L"Scene",
	L"Mouse",
	L"Folder",
	L"SceneFolder",
	L"GUIFolder",
	L"AssetFolder",
	L"TexturePackage",
	L"SoundObject",
	L"SoundObjectBuffer",
	L"Sprite",
	L"UI_Frame",
	L"DraggableFrame",
	L"MessageBoxUI",
	L"ScrollingFrame",
	L"TextLabel",
	L"TextButton",
	L"Entity",
	L"TerrainFolder",
	L"Workspace"
	L"ImageLabel",
	L"Collider2D",
	L"ImageButton"
};