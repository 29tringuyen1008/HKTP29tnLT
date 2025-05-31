#pragma once
#include <string>
#include "Camera.h"

#include "GameObjectBase.h"
#include "Entities.h"
#include "../Core/Texture.h"
#include "../Core/Audio.h"
#include "../Core/Array.h"

class Program;
class Scene;
class Folder;
class Collider2D;
class GUIFolder;

struct Tag
{
	std::wstring Name;
};

struct PropertyValue
{
	std::wstring Name = L"";
	void* Value = nullptr;
};

/////////////////////////////////////////////////////////////////////

enum SceneTypes
{
	ST_Static = 0,
	ST_2DSurface = 1,
};

struct SceneDescription
{
	SceneTypes Type;
	float WorldWidth;
	float WorldHeight;
};

class MouseObject : public ProgramObject
{
public:
	MouseObject(Program* m_Program) : ProgramObject(m_Program) {};

	tn::Vector2 Position = { 0,0 };
	float MouseWheelDelta = 0.0f;

	bool LeftMouseUp = false;
	bool LeftMouseDown = false;
	bool LeftMouseHolded = false;

	bool RightMouseUp = false;
	bool RightMouseDown = false;
	bool RightMouseHolded = false;
};

class Folder : public ProgramObject
{
public:
	Folder(Program* m_Program) : ProgramObject(m_Program) {};
	ObjectClass GetType() override { return ObjectClass::Folder; };
};

class SceneFolder : public Folder //Special Object
{
public:
	SceneFolder(Program* m_Program) : Folder(m_Program) {};
	ObjectClass GetType() override { return ObjectClass::SceneFolder; };
};

class GUIFolder : public Folder //Special Object
{
public:
	GUIFolder(Program* m_Program) : Folder(m_Program) {};
	ObjectClass GetType() override { return ObjectClass::GUIFolder; };
};

class AssetFolder : public Folder //Special Object
{
public:
	AssetFolder(Program* m_Program) : Folder(m_Program) {};
	ObjectClass GetType() override { return ObjectClass::AssetFolder; };
};

class TerrainFolder : public Folder //Special Object
{
public:
	TerrainFolder(Program* m_Program) : Folder(m_Program) {};
	ObjectClass GetType() override { return ObjectClass::TerrainFolder; };
};

class Sprite;
class Workspace : public Folder //Special Object
{
public:
	Workspace(Program* m_Program) : Folder(m_Program) {};
	ObjectClass GetType() override { return ObjectClass::Workspace; };

	tn::NormalArray<Sprite*> Raycast(tn::Ray2D Ray);
};

class Scene : public ProgramObject
{
public:
	TerrainFolder* Terrain = nullptr; //DO NOT REPLACE
	Workspace* SceneWorkspace = nullptr; //DO NOT REPLACE

	tn::Texture* Skybox = nullptr;

	Scene(Program* m_Program) : ProgramObject(m_Program) {};
	Scene(Program* m_Program, SceneDescription Description) : ProgramObject(m_Program)
	{
		this->Description = Description;
		if (this->Description.Type == ST_2DSurface)
		{
			Terrain = new TerrainFolder(m_Program);
			Terrain->SetName(L"Terrain");
			Terrain->SetParent(this);

			SceneWorkspace = new Workspace(m_Program);
			SceneWorkspace->SetName(L"Workspace");
			SceneWorkspace->SetParent(this);
		}
	};
	ObjectClass GetType() override { return ObjectClass::Scene; };

	void SetDesc(SceneDescription Desc) {
		if (Desc.Type == ST_2DSurface and this->Description.Type == Desc.Type)
		{
			this->Description = Desc;
		}
		else
		{
			this->Description = Desc;
			if (this->Description.Type == ST_2DSurface)
			{
				Terrain = new TerrainFolder(m_Program);
				Terrain->SetName(L"Terrain");
				Terrain->SetParent(this);

				SceneWorkspace = new Workspace(m_Program);
				SceneWorkspace->SetName(L"Workspace");
				SceneWorkspace->SetParent(this);
			}
			else
			{
				if (Terrain) Terrain->Release();
				if (SceneWorkspace) SceneWorkspace->Release();
			}
		}
	}
	SceneDescription GetDesc()
	{
		return this->Description;
	}

