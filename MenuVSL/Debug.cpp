#include "Debug.h"

#include "MenuVSL.h"
#include "Log.h"

void Debug::AddLine(std::string text)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Debug: " << text << std::endl;

    lines.push_back(text);

    if (lines.size() >= 16) lines.erase(lines.begin());
}

void Debug::Clear()
{
    lines.clear();
}

void Debug::Draw()
{
    if(!visible)
    {
        if(MenuVSL::m_FirstUpdated)
        {
            return;
        }
    }

    auto menuVSL = MenuVSL::Instance;

    float x = 20;
    float y = 260;

    for(auto line : lines)
    {
       menuVSL->DrawString(line, CVector2D(x, y), CRGBA(255, 255, 255), eFontAlignment::ALIGN_LEFT);

       y += 25;
    }
}