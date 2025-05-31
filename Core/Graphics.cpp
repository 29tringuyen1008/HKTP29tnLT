#include "Graphics.h"

#pragma region SYSTEM

tn::GraphicsDevice::GraphicsDevice() {
	this->m_GraphicsD3D9 = nullptr;
	this->m_DeviceD3D9 = nullptr;
	this->m_GraphicErr = nullptr;
	this->m_VertexShaderD3D9 = nullptr;
	this->m_PixelShaderD3D9 = nullptr;
	this->m_VertexDeclaration = nullptr;

	this->m_Prop = {};
}

void tn::GraphicsDevice::Release() {
	tn::SafeReleaseWinSys(&m_GraphicsD3D9);
	tn::SafeReleaseWinSys(&m_DeviceD3D9);
	tn::SafeReleaseWinSys(&m_VertexDeclaration);
	tn::SafeReleaseWinSys(&m_VertexShaderD3D9);
	tn::SafeReleaseWinSys(&m_PixelShaderD3D9);
}

void tn::GraphicsDevice::SetErrorData(tn::FunctionError* TargetErr) {
	this->m_GraphicErr = TargetErr;
}

bool tn::GraphicsDevice::Initialize(GraphicInterfaceSetupProperties* TargetDesc) {
	this->m_Prop = (*TargetDesc);

	this->m_GraphicsD3D9 = Direct3DCreate9(D3D_SDK_VERSION);

	D3DPRESENT_PARAMETERS d3dpresentparamenters = {};
	ZeroMemory(&d3dpresentparamenters, sizeof(D3DPRESENT_PARAMETERS));
	if (TargetDesc->DepthStencilType == D16) {
		d3dpresentparamenters.AutoDepthStencilFormat = D3DFMT_D16;
	}
	else {
		d3dpresentparamenters.AutoDepthStencilFormat = D3DFMT_D32;
	};
	d3dpresentparamenters.BackBufferCount = TargetDesc->BackBufferCount;
	if (TargetDesc->SurfacesDimensionalType == SDT_16Bits) {
		d3dpresentparamenters.BackBufferFormat = D3DFMT_A1R5G5B5;
	}
	else if (TargetDesc->SurfacesDimensionalType == SDT_32Bits) {
		d3dpresentparamenters.BackBufferFormat = D3DFMT_A8R8G8B8;
	}
	else {
		d3dpresentparamenters.BackBufferFormat = D3DFMT_UNKNOWN;
	}
	d3dpresentparamenters.BackBufferWidth = TargetDesc->ScreenWidth;
	d3dpresentparamenters.BackBufferHeight = TargetDesc->ScreenHeight;
	if (m_Prop.DepthStencilEnable == true) {
		d3dpresentparamenters.EnableAutoDepthStencil = TRUE;
	}
	else {
		d3dpresentparamenters.EnableAutoDepthStencil = FALSE;
	}
	d3dpresentparamenters.Flags = 0;
	d3dpresentparamenters.FullScreen_RefreshRateInHz = TargetDesc->RefreshRateInHz;
	d3dpresentparamenters.hDeviceWindow = TargetDesc->UI->hWnd;
	d3dpresentparamenters.MultiSampleQuality = 0;
	d3dpresentparamenters.MultiSampleType = D3DMULTISAMPLE_NONE;

	if (TargetDesc->SwapChainEffect == Discard) {
		d3dpresentparamenters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	}
	else if (TargetDesc->SwapChainEffect == Copy) {
		d3dpresentparamenters.SwapEffect = D3DSWAPEFFECT_COPY;
	}
	else if (TargetDesc->SwapChainEffect == Flip) {
		d3dpresentparamenters.SwapEffect = D3DSWAPEFFECT_FLIP;
	}
	else { d3dpresentparamenters.SwapEffect = D3DSWAPEFFECT_DISCARD; };
	if (TargetDesc->Windowed == true) {
		d3dpresentparamenters.Windowed = TRUE;
	}
	else {
		d3dpresentparamenters.Windowed = FALSE;
	}

	D3DDEVTYPE d3ddevtypes[] = { D3DDEVTYPE_HAL, D3DDEVTYPE_NULLREF, D3DDEVTYPE_REF };

	HRESULT hr = 0;

	for (int i = 0; i < 3; i++) {
		hr = m_GraphicsD3D9->CreateDevice(D3DADAPTER_DEFAULT, d3ddevtypes[i], TargetDesc->UI->hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpresentparamenters, &m_DeviceD3D9);
		if (SUCCEEDED(hr)) {
			break;
		}
	}

	if (FAILED(hr)) {
		m_GraphicsD3D9->Release();
		WriteError(this->m_GraphicErr, 0);
		return false;
	}

	m_DeviceD3D9->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_DeviceD3D9->SetRenderState(D3DRS_ZENABLE, TRUE);
	m_DeviceD3D9->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	m_DeviceD3D9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	m_DeviceD3D9->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_DeviceD3D9->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	m_DeviceD3D9->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_DeviceD3D9->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	m_DeviceD3D9->SetRenderState(D3DRS_ADAPTIVETESS_Y, MAKEFOURCC('A', 'T', 'O', 'C'));

	m_DeviceD3D9->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	m_DeviceD3D9->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	//m_DeviceD3D9->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	m_DeviceD3D9->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_DeviceD3D9->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_DeviceD3D9->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	return true;
}

