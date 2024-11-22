#include "ScreenButton.h"

#include "MenuVSL.h"
#include "Input.h"

ScreenButton::ScreenButton()
{}

ScreenButton::~ScreenButton()
{}

void ScreenButton::Update(int dt)
{
    hasPressedThisFrame = false;

    auto position = m_Position;
    auto size = m_Size;

    CVector2D imagePostion = position;
    imagePostion.x -= size.x / 2;
    imagePostion.y -= size.y / 2;

    isPointerOver = Input::IsPointInsideRect(Input::GetTouchPos(), imagePostion, size, false);

    if(isPointerOver)
	{
		if (Input::hasTouchBeenReleasedThisFrame)
		{
			hasPressedThisFrame = true;

			if(onClick) onClick();
		}
	}
}

void ScreenButton::Draw()
{
    auto menuVSL = MenuVSL::Instance;

	auto position = m_Position;
    auto size = m_Size;

    CVector2D imagePostion = position;
    imagePostion.x -= size.x / 2;
    imagePostion.y -= size.y / 2;

    menuVSL->SetDrawWithFixedScale(false);

	menuVSL->DrawSprite(&m_Sprite, imagePostion, size);

    float fontHeight = MenuVSL::GetFontHeight();

    CVector2D textPosition = position;
    textPosition.x += size.x/2 + 5;
    textPosition.y -= fontHeight / 2;

    if(m_Text.size() > 0)
        menuVSL->DrawString(m_Text, textPosition, CRGBA(255, 255, 255), eFontAlignment::ALIGN_LEFT);

    menuVSL->SetDrawWithFixedScale(true);
}

void ScreenButton::SetToBeRemoved()
{
    m_CanBeRemoved = true;
}