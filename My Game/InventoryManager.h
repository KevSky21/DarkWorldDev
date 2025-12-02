#pragma once

#include <vector>

#include "Item.h"
#include "Keyboard.h"
#include "SimpleMath.h"
#include "SpriteRenderer.h"

using namespace DirectX::SimpleMath;

class CPlayer;

/// \brief The inventory manager class.
/// Manages item storage, UI display, hotbar, and user interaction.
class CInventoryManager {
 private:
  std::vector<CItem*> m_vItems;         ///< Array of item slots
  static const int m_nMaxSlots = 24;    ///< Maximum inventory slots
  static const int m_nHotbarSlots = 6;  ///< Number of hotbar slots (first row)
  int m_nSelectedSlot = 0;              ///< Currently selected slot
  int m_nHotbarSelection = 0;  ///< Currently selected hotbar slot (0-5)
  bool m_bIsOpen = false;      ///< Whether full inventory UI is visible

  LSpriteRenderer* m_pRenderer = nullptr;  ///< Pointer to renderer

  // Screen dimensions (set during initialization)
  float m_fScreenWidth = 1280.0f;
  float m_fScreenHeight = 720.0f;

  // UI Layout constants
  static constexpr float m_fSlotSize = 64.0f;  ///< Size of each inventory slot
  static constexpr float m_fSlotPadding = 4.0f;    ///< Padding between slots
  static constexpr int m_nSlotsPerRow = 6;         ///< Number of slots per row
  static constexpr float m_fPanelPadding = 20.0f;  ///< Padding inside panel

  // Calculated positions (updated in UpdateLayout)
  Vector2 m_vInventoryPos;  ///< Top-left position of inventory grid
  Vector2 m_vHotbarPos;     ///< Position of hotbar at bottom of screen
  Vector2 m_vPanelPos;      ///< Position of background panel
  Vector2 m_vPanelSize;     ///< Size of background panel

  struct SDroppedItem {
    CItem* pItem = nullptr;
    Vector2 vPos = Vector2::Zero;
    float fTimer = 0.0f;
    float fPickupDelay = 0.0f;
  };

  std::vector<SDroppedItem>
      m_vDroppedItems;  ///< World items spawned from drops

  CPlayer* m_pPlayer = nullptr;  ///< Player pointer for world drop placement

  static constexpr float m_fPickupRadius = 32.0f;
  static constexpr float m_fPickupDelayTime = 0.25f;
  static constexpr float m_fBobAmplitude = 6.0f;
  static constexpr float m_fBobSpeed = 2.5f;

  /// \brief Update layout positions based on screen size.
  void UpdateLayout();

  /// \brief Get the position of a slot by index (in inventory grid).
  Vector2 GetSlotPosition(int slotIndex) const;

  /// \brief Get the position of a hotbar slot.
  Vector2 GetHotbarSlotPosition(int slotIndex) const;

  /// \brief Get slot index from screen position (for mouse input).
  int GetSlotAtPosition(const Vector2& pos) const;

  /// \brief Draw a single inventory slot.
  /// \param slotIndex Index of slot to draw
  /// \param pos Position to draw at
  /// \param isHotbar Whether this is a hotbar slot
  void DrawSlot(int slotIndex, const Vector2& pos, bool isHotbar = false);

  /// \brief Draw the background panel.
  void DrawPanel();

  /// \brief Draw the hotbar at the bottom of the screen.
  void DrawHotbar();

  /// \brief Draw item tooltip/info for selected item.
  void DrawItemInfo();

  /// \brief Convert sprite Y coordinate to text Y coordinate.
  /// Sprites use Y-up (0 at bottom), text uses Y-down (0 at top).
  float SpriteYToTextY(float spriteY) const;

 public:
  /// \brief Constructor.
  /// \param renderer Pointer to sprite renderer
  CInventoryManager(LSpriteRenderer* renderer);

  /// \brief Destructor - cleans up all items.
  ~CInventoryManager();

  /// \brief Set screen dimensions for layout calculations.
  /// \param width Screen width
  /// \param height Screen height
  void SetScreenSize(float width, float height);

  /// \brief Provide player reference for drop placement.
  void SetPlayer(CPlayer* player);

  /// \brief Update world drop state (bobbing, pickup detection).
  void Update(float dt);

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
  void Toggle();

  /// \brief Check if inventory is open.
  bool IsOpen() const { return m_bIsOpen; }

  /// \brief Set inventory open state.
  void SetOpen(bool open) { m_bIsOpen = open; }

  /// \brief Handle keyboard input for inventory navigation.
  /// \param pKeyboard Pointer to keyboard controller
  void HandleInput(LKeyboard* pKeyboard);

  /// \brief Handle number key input for hotbar selection (1-6).
  /// \param pKeyboard Pointer to keyboard controller
  void HandleHotbarInput(LKeyboard* pKeyboard);

  /// \brief Draw the full inventory UI (when open).
  void Draw();

  /// \brief Draw just the hotbar (always visible).
  void DrawHotbarOnly();

  /// \brief Draw items dropped into the world.
  void DrawWorldItems();

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

  /// \brief Get the currently selected hotbar slot.
  int GetHotbarSelection() const { return m_nHotbarSelection; }

  /// \brief Get item in currently selected hotbar slot.
  CItem* GetHotbarItem() const;

  /// \brief Use the item in the currently selected hotbar slot.
  void UseHotbarItem();
};