#pragma endregion

#pragma region TEXTURE

bool tn::GraphicsDevice::ExportTexture(Texture* Texture)
{
	HRESULT hr = 0;

	D3DFORMAT currentbackbufferformat = D3DFMT_A8R8G8B8;
	if (m_Prop.SurfacesDimensionalType == SDT_16Bits) {
		currentbackbufferformat = D3DFMT_A4R4G4B4;
	}

	UINT autowidth, autoheight;

	Texture->m_TextureType = Texture->TextureType;

	if (Texture->IsExported() == true)
	{
		if (Texture->m_SurfaceD3D9) Texture->m_SurfaceD3D9->Release();
		if (Texture->m_TextureD3D9) Texture->m_TextureD3D9->Release();
	}

	if (Texture->m_TextureType == TextureTypes::Image)
	{
		Texture->pBitmap->GetSize(&autowidth, &autoheight);
		WICRect rect = { };

		if (Texture->UseFullSize == true)
		{
			rect.X = 0;
			rect.Y = 0;
			rect.Width = autowidth;
			rect.Height = autoheight;
			hr = m_DeviceD3D9->CreateTexture(autowidth, autoheight, 1, 0, currentbackbufferformat, D3DPOOL_MANAGED, &Texture->m_TextureD3D9, nullptr);
		}
		else
		{
			rect.X = Texture->SourceDestination[0].X;
			rect.Y = Texture->SourceDestination[0].Y;
			rect.Width = Texture->SourceDestination[1].X;
			rect.Height = Texture->SourceDestination[1].Y;
			hr = m_DeviceD3D9->CreateTexture(static_cast<UINT>(Texture->TextureSize.X), static_cast<UINT>(Texture->TextureSize.Y), 1, 0, currentbackbufferformat, D3DPOOL_MANAGED, &Texture->m_TextureD3D9, nullptr);
		}
		if (FAILED(hr)) return false;

		D3DLOCKED_RECT lockedRect;
		hr = Texture->m_TextureD3D9->LockRect(0, &lockedRect, NULL, D3DLOCK_DISCARD);
		if (FAILED(hr)) {
			//std::wcout << L"Me oi me" << std::endl;
			return false;
		};

		unsigned long Stride = rect.Width * 4UL;
		unsigned long BufferSize = Stride * rect.Height;

		std::unique_ptr<BYTE> buffer(new BYTE[BufferSize]);

		hr = Texture->pBitmap->CopyPixels(&rect, Stride, BufferSize, buffer.get());
		//std::cout << buffer << std::endl;
		memcpy_s(lockedRect.pBits, BufferSize, buffer.get(), BufferSize);

		Texture->m_TextureD3D9->UnlockRect(0);
	}
	else if (Texture->m_TextureType == TextureTypes::BytesBased)
	{
		hr = m_DeviceD3D9->CreateTexture(static_cast<UINT>(Texture->TextureSize.X), static_cast<UINT>(Texture->TextureSize.Y), 1, 0, currentbackbufferformat, D3DPOOL_MANAGED, &Texture->m_TextureD3D9, nullptr);
		if (FAILED(hr)) return false;

		D3DLOCKED_RECT lockedRect;
		hr = Texture->m_TextureD3D9->LockRect(0, &lockedRect, NULL, D3DLOCK_DISCARD);
		if (FAILED(hr)) {
			return false;
		};

		unsigned long Stride = Texture->SourceDestination[1].X * 4;
		unsigned long BufferSize = Stride * Texture->SourceDestination[1].Y;
		lockedRect.Pitch = BufferSize;

		memcpy_s(lockedRect.pBits, BufferSize, Texture->ptr_Data, BufferSize);

		Texture->m_TextureD3D9->UnlockRect(0);
	}
	else if (Texture->m_TextureType == TextureTypes::DeviceContext)
	{
		hr = m_DeviceD3D9->CreateTexture(static_cast<UINT>(Texture->TextureSize.X), static_cast<UINT>(Texture->TextureSize.Y), 1, D3DUSAGE_DYNAMIC, currentbackbufferformat, D3DPOOL_DEFAULT, &Texture->m_TextureD3D9, nullptr);
		if (FAILED(hr)) return false;
	}
	else if (Texture->m_TextureType == TextureTypes::Bitmap_GDIPlus)
	{
		if (Texture->pBitmap_Gdiplus == nullptr) return false;

		hr = m_DeviceD3D9->CreateTexture(static_cast<UINT>(Texture->TextureSize.X), static_cast<UINT>(Texture->TextureSize.Y), 1, D3DUSAGE_DYNAMIC, currentbackbufferformat, D3DPOOL_DEFAULT, &Texture->m_TextureD3D9, nullptr);
		if (FAILED(hr)) return false;

		D3DLOCKED_RECT rect;
		if (SUCCEEDED(Texture->m_TextureD3D9->LockRect(0, &rect, NULL, 0))) {
			Gdiplus::BitmapData bitmapData;
			Gdiplus::Rect rectGDI(0, 0, Texture->SourceDestination[1].X, Texture->SourceDestination[1].Y);

			unsigned long Stride = Texture->SourceDestination[1].X * 4;
			unsigned long BufferSize = Stride * Texture->SourceDestination[1].Y;

			if (Texture->pBitmap_Gdiplus->LockBits(&rectGDI, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bitmapData) == Gdiplus::Ok) 
			{
				for (UINT y = 0; y < Texture->SourceDestination[1].Y; ++y) {
					BYTE* srcRow = (BYTE*)bitmapData.Scan0 + y * bitmapData.Stride;
					BYTE* destRow = (BYTE*)rect.pBits + y * rect.Pitch;
					memcpy(destRow, srcRow, Stride); // Copy 4 bytes per pixel (ARGB)
				}

				Texture->pBitmap_Gdiplus->UnlockBits(&bitmapData); // Unlock GDI+ bitmap
			}
			Texture->m_TextureD3D9->UnlockRect(0);
		}
	}

	Texture->IsTextureExported = true;
	Texture->m_GraphicDevice = this;

	return true;
}