	void AddTile(ProgramObject* ObjectAnyClass)
	{
		if (Description.Type == ST_2DSurface)
		{
			Terrain->AddChild(ObjectAnyClass);
		}
		else
		{
			this->AddChild(ObjectAnyClass);
		}
	}

	void AddObjectToScene(Object* ObjectAnyClass)
	{
		if (Description.Type == ST_2DSurface)
		{
			SceneWorkspace->AddChild(ObjectAnyClass);
		}
		else
		{
			this->AddChild(ObjectAnyClass);
		}
	}

	void Release() override
	{
		if (Terrain) Terrain->Release();
		if (SceneWorkspace) SceneWorkspace->Release();
		Object::Release();
	}
private:
	friend class Program;
	friend class MapEditor;

	SceneDescription Description = {};
};

#define TN_TEXTURE_ERR_NODEVICE -1
#define TN_TEXTURE_ERR_IMPORT -2
#define TN_TEXTURE_ERR_READYTORENDER -3

class Sprite : public ProgramObject
{
public:
	Sprite(Program* m_Program);

	tn::Vector2 Size;
	tn::Color3 Color;
	tn::Vector2 Position;
	float ZDepth = 0;
	float Alpha = 1.0f;

	tn::Rect GetRect();

	void SetTexture(tn::Texture* t);
	tn::Texture* GetTexture();

	void Update() override;
	ObjectClass GetType() override { return ObjectClass::Sprite; };
private:
	friend class Program;
	friend class Collider2D;

	tn::Vertex Vertices[4];
	tn::Texture* Texture;
};

class Entity : public Sprite
{
public:
	Entity(Program* m_Program) : Sprite(m_Program) {};
	~Entity() {};
	ObjectClass GetType() override { return ObjectClass::Entity; };
	EntityPrototype* ReferencePrototype = nullptr;
	tn::Rect CollisionBox = { 0, 0 };
	tn::Rect SelectionBox = { 0, 0 };
	float Height = 0.0f;
	float MaxHealth = 100.0f;
	float CurrentHealth = 100.0f;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class Draggable_UI_Frame;
class ScrollingFrame;

class UI_Frame : public ProgramObject
{
public:
	UI_Frame(Program* m_Program) : ProgramObject(m_Program) {};
	~UI_Frame();
	virtual int Load();
	virtual void Update() override;
	virtual void Release() override;
	virtual ObjectClass GetType() override { return ObjectClass::UI_Frame; };

	tn::GraphicsDevice* GraphicDevice = nullptr;
	tn::UDim2 Size;
	tn::UDim2 Position;
	tn::Color3 BackgroundColor = { 0,0,0 };
	float BackgroundTransparencyAlpha = 1.0f;

	bool Visible = true;
	float ZDepth = 1.0f;
	float Alpha = 1.0f;

	typedef void (*MouseFunc_) (Program* p, UI_Frame* Base, MouseObject* Mouse);
	void OnMouseTouched(MouseFunc_ Function);
	void OnMouseEnter(MouseFunc_ Function);
	void OnMouseLeave(MouseFunc_ Function);
protected:
	virtual void UpdateSize();
	virtual void UpdateTexture();
	virtual void UpdateVertices();
	friend class Program;
	friend class Draggable_UI_Frame;
	friend class ScrollingFrame;

	bool IsSurface = false;
	tn::Vector2 ParentBaseSize = {}, ParentBasePos = {}, ParentCanvaPos = {}, ParentCanvaSize = {}; 
	tn::Rect ParentTouchableRect = {};
	tn::Vector2 VisibleTextureCoordinate = {};

