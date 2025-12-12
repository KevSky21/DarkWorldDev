#pragma once

#include "box2d/box2d.h"
#include "SpriteRenderer.h"

class CBullet {
 public:
  CBullet(b2World* world, const b2Vec2& pos, const b2Vec2& vel);
  ~CBullet();

  void Update(float dt);
  void Draw(LSpriteRenderer* renderer);

  bool IsDead() const;
  b2Body* GetBody() const;

 private:
  b2Body* m_body = nullptr;
  float m_life = 2.0f;
};
