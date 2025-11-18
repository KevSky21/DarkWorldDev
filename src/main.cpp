#include "core/input/InputManager.h"
#include <iostream>

int main() {
  InputManager inputManager;
  inputManager.Update();

  std::cout << "Game Initialized" << std::endl;
  return 0;
}
