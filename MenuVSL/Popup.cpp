#include "Popup.h"

#include "MenuVSL.h"

Popup::Popup()
{}

Popup::~Popup()
{}

void Popup::Update(int dt)
{
    m_TimeElapsed += dt;
}

void Popup::Draw()
{
    auto menuVSL = MenuVSL::Instance;

    //
    unsigned char alpha = 255;

    if(m_TimeElapsed >= 0 && m_TimeElapsed < m_FadeTime)
    {
        float in = ((float)m_FadeTime - (float)m_TimeElapsed) / (float)m_FadeTime;
        alpha = 255 - ucharIntensity(255, in);
    }
    if(m_TimeElapsed >= m_Time - m_TimeElapsed && m_TimeElapsed <= m_Time)
    {
        float out = ((float)m_Time - (float)m_TimeElapsed) / (float)m_FadeTime;
        alpha = ucharIntensity(255, out);
    }
    //

    CRGBA textColor = { 255, 255, 255, alpha };
    CRGBA titleBoxColor = CRGBA(20, 90, 200, alpha);
    CRGBA boxColor = CRGBA(0, 0, 0, alpha);

    //

    CVector2D pos = m_Position;
    float width = 500;
    float titleHeight = 30;

    menuVSL->DrawRectWithString(m_Title, pos, CVector2D(width, titleHeight), titleBoxColor, textColor, eFontAlignment::ALIGN_LEFT);

    pos.y += titleHeight;

    menuVSL->DrawRectWithStringMultiline(m_Text, pos, width, CVector2D(10, 10), boxColor, textColor, eFontAlignment::ALIGN_CENTER);
}

bool Popup::HasCompleted()
{
    return m_TimeElapsed >= m_Time;
}