void tn::GraphicsDevice::SetTexture(unsigned int Level, Texture* Texture) {
	if (Level > 1) return;

	if (Texture != nullptr) {
		if (Texture->m_TextureD3D9)
		{
			m_DeviceD3D9->SetTexture(Level, Texture->m_TextureD3D9);
		}
		else
		{
			m_DeviceD3D9->SetTexture(Level, nullptr);
		}
	}
	else {
		m_DeviceD3D9->SetTexture(Level, nullptr);
	}
}

#pragma endregion

#pragma region SHADER

bool tn::GraphicsDevice::LoadShader(Shader** Shader, ShaderDescription* ShaderDesc) {
	(*Shader) = new tn::Shader();
	(*Shader)->m_desc = (*ShaderDesc);

	HRESULT hr = 0;

	ID3DBlob* ShaderData = nullptr;
	hr = D3DCompileFromFile(m_Prop.StartingShaderDestination, NULL, NULL, ShaderDesc->StartingPoint, ShaderDesc->ShaderModel, 0, 0, &ShaderData, NULL);
	if (FAILED(hr)) {
		return false;
	}

	if (ShaderDesc->ShaderType == Shader_Vertex) {
		IDirect3DVertexShader9* VertexShader;
		m_DeviceD3D9->CreateVertexShader((const DWORD*)ShaderData->GetBufferPointer(), &VertexShader);
		(*Shader)->Data = static_cast<void*>(VertexShader);
	}
	else if (ShaderDesc->ShaderType == Shader_Pixel) {
		IDirect3DPixelShader9* PixelShader;
		m_DeviceD3D9->CreatePixelShader((const DWORD*)ShaderData->GetBufferPointer(), &PixelShader);
		(*Shader)->Data = static_cast<void*>(PixelShader);
	}

	ShaderData->Release();

	return true;
}

