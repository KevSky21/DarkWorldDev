#include "InventoryManager.h"

#include <algorithm>
#include <cstring>
#include <sstream>

#include "SpriteDesc.h"

/// Constructor initializes the inventory with empty slots.
/// \param renderer Pointer to the sprite renderer

CInventoryManager::CInventoryManager(LSpriteRenderer* renderer)
    : m_pRenderer(renderer) {
  m_vItems.resize(m_nMaxSlots, nullptr);
  UpdateLayout();
}

/// Destructor cleans up all items in inventory.

CInventoryManager::~CInventoryManager() {
  for (CItem* item : m_vItems) {
    delete item;
  }
  m_vItems.clear();
}

/// Set screen dimensions and recalculate layout.
/// \param width Screen width
/// \param height Screen height

void CInventoryManager::SetScreenSize(float width, float height) {
  m_fScreenWidth = width;
  m_fScreenHeight = height;
  UpdateLayout();
}

/// Update all layout positions based on current screen size.
/// Coordinate system: (0,0) at bottom-left, Y increases upward for sprites
/// Text uses (0,0) at top-left, Y increases downward

void CInventoryManager::UpdateLayout() {
  // Calculate inventory grid dimensions
  float gridWidth =
      m_nSlotsPerRow * (m_fSlotSize + m_fSlotPadding) - m_fSlotPadding;
  int numRows = (m_nMaxSlots + m_nSlotsPerRow - 1) / m_nSlotsPerRow;
  float gridHeight = numRows * (m_fSlotSize + m_fSlotPadding) - m_fSlotPadding;

  // Panel size includes padding and space for title/info
  m_vPanelSize.x = gridWidth + m_fPanelPadding * 2;
  m_vPanelSize.y = gridHeight + m_fPanelPadding * 2 + 50.0f + 80.0f;

  // Center the panel on screen
  m_vPanelPos.x = (m_fScreenWidth - m_vPanelSize.x) / 2.0f;
  m_vPanelPos.y = (m_fScreenHeight - m_vPanelSize.y) / 2.0f;

  // Inventory grid position (top-left of first slot)
  // Start below the title area
  m_vInventoryPos.x = m_vPanelPos.x + m_fPanelPadding;
  m_vInventoryPos.y = m_vPanelPos.y + m_vPanelSize.y - m_fPanelPadding - 50.0f;

  // Hotbar at bottom center of screen
  float hotbarWidth =
      m_nHotbarSlots * (m_fSlotSize + m_fSlotPadding) - m_fSlotPadding;
  m_vHotbarPos.x = (m_fScreenWidth - hotbarWidth) / 2.0f;
  m_vHotbarPos.y = 30.0f;
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
          delete item;
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

  return false;
}

/// Remove an item from a specific slot.
/// \param slotIndex Index of slot to remove from
/// \return True if item was removed

