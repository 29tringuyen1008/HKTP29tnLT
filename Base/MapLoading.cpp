#include "JSON.h"
#include "MapLoading.h"
#include "GameObject.h"
#include "Base.h"

int MapLoading::Load2DMap(Program* Program, Scene** TargetScene, const wchar_t* FilenameJSON)
{
	if (TargetScene == nullptr)
	{
		return LOAD2DMAP_ERR_NO_SCENE;
	}

	JSONParser Parser = {};
	HANDLE hFile = CreateFileW(FilenameJSON, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return LOAD2DMAP_FILE_NOT_FOUND;
	}

	JSONElement mapjs = {};
	Parser.LoadFromFile(hFile);
	Parser.SetOutput(&mapjs);
	int ErrCode = Parser.StartParse();
	if (ErrCode != 0)
	{
		
		return ErrCode;
	};

	SceneDescription SceneDesc = {};
	SceneDesc.Type = ST_2DSurface;
	SceneDesc.WorldWidth = mapjs["world_width"];
	SceneDesc.WorldHeight = mapjs["world_height"];

	if ((*TargetScene) == nullptr)
	{
		(*TargetScene) = new Scene(Program, SceneDesc);
	}
	else if ((*TargetScene)->GetDesc().Type != ST_2DSurface)
	{
		return LOAD2DMAP_ERR_SCENE_NOT_2D_SURFACE;
	}

	(*TargetScene)->SetDesc(SceneDesc);

	JSONElement TileList = mapjs[L"tiles"];
	if (TileList.GetEnumType() == JSONElementTypeEnum::Array)
	{
		for (JSONElement tile : TileList.GetChildElements())
		{
			Sprite* TileSprite = new Sprite(Program);
			TileSprite->SetName(L"Tile");
			//TileSprite->SetTexture(dynamic_cast<tn::Texture*>(Program->GameHierachy.Assets->GetChild(tile[L"tileType"])));
			TileSprite->SetTexture(dynamic_cast<tn::Texture*>(Program->GetFirstLoadedTexture(tile["tileType"])));
			TileSprite->Alpha = 1.0f;
			TileSprite->Size.X = PixelsOfTile;
			TileSprite->Size.Y = PixelsOfTile;
			TileSprite->Position.X = (float)tile["x"] * PixelsOfTile;
			TileSprite->Position.Y = (float)tile["y"] * -PixelsOfTile;
			TileSprite->ZDepth = 2.0f;
			(*TargetScene)->AddTile(TileSprite);
		}
	}

	CloseHandle(hFile);
}