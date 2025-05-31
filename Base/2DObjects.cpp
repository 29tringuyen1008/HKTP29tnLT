#include "GameObject.h"
#include "Base.h"
#include <gdiplus.h>

Sprite::Sprite(Program* m_Program) : ProgramObject(m_Program)
{

}

tn::Rect Sprite::GetRect()
{
	tn::Rect rt = {};
	rt.Left = -Size.X / 2;
	rt.Right = Size.X / 2;
	rt.Top = -Size.Y / 2;
	rt.Bottom = Size.Y / 2;
	tn::TransformRect(&rt, this->Position);
	return rt;
}

void Sprite::SetTexture(tn::Texture* t)
{
	this->Texture = t;
}

tn::Texture* Sprite::GetTexture()
{
	return this->Texture;
}

void Sprite::Update()
{
	Vertices[0] = { -Size.X / 2, -Size.Y / 2, ZDepth, Color.Red, Color.Green, Color.Blue, Alpha, 0.0f, 0.0f };
	Vertices[1] = { Size.X / 2, -Size.Y / 2, ZDepth, Color.Red, Color.Green, Color.Blue, Alpha, 1.0f, 0.0f };
	Vertices[2] = { -Size.X / 2, Size.Y / 2, ZDepth, Color.Red, Color.Green, Color.Blue, Alpha, 0.0f, 1.0f };
	Vertices[3] = { Size.X / 2, Size.Y / 2, ZDepth, Color.Red, Color.Green, Color.Blue, Alpha, 1.0f, 1.0f };
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UI_Frame::~UI_Frame()
{

}

int UI_Frame::Load()
{
	if (this->CompiledTexture == nullptr)
	{
		this->CompiledTexture = new tn::Texture();
		this->CompiledTexture->TextureType = TextureTypes::Bitmap_GDIPlus;
		this->CompiledTexture->UseFullSize = true;
	}
	else
	{
		return -1;
	}

	return 0;
}

void UI_Frame::UpdateSize()
{
	const float MIN = 1.0f;
	const float AVG = 1.5f;
	const float MAX = 2.0f;
	if (this->Parent != nullptr)
	{
		if (this->Parent->GetType() == ObjectClass::GUIFolder or IsItsDescendentAFolderAndAncestorOfGUIFolder == true)
		{
			if (this->ReferenceCamera != nullptr)
			{
				ParentBaseSize = this->ReferenceCamera->ViewportSize;
				ParentCanvaSize = ParentBaseSize;
				ParentBasePos = { 0.0f };
				ParentCanvaPos = { 0.0f };
				ParentTouchableRect = {
					0,
					ParentBaseSize.X,
					0,
					ParentBaseSize.Y
				};
			}
			else
			{
				ParentBaseSize = { 0,0 };
				ParentCanvaSize = { 0,0 };
				ParentBasePos = { 0.0f };
				ParentCanvaPos = { 0.0f };
				ParentTouchableRect = {};
			}
		}
		else if (this->Parent->GetType() == ObjectClass::Scene)
		{
			this->IsSurface = true;
			if (this->ReferenceCamera != nullptr)
			{
				ParentBaseSize = this->ReferenceCamera->ViewportSize;
				ParentCanvaSize = ParentBaseSize;
				ParentBasePos = { 0.0f };
				ParentCanvaPos = { 0.0f };
				ParentTouchableRect = {
					0, this->ReferenceCamera->ViewportSize.X, 0, this->ReferenceCamera->ViewportSize.Y
				};
			}
			else
			{
				ParentBaseSize = { 0,0 };
				ParentCanvaSize = { 0,0 };
				ParentBasePos = { 0.0f };
				ParentCanvaPos = { 0.0f };
				ParentTouchableRect = {};
			}
		}
		else if (this->Parent->GetType() == ObjectClass::ScrollingFrame)
		{
			ScrollingFrame* sf = dynamic_cast<ScrollingFrame*>(this->Parent);
			if (sf != nullptr)
			{
				ParentBaseSize = sf->AbsoluteSize;
				ParentBasePos = sf->AbsolutePosition;
				ParentCanvaPos = sf->CanvaPos;
				ParentCanvaSize = sf->CanvaSize;
				ParentTouchableRect = this->TouchableRect;
			}
		}
		else if (this->Parent->GetType() == ObjectClass::Sprite)
		{
			Sprite* SpriteA = dynamic_cast<Sprite*>(this->Parent);
			if (SpriteA != nullptr)
			{
				ParentBaseSize = SpriteA->Size + (SpriteA->Size / 2);
				ParentBasePos = SpriteA->Position - (SpriteA->Position / 2);
				ParentCanvaPos = {};
				ParentCanvaSize = {};
				ParentTouchableRect = {
					ParentBasePos.X, ParentBasePos.X + ParentBaseSize.X,
					ParentBasePos.Y,
					ParentBasePos.Y + ParentBaseSize.Y
				};
			}
		}
		else
		{
			UI_Frame* uf = dynamic_cast<UI_Frame*>(this->Parent);
			if (uf != nullptr)
			{
				ParentBaseSize = uf->AbsoluteSize;
				ParentBasePos = uf->AbsolutePosition;
				ParentCanvaPos = ParentBasePos;
				ParentCanvaSize = ParentBaseSize;
				ParentTouchableRect = uf->TouchableRect;
			}
		}
	}
	else
	{
		ParentBaseSize = this->ReferenceCamera->ViewportSize;
		ParentCanvaSize = ParentBaseSize;
		ParentBasePos = { 0.0f };
		ParentCanvaPos = { 0.0f };
		ParentTouchableRect = {
			0,
			ParentBaseSize.X,
			0,
			ParentBaseSize.Y
		};
	}

	this->AbsoluteSize = {
		this->Size.Offset.X + (ParentCanvaSize.X * this->Size.Scale.X),
		this->Size.Offset.Y + (ParentCanvaSize.Y * this->Size.Scale.Y),
	};

	this->AbsolutePosition = {
		ParentCanvaPos.X + this->Position.Offset.X + (ParentCanvaSize.X * this->Position.Scale.X),
		ParentCanvaPos.Y + this->Position.Offset.Y + (ParentCanvaSize.Y * this->Position.Scale.Y),
	};
}

void UI_Frame::UpdateVertices()
{
	if (this->IsSurface)
	{
		Vertices[0] = { 0, 0, ZDepth, BackgroundColor.Red, BackgroundColor.Green, BackgroundColor.Blue, Alpha, 0.0f, 0.0f };
		Vertices[1] = { AbsoluteSize.X, 0, ZDepth, BackgroundColor.Red, BackgroundColor.Green, BackgroundColor.Blue, Alpha, 1.0f, 0.0f };
		Vertices[2] = { 0, AbsoluteSize.Y, ZDepth, BackgroundColor.Red, BackgroundColor.Green, BackgroundColor.Blue, Alpha, 0.0f, 1.0f };
		Vertices[3] = { AbsoluteSize.X, AbsoluteSize.Y, ZDepth, BackgroundColor.Red, BackgroundColor.Green, BackgroundColor.Blue, Alpha, 1.0f, 1.0f };
	}
	else
	{
		this->TouchableRect.Left = this->AbsolutePosition.X;
		this->TouchableRect.Top = this->AbsolutePosition.Y;
		this->TouchableRect.Right = this->AbsolutePosition.X + this->AbsoluteSize.X;
		this->TouchableRect.Bottom = this->AbsolutePosition.Y + this->AbsoluteSize.Y;

		VisibleTextureCoordinatesRect[0] = 0.0f;
		VisibleTextureCoordinatesRect[1] = 0.0f;
		VisibleTextureCoordinatesRect[2] = 1.0f;
		VisibleTextureCoordinatesRect[3] = 1.0f;

		if (this->AbsolutePosition.X < this->ParentBasePos.X)
		{
			float clippedPixels = this->ParentBasePos.X - this->TouchableRect.Left;
			this->TouchableRect.Left = this->ParentBasePos.X;

			float uClipStart = clippedPixels / this->AbsoluteSize.X;
			VisibleTextureCoordinatesRect[0] = uClipStart;
		}

		if ((this->AbsolutePosition.X + this->AbsoluteSize.X) > (this->ParentBasePos.X + this->ParentBaseSize.X))
		{
			float clippedPixels = this->TouchableRect.Right - (this->ParentBasePos.X + this->ParentBaseSize.X);
			this->TouchableRect.Right = this->ParentBasePos.X + this->ParentBaseSize.X;

			float uClipEnd = 1.0f - (clippedPixels / this->AbsoluteSize.X);
			VisibleTextureCoordinatesRect[2] = uClipEnd;
		}

		if (this->AbsolutePosition.Y < this->ParentBasePos.Y)
		{
			float clippedPixels = this->ParentBasePos.Y - this->TouchableRect.Top;
			this->TouchableRect.Top = this->ParentBasePos.Y;

			float vClipStart = clippedPixels / this->AbsoluteSize.Y;
			VisibleTextureCoordinatesRect[1] = vClipStart;
		}

		if ((this->AbsolutePosition.Y + this->AbsoluteSize.Y) > (this->ParentBasePos.Y + this->ParentBaseSize.Y))
		{
			float clippedPixels = this->TouchableRect.Bottom - (this->ParentBasePos.Y + this->ParentBaseSize.Y);
			this->TouchableRect.Bottom = this->ParentBasePos.Y + this->ParentBaseSize.Y;

			float vClipEnd = 1.0f - (clippedPixels / this->AbsoluteSize.Y);
			VisibleTextureCoordinatesRect[3] = vClipEnd;
		}

		//std::cout << this->TouchableRect.Left << " " << this->TouchableRect.Right << " " << this->TouchableRect.Top << " " << this->TouchableRect.Bottom << std::endl;
		Vertices[0] = { this->TouchableRect.Left, this->TouchableRect.Top, ZDepth, BackgroundColor.Red, BackgroundColor.Green, BackgroundColor.Blue, Alpha, VisibleTextureCoordinatesRect[0], VisibleTextureCoordinatesRect[1] };
		Vertices[1] = { this->TouchableRect.Right, this->TouchableRect.Top, ZDepth, BackgroundColor.Red, BackgroundColor.Green, BackgroundColor.Blue, Alpha, VisibleTextureCoordinatesRect[2], VisibleTextureCoordinatesRect[1] };
		Vertices[2] = { this->TouchableRect.Left, this->TouchableRect.Bottom, ZDepth, BackgroundColor.Red, BackgroundColor.Green, BackgroundColor.Blue, Alpha, VisibleTextureCoordinatesRect[0], VisibleTextureCoordinatesRect[3] };
		Vertices[3] = { this->TouchableRect.Right, this->TouchableRect.Bottom, ZDepth, BackgroundColor.Red, BackgroundColor.Green, BackgroundColor.Blue, Alpha, VisibleTextureCoordinatesRect[2], VisibleTextureCoordinatesRect[3] };
	}
}

void UI_Frame::Update()
{
	if (this->GraphicDevice == nullptr) return;
	if (this->CompiledTexture == nullptr) return;

	this->UpdateSize();
	this->UpdateVertices();

	RECT rt = {};
	rt.right = static_cast<LONG>(this->AbsoluteSize.X);
	rt.bottom = static_cast<LONG>(this->AbsoluteSize.Y);

	if (this->pBitmap_Gdiplus == nullptr)
	{
		this->pBitmap_Gdiplus = new Gdiplus::Bitmap(this->AbsoluteSize.X, this->AbsoluteSize.Y, PixelFormat32bppARGB);
	}
	else
	{
		Gdiplus::SizeF sizeF = {};
		this->pBitmap_Gdiplus->GetPhysicalDimension(&sizeF);
		if (sizeF.Width != this->AbsoluteSize.X or sizeF.Width != this->AbsoluteSize.Y)
		{
			this->pBitmap_Gdiplus->SetResolution(this->AbsoluteSize.X, this->AbsoluteSize.Y);
		}
	}

	if (this->graphics == nullptr)
	{
		if (pBitmap_Gdiplus != nullptr)
		{
			graphics = new Gdiplus::Graphics(pBitmap_Gdiplus);
		}
	}
	else
	{
		this->UpdateTexture();
	}
}

void UI_Frame::UpdateTexture()
{
	this->graphics->Clear(Gdiplus::Color(BackgroundTransparencyAlpha * 255,
		(this->BackgroundColor.Red * 255),
		(this->BackgroundColor.Green * 255),
		(this->BackgroundColor.Blue * 255)
	));
	this->CompiledTexture->ImportBitmap_GDIPlus(pBitmap_Gdiplus);

	if (this->OldAbsoluteSize != AbsoluteSize)
	{
		this->CompiledTexture->TextureType = TextureTypes::Bitmap_GDIPlus;
		this->CompiledTexture->TextureSize = this->AbsoluteSize;
		this->CompiledTexture->SourceDestination[0] = { 0, 0 };
		this->CompiledTexture->SourceDestination[1] = this->AbsoluteSize;

		this->OldAbsoluteSize = this->AbsoluteSize;
	}
	GraphicDevice->ExportTexture(this->CompiledTexture);
}

void UI_Frame::Release()
{
	if (this->CompiledTexture != nullptr)
	{
		tn::SafeReleaseWinSys(&this->CompiledTexture);
	}

	if (graphics != nullptr)
	{
		if (m_Program != nullptr)
		{
			if (m_Program->CheckGDIPlusRun() == true)
			{
				delete graphics;
				graphics = nullptr;
			}
		}
	}

	if (this->pBitmap_Gdiplus != nullptr)
	{
		if (m_Program != nullptr)
		{
			if (m_Program->CheckGDIPlusRun() == true)
			{
				delete pBitmap_Gdiplus;
				pBitmap_Gdiplus = nullptr;
			}
		}
	}

	ProgramObject::Release();
}

void UI_Frame::OnMouseTouched(UI_Frame::MouseFunc_ Function)
{
	this->MouseTouchedFunction = Function;
}

void UI_Frame::OnMouseEnter(UI_Frame::MouseFunc_ Function)
{
	this->MouseEnterFunction = Function;
}

void UI_Frame::OnMouseLeave(UI_Frame::MouseFunc_ Function)
{
	this->MouseLeaveFunction = Function;
}

//

void DragInterface::ConnectDraggingFunctiom(DraggableFrame::Function_ Function)
{
	this->ConnectedDragFunc = Function;
}

void DragInterface::ConnectStopDraggingFunctiom(DraggableFrame::Function_ Function)
{
	this->ConnectedStopDragFunc = Function;
}

//

void MessageBoxUI::UpdateSize()
{
	UI_Frame::UpdateSize();

	this->FinalBarSizeY = this->BarSizeY.Y + (ParentBaseSize.Y * this->BarSizeY.X);

	this->AbsoluteSize = {
		this->Size.Offset.X + (ParentBaseSize.X * this->Size.Scale.X),
		this->Size.Offset.Y + FinalBarSizeY,
	};

	this->AbsolutePosition = {
		ParentBasePos.X + this->Position.Offset.X + (ParentBaseSize.X * this->Position.Scale.X),
		ParentBasePos.Y + this->Position.Offset.Y + (ParentBaseSize.Y * this->Position.Scale.Y),
	};
}

void MessageBoxUI::UpdateTexture()
{
	graphics->Clear(Gdiplus::Color(BackgroundTransparencyAlpha * 255,
		(this->BackgroundColor.Red * 255),
		(this->BackgroundColor.Green * 255),
		(this->BackgroundColor.Blue * 255)
	));

	Gdiplus::SolidBrush BarBrush(Gdiplus::Color(
		BarAlpha * 255,
		(this->BarColor.Red * 255),
		(this->BarColor.Green * 255),
		(this->BarColor.Blue * 255)
	));

	Gdiplus::RectF BarRect(0, 0, AbsoluteSize.X, this->BarSizeY.Y + (this->BarSizeY.X * this->AbsoluteSize.Y));
	graphics->FillRectangle(&BarBrush, BarRect);

	graphics->SetTextRenderingHint(Gdiplus::TextRenderingHintClearTypeGridFit);

	Gdiplus::FontFamily fontFamily(this->Font.data());
	if (fontFamily.GetLastStatus() == Gdiplus::Status::Ok)
	{
		Gdiplus::Font font(&fontFamily, 25, 0, Gdiplus::UnitPixel);
		if (font.GetLastStatus() == Gdiplus::Status::Ok)
		{
			Gdiplus::SolidBrush textbrush(Gdiplus::Color(
				255,
				abs(this->BarTextColor.Red * 255),
				abs(this->BarTextColor.Green * 255),
				abs(this->BarTextColor.Blue * 255)
			));
			if (textbrush.GetLastStatus() == Gdiplus::Status::Ok)
			{
				Gdiplus::StringFormat stringFormat = {};
				stringFormat.SetLineAlignment(Gdiplus::StringAlignmentCenter);
				if (stringFormat.GetLastStatus() == Gdiplus::Status::Ok)
				{
					graphics->DrawString(this->BarText.data(), this->BarText.length(), &font, BarRect, &stringFormat, &textbrush);

					this->CompiledTexture->ImportBitmap_GDIPlus(UI_Frame::pBitmap_Gdiplus);

					if (this->OldAbsoluteSize != AbsoluteSize)
					{
						this->CompiledTexture->TextureType = TextureTypes::Bitmap_GDIPlus;
						this->CompiledTexture->TextureSize = this->AbsoluteSize;
						this->CompiledTexture->SourceDestination[0] = { 0, 0 };
						this->CompiledTexture->SourceDestination[1] = this->AbsoluteSize;

						this->OldAbsoluteSize = this->AbsoluteSize;
					}
					GraphicDevice->ExportTexture(this->CompiledTexture);
				}
			}
		}
	}
}

tn::Vector2 MessageBoxUI::GetTotalScaledSize()
{
	return this->AbsoluteSize;
}

tn::UDim2 MessageBoxUI::GetFullUDim2Size()
{
	tn::UDim2 FullUdim2 = {};
	FullUdim2.Scale.X = this->Size.Scale.X;
	FullUdim2.Scale.Y = this->Size.Scale.Y + this->BarSizeY.X;
	FullUdim2.Offset.X = this->Size.Offset.X;
	FullUdim2.Offset.Y = this->Size.Offset.Y + this->BarSizeY.Y;
	return FullUdim2;
}

//

ScrollingFrame::ScrollingFrame(Program* m_Program) : UI_Frame(m_Program)
{

}

void ScrollingFrame::MoveThumb(tn::Vector2 Offset)
{
	if (ScrollBar.Enabled == true)
	{
		if (Offset.Y < this->MinThumbPos.Y)
		{
			this->ThumbPos.Y = this->MinThumbPos.Y;
			this->ScrollBar.ScrollRatio = 0.0f;
		}
		else if (Offset.Y > this->MaxThumbPos.Y)
		{
			this->ThumbPos.Y = this->MaxThumbPos.Y;
			this->ScrollBar.ScrollRatio = 1.0f;
		}
		else
		{
			this->ThumbPos.Y = Offset.Y;
			float UnRoundedRatio = (this->ThumbPos.Y - this->MinThumbPos.Y) / (this->MaxThumbPos.Y - this->MinThumbPos.Y);
			this->ScrollBar.ScrollRatio = std::round(UnRoundedRatio * 100.0f) / 100.0f;
		}
	}
}

void ScrollingFrame::UpdateSize()
{
	UI_Frame::UpdateSize();

	this->CanvaSize = {
		this->AbsoluteSize.X,
		this->AbsoluteSize.Y + this->AdditionalCanvaY.Offset + (ParentBaseSize.Y * this->AdditionalCanvaY.Scale)
	};

	///////////////

	this->ScrollPos = { this->AbsoluteSize.X - 30, 5 };
	this->ScrollSize = { 25, this->AbsoluteSize.Y - 10 };

	this->ThumbPosRelativeToBaseUI = { this->AbsolutePosition.X + ThumbPos.X, this->AbsolutePosition.Y + ThumbPos.Y };
	this->ThumbSize = { 25, this->ScrollSize.Y * (this->AbsoluteSize.Y / this->CanvaSize.Y) };

	this->MinThumbPos = { this->AbsoluteSize.X - 30, ScrollPos.Y };
	this->MaxThumbPos = { this->MinThumbPos.X, this->ScrollSize.Y - this->ThumbSize.Y + this->ScrollPos.Y };

	if (this->IsCreatedRecently == true)
	{
		this->ThumbPos = this->MinThumbPos;
		this->IsCreatedRecently = false;
	}

	this->CanvaPos =
	{
		this->AbsolutePosition.X,
		this->AbsolutePosition.Y - (this->CanvaSize.Y * this->ScrollBar.ScrollRatio)
	};

	this->TotalScrollPosRelativeToParent = ScrollPos + this->AbsolutePosition;

	/*
	Vertices[0] = { this->AbsolutePosition.X,					 this->AbsolutePosition.Y,					  ZDepth, this->BackgroundColor.Red, this->BackgroundColor.Green, this->BackgroundColor.Blue, Alpha, 0.0f, 0.0f };
	Vertices[1] = { this->AbsolutePosition.X + this->AbsoluteSize.X, this->AbsolutePosition.Y,					  ZDepth, this->BackgroundColor.Red, this->BackgroundColor.Green, this->BackgroundColor.Blue, Alpha, 1.0f, 0.0f };
	Vertices[2] = { this->AbsolutePosition.X,					 this->AbsolutePosition.Y + this->AbsoluteSize.Y, ZDepth, this->BackgroundColor.Red, this->BackgroundColor.Green, this->BackgroundColor.Blue, Alpha, 0.0f, 1.0f };
	Vertices[3] = { this->AbsolutePosition.X + this->AbsoluteSize.X, this->AbsolutePosition.Y + this->AbsoluteSize.Y, ZDepth, this->BackgroundColor.Red, this->BackgroundColor.Green, this->BackgroundColor.Blue, Alpha, 1.0f, 1.0f };
	*/
}

void ScrollingFrame::UpdateTexture()
{
	graphics->Clear(Gdiplus::Color(BackgroundTransparencyAlpha * 255,
		(this->BackgroundColor.Red * 255),
		(this->BackgroundColor.Green * 255),
		(this->BackgroundColor.Blue * 255)
	));

	if (ScrollBar.Enabled == true)
	{
		Gdiplus::Pen pen(Gdiplus::Color(255,
			(this->ScrollBar.ScrollBarColor.Red * 255),
			(this->ScrollBar.ScrollBarColor.Green * 255),
			(this->ScrollBar.ScrollBarColor.Blue * 255)
		), 2.0f);

		Gdiplus::SolidBrush ThumbColor(Gdiplus::Color(255,
			(this->ScrollBar.ThumbColor.Red * 255),
			(this->ScrollBar.ThumbColor.Green * 255),
			(this->ScrollBar.ThumbColor.Blue * 255)
		));

		float clampedratio = tn::math::clamp(this->ScrollBar.ScrollRatio, 0.0f, 1.0f);

		/*
		Gdiplus::RectF thumbrt = { this->AbsoluteSize.X - 30, 5, 25, (this->AbsoluteSize.Y * (this->AbsoluteSize.Y / this->CanvaSize.Y)) };
		Gdiplus::RectF ScrollRect = { this->AbsoluteSize.X - 30, 5, 25, this->AbsoluteSize.Y - 10 };
		thumbrt.Y = 5 + ((ScrollRect.Height - thumbrt.Height) * clampedratio);

		this->ThumbPosRelative = { this->AbsolutePosition.X + thumbrt.X, this->AbsolutePosition.Y + thumbrt.Y };
		this->ThumbSize = { thumbrt.Width, thumbrt.Height };

		this->TotalScrollPos = { ScrollRect.X, ScrollRect.Y };
		this->TotalScrollSize = { ScrollRect.Width, ScrollRect.Height };
		*/

		Gdiplus::RectF thumbrt = { this->ThumbPos.X, this->ThumbPos.Y, this->ThumbSize.X, this->ThumbSize.Y };
		Gdiplus::RectF ScrollRect = { this->ScrollPos.X, this->ScrollPos.Y, ScrollSize.X, ScrollSize.Y };

		graphics->DrawRectangle(&pen, ScrollRect);
		graphics->FillRectangle(&ThumbColor, thumbrt);
	}

	this->CompiledTexture->ImportBitmap_GDIPlus(UI_Frame::pBitmap_Gdiplus);
	if (this->OldAbsoluteSize != AbsoluteSize)
	{
		this->CompiledTexture->TextureType = TextureTypes::Bitmap_GDIPlus;
		this->CompiledTexture->TextureSize = this->AbsoluteSize;
		this->CompiledTexture->SourceDestination[0] = { 0, 0 };
		this->CompiledTexture->SourceDestination[1] = this->AbsoluteSize;
		this->OldAbsoluteSize = this->AbsoluteSize;
	}
	GraphicDevice->ExportTexture(this->CompiledTexture);
}

//

void TextLabel::UpdateTexture()
{
	graphics->SetTextRenderingHint(Gdiplus::TextRenderingHintClearTypeGridFit);

	graphics->Clear(Gdiplus::Color(BackgroundTransparencyAlpha * 255,
		(this->BackgroundColor.Red * 255),
		(this->BackgroundColor.Green * 255),
		(this->BackgroundColor.Blue * 255)
	));

	Gdiplus::StringFormat stringFormat = {};
	if (this->ParagraphAlignment == ParagraphAlignmentEnum::ParagraphAlignmentLeft) stringFormat.SetAlignment(Gdiplus::StringAlignmentNear);
	else if (this->ParagraphAlignment == ParagraphAlignmentEnum::ParagraphAlignmentMiddle) stringFormat.SetAlignment(Gdiplus::StringAlignmentCenter);
	else if (this->ParagraphAlignment == ParagraphAlignmentEnum::ParagraphAlignmentRight) stringFormat.SetAlignment(Gdiplus::StringAlignmentFar);

	if (this->TextAlignment == TextAlignmentEnum::TextAlignmentTop) stringFormat.SetLineAlignment(Gdiplus::StringAlignmentNear);
	else if (this->TextAlignment == TextAlignmentEnum::TextAlignmentCenter) stringFormat.SetLineAlignment(Gdiplus::StringAlignmentCenter);
	else if (this->TextAlignment == TextAlignmentEnum::TextAlignmentBottom) stringFormat.SetLineAlignment(Gdiplus::StringAlignmentFar);

	if (this->WordBreak == false) stringFormat.SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap);

	INT BoldItalic = 0;
	if (this->Bold == true and this->Italic == false)
	{
		BoldItalic = Gdiplus::FontStyleBold;
	}
	else if (this->Bold == false and this->Italic == true)
	{
		BoldItalic = Gdiplus::FontStyleItalic;
	}
	else if (this->Bold == true and this->Italic == true)
	{
		BoldItalic = Gdiplus::FontStyleBoldItalic;
	}

	Gdiplus::FontFamily fontFamily(this->Font.data());
	if (fontFamily.GetLastStatus() == Gdiplus::Status::Ok)
	{
		Gdiplus::Font font(&fontFamily, this->TextSize, BoldItalic, Gdiplus::UnitPixel);
		if (font.GetLastStatus() == Gdiplus::Status::Ok)
		{
			Gdiplus::SolidBrush textbrush(Gdiplus::Color(
				255,
				abs(this->TextColor.Red * 255),
				abs(this->TextColor.Green * 255),
				abs(this->TextColor.Blue * 255)
			));
			if (textbrush.GetLastStatus() == Gdiplus::Status::Ok)
			{
				Gdiplus::RectF layoutRect(0, 0, this->AbsoluteSize.X, this->AbsoluteSize.Y);

				graphics->DrawString(Text.data(), Text.length(), &font, layoutRect, &stringFormat, &textbrush);
				this->CompiledTexture->ImportBitmap_GDIPlus(pBitmap_Gdiplus);

				if (this->OldAbsoluteSize != AbsoluteSize)
				{
					this->CompiledTexture->TextureType = TextureTypes::Bitmap_GDIPlus;
					this->CompiledTexture->TextureSize = this->AbsoluteSize;
					this->CompiledTexture->SourceDestination[0] = { 0, 0 };
					this->CompiledTexture->SourceDestination[1] = this->AbsoluteSize;

					this->OldAbsoluteSize = this->AbsoluteSize;
				}
				GraphicDevice->ExportTexture(this->CompiledTexture);
			}
		}
	}
}

