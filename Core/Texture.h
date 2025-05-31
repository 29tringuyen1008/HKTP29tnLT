#pragma once
#include "Object.h"
#include "Graphics.h"
#include "Misc.h"

#define TN_TEXTURE_ERR_NODEVICE -1
#define TN_TEXTURE_ERR_IMPORT -2
#define TN_TEXTURE_ERR_READYTORENDER -3

enum TextureTypes {
	Image = 0,
	BytesBased = 1,
	DeviceContext = 2,
	Bitmap_GDIPlus = 3,
};

namespace tn {
	class Texture : public Object {
	public:
		std::wstring Filename = L"";
		bool UseFullSize = false;
		Vector2 SourceDestination[2] = { 0.0f,0.0f,0.0f,0.0f };
		Vector2 TextureSize = { 0.0f, 0.0f };
		TextureTypes TextureType = TextureTypes::Image;

		//SYSTEM
		Texture();
		Texture(const wchar_t* Filename);
		Texture(const wchar_t* Filename, Vector2 SourceDestination[2]);
		Texture(unsigned char* Data, Vector2 SourceDestination[2]);

		int WasImageLoaded();
		bool IsExported();

		bool ImportImage();
		//GDI+ type Only
		bool ImportBitmap_GDIPlus(Gdiplus::Bitmap* BitmapPtr);

		//Image Only
		void PasteDataToBitmap_GDIPlus(Gdiplus::Bitmap** TargetBitmap);

		//Image Only
		void GetLoadedImageSize(UINT* OutputWidth, UINT* OutputHeight);

		void SetErrorData(tn::FunctionError* TargetErr);

		void MapHandleToDeviceContext(HDC* lp_hDC);
		void UnmapHandleToDeviceContext(HDC hDC);
		void Release() override;

	private:
		friend class GraphicsDevice;

		IWICImagingFactory* pIWICFactory = nullptr;
		IWICBitmapDecoder* pDecoder = NULL;
		IWICBitmapFrameDecode* pSource = NULL;
		//IWICStream* pStream = NULL;
		//IWICBitmapClipper* pClipper = nullptr;
		IWICFormatConverter* pConverter = NULL;
		//IWICBitmapScaler* pScaler = NULL;
		IWICBitmap* pBitmap = NULL;
		Gdiplus::Bitmap* pBitmap_Gdiplus = nullptr;

		IDirect3DTexture9* m_TextureD3D9 = nullptr;
		IDirect3DSurface9* m_SurfaceD3D9 = nullptr;

		bool IsImageLoaded = false;
		bool IsTextureExported = false;

		unsigned char* ptr_Data = nullptr;
		GraphicsDevice* m_GraphicDevice = nullptr;
		TextureTypes m_TextureType = TextureTypes::Image; 
		tn::FunctionError* m_Err = nullptr;
	};
}