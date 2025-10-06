#ifndef KEYBOARDCONTROLLER_H
#define KEYBOARDCONTROLLER_H

using namespace std;

#include <unordered_map>


enum class KeyState {
    NONE,
    PRESSED,
    HELD,
    RELEASED

};


class KeyboardController {
public:
    KeyboardController();


    void Update(); // Call this once per frame to update key states


    void OnKeyPressed(int keyCode);   // Call this when a key is pressed
    void OnKeyReleased(int keyCode);  // Call this when a key is released

    bool isKeyPressed(int keyCode) const;   // True only on the frame the key was pressed
    bool isKeyHeld(int keyCode) const;      // True while the key is held down
    bool isKeyReleased(int keyCode) const;  // True only on the frame the key was released


    void Clear(); // Clear all key states (per frame)


    private:

    unordered_map<int, KeyState> keyStates; // Maps key codes to their current states
    
};

#endif // KEYBOARDCONTROLLER_H
