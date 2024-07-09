#pragma once

#include "menu/IMenuVSL.h"

#include "Window.h"
#include "Item.h"

#include <map>

struct MenuCredits {
	std::string text = "MenuVSL v1.0.0 (by ~y~Danilo1301~w~)";
	bool hasShownCredits = false;
	int time = 0;
	int timeElapsed = 0;
	int fadeTime = 1000;
	int height = 80;
};

class MenuVSL : public IMenuVSL {
public:
    static MenuVSL* Instance;
    static std::vector<Window*> m_Windows;
    static CVector2D m_FontScale;

    static std::map<int, Window*> m_CleoWindows;
    static std::map<int, Item*> m_CleoItems;

    MenuCredits m_Credits;

    MenuVSL();

    IWindow* AddWindow();
    std::vector<IWindow*> GetWindows();

    void DrawRect(CRect rect, CRGBA color);
    void DrawRect(CVector2D position, CVector2D size, CRGBA color);
    void DrawString(std::string text, CVector2D position, CRGBA color, eFontAlignment align);
    void DrawRectWithString(std::string text, CVector2D pos, CVector2D boxSize, CRGBA boxColor, CRGBA textColor, eFontAlignment align);
    //

    void Update(int dt);
    void Draw();
    
    void RemoveWindowNow(Window* window);

    static int AddCleoWindow();
    static int AddCleoItem(Item* item);

    static float GetFontHeight();

    void ShowCredits(int time);
};

extern IMenuVSL* menuVSL;