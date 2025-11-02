#include "core/input/KeyboardController.h"
#include <conio.h> // for _kbhit() and _getch()

KeyboardController::KeyboardController() { keyStates.clear(); }

void KeyboardController::Update() {
  // First, update states: PRESSED → HELD, RELEASED → NONE
  for (auto &[key, state] : keyStates) {
    if (state == KeyState::PRESSED)
      state = KeyState::HELD;
    else if (state == KeyState::RELEASED)
      state = KeyState::NONE;
  }

  // Read all key presses in this frame
  while (_kbhit()) {
    int key = _getch();
    if (keyStates[key] == KeyState::NONE ||
        keyStates[key] == KeyState::RELEASED) {
      keyStates[key] = KeyState::PRESSED;
    }
  }
}

void KeyboardController::OnKeyPressed(int keyCode) {
  keyStates[keyCode] = KeyState::PRESSED;
}

void KeyboardController::OnKeyReleased(int keyCode) {
  keyStates[keyCode] = KeyState::RELEASED;
}

bool KeyboardController::isKeyPressed(int keyCode) const {
  auto it = keyStates.find(keyCode);
  return it != keyStates.end() && it->second == KeyState::PRESSED;
}

bool KeyboardController::isKeyHeld(int keyCode) const {
  auto it = keyStates.find(keyCode);
  return it != keyStates.end() &&
         (it->second == KeyState::HELD || it->second == KeyState::PRESSED);
}

bool KeyboardController::isKeyReleased(int keyCode) const {
  auto it = keyStates.find(keyCode);
  return it != keyStates.end() && it->second == KeyState::RELEASED;
}

void KeyboardController::Clear() { keyStates.clear(); }