void TextButton::UpdateTexture()
{
	float darkvalue;
	if (this->IsCursorTouching == true)
	{
		darkvalue = 10;
	}
	else
	{
		darkvalue = 0;
	}

	graphics->SetTextRenderingHint(Gdiplus::TextRenderingHintClearTypeGridFit);

	graphics->Clear(Gdiplus::Color(BackgroundTransparencyAlpha * 255,
		abs(this->BackgroundColor.Red * 255 - darkvalue),
		abs(this->BackgroundColor.Green * 255 - darkvalue),
		abs(this->BackgroundColor.Blue * 255 - darkvalue)
	));

	Gdiplus::StringFormat stringFormat = {};
	if (this->ParagraphAlignment == ParagraphAlignmentEnum::ParagraphAlignmentLeft) stringFormat.SetAlignment(Gdiplus::StringAlignmentNear);
	else if (this->ParagraphAlignment == ParagraphAlignmentEnum::ParagraphAlignmentMiddle) stringFormat.SetAlignment(Gdiplus::StringAlignmentCenter);
	else if (this->ParagraphAlignment == ParagraphAlignmentEnum::ParagraphAlignmentRight) stringFormat.SetAlignment(Gdiplus::StringAlignmentFar);

	if (this->TextAlignment == TextAlignmentEnum::TextAlignmentTop) stringFormat.SetLineAlignment(Gdiplus::StringAlignmentNear);
	else if (this->TextAlignment == TextAlignmentEnum::TextAlignmentCenter) stringFormat.SetLineAlignment(Gdiplus::StringAlignmentCenter);
	else if (this->TextAlignment == TextAlignmentEnum::TextAlignmentBottom) stringFormat.SetLineAlignment(Gdiplus::StringAlignmentFar);

	if (this->WordBreak == false) stringFormat.SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap);

	INT BoldItalic = Gdiplus::FontStyleRegular;
	if (this->Bold == true and this->Italic == false)
	{
		BoldItalic = Gdiplus::FontStyleBold;
	}
	else if (this->Bold == false and this->Italic == true)
	{
		BoldItalic = Gdiplus::FontStyleItalic;
	}
	else if (this->Bold == true and this->Italic == true)
	{
		BoldItalic = Gdiplus::FontStyleBoldItalic;
	}

	Gdiplus::FontFamily fontFamily(this->Font.data());
	if (fontFamily.GetLastStatus() == Gdiplus::Status::Ok) {
		Gdiplus::Font font(&fontFamily, this->TextSize, BoldItalic, Gdiplus::UnitPixel);
		if (font.GetLastStatus() == Gdiplus::Status::Ok) {
			Gdiplus::SolidBrush textbrush(Gdiplus::Color(
				255,
				abs(this->TextColor.Red * 255),
				abs(this->TextColor.Green * 255),
				abs(this->TextColor.Blue * 255)
			));
			if (textbrush.GetLastStatus() == Gdiplus::Status::Ok) {
				Gdiplus::RectF layoutRect(0, 0, this->AbsoluteSize.X, this->AbsoluteSize.Y);
				graphics->DrawString(Text.data(), Text.length(), &font, layoutRect, &stringFormat, &textbrush);
				this->CompiledTexture->ImportBitmap_GDIPlus(pBitmap_Gdiplus);

				if (this->OldAbsoluteSize != AbsoluteSize)
				{
					this->CompiledTexture->TextureType = TextureTypes::Bitmap_GDIPlus;
					this->CompiledTexture->TextureSize = this->AbsoluteSize;
					this->CompiledTexture->SourceDestination[0] = { 0, 0 };
					this->CompiledTexture->SourceDestination[1] = this->AbsoluteSize;

					this->OldAbsoluteSize = this->AbsoluteSize;
				}
				GraphicDevice->ExportTexture(this->CompiledTexture);
			}
		}
	}
}

