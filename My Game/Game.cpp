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

void ContactListener::BeginContact(b2Contact* contact) {
  b2Fixture* A = contact->GetFixtureA();
  b2Fixture* B = contact->GetFixtureB();


  if (A->IsSensor()) {
    CPlayer* Ap = (CPlayer*)A->GetUserData().pointer;

    float AsensorY = A->GetAABB(0).GetCenter().y;
    float AsensorX = A->GetAABB(0).GetCenter().x;
    float AbodyY = Ap->GetBody()->GetPosition().y;
    float AbodyX = Ap->GetBody()->GetPosition().x;
  
        if (AsensorY < AbodyY)
      Ap->m_groundContacts = true;
    else if (AsensorY > AbodyY)
      Ap->m_headContacts = true;
    else if (AsensorX < AbodyX)
      Ap->m_leftWallContacts = true;
    else
      Ap->m_rightWallContacts = true;
  }
  if (B->IsSensor()) {
    CPlayer* Bp = (CPlayer*)B->GetUserData().pointer;

    float BsensorY = B->GetAABB(0).GetCenter().y;
    float BsensorX = B->GetAABB(0).GetCenter().x;
    float BbodyY = Bp->GetBody()->GetPosition().y;
    float BbodyX = Bp->GetBody()->GetPosition().x;
  
    if (BsensorY < BbodyY)
      Bp->m_groundContacts = true;
    else if (BsensorY > BbodyY)
      Bp->m_headContacts = true;
    else if (BsensorX < BbodyX)
      Bp->m_leftWallContacts = true;
    else
      Bp->m_rightWallContacts = true;
  }
}

void ContactListener::EndContact(b2Contact* contact) {
  b2Fixture* A = contact->GetFixtureA();
  b2Fixture* B = contact->GetFixtureB();
 
  

  if (A->IsSensor()) {
    CPlayer* Ap = (CPlayer*)A->GetUserData().pointer;

    float AsensorY = A->GetAABB(0).GetCenter().y;
    float AsensorX = A->GetAABB(0).GetCenter().x;
    float AbodyY = Ap->GetBody()->GetPosition().y;
    float AbodyX = Ap->GetBody()->GetPosition().x;

   
    if (AsensorY < AbodyY)
      Ap->m_groundContacts = false;
    else if (AsensorY > AbodyY)
      Ap->m_headContacts = false;
    else if (AsensorX < AbodyX)
      Ap->m_leftWallContacts = false;
    else
      Ap->m_rightWallContacts = false;
  }
  if (B->IsSensor()) {
    CPlayer* Bp = (CPlayer*)B->GetUserData().pointer;

    float BsensorY = B->GetAABB(0).GetCenter().y;
    float BsensorX = B->GetAABB(0).GetCenter().x;
    float BbodyY = Bp->GetBody()->GetPosition().y;
    float BbodyX = Bp->GetBody()->GetPosition().x;
   
    if (BsensorY < BbodyY)
      Bp->m_groundContacts = false;
    else if (BsensorY > BbodyY)
      Bp->m_headContacts = false;
    else if (BsensorX < BbodyX)
      Bp->m_leftWallContacts = false;
    else
      Bp->m_rightWallContacts = false;
  }
}


