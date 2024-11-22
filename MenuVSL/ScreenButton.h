#pragma once

#include "menu/IScreenButton.h"

#include "rw/rwcore.h"

class ScreenButton : public IScreenButton {
public:
    CSprite2d m_Sprite;
    bool m_CanBeRemoved = false;

	bool hasPressedThisFrame = false;
	bool isPointerOver = false;
	//bool isPressed = false;
    
    ScreenButton();
    ~ScreenButton();

    void Update(int dt);
    void Draw();

    /* 1.5.0 */

    void SetToBeRemoved();

    //
};