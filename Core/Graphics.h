#pragma once
#include "Misc.h"
#include "Physics.h"
#include "Windows.h"
#include "GraphicsStructs.h"
#include "GraphicsEnum.h"
#include "Shader.h"

#include <vector>
#include <type_traits>

#include <Windows.h>
#include <gdiplus.h>

#include <d3d9.h>
#include <d3d9helper.h>

#include <d3dcompiler.h>
#include <wincodec.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <DirectXColors.h>

#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "gdiplus.lib")
#pragma comment (lib, "d3dcompiler.lib")
#pragma comment (lib, "windowscodecs.lib")

#include "../Core/Texture.h"

namespace tn
{
	class Shader;
	class Texture;
	struct ShaderDescription;

	struct FunctionError;

	struct Light {
		Vector3 Position = { 0.0f, 0.0f, 0.0f }; //Excluding Directional light
		Vector3 Direction = { 0.0f, 0.0f, 0.0f }; //Excluding Point light
		Color3 Color = { 0,0,0 };
		Color3 AmbientColor = { 0,0,0 };
		float Transparency = 0.0f;
		float Range = 100.0f; //Excluding Directional light
		float InnerConeLightAngle = tn::ConvertDegreeToRadian(45.0f); //Value by Radian, Spotlight only
		float OuterConeLightAngle = tn::ConvertDegreeToRadian(90.0f); //Value by Radian, Spotlight only
		float ArrayOfAttenuation[3] = { 0.0f,0.0f,0.0f }; //Theo thứ tự: Constant, Normal, Square
		float Falloff = 1.0f;
		int Type = 1; //1 - Point, 2 - Spotlight, 3 - Directional
		unsigned long Index = 0;
		bool Enabled = true;
	};

	struct GraphicInterfaceSetupProperties {
		DepthStencilTypes DepthStencilType = DepthStencilTypes::D32; //Loại Stencil và độ sâu cho chiều z;
		bool DepthStencilEnable = true;
		int BackBufferCount = 1; 
		unsigned long ModifyFlags = 0; 
		unsigned int RefreshRateInHz = 0; //Dành cho màn hình có kích thước đầy đủ. Lấy số đó bằng 0 nếu Windowed đã bật - Used for Fullscreen. The value is 0 if Windowed is true
		unsigned int ScreenWidth = 1280; //Chiều dài của màn hình đồ họa - Width for graphic.
		unsigned int ScreenHeight = 720; //Chiều rộng của màn hình đồ họa - Height for graphic.
		unsigned int MultiSampleType = 0; //Lấy số từ 0 đến 15;
		SurfacesDimensionalTypes SurfacesDimensionalType = SDT_32Bits; //Loại của cảnh chiều cho đồ họa
		SwapChainEffects SwapChainEffect = SwapChainEffects::Discard; //Dạng Discard là dạng mặc định
		Window* UI = nullptr;
		bool Windowed = true;
		const wchar_t* StartingShaderDestination = L""; //.hlsl or .fx
	};

	class GraphicsDevice
	{
	public:

		//SYSTEM
		GraphicsDevice();
		
		void SetErrorData(tn::FunctionError* TargetErr);
		bool Initialize(GraphicInterfaceSetupProperties* TargetDesc);
		void Release();

		//TEXTURE

		bool ExportTexture(Texture* Texture);
		void SetTexture(unsigned int Level, Texture* Texture);

		//SHADER 

		bool LoadShader(Shader** Shader, ShaderDescription* ShaderDesc);
		void SetShader(Shader* Shader, ShaderTypes ShaderType);

		//MATRIX

		void SetWorldMatrix(tn::Matrix4x4 Matrix);
		void SetViewMatrix(tn::Matrix4x4 Matrix);
		void SetProjectionMatrix(tn::Matrix4x4 Matrix);

		//RENDER

		void ClearScreen(Color3 Color);
		void ClearDepth(float Depth);
		void BeginScene();
		void EndScene();
		
		void Draw(DrawTypes DrawType, tn::Vertex* Vertices, unsigned int NumberOfVertices, unsigned int StartWithVertexNumber, unsigned int PrimitiveCount);
		void Draw(DrawTypes DrawType, tn::Vertex* Vertices, void* Indices, unsigned int SizeOfIndexType, unsigned int NumberOfVertices, unsigned int NumberOfIndices, unsigned int StartWithIndexNumber, unsigned int PrimitiveCount);
		void Present();

	private:
		friend class Shader;
		friend class Image;
		friend class Texture;

		IDirect3D9* m_GraphicsD3D9;
		IDirect3DDevice9* m_DeviceD3D9;
		IDirect3DVertexDeclaration9* m_VertexDeclaration;
		IDirect3DVertexShader9* m_VertexShaderD3D9;
		IDirect3DPixelShader9* m_PixelShaderD3D9;

		tn::FunctionError* m_GraphicErr;
		GraphicInterfaceSetupProperties m_Prop;
		int Dimension = 0; //0 - 3D; 1 - 2D
	};
}