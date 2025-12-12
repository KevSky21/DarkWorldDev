#include "Player.h"


#include "GameDefines.h"
#include "Keyboard.h"
#include "SpriteRenderer.h"
#include "TileManager.h"




void CPlayer::Update(float dt, LKeyboard *pKeyboard, CTileManager *pTiles) {
  float scale = 32.0f;

  b2Vec2 pos = mBody->GetPosition();

  // convert Box2D meters → engine pixels
  m_vPos.x = pos.x * scale;
  m_vPos.y = pos.y * scale;
 


  b2Vec2 vel = mBody->GetLinearVelocity();
  float target = 0.0f;

  if (pKeyboard->Down('A')) target = -m_fSpeed;
  if (pKeyboard->Down('D')) target = m_fSpeed;

  // smooth acceleration toward target
  float accel = 15.0f;

  vel.x += (target - vel.x) * accel * dt;




  m_fAttackTimer -= dt;
  if (m_fAttackTimer < 0) m_fAttackTimer = 0;



 
  // --- Attack Input ---
  if (pKeyboard->TriggerDown('J') && m_fAttackTimer == 0.0f) {
    m_bIsAttacking = true;
    m_fAttackTimer = m_fAttackCooldown;
  }

  if (pKeyboard->TriggerDown('F')) {
    RequestShoot();
  }


  //if (m_bIsAttacking) {
  //  Vector2 attackCenter =
  //      m_vPos + Vector2(m_fAttackRange * (dir.x >= 0 ? 1 : -1), 0);

  //  // For now, just visualize or log the attack
  //  // (later you’ll check collision against enemies)
  //  // Example placeholder:
  //  OutputDebugStringA("Attack!\n");

  //  // Example: if you had an enemy vector
  //  // for (auto& e : enemies)
  //  //     if ((e->GetPos() - attackCenter).Length() < (m_fAttackRadius +
  //  //     e->GetRadius()))
  //  //         e->TakeDamage(1);

  //  // Keep visible until cooldown expires for testing
  //  if (m_fAttackTimer <= 0.0f) m_bIsAttacking = false;  // one frame attack
  //}


  b2Fixture* sensor = mBody->GetFixtureList()->GetNext();  // main fixture → foot sensor
  const b2Transform& xf = mBody->GetTransform();
  b2Vec2 sensorWorld = b2Mul(xf, sensor->GetAABB(0).GetCenter());

  m_debugFootPos.x = sensorWorld.x * scale;
  m_debugFootPos.y = sensorWorld.y * scale;

  b2Vec2 localCenter = sensor->GetAABB(0).GetCenter();
  m_sensorCenter.x = localCenter.x * scale;
  m_sensorCenter.y = localCenter.y * scale;

  if (IsGrounded())
    m_coyoteTimer = m_coyoteTimeMax;
  else
    m_coyoteTimer -= dt;

if (m_coyoteTimer > 0.0f && pKeyboard->TriggerDown(VK_SPACE)) {
    m_coyoteTimer = 0.0f;

    b2Vec2 v = mBody->GetLinearVelocity();
    v.y = 7.0f;  // meters per second
    mBody->SetLinearVelocity(v);
  }

  mBody->SetLinearVelocity(b2Vec2(vel.x, mBody->GetLinearVelocity().y));
}

void CPlayer::Draw() {
  LSpriteDesc2D desc;
  desc.m_nSpriteIndex =
      m_bIsAttacking
          ? static_cast<UINT>(eSprite::Jab)
          : static_cast<UINT>(eSprite::Step);  // temporary player sprite
  desc.m_vPos = m_vPos;
  m_pRenderer->Draw(&desc);

  desc.m_vPos.x = m_vPos.x;
  desc.m_vPos.y = m_vPos.y;

  m_pRenderer->Draw(&desc);
  bool debug = false;
  if (debug) {
    if (IsGrounded()) {
      LSpriteDesc2D d;
      d.m_nSpriteIndex = (UINT)eSprite::DebugGreen;
      d.m_vPos = Vector2(m_vPos.x - 4, m_vPos.y - 40);
      m_pRenderer->Draw(&d);
    }
  }



  Vector2 healthEndpointOne;
  Vector2 healthEndpointTwo;

  // m_pRenderer->DrawLine(eSprite::Dirt, &healthEndpointOne,
  // &healthEndpointTwo);

  // --- Debug draw for attack hitbox ---
  if (m_bIsAttacking) {
    Vector2 attackCenter = m_vPos + Vector2(m_fAttackRange * m_iFacingDir, 0);
    BoundingBox box;
    box.Center = Vector3(attackCenter.x, attackCenter.y, 0);
    box.Extents = Vector3(m_fAttackRadius, m_fAttackRadius, 0);
    m_pRenderer->DrawBoundingBox(eSprite::Dirt, box);
  }
}

void CPlayer::TakeDamage(UINT damage) {
  if (m_uHealth - damage >= 0) {
    m_uHealth -= damage;
  } else {
    m_uHealth = 0;
  }
}

void CPlayer::HealDamage(UINT heal) {
  if (m_uHealth + heal <= m_uMaxHealth) {
    m_uHealth += heal;
  } else {
    m_uHealth = m_uMaxHealth;
  }
}

void CPlayer::RequestShoot() { m_wantsToShoot = true; }


