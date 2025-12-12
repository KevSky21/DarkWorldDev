#pragma once
#include "Keyboard.h"
#include "SimpleMath.h"
#include "SpriteRenderer.h"
#include "Game.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

class CTileManager;
class CGame;         


class CPlayer {
 private:
  LSpriteRenderer *m_pRenderer = nullptr;
  CGame *m_pGame = nullptr;
  b2Body *mBody;
  b2World *mWorld;
  Vector2 m_sensorCenter;
  float m_sensorHalfW = 0.0f;
  float m_sensorHalfH = 0.0f;
  float m_halfSpriteW = 8.0f;
  float m_halfSpriteH = 16.0f;


  Vector2 m_vPos = {100.0f, 1000.0f}; //1M = 16 Pixels
  Vector2 m_vVel = {0.0f, 0.0f};
  float m_fSpeed = 5.0f;
  float m_fRadius = 16.0f;
  float m_fWidth = 14.0f / 16.0f;
  float m_fHeight = 24.0f / 16.0f ;
  UINT m_uHealth = 100;
  const UINT m_uMaxHealth = 100;

  bool m_groundContacts = false;
  int m_headContacts = 0;
  int m_leftWallContacts = 0;
  int m_rightWallContacts = 0;


  Vector2 m_debugFootPos;


  float m_coyoteTimer = 0.0f;
  float m_coyoteTimeMax = 0.1f;  // ~100 ms grace window

  float m_fJumpSpeed = 450.0f;  // upward impulse
  bool m_bIsGrounded = false;   // whether player is standing on floor
  bool m_wantsToShoot = false;

  bool m_bIsAttacking = false;
  float m_fAttackCooldown = 0.5f;  // seconds between attacks
  float m_fAttackTimer = 0.0f;
  float m_fAttackRange = 40.0f;   // pixels in front of player
  float m_fAttackRadius = 16.0f;  // size of attack hitbox

  //-- Test for visual hitbox
  int m_iFacingDir = 1;  // 1 = right, -1 = left

 public:
  friend class ContactListener;
  CPlayer(LSpriteRenderer *renderer, b2World *world, CGame *game)

  : m_pRenderer(renderer), mWorld(world), m_pGame(game)
  {

    float scale = 32.0f;
    enum SensorType { FOOT, HEAD, LEFT, RIGHT };
    //Body
    b2BodyDef def;
    def.type = b2_dynamicBody;
    def.position.Set(m_vPos.x / scale, m_vPos.y / scale);
    def.fixedRotation = true;  // THIS IS REQUIRED

    
    mBody = mWorld->CreateBody(&def);
    m_pGame->RegisterDebugBody(mBody);

    b2PolygonShape box;
    float halfWB = (m_fWidth * 0.5f);  
    float halfHB = (m_fHeight * 0.5f);
    box.SetAsBox(halfWB, halfHB);

    b2FixtureDef fix;
    fix.shape = &box;
    fix.density = 1.0f;
    fix.friction = 0.0f;


    mBody->CreateFixture(&fix);

    //Foot
    b2PolygonShape footBox;

    float halfW = m_fWidth * 0.5f;
    float halfH = m_fHeight * 0.5f;

    b2PolygonShape footShape;

    float sensorHalfW = halfW * 0.9f;  
    float sensorHalfH = 0.02f;       
    float sensorYOffset = -halfH - sensorHalfH;


    footShape.SetAsBox(sensorHalfW, sensorHalfH, b2Vec2(0.0f, sensorYOffset),
                       0.0f);

    b2FixtureDef sensorFix;
    sensorFix.shape = &footShape;
    sensorFix.isSensor = true;
    sensorFix.userData.pointer = (uintptr_t)this;

    mBody->CreateFixture(&sensorFix);

    // HEAD SENSOR (directly above the player)
    b2PolygonShape headShape;

    float headHalfW = halfW * 0.9f;
    float headHalfH = 0.02f;                // thin sensor
    float headYOffset = halfH + headHalfH;  // above the body

    headShape.SetAsBox(headHalfW, headHalfH, b2Vec2(0.0f, headYOffset), 0.0f);

    b2FixtureDef headFix;
    headFix.shape = &headShape;
    headFix.isSensor = true;
    headFix.userData.pointer = (uintptr_t)this;

    mBody->CreateFixture(&headFix);

    //Left
    b2PolygonShape leftShape;

    float wallHalfW = 0.02f;         
    float wallHalfH = halfH * 0.8f;  
    float leftX = -halfW - wallHalfW;

    leftShape.SetAsBox(wallHalfW, wallHalfH, b2Vec2(leftX, 0.0f), 0.0f);

    b2FixtureDef leftFix;
    leftFix.shape = &leftShape;
    leftFix.isSensor = true;
    leftFix.userData.pointer = (uintptr_t)this;

    mBody->CreateFixture(&leftFix);

    //Right
    b2PolygonShape rightShape;

    float rightX = halfW + wallHalfW;

    rightShape.SetAsBox(wallHalfW, wallHalfH, b2Vec2(rightX, 0.0f), 0.0f);

    b2FixtureDef rightFix;
    rightFix.shape = &rightShape;
    rightFix.isSensor = true;
    rightFix.userData.pointer = (uintptr_t)this;

    mBody->CreateFixture(&rightFix);


  }
  b2Body *GetBody() const { return mBody; }
  bool IsGrounded() const { return m_groundContacts; }
  bool IsHeadBlocked() const { return m_headContacts > 0; }
  bool TouchingLeftWall() const { return m_leftWallContacts > 0; }
  bool TouchingRightWall() const { return m_rightWallContacts > 0; }

  void RequestShoot();
  bool WantsToShoot() const { return m_wantsToShoot; }
  void ClearShootRequest() { m_wantsToShoot = false; }


  void Update(float dt, LKeyboard *pKeyboard, CTileManager *pTiles);
  void Draw();
  void TakeDamage(UINT damage);
  void HealDamage(UINT heal);
  const Vector2 &GetPos() const { return m_vPos; }
  float GetRadius() const { return m_fRadius; }

};