void tn::GraphicsDevice::SetShader(Shader* Shader, ShaderTypes ShaderType) {
	if (ShaderType == Shader_Vertex) {
		if (Shader != nullptr) {
			IDirect3DVertexShader9* VertexShader = static_cast<IDirect3DVertexShader9*>(Shader->Data);
			if (VertexShader) {
				m_DeviceD3D9->SetVertexShader(VertexShader);
			}
		}
		else {
			m_DeviceD3D9->SetVertexShader(nullptr);
		}
	}
	else if (ShaderType == Shader_Pixel) {
		if (Shader != nullptr) {
			IDirect3DPixelShader9* PixelShader = static_cast<IDirect3DPixelShader9*>(Shader->Data);
			if (PixelShader) {
				m_DeviceD3D9->SetPixelShader(PixelShader);
			}
		}
		else {
			m_DeviceD3D9->SetPixelShader(nullptr);
		}
	}
}

#pragma endregion

#pragma region MATRIX

void tn::GraphicsDevice::SetWorldMatrix(tn::Matrix4x4 Matrix) {
	D3DMATRIX d3dmatrix = Matrix;
	m_DeviceD3D9->SetTransform(D3DTS_WORLD, &d3dmatrix);
}

void tn::GraphicsDevice::SetViewMatrix(tn::Matrix4x4 Matrix) {
	D3DMATRIX d3dmatrix = Matrix;
	m_DeviceD3D9->SetTransform(D3DTS_VIEW, &d3dmatrix);
}

void tn::GraphicsDevice::SetProjectionMatrix(tn::Matrix4x4 Matrix) {
	D3DMATRIX d3dmatrix = Matrix;
	m_DeviceD3D9->SetTransform(D3DTS_PROJECTION, &d3dmatrix);
}

#pragma endregion

#pragma region RENDER

void tn::GraphicsDevice::ClearScreen(Color3 Color) {
	m_DeviceD3D9->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_RGBA(static_cast<int>(Color.Red * 255.0f), static_cast<int>(Color.Green * 255.0f), static_cast<int>(Color.Blue * 255.0f), 255), 1.0f, 0);
}

void tn::GraphicsDevice::ClearDepth(float Depth) {
	m_DeviceD3D9->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
}

void tn::GraphicsDevice::BeginScene() {
	m_DeviceD3D9->BeginScene();
}

void tn::GraphicsDevice::EndScene() {
	m_DeviceD3D9->EndScene();
}

