#include "Base.h"
#include "Json.h"

MessageBoxUI* msgBox = nullptr;
void PrechangeLang(Program* m_Program, TextButton* Body, MouseObject* Mouse)
{
	size_t numberoflang = m_Program->Localization.GetMaximumSize();
	size_t& currentindexoflang = m_Program->CurrentLangIndex;
	if (currentindexoflang < numberoflang - 1)
		currentindexoflang++;
	else
		currentindexoflang = 0;

	JSONElement element = {};
	element.InitializeAsObject();
	
	std::wstring str = L"";
	(void)m_Program->GetLangByIndex(&str, currentindexoflang);
	JSONElement langelement = str;
	langelement.SetName(L"lang");
	element << langelement;

	JSONGenerator Parser = {};
	Parser.SetFile(L"./playerdata.json");
	Parser.SetElement(&element);
	Parser.Start();

	Body->Text = str;
	if (msgBox == nullptr)
	{
		m_Program->CreateMessageBox(m_Program->GetLocaleText(m_Program->CurrentLang.data(), L"notice"), m_Program->GetLocaleText(m_Program->CurrentLang.data(), L"language_notice"), &msgBox);
	}
}

struct VolSec
{
	VolSec() {};
	VolSec(float _Volume, const wchar_t* _VolumeDisplay) : Volume(_Volume), VolumeDisplay(_VolumeDisplay) {};
	VolSec(const VolSec& _VolSec)
	{
		Volume = _VolSec.Volume;
		VolumeDisplay = _VolSec.VolumeDisplay;
	}
	float Volume = 0.0f;
	std::wstring VolumeDisplay = L"0%";
};
tn::NormalArray<VolSec> VolumeSelection = {
	VolSec{ 0.0f, L"0%"}, VolSec{ 0.25f, L"25%"}, VolSec{ 0.5f, L"50%" }, VolSec{ 0.75f, L"75%" }, VolSec{ 1.0f, L"100%" }
};
static size_t NumberOfVolIndices = VolumeSelection.GetMaximumSize();
static size_t CurrentVolIndex = VolumeSelection.GetMaximumSize()-1;

void ChangeVol(Program* m_Program, TextButton* Body, MouseObject* Mouse)
{
	if (CurrentVolIndex < NumberOfVolIndices - 1)
		CurrentVolIndex++;
	else
		CurrentVolIndex = 0.0f;

	Body->Text = VolumeSelection[CurrentVolIndex].VolumeDisplay;
	m_Program->MainSoundFramework->SetGlobalVolume(VolumeSelection[CurrentVolIndex].Volume);
}

