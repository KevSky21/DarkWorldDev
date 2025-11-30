/// \file Game.cpp
/// \brief Code for the game class CGame.

#include "Game.h"

#include "Common.h"
#include "ComponentIncludes.h"
#include "GameDefines.h"
#include "Player.h"
#include "SpriteRenderer.h"
#include "TileManager.h"
#include "shellapi.h"

/// Delete the sprite descriptor. The renderer needs to be deleted before this
/// destructor runs so it will be done elsewhere.

CGame::~CGame() {
  delete m_pSpriteDesc;
  delete m_pInventory;  // cleans inventory

}  // destructor

/// Create the renderer and the sprite descriptor load images and sounds, and
/// begin the game.

void CGame::Initialize() {
  m_pRenderer = new LSpriteRenderer(eSpriteMode::Batched2D);
  m_pRenderer->Initialize(eSprite::Size);
  LoadImages();  // load images from xml file list

  m_pTileManager = new CTileManager(m_pRenderer);
  m_pTileManager->LoadMap("Media/Maps/testmap.txt");
  m_pPlayer = new CPlayer(m_pRenderer);

  // Initialize inventory with screen dimensions
  m_pInventory = new CInventoryManager(m_pRenderer);
  m_pInventory->SetScreenSize((float)m_nWinWidth, (float)m_nWinHeight);

  LoadSounds();  // load the sounds for this game

  BeginGame();
}  // Initialize

/// Load the specific images needed for this game. This is where `eSprite`
/// values from `GameDefines.h` get tied to the names of sprite tags in
/// `gamesettings.xml`. Those sprite tags contain the name of the corresponding
/// image file. If the image tag or the image file are missing, then the game
/// should abort from deeper in the Engine code leaving you with an error
/// message in a dialog box.

void CGame::LoadImages() {
  m_pRenderer->BeginResourceUpload();

  m_pRenderer->Load(eSprite::Background, "background");
  // m_pRenderer->Load(eSprite::TextWheel,  "textwheel");
  m_pRenderer->Load(eSprite::Pig, "pig");
  m_pRenderer->Load(eSprite::Dirt, "dirt");
  m_pRenderer->Load(eSprite::Step, "step");
  m_pRenderer->Load(eSprite::Jab, "jab");

  m_pRenderer->Load(eSprite::InventorySlot, "inventory_slot");
  m_pRenderer->Load(eSprite::InventoryPanel, "inventory_panel");
  m_pRenderer->Load(eSprite::InventorySlotSelected, "inventory_slot_selected");

  // Load item sprites
  m_pRenderer->Load(eSprite::ItemPotion, "item_potion");
  m_pRenderer->Load(eSprite::ItemKey, "item_key");
  m_pRenderer->Load(eSprite::ItemCoin, "item_coin");
  m_pRenderer->Load(eSprite::ItemApple, "item_apple");
  m_pRenderer->Load(eSprite::ItemShield, "item_shield");
  m_pRenderer->EndResourceUpload();
}  // LoadImages

/// Initialize the audio player and load game sounds.

void CGame::LoadSounds() {
  m_pAudio->Initialize(eSound::Size);
  //  m_pAudio->Load(eSound::Grunt, "grunt");
  // m_pAudio->Load(eSound::Clang, "clang");
  // m_pAudio->Load(eSound::Oink, "oink");
}  // LoadSounds

/// Release all of the DirectX12 objects by deleting the renderer.

void CGame::Release() {
  delete m_pRenderer;
  delete m_pPlayer;
  m_pRenderer = nullptr;  // for safety
}  // Release

/// Call this function to start a new game. This should be re-entrant so that
/// you can restart a new game without having to shut down and restart the
/// program.

void CGame::BeginGame() {
  delete m_pSpriteDesc;
  m_pSpriteDesc = new LSpriteDesc2D((UINT)eSprite::TextWheel, m_vWinCenter);
  m_pSpriteDesc = new LSpriteDesc2D((UINT)eSprite::Pig, m_vWinCenter / 4);

  // Add some test items to inventory
  CItem* potion = new CItem(1, "Health Potion", "Restores 50 HP",
                            eSprite::ItemPotion, eItemType::Consumable);
  m_pInventory->AddItem(potion);

  CItem* key = new CItem(2, "Rusty Key", "Opens old doors", eSprite::ItemKey,
                         eItemType::QuestItem, false);
  m_pInventory->AddItem(key);

  CItem* apple = new CItem(5, "Apple", "Restores 10 HP", eSprite::ItemApple,
                           eItemType::Consumable, true, 20);
  apple->SetQuantity(5);
  m_pInventory->AddItem(apple);

  CItem* shield =
      new CItem(6, "Wooden Shield", "Blocks some damage", eSprite::ItemShield,
                eItemType::Equipment, false, 1);
  m_pInventory->AddItem(shield);
}  // BeginGame

/// Poll the keyboard state and respond to the key presses that happened since
/// the last frame.

