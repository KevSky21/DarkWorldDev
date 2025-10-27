#pragma once

#include <vector>
#include "Item.h"
#include "SpriteRenderer.h"
#include "Keyboard.h"
#include "SimpleMath.h"

using namespace DirectX::SimpleMath;

/// \brief The inventory manager class.
/// Manages item storage, UI display, and user interaction.
class CInventoryManager {
private:
    std::vector<CItem*> m_vItems;    ///< Array of item slots
    const int m_nMaxSlots = 24;      ///< Maximum inventory slots
    int m_nSelectedSlot = -1;        ///< Currently selected slot (-1 = none)
    bool m_bIsOpen = false;          ///< Whether inventory UI is visible

    LSpriteRenderer* m_pRenderer = nullptr; ///< Pointer to renderer

    // UI Layout constants
    const float m_fSlotSize = 48.0f;        ///< Size of each inventory slot
    const float m_fSlotPadding = 4.0f;      ///< Padding between slots
    const int m_nSlotsPerRow = 6;           ///< Number of slots per row
    Vector2 m_vInventoryPos = { 200.0f, 150.0f }; ///< Top-left position of inventory

    // UI Colors (RGBA)
    Vector4 m_cSlotBackground = { 0.2f, 0.2f, 0.2f, 0.9f };
    Vector4 m_cSlotBorder = { 0.5f, 0.5f, 0.5f, 1.0f };
    Vector4 m_cSelectedSlot = { 1.0f, 0.8f, 0.2f, 1.0f };

    /// \brief Get the position of a slot by index.
    Vector2 GetSlotPosition(int slotIndex) const;

    /// \brief Get slot index from screen position (for mouse input).
    int GetSlotAtPosition(const Vector2& pos) const;

    /// \brief Draw a single inventory slot.
    void DrawSlot(int slotIndex, const Vector2& pos);

public:
    /// \brief Constructor.
    /// \param renderer Pointer to sprite renderer
    CInventoryManager(LSpriteRenderer* renderer);

    /// \brief Destructor - cleans up all items.
    ~CInventoryManager();

    /// \brief Add an item to the inventory.
    /// \param item Pointer to item to add
    /// \return True if successfully added
    bool AddItem(CItem* item);

    /// \brief Remove an item from a specific slot.
    /// \param slotIndex Index of slot to remove from
    /// \return True if successfully removed
    bool RemoveItem(int slotIndex);

    /// \brief Get item at specific slot.
    /// \param slotIndex Slot index to check
    /// \return Pointer to item or nullptr if slot empty
    CItem* GetItem(int slotIndex) const;

    /// \brief Use/activate the currently selected item.
    void UseSelectedItem();

    /// \brief Drop the currently selected item.
    void DropSelectedItem();

    /// \brief Toggle inventory open/closed.
    void Toggle() { m_bIsOpen = !m_bIsOpen; }

    /// \brief Check if inventory is open.
    bool IsOpen() const { return m_bIsOpen; }

    /// \brief Set inventory open state.
    void SetOpen(bool open) { m_bIsOpen = open; }

    /// \brief Handle keyboard input for inventory navigation.
    /// \param pKeyboard Pointer to keyboard controller
    void HandleInput(LKeyboard* pKeyboard);

    /// \brief Draw the inventory UI.
    void Draw();

    /// \brief Check if inventory has room for an item.
    /// \param item Item to check
    /// \return True if item can be added
    bool HasRoom(CItem* item) const;

    /// \brief Get number of items of a specific type.
    /// \param itemID Item ID to count
    /// \return Total quantity
    int GetItemCount(int itemID) const;

    /// \brief Move selection in direction.
    /// \param direction -1 for left/up, +1 for right/down, -6/+6 for row up/down
    void MoveSelection(int direction);
};