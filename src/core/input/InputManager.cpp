#include "core/input/InputManager.h"

InputManager::InputManager() {}

void InputManager::Update() {
    keyboard.Update();
}

KeyboardController& InputManager::GetKeyboard() {
    return keyboard;
}
