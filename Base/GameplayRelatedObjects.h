#pragma once
#include "GameObject.h"
#include "Item.h"

namespace GameplayRelatedObjects {
	class InventorySlot : public ImageButton
	{
	public:
		InventorySlot(Program* m_Program) : ImageButton(m_Program) {};
		InventoryItem* ReferenceInventoryItem = nullptr;
	};

	class SelectedInventoryItemBox : public ImageLabel, public DragInterface
	{
	public:
		SelectedInventoryItemBox(Program* m_Program) : ImageLabel(m_Program) {};
		InventoryItem* ReferenceInventoryItem = nullptr;
	protected:
		void UpdateTexture() override;
	};
}