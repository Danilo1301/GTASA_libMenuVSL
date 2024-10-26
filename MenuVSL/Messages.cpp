#include "Messages.h"

#include "Input.h"
#include "MenuVSL.h"

std::string Messages::m_Message = "";
int Messages::m_TimeLeft = 0;

void Messages::SetMessage(std::string message)
{
    m_Message = message;
}

void Messages::ShowMessage(std::string message, int time)
{
    SetMessage(message);
    m_TimeLeft = time;
}

void Messages::Draw()
{
    std::string text = m_Message;

    if(text.size() == 0) return;

    auto screenSize = Input::GetCellphoneScreenSize();

    float boxWidth = 3000;

    int extraLines = countSubstrOccurrences(text, "~n~");
    float lineSizeY = MenuVSL::GetFontHeight();

    CVector2D position = CVector2D(
        screenSize.x/2 - boxWidth / 2,
        screenSize.y - 60 - (extraLines * lineSizeY)
    );

    CVector2D padding = CVector2D(
        0, 10
    );

    CRGBA boxColor = CRGBA(0, 0, 0, 100);
    CRGBA textColor = CRGBA(255, 255, 255);
    
    MenuVSL::Instance->DrawRectWithStringMultiline(text, position, boxWidth, padding, boxColor, textColor, eFontAlignment::ALIGN_CENTER);
}

void Messages::Update(int dt)
{   
    if(m_TimeLeft > 0)
    {
        m_TimeLeft -= dt;

        if(m_TimeLeft < 0) m_TimeLeft = 0;
    }

    if(m_Message.size() > 0 && m_TimeLeft <= 0)
    {
        m_Message = "";
    }
}