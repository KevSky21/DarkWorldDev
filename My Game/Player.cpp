#include "Player.h"
#include "SpriteRenderer.h"
#include "Keyboard.h" 
#include "GameDefines.h"
#include "TileManager.h"

void CPlayer::Update(float dt, LKeyboard* pKeyboard, CTileManager* pTiles) {
    
    Vector2 dir(0, 0);
    if (pKeyboard->Down('A')) dir.x -= 1;
    if (pKeyboard->Down('D')) dir.x += 1;

    if (dir.LengthSquared() > 0)
        dir.Normalize();

    m_fAttackTimer -= dt;
    if (m_fAttackTimer < 0) m_fAttackTimer = 0;

    m_vVel.x = dir.x * m_fSpeed; // horizontal velocity

    // --- Gravity ---
    m_vVel.y += m_fGravity * dt; // apply gravity (downward acceleration)

    // --- Jump input ---
    if (m_bIsGrounded && pKeyboard->TriggerDown(VK_SPACE)) {
        m_vVel.y = m_fJumpSpeed;   // jump impulse
        m_bIsGrounded = false;
    }

    // --- Predict new position separately for X and Y ---
    Vector2 newPos = m_vPos;

    // --- Collision check ---
    if (pTiles && pTiles->CheckCollision(newPos, 16.0f)) {
        // Simple floor collision response
        if (m_vVel.y < 0) {          // falling
            m_vVel.y = 0;
            m_bIsGrounded = true;
        }
        newPos = m_vPos;             // stop movement into wall
    }
    else {
        m_bIsGrounded = false;
    }

    // Horizontal movement first
    Vector2 testPosX = newPos + Vector2(m_vVel.x * dt, 0);
    if (!pTiles->CheckCollision(testPosX, 16.0f)) {
        newPos.x = testPosX.x; // no horizontal collision, apply move
    }

    // Vertical movement next
    Vector2 testPosY = newPos + Vector2(0, m_vVel.y * dt);
    if (pTiles->CheckCollision(testPosY, 16.0f)) {
        // hit floor or ceiling
        if (m_vVel.y < 0) {         // falling
            m_bIsGrounded = true;
        }
        m_vVel.y = 0;
    }
    else {
        m_bIsGrounded = false;
        newPos.y = testPosY.y;
    }
    if (dir.x != 0)
        m_iFacingDir = (dir.x > 0 ? 1 : -1);

    // --- Attack Input ---
    if (pKeyboard->TriggerDown('J') && m_fAttackTimer == 0.0f) {
        m_bIsAttacking = true;
        m_fAttackTimer = m_fAttackCooldown;
    }

    if (m_bIsAttacking) {
        Vector2 attackCenter = m_vPos + Vector2(m_fAttackRange * (dir.x >= 0 ? 1 : -1), 0);

        // For now, just visualize or log the attack
        // (later you’ll check collision against enemies)
        // Example placeholder:
        OutputDebugStringA("Attack!\n");

        // Example: if you had an enemy vector
        // for (auto& e : enemies)
        //     if ((e->GetPos() - attackCenter).Length() < (m_fAttackRadius + e->GetRadius()))
        //         e->TakeDamage(1);
        
        // Keep visible until cooldown expires for testing
        if (m_fAttackTimer <= 0.0f)
        m_bIsAttacking = false; // one frame attack
    }

    m_vPos = newPos;
}

void CPlayer::Draw() {
    LSpriteDesc2D desc;
    desc.m_nSpriteIndex = static_cast<UINT>(eSprite::Pig); // temporary player sprite
    desc.m_vPos = m_vPos;
    m_pRenderer->Draw(&desc);

    // --- Debug draw for attack hitbox ---
    if (m_bIsAttacking) {
        Vector2 attackCenter = m_vPos + Vector2(m_fAttackRange * m_iFacingDir, 0);
        BoundingBox box;
        box.Center = Vector3(attackCenter.x, attackCenter.y, 0);
        box.Extents = Vector3(m_fAttackRadius, m_fAttackRadius, 0);
        m_pRenderer->DrawBoundingBox(eSprite::Dirt, box);
    }
}
