#include "Debug.h"

#include "MenuVSL.h"
#include "Log.h"

void Debug::AddLine(std::string text)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Debug: " << text << std::endl;

    m_Lines.push_back(text);

    if (m_Lines.size() >= 16) m_Lines.erase(m_Lines.begin());
}

void Debug::Clear()
{
    m_Lines.clear();
}

void Debug::Draw()
{
    if(!m_Visible) return;

    auto menuVSL = MenuVSL::Instance;

    float x = 20;
    float y = 260;

    for(auto line : m_Lines)
    {
       menuVSL->DrawString(line, CVector2D(x, y), CRGBA(255, 255, 0), eFontAlignment::ALIGN_LEFT);

       y += 25;
    }
}