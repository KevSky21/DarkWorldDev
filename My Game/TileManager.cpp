#include "TileManager.h"

#include "Sprite.h"
#include "SpriteRenderer.h"

#include <fstream>
#include <iostream>

CTileManager::CTileManager(LSpriteRenderer *renderer, float tileSize)
    : m_pRenderer(renderer), m_fTileSize(tileSize) {}

CTileManager::~CTileManager() {
  if (m_chMap != nullptr) {
    for (size_t i = 0; i < m_nHeight; i++)
      delete[] m_chMap[i];
    delete[] m_chMap;
  }
}

void CTileManager::LoadMap(const char *filename) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    std::cerr << "Could not open map file: " << filename << std::endl;
    return;
  }

  std::vector<std::string> lines;
  std::string line;
  while (std::getline(file, line)) {
    if (!line.empty())
      lines.push_back(line);
  }

  m_nHeight = lines.size();
  m_nWidth = lines[0].size();

  // Allocate and copy the map
  m_chMap = new char *[m_nHeight];
  for (size_t i = 0; i < m_nHeight; i++) {
    m_chMap[i] = new char[m_nWidth];
    for (size_t j = 0; j < m_nWidth; j++) {
      m_chMap[i][j] = lines[i][j];
    }
  }

  m_solidTiles.clear();
  // Identify solid tiles (represented by '#')
  for (size_t i = 0; i < m_nHeight; i++) {
    for (size_t j = 0; j < m_nWidth; j++) {
      if (m_chMap[i][j] == '#') {
        float x = (j + 0.5f) * m_fTileSize;
        float y = (m_nHeight - 1 - i + 0.5f) * m_fTileSize;
        m_solidTiles.push_back(Vector2(x, y));
      }
    }
  }
  //m_vWorldSize = Vector2((float)m_nWidth, (float)m_nHeight) * m_fTileSize;
  std::cout << "Loaded map: " << m_nWidth << "x" << m_nHeight << std::endl;
}

void CTileManager::Draw() {
  if (!m_chMap) return;

  LSpriteDesc2D desc;
  float yOffset = -200.0f;  // or your desired offset

  for (size_t i = 0; i < m_nHeight; i++) {
    for (size_t j = 0; j < m_nWidth; j++) {
      desc.m_nSpriteIndex = (UINT)eSprite::Dirt;

      desc.m_vPos.x = (j + 0.5f) * m_fTileSize;

      // match LoadMap() Y calculation
      desc.m_vPos.y = (m_nHeight - 1 - i + 0.5f) * m_fTileSize + yOffset;

      m_pRenderer->Draw(&desc);
    }
  }
}


// Check for collision with solid tiles
bool CTileManager::CheckCollision(const Vector2 &pos, float radius) {
  const float yOffset = -200.0f;

  float half = m_fTileSize * 0.5f;

  for (auto &tilePos : m_solidTiles) {
    float tileX = tilePos.x;
    float tileY = tilePos.y + yOffset;

    if (fabs(pos.x - tileX) < (radius + half) &&
        fabs(pos.y - tileY) < (radius + half)) {
      return true;
    }
  }
  return false;
}
