#include "MapEditor.h"
#include "Base.h"

MapEditor::MapEditor()
{

}

MapEditor::~MapEditor()
{

}

void MapEditor::CreateMap()
{
	if (TargetScene != nullptr)
	{
		SceneDescription sd = {};
		sd.Type = ST_2DSurface;
		sd.WorldWidth = this->Width;
		sd.WorldHeight = this->Height;

		TargetScene->SetDesc(sd);
		for (float y = (-sd.WorldHeight / 2); y <= (sd.WorldHeight / 2); y += 1)
		{
			for (float x = (-sd.WorldWidth / 2); x <= (sd.WorldWidth / 2); x += 1)
			{
				float l = (((size_t)(x / 10.0f) % 2 + 1.0f) + ((size_t)(y / 10.0f) % 2 + 1.0f)) / 2.0f;
				Sprite* Tile = new Sprite(TargetScene->m_Program);
				Tile->SetName(L"Tile");
				Tile->Color = { 0.0f, 1.0f, 0.0f };
				if (Program != nullptr)
				{
					Tile->SetTexture(dynamic_cast<tn::Texture*>(Program->GetFirstLoadedTexture(L"GrassTile")));
				}
				Tile->Alpha = 1.0f;
				Tile->Size.X = PixelsOfTile;
				Tile->Size.Y = PixelsOfTile;
				Tile->Position.X = x * PixelsOfTile;
				Tile->Position.Y = y * PixelsOfTile;
				Tile->ZDepth = 2.0f;
				TargetScene->AddTile(Tile);
			}
		}
	}
}

void MapEditor::SaveMap()
{

}