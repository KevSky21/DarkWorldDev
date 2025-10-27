#include "Item.h"

/// Constructor initializes all item properties.
/// \param id Unique item ID
/// \param name Display name
/// \param desc Description text
/// \param sprite Sprite to use for display
/// \param type Item category
/// \param stackable Whether item can stack
/// \param maxStack Maximum stack size

CItem::CItem(int id, const std::string& name, const std::string& desc,
    eSprite sprite, eItemType type, bool stackable, int maxStack)
    : m_nItemID(id)
    , m_sName(name)
    , m_sDescription(desc)
    , m_eSpriteType(sprite)
    , m_eItemType(type)
    , m_nQuantity(1)
    , m_bStackable(stackable)
    , m_nMaxStack(maxStack)
{
}

/// Check if this stack can accept additional items.
/// \param amount Number of items to add
/// \return True if stack has room

bool CItem::CanAddToStack(int amount) const {
    if (!m_bStackable) return false;
    return (m_nQuantity + amount) <= m_nMaxStack;
}