void TextButton::OnMouseButton1Click(void (*Function) (Program* Program, TextButton* Body, MouseObject* Mouse))
{
	this->OnMouseButton1ClickFunction = Function;
}

void TextButton::OnMouseButton1Down(void (*Function) (Program* Program, TextButton* Body, MouseObject* Mouse))
{
	this->OnMouseButton1DownFunction = Function;
}

void TextButton::OnMouseButton1Up(void (*Function) (Program* Program, TextButton* Body, MouseObject* Mouse))
{
	this->OnMouseButton1UpFunction = Function;
}

void ImageLabel::UpdateTexture()
{
	this->graphics->Clear(Gdiplus::Color(BackgroundTransparencyAlpha * 255,
		(this->BackgroundColor.Red * 255),
		(this->BackgroundColor.Green * 255),
		(this->BackgroundColor.Blue * 255)
	));

	UINT w, h = 0;
	if (this->TargetTexture != nullptr)
	{
		this->TargetTexture->GetLoadedImageSize(&w, &h);
		Gdiplus::Bitmap* bmp = new Gdiplus::Bitmap(w, h, PixelFormat32bppPARGB);
		Gdiplus::RectF rt = {
			0,0,this->AbsoluteSize.X,this->AbsoluteSize.Y
		};
		this->TargetTexture->PasteDataToBitmap_GDIPlus(&bmp);
		this->graphics->DrawImage(bmp, rt);
		delete bmp;
	}

	this->CompiledTexture->ImportBitmap_GDIPlus(pBitmap_Gdiplus);

	if (this->OldAbsoluteSize != AbsoluteSize)
	{
		this->CompiledTexture->TextureType = TextureTypes::Bitmap_GDIPlus;
		this->CompiledTexture->TextureSize = this->AbsoluteSize;
		this->CompiledTexture->SourceDestination[0] = { 0, 0 };
		this->CompiledTexture->SourceDestination[1] = this->AbsoluteSize;

		this->OldAbsoluteSize = this->AbsoluteSize;
	}
	GraphicDevice->ExportTexture(this->CompiledTexture);
}