bool CInventoryManager::RemoveItem(int slotIndex) {
  if (slotIndex < 0 || slotIndex >= m_nMaxSlots) return false;
  if (!m_vItems[slotIndex]) return false;

  delete m_vItems[slotIndex];
  m_vItems[slotIndex] = nullptr;

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
/// Returns bottom-left corner of slot in sprite coordinates
/// \param slotIndex Index of the slot
/// \return Screen position

Vector2 CInventoryManager::GetSlotPosition(int slotIndex) const {
  int row = slotIndex / m_nSlotsPerRow;
  int col = slotIndex % m_nSlotsPerRow;

  float x = m_vInventoryPos.x + col * (m_fSlotSize + m_fSlotPadding);
  // Row 0 is at top, so we go down as row increases
  float y =
      m_vInventoryPos.y - row * (m_fSlotSize + m_fSlotPadding) - m_fSlotSize;

  return Vector2(x, y);
}

/// Calculate the screen position for a hotbar slot.
/// \param slotIndex Index of the hotbar slot (0-5)
/// \return Screen position (bottom-left corner)

Vector2 CInventoryManager::GetHotbarSlotPosition(int slotIndex) const {
  float x = m_vHotbarPos.x + slotIndex * (m_fSlotSize + m_fSlotPadding);
  float y = m_vHotbarPos.y;
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

/// Toggle inventory and reset selection to first slot.

void CInventoryManager::Toggle() {
  m_bIsOpen = !m_bIsOpen;
  if (m_bIsOpen) {
    m_nSelectedSlot = m_nHotbarSelection;
  }
}

/// Handle keyboard input for inventory navigation.
/// \param pKeyboard Pointer to keyboard controller

void CInventoryManager::HandleInput(LKeyboard* pKeyboard) {
  if (!m_bIsOpen) return;

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

  // Use item with Enter or E
  if (pKeyboard->TriggerDown(VK_RETURN) || pKeyboard->TriggerDown('E')) {
    UseSelectedItem();
  }

  // Drop item with Delete or Q
  if (pKeyboard->TriggerDown(VK_DELETE) || pKeyboard->TriggerDown('Q')) {
    DropSelectedItem();
  }
}

/// Handle number key input for hotbar selection.
/// \param pKeyboard Pointer to keyboard controller

void CInventoryManager::HandleHotbarInput(LKeyboard* pKeyboard) {
  // Number keys 1-6 select hotbar slots
  for (int i = 0; i < m_nHotbarSlots; i++) {
    if (pKeyboard->TriggerDown('1' + i)) {
      m_nHotbarSelection = i;
      if (m_bIsOpen) {
        m_nSelectedSlot = i;
      }
    }
  }

  // Bracket keys to cycle hotbar
  if (pKeyboard->TriggerDown(VK_OEM_4)) {
    m_nHotbarSelection =
        (m_nHotbarSelection - 1 + m_nHotbarSlots) % m_nHotbarSlots;
  }
  if (pKeyboard->TriggerDown(VK_OEM_6)) {
    m_nHotbarSelection = (m_nHotbarSelection + 1) % m_nHotbarSlots;
  }
}

/// Move the selection by a given offset.
/// \param direction Offset to move selection

void CInventoryManager::MoveSelection(int direction) {
  int newSlot = m_nSelectedSlot + direction;

  // Handle horizontal wrapping
  if (direction == 1 &&
      (m_nSelectedSlot % m_nSlotsPerRow) == (m_nSlotsPerRow - 1)) {
    return;
  }
  if (direction == -1 && (m_nSelectedSlot % m_nSlotsPerRow) == 0) {
    return;
  }

  // Clamp to valid range
  if (newSlot < 0) newSlot = 0;
  if (newSlot >= m_nMaxSlots) newSlot = m_nMaxSlots - 1;

  m_nSelectedSlot = newSlot;

  // Update hotbar selection if in first row
  if (m_nSelectedSlot < m_nHotbarSlots) {
    m_nHotbarSelection = m_nSelectedSlot;
  }
}

/// Use the currently selected item.

void CInventoryManager::UseSelectedItem() {
  if (m_nSelectedSlot < 0 || !m_vItems[m_nSelectedSlot]) return;

  CItem* item = m_vItems[m_nSelectedSlot];

  // Only consumables can be used
  if (item->GetType() == eItemType::Consumable) {
    item->RemoveQuantity(1);

    if (item->IsEmpty()) {
      RemoveItem(m_nSelectedSlot);
    }
  }
}

/// Drop the currently selected item.

void CInventoryManager::DropSelectedItem() {
  if (m_nSelectedSlot < 0 || !m_vItems[m_nSelectedSlot]) return;

  // TODO: Create item entity in game world at player position
  RemoveItem(m_nSelectedSlot);
}

/// Get item in currently selected hotbar slot.

CItem* CInventoryManager::GetHotbarItem() const {
  if (m_nHotbarSelection < 0 || m_nHotbarSelection >= m_nHotbarSlots)
    return nullptr;
  return m_vItems[m_nHotbarSelection];
}

/// Use the item in the currently selected hotbar slot.

void CInventoryManager::UseHotbarItem() {
  CItem* item = GetHotbarItem();
  if (!item) return;

  if (item->GetType() == eItemType::Consumable) {
    item->RemoveQuantity(1);

    if (item->IsEmpty()) {
      RemoveItem(m_nHotbarSelection);
    }
  }
}

/// Convert sprite Y coordinate to screen text Y coordinate.
/// Sprites: Y=0 at bottom, Y increases upward
/// Text: Y=0 at top, Y increases downward

float CInventoryManager::SpriteYToTextY(float spriteY) const {
  return m_fScreenHeight - spriteY;
}

/// Draw the background panel for the inventory.

void CInventoryManager::DrawPanel() {
  LSpriteDesc2D desc;
  desc.m_nSpriteIndex = (UINT)eSprite::InventoryPanel;

  // Panel center position
  float centerX = m_vPanelPos.x + m_vPanelSize.x / 2.0f;
  float centerY = m_vPanelPos.y + m_vPanelSize.y / 2.0f;
  desc.m_vPos = Vector2(centerX, centerY);

  // Scale panel to fit
  desc.m_fXScale = m_vPanelSize.x / 64.0f;
  desc.m_fYScale = m_vPanelSize.y / 64.0f;

  m_pRenderer->Draw(&desc);
}

/// Draw a single inventory slot.
/// \param slotIndex Index of slot to draw
/// \param pos Position to draw at (bottom-left corner in sprite coords)
/// \param isHotbar Whether this is a hotbar slot

void CInventoryManager::DrawSlot(int slotIndex, const Vector2& pos,
                                 bool isHotbar) {
  LSpriteDesc2D desc;

  // Determine if this slot is selected
  bool isSelected = false;
  if (isHotbar) {
    isSelected = (slotIndex == m_nHotbarSelection);
  } else if (m_bIsOpen) {
    isSelected = (slotIndex == m_nSelectedSlot);
  }

  // Draw slot background
  if (isSelected) {
    desc.m_nSpriteIndex = (UINT)eSprite::InventorySlotSelected;
  } else {
    desc.m_nSpriteIndex = (UINT)eSprite::InventorySlot;
  }

  // Sprites draw from center, pos is bottom-left so add half size
  Vector2 slotCenter = pos + Vector2(m_fSlotSize / 2.0f, m_fSlotSize / 2.0f);
  desc.m_vPos = slotCenter;
  m_pRenderer->Draw(&desc);

  // Draw item if slot has one
  CItem* item = m_vItems[slotIndex];
  if (item) {
    desc.m_nSpriteIndex = (UINT)item->GetSprite();
    desc.m_vPos = slotCenter;
    m_pRenderer->Draw(&desc);

    // Draw quantity if stackable and > 1
    if (item->IsStackable() && item->GetQuantity() > 1) {
      std::string qtyStr = std::to_string(item->GetQuantity());
      // Position at bottom-right of slot, convert to text coords
      float textX = pos.x + m_fSlotSize - 18.0f;
      float textY = SpriteYToTextY(pos.y + 20.0f);
      m_pRenderer->DrawScreenText(qtyStr.c_str(), Vector2(textX, textY));
    }
  }
}

/// Draw item info for the selected item.

void CInventoryManager::DrawItemInfo() {
  if (m_nSelectedSlot < 0 || !m_vItems[m_nSelectedSlot]) return;

  CItem* item = m_vItems[m_nSelectedSlot];

  // Info area lives in the reserved 80px section added in UpdateLayout().
  constexpr float infoAreaHeight = 95.0f;
  constexpr float infoPadding = 10.0f;
  constexpr float lineHeight = 18.0f;

  float infoBottomSpriteY = m_vPanelPos.y + m_fPanelPadding + infoPadding;
  float infoTopSpriteY =
      infoBottomSpriteY + infoAreaHeight - infoPadding * 2.0f;
  float usableHeight = infoTopSpriteY - infoBottomSpriteY;
  if (usableHeight <= 0.0f) return;

  int maxLines = static_cast<int>(usableHeight / lineHeight);
  if (maxLines <= 0) return;

  float textX = m_vPanelPos.x + m_fPanelPadding;
  float usableWidth = m_vPanelSize.x - m_fPanelPadding * 2.0f;

  // Approximate characters per line assuming ~9px per glyph.
  size_t maxCharsPerLine =
      static_cast<size_t>(std::max(usableWidth / 9.0f, 8.0f));

  auto wrapText = [&](const std::string& text) {
    std::vector<std::string> lines;
    if (text.empty()) return lines;

    std::istringstream stream(text);
    std::string word;
    std::string current;

    while (stream >> word) {
      std::string candidate = current.empty() ? word : current + " " + word;
      if (candidate.length() > maxCharsPerLine) {
        if (!current.empty()) {
          lines.push_back(current);
          current = word;
        } else {
          // Single word longer than the line, hard truncate.
          if (word.length() > maxCharsPerLine) {
            lines.push_back(word.substr(0, maxCharsPerLine - 3) + "...");
          } else {
            lines.push_back(word);
          }
          current.clear();
        }
      } else {
        current = candidate;
      }
    }

    if (!current.empty()) {
      lines.push_back(current);
    }
    return lines;
  };

  std::vector<std::string> lines;
  lines.push_back(item->GetName());
  std::vector<std::string> descriptionLines = wrapText(item->GetDescription());
  lines.insert(lines.end(), descriptionLines.begin(), descriptionLines.end());
  if (item->IsStackable()) {
    lines.push_back("Qty: " + std::to_string(item->GetQuantity()));
  }

  float textY = SpriteYToTextY(infoTopSpriteY);
  for (int i = 0; i < static_cast<int>(lines.size()) && i < maxLines; ++i) {
    m_pRenderer->DrawScreenText(lines[i].c_str(), Vector2(textX, textY));
    textY += lineHeight;
  }
}

/// Draw the hotbar at the bottom of the screen.

void CInventoryManager::DrawHotbar() {
  for (int i = 0; i < m_nHotbarSlots; i++) {
    Vector2 slotPos = GetHotbarSlotPosition(i);
    DrawSlot(i, slotPos, true);
  }
}

/// Draw just the hotbar (always visible at bottom of screen).

void CInventoryManager::DrawHotbarOnly() { DrawHotbar(); }

/// Draw the entire inventory UI.

void CInventoryManager::Draw() {
  if (!m_bIsOpen) return;

  // Draw semi-transparent background panel
  DrawPanel();

  // Draw title at top of panel
  float titleX = m_vPanelPos.x + m_fPanelPadding;
  float titleY =
      SpriteYToTextY(m_vPanelPos.y + m_vPanelSize.y - m_fPanelPadding - 5.0f);
  m_pRenderer->DrawScreenText("INVENTORY", Vector2(titleX, titleY));

  // Draw controls hint at top right, keep inside panel bounds
  const char* controlsText = "[I] Close";
  float approxCharWidth = 15.0f;
  float controlsWidth =
      static_cast<float>(std::strlen(controlsText)) * approxCharWidth;
  float controlsX =
      m_vPanelPos.x + m_vPanelSize.x - m_fPanelPadding - controlsWidth;
  controlsX = std::max(controlsX, m_vPanelPos.x + m_fPanelPadding);
  m_pRenderer->DrawScreenText(controlsText, Vector2(controlsX, titleY));

  // Draw all inventory slots
  for (int i = 0; i < m_nMaxSlots; i++) {
    Vector2 slotPos = GetSlotPosition(i);
    DrawSlot(i, slotPos, false);
  }

  // Draw selected item info
  DrawItemInfo();
}

/// Check if inventory has room for an item.

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
