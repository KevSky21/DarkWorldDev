/// \file TileManager.h
/// \brief Interface for the tile manager CTileManager.

#pragma once

#include "Common.h"
#include "Sprite.h"
#include "SpriteRenderer.h"
#include "GameDefines.h"
#include <vector>

/// \brief The tile manager.
/// Responsible for loading a tile map and drawing it.
struct TileRect {
  int x, y, w, h;
};

class CTileManager {
private:
  size_t m_nWidth = 0;       ///< Number of tiles wide.
  size_t m_nHeight = 0;      ///< Number of tiles high.
  float m_fTileSize = 16.0f; ///< Tile width and height (adjust later).

  char **m_chMap = nullptr; ///< The level map.

  LSpriteRenderer *m_pRenderer = nullptr; ///< Renderer (provided by Game).

  std::vector<Vector2> m_solidTiles; // positions of solid tiles for collision

public:

  CTileManager(LSpriteRenderer *renderer, float tileSize = 16.0f);
  ~CTileManager();

  void LoadMap(const char *filename); ///< Load a map from text file
  void Draw();                        ///< Draw all tiles

  const std::vector<Vector2> &GetSolidTiles() const { return m_solidTiles; }
  const float &GetTileSize() const { return m_fTileSize; }
  int GetMapHeight() const { return m_nHeight; }
  std::vector<TileRect> m_solidRects;
  const std::vector<TileRect> &GetSolidRects() { return m_solidRects; }
};



