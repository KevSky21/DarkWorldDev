
#include "InventoryManager.h"
#include "SpriteDesc.h"
#include <algorithm>

/// Constructor initializes the inventory with empty slots.
/// \param renderer Pointer to the sprite renderer

CInventoryManager::CInventoryManager(LSpriteRenderer* renderer)
    : m_pRenderer(renderer)
{
    m_vItems.resize(m_nMaxSlots, nullptr);
}

/// Destructor cleans up all items in inventory.

CInventoryManager::~CInventoryManager() {
    for (CItem* item : m_vItems) {
        delete item;
    }
    m_vItems.clear();
}

/// Add an item to the inventory. Attempts to stack with existing items first.
/// \param item Item to add
/// \return True if item was successfully added

bool CInventoryManager::AddItem(CItem* item) {
    if (!item) return false;

    // Try to stack with existing items of same type
    if (item->IsStackable()) {
        for (int i = 0; i < m_nMaxSlots; i++) {
            if (m_vItems[i] && m_vItems[i]->GetID() == item->GetID()) {
                if (m_vItems[i]->CanAddToStack(item->GetQuantity())) {
                    m_vItems[i]->AddQuantity(item->GetQuantity());
                    delete item; // Item was merged, delete the input
                    return true;
                }
            }
        }
    }

    // Find first empty slot
    for (int i = 0; i < m_nMaxSlots; i++) {
        if (m_vItems[i] == nullptr) {
            m_vItems[i] = item;
            return true;
        }
    }

    return false; // Inventory full
}

/// Remove an item from a specific slot.
/// \param slotIndex Index of slot to remove from
/// \return True if item was removed

bool CInventoryManager::RemoveItem(int slotIndex) {
    if (slotIndex < 0 || slotIndex >= m_nMaxSlots) return false;
    if (!m_vItems[slotIndex]) return false;

    delete m_vItems[slotIndex];
    m_vItems[slotIndex] = nullptr;

    if (m_nSelectedSlot == slotIndex) {
        m_nSelectedSlot = -1;
    }

    return true;
}

/// Get the item at a specific slot.
/// \param slotIndex Slot to check
/// \return Pointer to item or nullptr

CItem* CInventoryManager::GetItem(int slotIndex) const {
    if (slotIndex < 0 || slotIndex >= m_nMaxSlots) return nullptr;
    return m_vItems[slotIndex];
}

/// Calculate the screen position for a slot based on its index.
/// \param slotIndex Index of the slot
/// \return Screen position

Vector2 CInventoryManager::GetSlotPosition(int slotIndex) const {
    int row = slotIndex / m_nSlotsPerRow;
    int col = slotIndex % m_nSlotsPerRow;

    float x = m_vInventoryPos.x + col * (m_fSlotSize + m_fSlotPadding);
    float y = m_vInventoryPos.y + row * (m_fSlotSize + m_fSlotPadding);

    return Vector2(x, y);
}

/// Get the slot index at a screen position (for mouse input).
/// \param pos Screen position
/// \return Slot index or -1 if not over a slot

int CInventoryManager::GetSlotAtPosition(const Vector2& pos) const {
    for (int i = 0; i < m_nMaxSlots; i++) {
        Vector2 slotPos = GetSlotPosition(i);

        if (pos.x >= slotPos.x && pos.x < slotPos.x + m_fSlotSize &&
            pos.y >= slotPos.y && pos.y < slotPos.y + m_fSlotSize) {
            return i;
        }
    }
    return -1;
}

/// Handle keyboard input for inventory navigation.
/// \param pKeyboard Pointer to keyboard controller

void CInventoryManager::HandleInput(LKeyboard* pKeyboard) {
    if (!m_bIsOpen) return;

    pKeyboard->GetState();

    // Initialize selection if nothing selected
    if (m_nSelectedSlot == -1 && pKeyboard->TriggerDown(VK_RIGHT)) {
        m_nSelectedSlot = 0;
        return;
    }

    // Navigate with arrow keys
    if (pKeyboard->TriggerDown(VK_RIGHT)) {
        MoveSelection(1);
    }
    if (pKeyboard->TriggerDown(VK_LEFT)) {
        MoveSelection(-1);
    }
    if (pKeyboard->TriggerDown(VK_DOWN)) {
        MoveSelection(m_nSlotsPerRow);
    }
    if (pKeyboard->TriggerDown(VK_UP)) {
        MoveSelection(-m_nSlotsPerRow);
    }

    // Use item with Enter
    if (pKeyboard->TriggerDown(VK_RETURN)) {
        UseSelectedItem();
    }

    // Drop item with Delete
    if (pKeyboard->TriggerDown(VK_DELETE)) {
        DropSelectedItem();
    }
}

/// Move the selection by a given offset.
/// \param direction Offset to move selection