	tn::Vector2 AbsoluteSize = {};
	tn::Vector2 AbsolutePosition = {};
	float VisibleTextureCoordinatesRect[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
	tn::Rect TouchableRect = {};
	tn::Vector2 OldAbsoluteSize = {};
	bool IsItsDescendentAFolderAndAncestorOfGUIFolder = false;

	HDC hDC;
	Gdiplus::Bitmap* pBitmap_Gdiplus = nullptr;
	Gdiplus::Graphics* graphics = nullptr;

	tn::Texture* CompiledTexture = nullptr;
	//float VisibleTextureCoordinate[2] = {};

	bool IsCursorTouching = false;

	tn::Vertex Vertices[4];
	Camera* ReferenceCamera;

	MouseFunc_ MouseTouchedFunction = nullptr;
	MouseFunc_ MouseEnterFunction = nullptr;
	MouseFunc_ MouseLeaveFunction = nullptr;
};

class DragInterface
{
public:
	DragInterface() {};

	typedef void (*Function_) (Program* Program, DragInterface* Body, MouseObject* Mouse);
	virtual void ConnectDraggingFunctiom(Function_ Function);
	virtual void ConnectStopDraggingFunctiom(Function_ Function);
private:
	friend class Program;
	friend LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT hMsg, WPARAM wParam, LPARAM lParam);

	tn::Vector2 InitialMousePosOnUI = {};
	bool IsBeingDragged = false;

	Function_ ConnectedDragFunc = nullptr;
	Function_ ConnectedStopDragFunc = nullptr;
};

class DraggableFrame : public UI_Frame, public DragInterface
{
public:
	DraggableFrame(Program* m_Program) : UI_Frame(m_Program) {};
	ObjectClass GetType() override { return ObjectClass::DraggableFrame; };
};

class MessageBoxUI : public UI_Frame
{
public:
	std::wstring BarText = L"Text";
	std::wstring Font = L"Times New Roman";
	tn::Color3 BarTextColor = { 1.0f,1.0f,1.0f };
	tn::Color3 BarColor = { 0,0,0 };
	//X is scale, Y is offset
	tn::Vector2 BarSizeY = { 0, 0 };
	float BarAlpha = 1.0f;

	tn::Vector2 GetTotalScaledSize();
	tn::UDim2 GetFullUDim2Size();

	MessageBoxUI(Program* m_Program) : UI_Frame(m_Program) {};
	ObjectClass GetType() override { return ObjectClass::MessageBoxUI; };
protected:
	void UpdateSize() override;
	void UpdateTexture() override;
private:
	friend class Program;
	float FinalBarSizeY = 0.0f;
	tn::Vector2 InitialMousePosOnUI = {};
	bool IsBeingDragged = false;
};

class ScrollingFrame : public UI_Frame
{
public:
	ScrollingFrame(Program* m_Program);

	struct
	{
		bool Enabled = true;
		float ScrollRatio = 0.0f; //0 to 1
		tn::Color3 ScrollBarColor = { 0.0f, 0.0f, 0.0f };
		tn::Color3 ThumbColor = { 1.0f, 1.0f, 1.0f };
	} ScrollBar;

	struct
	{
		float Scale;
		float Offset;
	} AdditionalCanvaY;

	void MoveThumb(tn::Vector2 Offset);
	ObjectClass GetType() override { return ObjectClass::ScrollingFrame; };
protected:
	friend class UI_Frame;
	friend class Program;

	void UpdateSize() override;
	void UpdateTexture() override;

	//tn::Vector2 ScrollBarHorizontalVertices;
	tn::Vector2 CanvaSize = {};
	tn::Vector2 CanvaPos = {};

	tn::Vector2 ScrollPos = {};
	tn::Vector2 ScrollSize = {};

	tn::Vector2 TotalScrollPosRelativeToParent = {};

	tn::Vector2 ThumbPos = {};
	tn::Vector2 MinThumbPos = {};
	tn::Vector2 MaxThumbPos = {};

	tn::Vector2 ThumbPosRelativeToBaseUI = {};
	tn::Vector2 ThumbSize = {};
	tn::Vector2 ScrollBarVerticalVertices;

	tn::Vector2 InitialMousePosOnThumb = {};

