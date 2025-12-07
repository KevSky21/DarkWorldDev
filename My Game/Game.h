/// \file Game.h
/// \brief Interface for the game class CGame.

#ifndef __L4RC_GAME_GAME_H__
#define __L4RC_GAME_GAME_H__
#include <iostream>
#include "Component.h"
#include "Settings.h"
#include "SpriteDesc.h"
#include "SpriteRenderer.h"
#include "TileManager.h"

/// \brief The game class.
///
/// The game class is the object-oriented implementation of the game. This class
/// must contain the following public member functions. `Initialize()` does
/// initialization and will be run exactly once at the start of the game.
/// `ProcessFrame()` will be called once per frame to create and render the
/// next animation frame. `Release()` will be called at game exit but before
/// any destructors are run.
class CPlayer;
class CTileManager;

class CGame : public LComponent, public LSettings {

private:
  bool m_bDrawFrameRate = false;          ///< Draw the frame rate.
  LSpriteDesc2D *m_pSpriteDesc = nullptr; ///< Sprite descriptor.
  LSpriteRenderer *m_pRenderer = nullptr; ///< Pointer to renderer.
  CTileManager *m_pTileManager = nullptr;
  CPlayer *m_pPlayer = nullptr;

  void LoadImages();        ///< Load images.
  void LoadSounds();        ///< Load sounds.
  void BeginGame();         ///< Begin playing the game.
  void CreateObjects() {}   ///< Create game objects.
  void KeyboardHandler();   ///< The keyboard handler.
  void RenderFrame();       ///< Render an animation frame.
  void DrawFrameRateText(); ///< Draw frame rate text to screen.
  void FollowCamera();       ///< Make camera follow player character.

 public:
  ~CGame(); ///< Destructor.

  void Initialize();   ///< Initialize the game.
  void ProcessFrame(); ///< Process an animation frame.
  void Release();      ///< Release the renderer.
}; // CGame

#endif //__L4RC_GAME_GAME_H__