void tn::GraphicsDevice::Draw(DrawTypes DrawType, tn::Vertex* Vertices, unsigned int NumberOfVertices, unsigned int StartWithVertexNumber, unsigned int PrimitiveCount) {
	IDirect3DVertexBuffer9* VertexBuffer = nullptr;

	D3DPRIMITIVETYPE d3dpt = D3DPT_FORCE_DWORD;
	if (DrawType == DrawType_LineList) {
		d3dpt = D3DPT_LINELIST;
	}
	else if (DrawType == DrawType_LineStrip) {
		d3dpt = D3DPT_LINESTRIP;
	}
	else if (DrawType == DrawType_PointList) {
		d3dpt = D3DPT_POINTLIST;
	}
	else if (DrawType == DrawType_TriangleList) {
		d3dpt = D3DPT_TRIANGLELIST;
	}
	else if (DrawType == DrawType_TriangleStrip) {
		d3dpt = D3DPT_TRIANGLESTRIP;
	}
	else if (DrawType == DrawType_TriangleFan) {
		d3dpt = D3DPT_TRIANGLEFAN;
	}

	struct FVFVERTEX {
		float X, Y, Z;
		D3DCOLOR Color;
		float u1, v1;
		float u2, v2;
	};

	//std::cout << NumberOfVertices << std::endl;

	unsigned int SizeOfStruct = 0;

	DWORD FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX2;

	void* ptrListVert = nullptr;
	FVFVERTEX* ListOfFVFVertices = new FVFVERTEX[NumberOfVertices];

	for (int i = 0; i < NumberOfVertices; i++) {
		ListOfFVFVertices[i].X = Vertices[i].Position.X;
		ListOfFVFVertices[i].Y = Vertices[i].Position.Y;
		ListOfFVFVertices[i].Z = Vertices[i].Position.Z;
		//ListOfFVFVertices[i].W = 1;
		ListOfFVFVertices[i].Color = D3DCOLOR_ARGB(static_cast<int>(Vertices[i].Alpha * 255.0f), static_cast<int>(Vertices[i].BackgroundColor.Red * 255.0f), static_cast<int>(Vertices[i].BackgroundColor.Green * 255.0f), static_cast<int>(Vertices[i].BackgroundColor.Blue * 255.0f));
		ListOfFVFVertices[i].u1 = Vertices[i].TextureCoordinates[0].X;
		ListOfFVFVertices[i].v1 = Vertices[i].TextureCoordinates[0].Y;
		ListOfFVFVertices[i].u2 = Vertices[i].TextureCoordinates[1].X;
		ListOfFVFVertices[i].v2 = Vertices[i].TextureCoordinates[1].Y;
	}

	SizeOfStruct = sizeof(FVFVERTEX);
	ptrListVert = ListOfFVFVertices;

	unsigned int SizeOfList = NumberOfVertices * SizeOfStruct;

	HRESULT hr = 0;
	hr = m_DeviceD3D9->CreateVertexBuffer(SizeOfList,
		0, FVF,
		D3DPOOL_MANAGED, &VertexBuffer, NULL);

	if (FAILED(hr)) {
		//std::cout << "HAFQVQVN" << std::endl;
		return;
	}

	//std::cout << static_cast<int>(255.0f * (1.0f - Vertices[0].Transparency)) << std::endl;

	VOID* pVertices;
	hr = VertexBuffer->Lock(0, SizeOfList, (void**)&pVertices, 0);
	if (FAILED(hr)) {
		//std::cout << "HAFQVQVN" << std::endl;
		return;
	}
	memcpy_s(pVertices, SizeOfList, ptrListVert, SizeOfList);
	hr = VertexBuffer->Unlock();
	if (FAILED(hr)) {
		//std::cout << "18F1C1" << std::endl;
		return;
	}

	m_DeviceD3D9->SetFVF(FVF);
	m_DeviceD3D9->SetStreamSource(0, VertexBuffer, 0, SizeOfStruct);
	m_DeviceD3D9->DrawPrimitive(d3dpt, StartWithVertexNumber, PrimitiveCount);
	tn::SafeReleaseWinSys(&VertexBuffer);

	delete[] ListOfFVFVertices;
}

