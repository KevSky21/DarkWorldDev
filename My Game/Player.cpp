#include "Player.h"
#include "SpriteRenderer.h"
#include "Keyboard.h" 
#include "GameDefines.h"
#include "TileManager.h"

void CPlayer::Update(float dt, LKeyboard* pKeyboard, CTileManager* pTiles) {
    Vector2 dir(0, 0);
    pKeyboard->GetState();

    if (pKeyboard->Down('A')) dir.x -= 1;
    if (pKeyboard->Down('D')) dir.x += 1;
    if (pKeyboard->Down('W')) dir.y += 1;
    if (pKeyboard->Down('S')) dir.y -= 1;

    if (dir.LengthSquared() > 0)
        dir.Normalize();

    Vector2 newPos = m_vPos + dir * m_fSpeed * dt;

    // --- Simplified collision check ---
    if (pTiles && pTiles->CheckCollision(newPos, 16.0f)) {
        // There’s a solid tile here — cancel movement
        newPos = m_vPos;
    }

    m_vPos = newPos;
}

void CPlayer::Draw() {
    LSpriteDesc2D desc;
    desc.m_nSpriteIndex = static_cast<UINT>(eSprite::Pig); // temporary player sprite
    desc.m_vPos = m_vPos;
    m_pRenderer->Draw(&desc);
}