void CGame::KeyboardHandler() {
  m_pKeyboard->GetState();  // get current keyboard state

  if (m_pKeyboard->TriggerDown(VK_F1))  // help
    ShellExecute(0, 0, "https://larc.unt.edu/code/physics/blank/", 0, 0,
                 SW_SHOW);

  if (m_pKeyboard->TriggerDown(VK_F2))  // toggle frame rate
    m_bDrawFrameRate = !m_bDrawFrameRate;

  // Always handle hotbar input (number keys 1-6)
  m_pInventory->HandleHotbarInput(m_pKeyboard);

  // Toggle inventory with 'I' key
  if (m_pKeyboard->TriggerDown('I')) m_pInventory->Toggle();

  // Handle inventory input when open
  if (m_pInventory->IsOpen()) m_pInventory->HandleInput(m_pKeyboard);

  // Use hotbar item with F key (when inventory is closed)
  if (!m_pInventory->IsOpen() && m_pKeyboard->TriggerDown('F'))
    m_pInventory->UseHotbarItem();

  if (m_pKeyboard->TriggerDown(VK_BACK))  // restart game
    BeginGame();                          // restart game

}  // KeyboardHandler

/// Draw the current frame rate to a hard-coded position in the window.
/// The frame rate will be drawn in a hard-coded position using the font
/// specified in gamesettings.xml.

void CGame::DrawFrameRateText() {
  const std::string s =
      std::to_string(m_pTimer->GetFPS()) + " fps";  // frame rate
  const Vector2 pos(m_nWinWidth - 128.0f, 30.0f);   // hard-coded position
  m_pRenderer->DrawScreenText(s.c_str(), pos);      // draw to screen
}  // DrawFrameRateText

/// Draw the game objects. The renderer is notified of the start and end of the
/// frame so that it can let Direct3D do its pipelining jiggery-pokery.

void CGame::RenderFrame() {
  m_pRenderer->BeginFrame();  // required before rendering

  if (m_pTileManager) m_pTileManager->Draw();

  if (m_pPlayer) m_pPlayer->Draw();

  // Draw UI elements in screen space (not affected by camera)
  {
    // Save current camera position
    Vector3 savedCameraPos = m_pRenderer->GetCameraPos();

    // Set camera to window center for UI rendering
    // This makes screen coordinates work as expected: (0,0) to (width,height)
    m_pRenderer->SetCameraPos(Vector3(m_nWinWidth / 2.0f, m_nWinHeight / 2.0f, 0.0f));

    // Always draw hotbar at bottom of screen
    if (m_pInventory) {
      m_pInventory->DrawHotbarOnly();
    }

    // Draw full inventory when open
    if (m_pInventory && m_pInventory->IsOpen()) {
      m_pInventory->Draw();
    }

    // Restore camera position
    m_pRenderer->SetCameraPos(savedCameraPos);
  }

  if (m_bDrawFrameRate) DrawFrameRateText();

  m_pRenderer->EndFrame();  // required after rendering
}  // RenderFrame

void CGame::FollowCamera() {
  if (m_pPlayer == nullptr) return;  // safety

  Vector3 vCameraPos(m_pPlayer->GetPos());  // player position

  /*if (m_vWorldSize.x > m_nWinWidth) {  // world wider than screen
     vCameraPos.x = std::max(
         vCameraPos.x, m_nWinWidth / 2.0f);  // stay away from the left edge
     vCameraPos.x = std::min(
         vCameraPos.x,
         m_vWorldSize.x - m_nWinWidth / 2.0f);  // stay away from the right edge
   }  // if
   else
     vCameraPos.x = m_vWorldSize.x / 2.0f;  // center horizontally.

   if (m_vWorldSize.y > m_nWinHeight) {  // world higher than screen
     vCameraPos.y = std::max(
         vCameraPos.y, m_nWinHeight / 2.0f);  // stay away from the bottom edge
     vCameraPos.y = std::min(
         vCameraPos.y,
         m_vWorldSize.y - m_nWinHeight / 2.0f);  // stay away from the top edge
   }  // if
   else
     vCameraPos.y = m_vWorldSize.y / 2.0f; */  // center vertically

  m_pRenderer->SetCameraPos(vCameraPos);  // camera to player
}  // FollowCamera
/// This function will be called regularly to process and render a frame
/// of animation, which involves the following. Handle keyboard input.
/// Notify the  audio player at the start of each frame so that it can prevent
/// multiple copies of a sound from starting on the same frame.
/// Move the game objects. Render a frame of animation.

void CGame::ProcessFrame() {
  KeyboardHandler();       // handle keyboard input
  m_pAudio->BeginFrame();  // notify audio player that frame has begun

  float dt = m_pTimer->GetFrameTime();

  if (!m_pInventory->IsOpen())
    m_pPlayer->Update(dt, m_pKeyboard, m_pTileManager);

  m_pTimer->Tick([&]() {  // all time-dependent function calls should go here
    const float t = m_pTimer->GetFrameTime();  // frame interval in seconds
    FollowCamera();
    // m_pSpriteDesc->m_fRoll += 0.125f*XM_2PI*t; //rotate at 1/8 RPS
  });

  RenderFrame();  // render a frame of animation
}  // ProcessFrame
