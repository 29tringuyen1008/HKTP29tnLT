#pragma once

struct ImageDataD3D9 {
	IDirect3DTexture9* Texture = NULL;
	IDirect3DSurface9* Surface = NULL;
};

struct QualitySamplerData {
	DWORD* ListOfAvailableQualityLevels = NULL;
	int NumberOfAvailableQualityLevels = 0;
};