void CGame::Initialize() {
  float tileSize = 32.0f;
 
  m_pRenderer = new LSpriteRenderer(eSpriteMode::Batched2D);
  m_pRenderer->Initialize(eSprite::Size);
  LoadImages();  // load images from xml file list

  m_pTileManager = new CTileManager(m_pRenderer, tileSize);
  m_pTileManager->LoadMap("Media/Maps/testmap.txt");
  mWorld = new b2World(b2Vec2(0.0f, -9.8f));
  m_listener = new ContactListener();
  mWorld->SetContactListener(m_listener);


  float scale = 32.0f;
  float half = m_pTileManager->GetTileSize() * 0.5f / scale;
  int mapHeight = m_pTileManager->GetMapHeight();

  for (const TileRect& r : m_pTileManager->GetSolidRects()) {
    float halfW = (r.w * tileSize * 0.5f) / scale;
    float halfH = (r.h * tileSize * 0.5f) / scale;
    float cx = (r.x + r.w * 0.5f) * tileSize;
    float cy = (mapHeight - r.y - r.h * 0.5f) * tileSize;

    b2BodyDef def;
    def.type = b2_staticBody;
    def.position.Set(cx / scale, cy / scale);

    b2Body* tileBody = mWorld->CreateBody(&def);

    b2PolygonShape box;
    box.SetAsBox(halfW, halfH);


    b2FixtureDef fd;
    fd.shape = &box;
    fd.friction = 2.5f;
    fd.restitution = 0.0f;
    fd.density = 0.0f;

    tileBody->CreateFixture(&fd);
    m_PhysicsTiles.push_back(tileBody);
    
    m_debugBodies.push_back(tileBody);

  }

  //for (const Vector2& tilePosPixels : m_pTileManager->GetSolidTiles()) 
  //{
  //  float x = tilePosPixels.x / scale;
  //  float y = tilePosPixels.y / scale;

  //  b2BodyDef def;
  //  def.position.Set(x, y);
  //  b2Body* tileBody = mWorld->CreateBody(&def);

  //  b2PolygonShape box;
  //  box.SetAsBox(half, half);

  //  b2FixtureDef fd;
  //  fd.shape = &box;
  //  fd.friction = 1.0f;     // high friction; 0 = ice, 1 = rubber
  //  fd.restitution = 0.0f;  // no bounce
  //  fd.density = 0.0f;      // static body

  //  tileBody->CreateFixture(&fd);
  //  m_PhysicsTiles.push_back(tileBody);
  //  
  //  m_debugBodies.push_back(tileBody);


  //}



  m_pPlayer = new CPlayer(m_pRenderer, mWorld, this);


  // Initialize inventory with screen dimensions
  m_pInventory = new CInventoryManager(m_pRenderer);
  m_pInventory->SetScreenSize((float)m_nWinWidth, (float)m_nWinHeight);
  m_pInventory->SetPlayer(m_pPlayer);



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
  m_pRenderer->Load(eSprite::Chapel, "chapel");
  m_pRenderer->Load(eSprite::Sky, "sky");
  m_pRenderer->Load(eSprite::Pig, "pig");
  m_pRenderer->Load(eSprite::Dirt, "dirt");
  m_pRenderer->Load(eSprite::Step, "step");
  m_pRenderer->Load(eSprite::Jab, "jab");
  m_pRenderer->Load(eSprite::DebugRed, "debugBox");
  m_pRenderer->Load(eSprite::DebugSquare, "debugSquareWOutline");
  m_pRenderer->Load(eSprite::DebugGreen, "contactSquare");
  m_pRenderer->Load(eSprite::Bullet, "bullet");
  
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
  bool DebugDraw = true;
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
  if (!m_pInventory->IsOpen() && m_pKeyboard->TriggerDown('Q'))
    m_pInventory->UseHotbarItem();

  if (m_pKeyboard->TriggerDown(VK_BACK))  // restart game
    BeginGame();                          // restart game

}  // KeyboardHandler

void CGame::SpawnBulletFromPlayer() {
  b2Vec2 pos = m_pPlayer->GetBody()->GetPosition();

  b2Vec2 dir = b2Vec2(1.0f, 0.0f);

  b2Vec2 vel = b2Vec2(15.0f, 0.0f);

  m_bullets.push_back(new CBullet(mWorld, pos, vel));
}

/// Draw the current frame rate to a hard-coded position in the window.
/// The frame rate will be drawn in a hard-coded position using the font
/// specified in gamesettings.xml.

