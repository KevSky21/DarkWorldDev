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

  m_solidRects.clear();
  std::vector<std::vector<bool>> used(m_nHeight,std::vector<bool>(m_nWidth, false));

  for (int y = 0; y < m_nHeight; ++y)
    for (int x = 0; x < m_nWidth; ++x) {
      if (m_chMap[y][x] != '1' || used[y][x]) continue;

      int w = 0;
      while (x + w < m_nWidth && m_chMap[y][x + w] == '1' && !used[y][x + w])
        w++;

      int h = 1;
      bool done = false;
      while (y + h < m_nHeight && !done) {
        for (int i = 0; i < w; ++i)
          if (m_chMap[y + h][x + i] != '1' || used[y + h][x + i]) {
            done = true;
            break;
          }
        if (!done) h++;
      }

      for (int yy = y; yy < y + h; ++yy)
        for (int xx = x; xx < x + w; ++xx) used[yy][xx] = true;

      m_solidRects.push_back({x, y, w, h});
    }

  //m_solidTiles.clear();
  //// Identify solid tiles (represented by '#')
  //for (size_t i = 0; i < m_nHeight; i++) {
  //  for (size_t j = 0; j < m_nWidth; j++) {
  //    if (m_chMap[i][j] == '1') {
  //      float x = (j * 0.5f) * m_fTileSize;
  //      float y = (m_nHeight - 1 - i * 0.5f) * m_fTileSize;
  //      m_solidTiles.push_back(Vector2(x, y));
  //    } else {
  //      continue;
  //    }
  //  }
  //}
  //m_vWorldSize = Vector2((float)m_nWidth, (float)m_nHeight) * m_fTileSize;
  std::cout << "Loaded map: " << m_nWidth << "x" << m_nHeight << std::endl;
}

void CTileManager::Draw() {
  if (!m_chMap) return;

LSpriteDesc2D d;
  d.m_nSpriteIndex = (UINT)eSprite::Dirt;

  for (int y = 0; y < m_nHeight; ++y) {
    for (int x = 0; x < m_nWidth; ++x) {
      if (m_chMap[y][x] != '1') continue;

      d.m_vPos.x = (x + 0.5f) * m_fTileSize;
      d.m_vPos.y = (m_nHeight - y - 0.5f) * m_fTileSize;

      m_pRenderer->Draw(&d);
    }
  }
}
