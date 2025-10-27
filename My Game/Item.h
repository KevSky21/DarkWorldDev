#pragma once

#include <string>
#include "GameDefines.h"

/// \brief Item types enumeration.
/// Defines different categories of items in the game.
enum class eItemType {
    Consumable,
    Equipment,
    QuestItem,
    Material,
    Misc
};

/// \brief The item class.
/// Represents an individual item that can be stored in inventory.
class CItem {
private:
    int m_nItemID;              ///< Unique item identifier
    std::string m_sName;        ///< Display name of the item
    std::string m_sDescription; ///< Item description
    eSprite m_eSpriteType;      ///< Sprite to display for this item
    eItemType m_eItemType;      ///< Category of item
    int m_nQuantity;            ///< Number of items in this stack
    int m_nMaxStack;            ///< Maximum stack size
    bool m_bStackable;          ///< Whether item can stack

public:
    /// \brief Constructor.
    /// \param id Unique item ID
    /// \param name Display name
    /// \param desc Description text
    /// \param sprite Sprite to use for display
    /// \param type Item category
    /// \param stackable Whether item can stack
    /// \param maxStack Maximum stack size
    CItem(int id, const std::string& name, const std::string& desc,
        eSprite sprite, eItemType type, bool stackable = true, int maxStack = 99);

    // Getters
    int GetID() const { return m_nItemID; }
    const std::string& GetName() const { return m_sName; }
    const std::string& GetDescription() const { return m_sDescription; }
    eSprite GetSprite() const { return m_eSpriteType; }
    eItemType GetType() const { return m_eItemType; }
    int GetQuantity() const { return m_nQuantity; }
    int GetMaxStack() const { return m_nMaxStack; }
    bool IsStackable() const { return m_bStackable; }

    // Setters
    void SetQuantity(int qty) { m_nQuantity = qty; }
    void AddQuantity(int qty) { m_nQuantity += qty; }
    void RemoveQuantity(int qty) { m_nQuantity -= qty; }

    /// \brief Check if stack has room for more items.
    /// \param amount Amount to check
    /// \return True if can add this amount to stack
    bool CanAddToStack(int amount) const;

    /// \brief Check if item is empty (quantity <= 0).
    /// \return True if no items remain
    bool IsEmpty() const { return m_nQuantity <= 0; }
};
