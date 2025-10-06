#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include "core/input/KeyboardController.h"

class InputManager {
public:
    InputManager();

    void Update(); // Call once per frame to update all input devices

    KeyboardController& GetKeyboard();

private:
    KeyboardController keyboard;
};

#endif // INPUTMANAGER_H
