#pragma once
#include "SpriteRenderer.h"
#include "SimpleMath.h"
#include "Keyboard.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

class CTileManager; //forward reference

class CPlayer {
private:
    Vector2 m_vPos = { 512.0f, 700.0f };
    Vector2 m_vVel = { 0.0f, 0.0f };
    float m_fSpeed = 200.0f;
    float m_fRadius = 16.0f;
    LSpriteRenderer* m_pRenderer = nullptr;

    float m_fGravity = -980.0f;       // pixels per second^2
    float m_fJumpSpeed = 450.0f;      // upward impulse
    bool m_bIsGrounded = false;       // whether player is standing on floor

    bool m_bIsAttacking = false;
    float m_fAttackCooldown = 0.5f;   // seconds between attacks
    float m_fAttackTimer = 0.0f;
    float m_fAttackRange = 40.0f;     // pixels in front of player
    float m_fAttackRadius = 16.0f;    // size of attack hitbox

    //-- Test for visual hitbox
    int m_iFacingDir = 1; // 1 = right, -1 = left


public:
    CPlayer(LSpriteRenderer* renderer) : m_pRenderer(renderer) {}
    void Update(float dt, LKeyboard* pKeyboard, CTileManager* pTiles);
    void Draw();
    const Vector2& GetPos() const { return m_vPos; }
    float GetRadius() const { return m_fRadius; }
};
