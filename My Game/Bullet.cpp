#include "Bullet.h"
#include "GameDefines.h"


CBullet::CBullet(b2World* world, const b2Vec2& pos, const b2Vec2& vel) {
  b2BodyDef def;
  def.type = b2_dynamicBody;
  def.bullet = true;
  def.position = pos;

  b2CircleShape shape;
  shape.m_radius = 0.1f;

  b2FixtureDef fd;
  fd.shape = &shape;
  fd.density = 1.0f;
  fd.friction = 0.0f;
  fd.restitution = 0.0f;

  m_body = world->CreateBody(&def);
  m_body->CreateFixture(&fd);
  m_body->SetLinearVelocity(vel);
}

CBullet::~CBullet() {}

void CBullet::Update(float dt) { m_life -= dt; }

void CBullet::Draw(LSpriteRenderer* renderer) {
  float scale = 32.0f;
  LSpriteDesc2D desc;
  desc.m_nSpriteIndex = (UINT)eSprite::Bullet;

  b2Vec2 p = m_body->GetPosition();
  desc.m_vPos.x = p.x * scale;
  desc.m_vPos.y = p.y * scale;

  renderer->Draw(&desc);
}

bool CBullet::IsDead() const { return m_life <= 0.0f; }

b2Body* CBullet::GetBody() const { return m_body; }
