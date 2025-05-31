#include "Texture.h"

tn::Texture::Texture() {

}

tn::Texture::Texture(const wchar_t* Filename) {
	this->Filename = std::wstring(Filename);
	this->UseFullSize = true;
	this->TextureType = TextureTypes::Image;
	this->SourceDestination[0] = { 0.0f, 0.0f };
	this->SourceDestination[1] = { 250.0f, 50.0f };
	this->TextureSize = { 250.0f, 50.0f };
}

tn::Texture::Texture(const wchar_t* Filename, Vector2 SourceDestination[2]) {
	this->Filename = std::wstring(Filename);
	this->TextureType = TextureTypes::Image;
	this->SourceDestination[0] = SourceDestination[0];
	this->SourceDestination[1] = SourceDestination[1];
	this->TextureSize = { SourceDestination[1].X - SourceDestination[0].X, SourceDestination[1].Y - SourceDestination[0].Y };
}

tn::Texture::Texture(unsigned char* Data, Vector2 SourceDestination[2]) {
	this->ptr_Data = Data;
	this->TextureType = TextureTypes::BytesBased;
	this->SourceDestination[0] = SourceDestination[0];
	this->SourceDestination[1] = SourceDestination[1];
	this->TextureSize = { SourceDestination[1].X - SourceDestination[0].X, SourceDestination[1].Y - SourceDestination[0].Y };
}

void tn::Texture::SetErrorData(tn::FunctionError* TargetErr) {
	this->m_Err = TargetErr;
}

bool tn::Texture::ImportImage() {
	HRESULT hr = 0;

	if (this->TextureType == TextureTypes::Image)
	{
		hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (void**)&pIWICFactory);
		if (FAILED(hr)) {

			WriteError(this->m_Err, -1);
			return false;
		}

		hr = pIWICFactory->CreateDecoderFromFilename(
			this->Filename.data(),
			NULL,
			GENERIC_READ,
			WICDecodeMetadataCacheOnLoad,
			&pDecoder
		);
		if (FAILED(hr)) {
			WriteError(this->m_Err, 0); //File not found
			return false;
		}
		hr = pDecoder->GetFrame(0, &pSource);
		if (FAILED(hr)) {
			WriteError(this->m_Err, 1); //Invalid file data
			return false;
		};

		hr = pIWICFactory->CreateFormatConverter(&pConverter);
		if (FAILED(hr)) {
			WriteError(this->m_Err, 2); //Invalid file format (requires images types such as .png, .jpg, etc.)
			return false;
		};

		hr = pConverter->Initialize(
			pSource,
			GUID_WICPixelFormat32bppPBGRA,
			WICBitmapDitherTypeNone,
			NULL,
			0.f,
			WICBitmapPaletteTypeMedianCut
		);
		if (FAILED(hr)) {
			WriteError(this->m_Err, 3); //Cannot initialize image data
			return false;
		};

		pIWICFactory->CreateBitmapFromSource(pConverter, WICBitmapCacheOnDemand, &pBitmap);
		if (FAILED(hr)) {
			WriteError(this->m_Err, 4); //Cannot create a bitmap
			return false;
		};

		this->IsImageLoaded = true;
	}

	return this->IsImageLoaded;
}

bool tn::Texture::ImportBitmap_GDIPlus(Gdiplus::Bitmap* BitmapPtr)
{
	if (this->TextureType == TextureTypes::Bitmap_GDIPlus)
	{
		this->pBitmap_Gdiplus = BitmapPtr;
	}

	return true;
}

void tn::Texture::PasteDataToBitmap_GDIPlus(Gdiplus::Bitmap** TargetBitmap)
{
	if (this->TextureType == TextureTypes::Image)
	{
		UINT uw, uh;
		pBitmap->GetSize(&uw, &uh);

		if (TargetBitmap != nullptr)
		{
			if (*TargetBitmap != nullptr)
			{
				if ((*TargetBitmap)->GetLastStatus() == Gdiplus::Ok) {
					Gdiplus::BitmapData bmpData = {};
					Gdiplus::Rect rt = {
						0, 0, (INT)uw, (INT)uh
					};
					Gdiplus::Status lockStatus = (*TargetBitmap)->LockBits(&rt, Gdiplus::ImageLockModeWrite, PixelFormat32bppPARGB, &bmpData);
					if (lockStatus == Gdiplus::Ok) {
						WICRect wicRect = { 0, 0, static_cast<INT>(uw), static_cast<INT>(uh) };
						pBitmap->CopyPixels(
							&wicRect, bmpData.Stride,
							bmpData.Stride * uh,
							static_cast<BYTE*>(bmpData.Scan0));

						(*TargetBitmap)->UnlockBits(&bmpData);
					}
				}
			}
		}
	}
}

void tn::Texture::GetLoadedImageSize(UINT* OutputWidth, UINT* OutputHeight)
{
	if (this->m_TextureType == TextureTypes::Image)
	{
		pBitmap->GetSize(OutputWidth, OutputHeight);
	}
}

int tn::Texture::WasImageLoaded()
{
	if (this->m_TextureType != TextureTypes::Image)
	{
		return -1;
	}
	else if (this->IsTextureExported == false)
	{
		return 1;
	}

	return 0;
};

bool tn::Texture::IsExported()
{
	return this->IsTextureExported;
}

void tn::Texture::MapHandleToDeviceContext(HDC* lp_hDC) {
	if (this->m_TextureType == TextureTypes::DeviceContext)
	{
		m_TextureD3D9->GetSurfaceLevel(0, &m_SurfaceD3D9);
		m_SurfaceD3D9->GetDC(lp_hDC);
	}
}

void tn::Texture::UnmapHandleToDeviceContext(HDC hDC) {
	if (this->m_TextureType == TextureTypes::DeviceContext)
	{
		m_SurfaceD3D9->ReleaseDC(hDC);
	}
}

void tn::Texture::Release() {
	tn::SafeReleaseWinSys(&pIWICFactory);
	tn::SafeReleaseWinSys(&pDecoder);
	tn::SafeReleaseWinSys(&pSource);
	//tn::SafeReleaseWinSys(&pStream);
	tn::SafeReleaseWinSys(&pConverter);
	tn::SafeReleaseWinSys(&pBitmap);

	this->m_GraphicDevice = nullptr;

	delete this;
}