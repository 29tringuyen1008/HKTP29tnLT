#include "Base.h"

DWORD CALLBACK Program::GameplayScript(LPVOID ThreadParameter)
{
	Program* m_Program = static_cast<Program*>(ThreadParameter);
	m_Program->SetObjectiveMessage(m_Program->GetLocaleText(m_Program->CurrentLang.data(), L"objmsg1"));
	Sleep(7500);
	m_Program->SetObjectiveMessage(m_Program->GetLocaleText(m_Program->CurrentLang.data(), L"objmsg2"));
	Sleep(7500);
	m_Program->SetObjectiveMessage(m_Program->GetLocaleText(m_Program->CurrentLang.data(), L"objmsg3"));

	bool IsWPressed = false, IsAPressed = false, IsSPressed = false, IsDPressed = false;
	repeat {
		if (m_Program->KeyPressed.KeyW == true)
		{
			IsWPressed = true;
		}

		if (m_Program->KeyPressed.KeyA == true)
		{
			IsAPressed = true;
		}

		if (m_Program->KeyPressed.KeyS == true)
		{
			IsSPressed = true;
		}

		if (m_Program->KeyPressed.KeyD == true)
		{
			IsDPressed = true;
		}
		Sleep(1);
	} until (IsWPressed == true and IsAPressed == true and IsSPressed == true and IsDPressed == true);

	m_Program->SetObjectiveMessage(m_Program->GetLocaleText(m_Program->CurrentLang.data(), L"objmsg4"));
	repeat {
		Sleep(1);
	} until (m_Program->KeyPressed.KeyE == true and m_Program->Toolbox.CharacterInventory.Body->Visible == true);
	m_Program->SetObjectiveMessage(m_Program->GetLocaleText(m_Program->CurrentLang.data(), L"objmsg5"));
	Sleep(7500);
	m_Program->SetObjectiveMessage(m_Program->GetLocaleText(m_Program->CurrentLang.data(), L"objmsg6"));
	bool CamcorderFound = false;
	while (CamcorderFound == false)
	{
		for (size_t i = 0; i < m_Program->NumberOfActiveItemSlots; i++)
		{
			UI_Frame* Slot = m_Program->Toolbox.ActiveSlots[i];
			if (Slot != nullptr)
			{
				if (ImageLabel* aiip = dynamic_cast<ImageLabel*>(Slot))
				{
					InventoryItem* item = m_Program->Inventory.ActiveItemSlots[i];
					if (item != nullptr)
					{
						if (item->BaseItem != nullptr)
						{
							if (item->BaseItem->PrototypeName == L"camcorder")
							{
								CamcorderFound = true;
							}
						}
					}
				}
			}
		}
		Sleep(1);
	}
	m_Program->SetObjectiveMessage(m_Program->GetLocaleText(m_Program->CurrentLang.data(), L"objmsg7"));
	Sleep(7500);
	m_Program->SetObjectiveMessage(m_Program->GetLocaleText(m_Program->CurrentLang.data(), L"objmsg8"));
	Sleep(7500);
	m_Program->SetObjectiveMessage(m_Program->GetLocaleText(m_Program->CurrentLang.data(), L"objmsg9"));

	bool isSpacePressedandhasCamcorder = false;
	repeat {
		if (m_Program->KeyPressed.Space == true)
		{
			if (InventoryItem* item = m_Program->Inventory.ActiveItemSlots[m_Program->Toolbox.ActiveItemIndex])
			{
				if (ItemDescription* itemDesc = item->BaseItem)
				{
					if (itemDesc->PrototypeName == L"camcorder")
					{
						isSpacePressedandhasCamcorder = true;
					}
				}
			}
		}
		Sleep(1);
	} until (isSpacePressedandhasCamcorder == true);

	m_Program->SetObjectiveMessage(m_Program->GetLocaleText(m_Program->CurrentLang.data(), L"objmsg10"));
	Sleep(12500);
	m_Program->SetObjectiveMessage(m_Program->GetLocaleText(m_Program->CurrentLang.data(), L"objmsg11"));
	bool isWoodenBoxPresent = false;
	while (!isWoodenBoxPresent)
	{
		if (Object* obj = m_Program->CurrentScene->SceneWorkspace->GetChild(L"wooden-block"))
		{
			isWoodenBoxPresent = true;
		}
		Sleep(1);
	}

	m_Program->SetObjectiveMessage(m_Program->GetLocaleText(m_Program->CurrentLang.data(), L"objmsg12"));

	isWoodenBoxPresent = true;
	while (isWoodenBoxPresent == true)
	{
		if (Object* obj = m_Program->CurrentScene->SceneWorkspace->GetChild(L"wooden-block"))
		{
			isWoodenBoxPresent = true;
		}
		else
		{
			isWoodenBoxPresent = false;
		}
		Sleep(1);
	}

	m_Program->SetObjectiveMessage(m_Program->GetLocaleText(m_Program->CurrentLang.data(), L"objmsg13"));
	Sleep(5000);
	m_Program->EndGameplay();
	m_Program->GameplayScriptThread = nullptr;
	return 0;
}