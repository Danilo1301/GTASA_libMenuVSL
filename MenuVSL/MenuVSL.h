#pragma once

#include "menu/IMenuVSL.h"

#include "Window.h"
#include "Item.h"

#include <map>
#include <string>

#include "sdk/Image.h"

struct MenuCredits {
	std::string text = "";
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
    static CVector2D m_DefaultFontScale;
    static CVector2D m_FontScale;

    static std::map<int, Window*> m_CleoWindows;
    static std::map<int, Item*> m_CleoItems;

    static std::map<std::string, int> globalIntVariables;

    static bool m_FirstUpdated;

    MenuCredits m_Credits;
    CSprite2d testSprite;

    Window* m_ActiveWindow = NULL;

    MenuVSL();

    /*1.0.0*/

    IWindow* AddWindow();
    std::vector<IWindow*> GetWindows();

    void DrawRect(CRect rect, CRGBA color);
    void DrawRect(CVector2D position, CVector2D size, CRGBA color);
    void DrawString(std::string text, CVector2D position, CRGBA color, eFontAlignment align);
    void DrawRectWithString(std::string text, CVector2D pos, CVector2D boxSize, CRGBA boxColor, CRGBA textColor, eFontAlignment align);

    /*1.1.0*/

    void DrawSprite(CSprite2d* sprite, CVector2D pos, CVector2D size);
    void* LoadRwTextureFromFile(std::string file, std::string textureName, bool flipHorizontal = false);
    IWindow* AddWindowOptionsString(std::string title, IWindow* parent, std::string* pString, std::vector<std::string>* options);
    IWindow* AddWindowMultiOptionsString(std::string title, IWindow* parent, std::vector<std::string>* selectedOptions, std::vector<std::string>* allOptions);
    IWindow* AddColorWindow(IWindow* parent, CRGBA* color, std::function<void()> onValueChange);

    /*1.2.0*/

    void SetGlobalIntVariable(std::string key, int value);
    int GetGlobalIntVariable(std::string key);

    CVector2D ConvertWorldPositionToScreenPosition(CVector worldPosition);
    void DrawWorldText(std::string text, CVector position, CRGBA color, eFontAlignment align);
    void AddOnRender(std::function<void()> fn);
    CVector2D GetFontScale();
    void SetFontScale(float sx, float sy);
    void ResetFontScale();

    /*1.3.0*/

    void AddOnVehicleRenderBefore(std::function<void(void*)> fn);
    void AddOnVehicleRenderAfter(std::function<void(void*)> fn);
    void AddOnUpdate(std::function<void()> fn);
    void AddOnProcessScripts(std::function<void()> fn);
    std::vector<MVehicle> GetVehicles();

    //

    void Update(int dt);
    void ProcessScripts();
    void Draw();

    void VehicleRenderBefore(void* pVehicle);
    void VehicleRenderAfter(void* pVehicle);
    
    void RemoveWindowNow(Window* window);

    static int AddCleoWindow();
    static int AddCleoItem(Item* item);

    static float GetFontHeight();

    void ShowCredits(int time);

    static bool CreateImageFromFile(std::string const& path, Image*& img);

    void CreateTestMenu();
};

extern IMenuVSL* menuVSL;