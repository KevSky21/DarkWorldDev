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


public:
    CPlayer(LSpriteRenderer* renderer) : m_pRenderer(renderer) {}
    void Update(float dt, LKeyboard* pKeyboard, CTileManager* pTiles);
    void Draw();
    const Vector2& GetPos() const { return m_vPos; }
    float GetRadius() const { return m_fRadius; }
};