void tn::GraphicsDevice::Draw(DrawTypes DrawType, tn::Vertex* Vertices, void* Indices, unsigned int SizeOfIndexType, unsigned int NumberOfVertices, unsigned int NumberOfIndices, unsigned int StartWithIndexNumber, unsigned int PrimitiveCount) {
	IDirect3DVertexBuffer9* VertexBuffer = nullptr;
	IDirect3DIndexBuffer9* IndexBuffer = nullptr;

	D3DPRIMITIVETYPE d3dpt = D3DPT_FORCE_DWORD;
	if (DrawType == DrawType_LineList) {
		d3dpt = D3DPT_LINELIST;
	}
	else if (DrawType == DrawType_LineStrip) {
		d3dpt = D3DPT_LINESTRIP;
	}
	else if (DrawType == DrawType_PointList) {
		d3dpt = D3DPT_POINTLIST;
	}
	else if (DrawType == DrawType_TriangleList) {
		d3dpt = D3DPT_TRIANGLELIST;
	}
	else if (DrawType == DrawType_TriangleStrip) {
		d3dpt = D3DPT_TRIANGLESTRIP;
	}
	else if (DrawType == DrawType_TriangleFan) {
		d3dpt = D3DPT_TRIANGLEFAN;
	}

	struct FVFVERTEX {
		float X, Y, Z;
		D3DCOLOR Color;
		float u1, v1;
		float u2, v2;
	};

	//std::cout << NumberOfVertices << std::endl;

	unsigned int SizeOfStruct = 0;

	DWORD FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;

	void* ptrListVert = nullptr;
	FVFVERTEX* ListOfFVFVertices = new FVFVERTEX[NumberOfVertices];

	for (int i = 0; i < NumberOfVertices; i++) {
		ListOfFVFVertices[i].X = Vertices[i].Position.X;
		ListOfFVFVertices[i].Y = Vertices[i].Position.Y;
		ListOfFVFVertices[i].Z = Vertices[i].Position.Z;
		ListOfFVFVertices[i].Color = D3DCOLOR_ARGB(static_cast<int>(Vertices[i].Alpha * 255.0f), static_cast<int>(Vertices[i].BackgroundColor.Red * 255.0f), static_cast<int>(Vertices[i].BackgroundColor.Green * 255.0f), static_cast<int>(Vertices[i].BackgroundColor.Blue * 255.0f));
		ListOfFVFVertices[i].u1 = Vertices[i].TextureCoordinates[0].X;
		ListOfFVFVertices[i].v1 = Vertices[i].TextureCoordinates[0].Y;
		ListOfFVFVertices[i].u2 = Vertices[i].TextureCoordinates[1].X;
		ListOfFVFVertices[i].v2 = Vertices[i].TextureCoordinates[1].Y;
	}

	SizeOfStruct = sizeof(FVFVERTEX);
	ptrListVert = ListOfFVFVertices;

	unsigned int SizeOfList = NumberOfVertices * SizeOfStruct;
	unsigned int SizeOfListIndices = NumberOfIndices * SizeOfIndexType;

	D3DFORMAT indexformat = D3DFMT_INDEX16;
	if (SizeOfIndexType <= sizeof(short)) {
		indexformat = D3DFMT_INDEX16;
	}
	else {
		indexformat = D3DFMT_INDEX32;
	}

	HRESULT hr = 0;

	hr = m_DeviceD3D9->CreateIndexBuffer(NumberOfIndices * SizeOfIndexType, 0, indexformat, D3DPOOL_MANAGED, &IndexBuffer, nullptr);

	if (FAILED(hr)) {
		return;
	}

	VOID* pINDICES;
	hr = IndexBuffer->Lock(0, SizeOfListIndices, (void**)&pINDICES, 0);
	if (FAILED(hr)) {
		return;
	}
	memcpy_s(pINDICES, SizeOfListIndices, Indices, SizeOfListIndices);
	hr = IndexBuffer->Unlock();
	if (FAILED(hr)) {
		return;
	}

	hr = m_DeviceD3D9->CreateVertexBuffer(SizeOfList,
		0, FVF,
		D3DPOOL_MANAGED, &VertexBuffer, NULL);

	if (FAILED(hr)) {
		//std::cout << "HAFQVQVN" << std::endl;
		return;
	}

	//std::cout << static_cast<int>(255.0f * (1.0f - Vertices[0].Transparency)) << std::endl;

	VOID* pVertices;
	hr = VertexBuffer->Lock(0, SizeOfList, (void**)&pVertices, 0);
	if (FAILED(hr)) {
		//std::cout << "HAFQVQVN" << std::endl;
		return;
	}
	memcpy_s(pVertices, SizeOfList, ptrListVert, SizeOfList);
	hr = VertexBuffer->Unlock();
	if (FAILED(hr)) {
		//std::cout << "18F1C1" << std::endl;
		return;
	}

	m_DeviceD3D9->SetFVF(FVF);
	m_DeviceD3D9->SetStreamSource(0, VertexBuffer, 0, SizeOfStruct);
	m_DeviceD3D9->SetIndices(IndexBuffer);
	m_DeviceD3D9->DrawIndexedPrimitive(d3dpt, 0, 0, NumberOfVertices, StartWithIndexNumber, PrimitiveCount);
	tn::SafeReleaseWinSys(&VertexBuffer);
	tn::SafeReleaseWinSys(&IndexBuffer);

	delete[] ListOfFVFVertices;
}

void tn::GraphicsDevice::Present() {
	m_DeviceD3D9->Present(nullptr, nullptr, nullptr, nullptr);
}

#pragma endregion