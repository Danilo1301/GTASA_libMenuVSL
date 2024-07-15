#include "Input.h"

#include "Log.h"

extern CVector2D *m_vecCachedPos;
extern bool *CTouchInterface_m_bTouchDown;
extern int (*OS_ScreenGetWidth)(void);
extern int (*OS_ScreenGetHeight)(void);

bool Input::isTouchPressed = false;
bool Input::hasTouchBeenPressedThisFrame = false;
bool Input::hasTouchBeenReleasedThisFrame = false;

void Input::Update()
{
    hasTouchBeenPressedThisFrame = false;
    hasTouchBeenReleasedThisFrame = false;

    if(IsTouchPressed())
    {
        if(!isTouchPressed)
        {
            isTouchPressed = true;
            hasTouchBeenPressedThisFrame = true;

            Log::Level(LOG_LEVEL::LOG_BOTH) << "Input: Touch pressed this frame" << std::endl;
        }
    } else {
        if(isTouchPressed)
        {
            isTouchPressed = false;
            hasTouchBeenReleasedThisFrame = true;

            Log::Level(LOG_LEVEL::LOG_BOTH) << "Input: Touch released this frame" << std::endl;
        }
    }
}

CVector2D Input::GetTouchPos()
{
    //auto orgPos = m_vecCachedPos;
    //auto gtaScreenSize = GetGTAScreenSize();
    //auto screenSize = GetScreenSize();

    return *m_vecCachedPos;

    /*
    return {
        orgPos->x / (screenSize.x / gtaScreenSize.x),
        orgPos->y / (screenSize.y / gtaScreenSize.y)
    };
    */
}

CVector2D Input::GetScreenSize()
{
    return CVector2D(OS_ScreenGetWidth(), OS_ScreenGetHeight());
}

bool Input::IsTouchPressed()
{
    return *CTouchInterface_m_bTouchDown;
}

bool Input::IsPointInsideRect(CVector2D pos, CVector2D rectPos, CVector2D rectSize)
{
    if (pos.x >= rectPos.x && pos.x <= rectPos.x + rectSize.x)
    {
        if (pos.y >= rectPos.y && pos.y <= rectPos.y + rectSize.y)
        {
            return true;
        }
    }

    return false;
}