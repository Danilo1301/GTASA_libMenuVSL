#pragma once

#include "menu/SimpleGTA.h"

class Input {
public:
    static bool isTouchPressed;
    static bool hasTouchBeenPressedThisFrame;
    static bool hasTouchBeenReleasedThisFrame;
    static void Update();

    static CVector2D GetTouchPos();
    static CVector2D GetTouchPosFixed();
    static CVector2D GetCellphoneScreenSize();
    static CVector2D GetGTAScreenSize();
    static bool IsTouchPressed();
    static bool IsPointInsideRect(CVector2D pos, CVector2D rectPos, CVector2D rectSize, bool fixed);
};