void Program::LoadSettingsUI()
{
	MessageBoxUI*& SettingFrame = this->SettingsGUI.SettingFrame;
	SettingFrame = new MessageBoxUI(this);
	SettingFrame->GraphicDevice = this->MainGraphicDevice;
	SettingFrame->SetName(L"SettingFrame");
	SettingFrame->Visible = false;
	SettingFrame->BarSizeY = { 0, 50 };
	SettingFrame->BarColor = { 0.25f,0.25f,0.25f };
	SettingFrame->BarText = this->GetLocaleText(this->CurrentLang.data(), L"cai_dat");
	SettingFrame->Size = { 0, 0, 580, 420 };
	SettingFrame->Position = { 0.5f, 0.5f, -(SettingFrame->Size.Offset.X / 2), -(SettingFrame->Size.Offset.Y / 2) };
	SettingFrame->ZDepth = 2.0f;
	SettingFrame->Load();

	ScrollingFrame*& ScrollFrame = this->SettingsGUI.ScrollFrame;
	ScrollFrame = new ScrollingFrame(this);
	ScrollFrame->GraphicDevice = this->MainGraphicDevice;
	ScrollFrame->SetName(L"ScrollFrame");
	ScrollFrame->Size = { 0, 0, 550, 300 };
	ScrollFrame->Position = { 0, 0, 25, 75 };
	ScrollFrame->Visible = true;
	ScrollFrame->BackgroundColor = { 0.5, 0.5, 0.5 };
	ScrollFrame->BackgroundTransparencyAlpha = 1.0f;
	ScrollFrame->AdditionalCanvaY.Offset = 500;
	ScrollFrame->ZDepth = 2.0f;
	SettingFrame->AddChild(ScrollFrame);
	ScrollFrame->Load();

	{
		TextLabel*& LanguageSettingText = this->SettingsGUI.LanguageSettingText;
		LanguageSettingText = new TextLabel(this);
		LanguageSettingText->SetName(L"LanguageSettingText");
		LanguageSettingText->GraphicDevice = this->MainGraphicDevice;
		LanguageSettingText->Position = { 0, 0, 0, 25 };
		LanguageSettingText->Size = { 0, 0, 150, 50 };
		LanguageSettingText->BackgroundColor = { 0.75f, 0.75f, 0.75f };
		LanguageSettingText->BackgroundTransparencyAlpha = 1.0f;
		LanguageSettingText->Text = this->GetLocaleText(this->CurrentLang.data(), L"lang");
		LanguageSettingText->TextSize = 18.0f;
		LanguageSettingText->TextColor = { 0.0f, 0.0f, 0.0f };
		LanguageSettingText->Font = VerdanaFont;
		LanguageSettingText->ParagraphAlignment = ParagraphAlignmentEnum::ParagraphAlignmentLeft;
		LanguageSettingText->Visible = true;
		LanguageSettingText->ZDepth = 2.0f;
		LanguageSettingText->SetParent(ScrollFrame);
		LanguageSettingText->Load();

		TextButton*& LanguageSettingButton = this->SettingsGUI.LanguageSettingButton;
		LanguageSettingButton = new TextButton(this);
		LanguageSettingButton->SetName(L"LanguageSettingButton");
		LanguageSettingButton->GraphicDevice = this->MainGraphicDevice;
		LanguageSettingButton->Position = { 0, 0, 150, 25 };
		LanguageSettingButton->Size = { 0, 0, SettingFrame->Size.Offset.X - LanguageSettingText->Size.Offset.X - 75, 50 };
		LanguageSettingButton->BackgroundColor = { 1, 1, 1 };
		LanguageSettingButton->BackgroundTransparencyAlpha = 1.0f;
		LanguageSettingButton->Text = this->CurrentLang;
		LanguageSettingButton->TextSize = 28.0f;
		LanguageSettingButton->TextColor = { 0.0f, 0.0f, 0.0f };
		LanguageSettingButton->Font = VerdanaFont;
		LanguageSettingButton->ParagraphAlignment = ParagraphAlignmentEnum::ParagraphAlignmentLeft;
		LanguageSettingButton->Visible = true;
		LanguageSettingButton->ZDepth = 2.0f;
		LanguageSettingButton->SetParent(ScrollFrame);
		LanguageSettingButton->Load();
		LanguageSettingButton->OnMouseButton1Click(&PrechangeLang);
	}

	{
		TextLabel*& VolumeSettingText = this->SettingsGUI.VolumeSettingText;
		VolumeSettingText = new TextLabel(this);
		VolumeSettingText->SetName(L"VolumeSettingText");
		VolumeSettingText->GraphicDevice = this->MainGraphicDevice;
		VolumeSettingText->Position = { 0, 0, 0, 75 };
		VolumeSettingText->Size = { 0, 0, 150, 100 };
		VolumeSettingText->BackgroundColor = { 0.75f, 0.75f, 0.75f };
		VolumeSettingText->BackgroundTransparencyAlpha = 1.0f;
		VolumeSettingText->Text = this->GetLocaleText(this->CurrentLang.data(), L"volume");
		VolumeSettingText->TextSize = 18.0f;
		VolumeSettingText->TextColor = { 0.0f, 0.0f, 0.0f };
		VolumeSettingText->Font = VerdanaFont;
		VolumeSettingText->ParagraphAlignment = ParagraphAlignmentEnum::ParagraphAlignmentLeft;
		VolumeSettingText->Visible = true;
		VolumeSettingText->ZDepth = 2.0f;
		VolumeSettingText->SetParent(ScrollFrame);
		VolumeSettingText->Load();

		TextButton*& VolumeSettingButton = this->SettingsGUI.VolumeSettingButton;
		VolumeSettingButton = new TextButton(this);
		VolumeSettingButton->SetName(L"VolumeSettingButton");
		VolumeSettingButton->GraphicDevice = this->MainGraphicDevice;
		VolumeSettingButton->Position = { 0, 0, 150, 100 };
		VolumeSettingButton->Size = { 0, 0, SettingFrame->Size.Offset.X - VolumeSettingText->Size.Offset.X - 75, 50 };
		VolumeSettingButton->BackgroundColor = { 1, 1, 1 };
		VolumeSettingButton->BackgroundTransparencyAlpha = 1.0f;
		VolumeSettingButton->Text = VolumeSelection[CurrentVolIndex].VolumeDisplay;
		VolumeSettingButton->TextSize = 28.0f;
		VolumeSettingButton->TextColor = { 0.0f, 0.0f, 0.0f };
		VolumeSettingButton->Font = VerdanaFont;
		VolumeSettingButton->ParagraphAlignment = ParagraphAlignmentEnum::ParagraphAlignmentLeft;
		VolumeSettingButton->Visible = true;
		VolumeSettingButton->ZDepth = 2.0f;
		VolumeSettingButton->SetParent(ScrollFrame);
		VolumeSettingButton->Load();
		VolumeSettingButton->OnMouseButton1Click(&ChangeVol);
	}
	
	TextButton*& CloseButtonOfSettingFrame = this->SettingsGUI.CloseButtonOfSettingFrame;
	CloseButtonOfSettingFrame = new TextButton(this);
	CloseButtonOfSettingFrame->SetName(L"CloseButton");
	CloseButtonOfSettingFrame->Size = { 0, 0, 100, 50 };
	CloseButtonOfSettingFrame->BackgroundColor = { 0.8f, 0.04f, 0.05f };
	CloseButtonOfSettingFrame->Position = { 0, 1.0f, 10, -(CloseButtonOfSettingFrame->Size.Offset.Y + 10) };
	CloseButtonOfSettingFrame->ZDepth = 1.0f;
	CloseButtonOfSettingFrame->TextSize = 32.0f;
	CloseButtonOfSettingFrame->Text = this->GetLocaleText(this->CurrentLang.data(), L"close");
	CloseButtonOfSettingFrame->Font = VerdanaFont;
	CloseButtonOfSettingFrame->Italic = false;
	CloseButtonOfSettingFrame->Bold = false;
	CloseButtonOfSettingFrame->TextColor = { 1.0f,1.0f,1.0f };
	CloseButtonOfSettingFrame->GraphicDevice = this->MainGraphicDevice;
	CloseButtonOfSettingFrame->TextAlignment = TextAlignmentEnum::TextAlignmentCenter;
	CloseButtonOfSettingFrame->ParagraphAlignment = ParagraphAlignmentEnum::ParagraphAlignmentMiddle;
	CloseButtonOfSettingFrame->Alpha = 1.0f;
	SettingFrame->AddChild(CloseButtonOfSettingFrame);
	CloseButtonOfSettingFrame->Load();
	CloseButtonOfSettingFrame->OnMouseButton1Click([](Program* Program, TextButton* Body, MouseObject* Mouse) -> void {
		if (Body)
		{
			if (UI_Frame* p = dynamic_cast<UI_Frame*>(Body->GetParent()))
			{
				p->Visible = false;
				p->Position = { 0.5f, 0.5f, -p->Size.Offset.X / 2, -p->Size.Offset.Y / 2 };
			}
			if (UI_Frame* p2 = dynamic_cast<UI_Frame*>(Body->GetParent()->GetParent()->GetChild(L"MainMenuFrame")))
			{
				p2->Visible = true;
			}

			if (tn::SoundBuffer* sound = dynamic_cast<tn::SoundBuffer*>(Program->GameHierachy.Assets->GetChild(L"gui_close")))
			{
				if (tn::Sound* sa = dynamic_cast<tn::Sound*>(Program->GameHierachy.Assets->GetChild(L"gui_close_sound")))
				{
					sa->Play();
				}
				else
				{
					tn::Sound* s = new tn::Sound();
					s->SetBuffer(sound);
					Program->GameHierachy.Assets->AddChild(s);
					s->Play();
				}
			}
		}
	});

	this->GameHierachy.GUI->AddChild(this->SettingsGUI.SettingFrame);
}