void ImageButton::UpdateTexture()
{
	this->graphics->Clear(Gdiplus::Color(BackgroundTransparencyAlpha * 255,
		(this->BackgroundColor.Red * 255),
		(this->BackgroundColor.Green * 255),
		(this->BackgroundColor.Blue * 255)
	));

	float w, h = 0;
	if (this->TargetTexture != nullptr)
	{
		UINT uw, uh = 0;
		this->TargetTexture->GetLoadedImageSize(&uw, &uh);
		Gdiplus::Bitmap* bmp = new Gdiplus::Bitmap(uw, uh, PixelFormat32bppPARGB);
		Gdiplus::RectF rt = {
			0,0,this->AbsoluteSize.X,this->AbsoluteSize.Y
		};
		this->TargetTexture->PasteDataToBitmap_GDIPlus(&bmp);
		this->graphics->DrawImage(bmp, rt);
		delete bmp;
		w = static_cast<float>(uw);
		h = static_cast<float>(uh);
	}

	this->CompiledTexture->ImportBitmap_GDIPlus(pBitmap_Gdiplus);

	if (this->OldAbsoluteSize != AbsoluteSize)
	{
		this->CompiledTexture->TextureType = TextureTypes::Bitmap_GDIPlus;
		this->CompiledTexture->TextureSize = this->AbsoluteSize;
		this->CompiledTexture->SourceDestination[0] = { 0, 0 };
		this->CompiledTexture->SourceDestination[1] = this->AbsoluteSize;

		this->OldAbsoluteSize = this->AbsoluteSize;
	}
	GraphicDevice->ExportTexture(this->CompiledTexture);
}