void CGame::RegisterDebugBody(b2Body* b) { m_debugBodies.push_back(b); }

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
  float scale = 32.0f;
  // =========================
  //   BACKGROUND DRAWING
  // =========================

  {
    const Vector2 cam = m_pRenderer->GetCameraPos();
    const float winW = (float)m_nWinWidth;
    const float winH = (float)m_nWinHeight;

    // Real PNG dimensions:
    const float bgW = 1024.0f;
    const float bgH = 576.0f;

    // Scale the background to fill the vertical window (768 tall)
    const float sc = 1.39f;  // ? 1.333  winH / bgH

    const float drawW = bgW * sc;  // scaled width

    float bgYOffset = 100.0f;  // shift downward


    // Start drawing slightly offscreen for no gaps
    float startX = cam.x - winW / 2.0f - drawW;

    
    // ---- Draw CHAPEL next (mid layer) ----
    for (float x = startX; x < cam.x + winW / 2.0f + drawW; x += drawW) {
      LSpriteDesc2D C;
      C.m_nSpriteIndex = (UINT)eSprite::Chapel;
      C.m_vPos = Vector2(x + (drawW / 2.0f) + -210.0f, cam.y + bgYOffset); // vertical center
      C.m_fXScale = sc;
      C.m_fYScale = sc + 1.0f;
      m_pRenderer->Draw(&C);
    }

    // ---- Draw SKY first (furthest) ----
    for (float x = startX; x < cam.x + winW / 2.0f + drawW; x += drawW) {
      LSpriteDesc2D S;
      S.m_nSpriteIndex = (UINT)eSprite::Sky;
      S.m_vPos = Vector2(x + (drawW / 2.0f) + -210.0f, cam.y + bgYOffset); // vertical center
      S.m_fXScale = sc;
      S.m_fYScale = sc + 1.0f;
      m_pRenderer->Draw(&S);
    }

  }

 
  //GroundDrawing
  m_pTileManager->Draw();

  //Player Draw
  if (m_pPlayer) m_pPlayer->Draw();

  for (CBullet* b : m_bullets) b->Draw(m_pRenderer);

  //Inv Draw
  if (m_pInventory) {
    m_pInventory->DrawWorldItems();
  }
  bool DebugDraw = false;
  //Debug Draw
if (DebugDraw) {
  
  for (b2Body* body : m_debugBodies) {
    for (b2Fixture* f = body->GetFixtureList(); f; f = f->GetNext()) {
      b2AABB aabb = f->GetAABB(0);

      // Convert AABB corners from meters → pixels
      float left = aabb.lowerBound.x * scale;
      float right = aabb.upperBound.x * scale;
      float bottom = aabb.lowerBound.y * scale;
      float top = aabb.upperBound.y * scale;

      // Center + half extents for drawing
      Vector2 center((left + right) * 0.5f, (top + bottom) * 0.5f);
      float halfW = (right - left) * 0.5f;
      float halfH = (top - bottom) * 0.5f;

      // Draw box
      LSpriteDesc2D d;
      d.m_nSpriteIndex = (UINT)eSprite::DebugSquare;
      d.m_vPos = center;
      d.m_fXScale = halfW / 8.0f;  // adjust if your debug sprite is 16×16 or 32×32
      d.m_fYScale = halfH / 8.0f;
      m_pRenderer->Draw(&d);
    }
  }
}


  // Draw UI elements in screen space (not affected by camera)
  {
    // Save current camera position
    Vector3 savedCameraPos = m_pRenderer->GetCameraPos();

    // Set camera to window center for UI rendering
    // This makes screen coordinates work as expected: (0,0) to (width,height)
    m_pRenderer->SetCameraPos(
        Vector3(m_nWinWidth / 2.0f, m_nWinHeight / 2.0f, 0.0f));

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
  if (m_pPlayer == nullptr) return;

  Vector3 vCameraPos(m_pPlayer->GetPos());

     
  const float verticalOffset = 200.0f;  
  vCameraPos.y += verticalOffset;
  
  m_pRenderer->SetCameraPos(vCameraPos); 
}  

void CGame::ProcessFrame() {
  KeyboardHandler();       // handle keyboard input
  m_pAudio->BeginFrame();  // notify audio player that frame has begun

  float dt = m_pTimer->GetFrameTime();

  for (auto it = m_bullets.begin(); it != m_bullets.end();) {
    (*it)->Update(dt);

    if ((*it)->IsDead()) {
      mWorld->DestroyBody((*it)->GetBody());
      delete *it;
      it = m_bullets.erase(it);
    } else {
      ++it;
    }
  }
  if (m_pPlayer->WantsToShoot()) {
    SpawnBulletFromPlayer();
    m_pPlayer->ClearShootRequest();
  }

  if (m_pInventory) {
    m_pInventory->Update(dt);
  }

  if (!m_pInventory->IsOpen())
    m_pPlayer->Update(dt, m_pKeyboard, m_pTileManager);

  m_pTimer->Tick([&]() {  // all time-dependent function calls should go here
    const float t = m_pTimer->GetFrameTime();
    mWorld->Step(dt, 8, 3);
    FollowCamera();
   
  });


  RenderFrame();
}


