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

class CTileManager {
private:
  size_t m_nWidth = 0;       ///< Number of tiles wide.
  size_t m_nHeight = 0;      ///< Number of tiles high.
  float m_fTileSize = 64.0f; ///< Tile width and height (adjust later).

  char **m_chMap = nullptr; ///< The level map.

  LSpriteRenderer *m_pRenderer = nullptr; ///< Renderer (provided by Game).

  std::vector<Vector2> m_solidTiles; // positions of solid tiles for collision

public:
  CTileManager(LSpriteRenderer *renderer, float tileSize = 64.0f);
  ~CTileManager();

  void LoadMap(const char *filename); ///< Load a map from text file
  void Draw();                        ///< Draw all tiles

  bool CheckCollision(const Vector2 &pos,
                      float radius); ///< Check for collision with solid tiles
};