	bool IsCreatedRecently = true;
	bool IsScrolling = false;
};

enum class ParagraphAlignmentEnum
{
	ParagraphAlignmentLeft = 0,
	ParagraphAlignmentMiddle = 1,
	ParagraphAlignmentRight = 2,
};

enum class TextAlignmentEnum
{
	TextAlignmentTop = 0,
	TextAlignmentCenter = 1,
	TextAlignmentBottom = 2,
};

class TextLabel : public UI_Frame
{
public:
	TextLabel(Program* m_Program) : UI_Frame(m_Program) {};
	ObjectClass GetType() override { return ObjectClass::TextLabel; };
	
	std::wstring Text = L"Text";
	bool Italic = false;
	bool Bold = false;
	tn::Color3 TextColor = { 1,1,1 };
	ParagraphAlignmentEnum ParagraphAlignment = ParagraphAlignmentEnum::ParagraphAlignmentMiddle;
	TextAlignmentEnum TextAlignment = TextAlignmentEnum::TextAlignmentCenter;
	std::wstring Font = L"Times New Roman";
	bool WordBreak = false;
	float TextSize = 12.0f;
protected:
	void UpdateTexture() override;
private:
	friend class Program;
};

class ImageLabel : public UI_Frame
{
public:
	ImageLabel(Program* m_Program) : UI_Frame(m_Program) {};
	ObjectClass GetType() override { return ObjectClass::ImageLabel; };

	tn::Texture* TargetTexture = nullptr;
protected:
	virtual void UpdateTexture() override;
private:
	friend class Program;
};

class TextButton : public UI_Frame
{
public:
	TextButton(Program* m_Program) : UI_Frame(m_Program) {};
	ObjectClass GetType() override { return ObjectClass::TextButton; };

	void OnMouseButton1Click(void (*Function) (Program* Program, TextButton* Body, MouseObject* Mouse));
	void OnMouseButton1Down(void (*Function) (Program* Program, TextButton* Body, MouseObject* Mouse));
	void OnMouseButton1Up(void (*Function) (Program* Program, TextButton* Body, MouseObject* Mouse));

	std::wstring Text = L"Text";
	bool Italic = false;
	bool Bold = false;
	tn::Color3 TextColor = { 1,1,1 };
	ParagraphAlignmentEnum ParagraphAlignment = ParagraphAlignmentEnum::ParagraphAlignmentMiddle;
	TextAlignmentEnum TextAlignment = TextAlignmentEnum::TextAlignmentCenter;
	std::wstring Font = L"Times New Roman";
	bool WordBreak = false;
	float TextSize = 0.0f;
protected:
	void UpdateTexture() override;
private:
	friend class Program;

	bool ButtonPressed = false;

	void (*OnMouseButton1ClickFunction) (Program* Program, TextButton* Body, MouseObject* MousePos) = nullptr;
	void (*OnMouseButton1DownFunction) (Program* Program, TextButton* Body, MouseObject* MousePos) = nullptr;
	void (*OnMouseButton1UpFunction) (Program* Program, TextButton* Body, MouseObject* MousePos) = nullptr;
};

class ImageButton : public TextButton
{
public:
	ImageButton(Program* m_Program) : TextButton(m_Program) {};
	ObjectClass GetType() override { return ObjectClass::ImageButton; };

	tn::Texture* TargetTexture = nullptr;
protected:
	void UpdateTexture() override;
private:
	friend class Program;
};

class Collider2D : public ProgramObject
{
public:
	Collider2D(Program* m_Program);
	ObjectClass GetType() override { return ObjectClass::Collider2D; };

	Object* BaseObject = nullptr;

	ObjArray GetIntersectionsWithin(ProgramObject* Object);
	bool IsOverlappedWith(Object* Object);
	bool IsOverlappedWithUI(UI_Frame* Object);
	void OnMouseClick(void (*Function) (Collider2D* Body, MouseObject* Mouse));
private:
	friend class Program;
	void (*MouseClickFunc) (Collider2D* Body, MouseObject* Mouse);
	bool IsCursorTouching = false;
	bool ColliderMousePressed = false;
};