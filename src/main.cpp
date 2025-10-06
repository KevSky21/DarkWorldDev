#include "core/input/InputManager.h"
#include <iostream>

using namespace std;

int main() {  
    InputManager inputManager;
    inputManager.Update();

    cout << "Game Initialized" << endl;
    return 0;
}