void CInventoryManager::MoveSelection(int direction) {
    if (m_nSelectedSlot == -1) {
        m_nSelectedSlot = 0;
        return;
    }

    int newSlot = m_nSelectedSlot + direction;

    // Clamp to valid range
    if (newSlot < 0) newSlot = 0;
    if (newSlot >= m_nMaxSlots) newSlot = m_nMaxSlots - 1;

    m_nSelectedSlot = newSlot;
}

/// Use the currently selected item.

void CInventoryManager::UseSelectedItem() {
    if (m_nSelectedSlot < 0 || !m_vItems[m_nSelectedSlot]) return;

    CItem* item = m_vItems[m_nSelectedSlot];

    // TODO: Implement item usage logic based on item type
    // For now, just remove one from stack
    item->RemoveQuantity(1);

    if (item->IsEmpty()) {
        RemoveItem(m_nSelectedSlot);
    }
}

/// Drop the currently selected item.

void CInventoryManager::DropSelectedItem() {
    if (m_nSelectedSlot < 0 || !m_vItems[m_nSelectedSlot]) return;

    // TODO: Create item entity in game world at player position
    RemoveItem(m_nSelectedSlot);
}

/// Draw a single inventory slot.
/// \param slotIndex Index of slot to draw
/// \param pos Position to draw at

void CInventoryManager::DrawSlot(int slotIndex, const Vector2& pos) {
    LSpriteDesc2D desc;

    // Draw slot background
    if (slotIndex == m_nSelectedSlot) {
        desc.m_nSpriteIndex = (UINT)eSprite::InventorySlotSelected;
    }
    else {
        desc.m_nSpriteIndex = (UINT)eSprite::InventorySlot;
    }
    desc.m_vPos = pos + Vector2(m_fSlotSize / 2, m_fSlotSize / 2);
    m_pRenderer->Draw(&desc);

    // Draw item if slot has one
    CItem* item = m_vItems[slotIndex];
    if (item) {
        desc.m_nSpriteIndex = (UINT)item->GetSprite();
        desc.m_vPos = pos + Vector2(m_fSlotSize / 2, m_fSlotSize / 2);
        m_pRenderer->Draw(&desc);

        // Draw quantity if stackable and > 1
        if (item->IsStackable() && item->GetQuantity() > 1) {
            std::string qtyStr = std::to_string(item->GetQuantity());
            m_pRenderer->DrawScreenText(qtyStr.c_str(),
                pos + Vector2(m_fSlotSize - 16, m_fSlotSize - 8));
        }
    }
}

/// Draw the entire inventory UI.

void CInventoryManager::Draw() {
    if (!m_bIsOpen) return;

    // Draw inventory background panel
    // TODO: Add a semi-transparent background sprite

    // Draw title
    const char* title = "Inventory";
    m_pRenderer->DrawScreenText(title, m_vInventoryPos - Vector2(0, 30));

    // Draw all slots
    for (int i = 0; i < m_nMaxSlots; i++) {
        Vector2 slotPos = GetSlotPosition(i);
        DrawSlot(i, slotPos);
    }

    // Draw selected item info
    if (m_nSelectedSlot >= 0 && m_vItems[m_nSelectedSlot]) {
        CItem* item = m_vItems[m_nSelectedSlot];
        Vector2 infoPos = m_vInventoryPos + Vector2(0,
            (m_nMaxSlots / m_nSlotsPerRow + 1) * (m_fSlotSize + m_fSlotPadding));

        m_pRenderer->DrawScreenText(item->GetName().c_str(), infoPos);
        m_pRenderer->DrawScreenText(item->GetDescription().c_str(),
            infoPos + Vector2(0, 20));
    }
}

/// Check if inventory has room for an item.
/// \param item Item to check
/// \return True if can be added

bool CInventoryManager::HasRoom(CItem* item) const {
    if (!item) return false;

    // Check for stackable items
    if (item->IsStackable()) {
        for (int i = 0; i < m_nMaxSlots; i++) {
            if (m_vItems[i] && m_vItems[i]->GetID() == item->GetID()) {
                if (m_vItems[i]->CanAddToStack(item->GetQuantity())) {
                    return true;
                }
            }
        }
    }

    // Check for empty slot
    for (int i = 0; i < m_nMaxSlots; i++) {
        if (m_vItems[i] == nullptr) return true;
    }

    return false;
}

/// Get total quantity of a specific item type.
/// \param itemID Item ID to count
/// \return Total quantity

int CInventoryManager::GetItemCount(int itemID) const {
    int count = 0;
    for (int i = 0; i < m_nMaxSlots; i++) {
        if (m_vItems[i] && m_vItems[i]->GetID() == itemID) {
            count += m_vItems[i]->GetQuantity();
        }
    }
    return count;
}