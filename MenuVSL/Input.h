#pragma once

#include "menu/MenuStructs.h"

class Input {
public:
    static bool isTouchPressed;
    static bool hasTouchBeenPressedThisFrame;
    static bool hasTouchBeenReleasedThisFrame;
    static void Update();

    static CVector2D GetTouchPos();
    static CVector2D GetScreenSize();
    static bool IsTouchPressed();
    static bool IsPointInsideRect(CVector2D pos, CVector2D rectPos, CVector2D rectSize);
};