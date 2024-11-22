#pragma once

#include "menu/IMenuVSL.h"

#include "Window.h"
#include "Item.h"
#include "Popup.h"
#include "ScreenButton.h"

#include <map>
#include <string>

#include "sdk/Image.h"

struct ModCredits {
    std::string text = "";
    bool hasShownCredits = false;
};

class MenuVSL : public IMenuVSL {
public:
    static MenuVSL* Instance;
    static std::vector<Window*> m_Windows;
    static std::vector<Popup*> m_Popups;
    static std::vector<ScreenButton*> m_ScreenButtons;
    static CVector2D m_DefaultFontScale;
    static CVector2D m_FontScale;
    static eFontStyle m_DefaultFontStyle;
    static eFontStyle m_FontStyle;
    static CVector2D m_GTAScreenSize;
    static CVector2D m_ScreenSize;

    static std::map<int, Window*> m_CleoWindows;
    static std::map<int, Item*> m_CleoItems;

    static std::map<std::string, int> globalIntVariables;

    static bool m_FirstUpdated;
    static bool m_DrawWithFixedScale;

    static CSprite2d m_WindowTitleSprite;
    static CSprite2d m_WindowBgSprite;
    static CSprite2d m_WindowSelectionSprite;
    
    CSprite2d testSprite;

    static std::vector<ModCredits> m_ModCredits;
    static bool m_CanShownCredits;

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

    void DrawRectWithStringMultiline(std::string text, CVector2D pos, float width, CVector2D padding, CRGBA boxColor, CRGBA textColor, eFontAlignment align);
    void ShowPopup(std::string title, std::string text, CVector2D pos, int time);

    void SetDrawWithFixedScale(bool enabled);

    IWindow* AddVector2WindowEx(IWindow* parent, CVector2D* vec, float min, float max, float addBy, std::function<void()> onChange, std::function<void()> onBack);
    IWindow* AddVector2Window(IWindow* parent, CVector2D* vec, float min, float max, float addBy);
    
    IWindow* AddVectorWindowEx(IWindow* parent, CVector* vec, float min, float max, float addBy, std::function<void()> onChange, std::function<void()> onBack);
    IWindow* AddVectorWindow(IWindow* parent, CVector* vec, float min, float max, float addBy);

    /* 1.4.0 */

    IWindow* ShowSelectLanguageWindow(IWindow* parent = NULL);
    void LoadLanguagesFolder(std::string folder);

    std::string GetLanguageLineFormatted(std::string key, ...);

    void ShowMessage(std::string key, int time);
    
    void AddModCredits(std::string key);

    /* 1.5.0 */

    IScreenButton* AddScreenButton(CVector2D position, std::string texture, CVector2D size);

    //

    void Update(int dt);
    void OnFirstUpdate();
    void ProcessScripts();
    void Draw();

    void VehicleRenderBefore(void* pVehicle);
    void VehicleRenderAfter(void* pVehicle);
    
    void RemoveWindowNow(Window* window);
    void RemoveScreenButtonNow(ScreenButton* screenButton);

    static int AddCleoWindow();
    static int AddCleoItem(Item* item);

    static float GetFontHeight();

    void ShowCredits(std::string text, int time);
    void RemovePopup(Popup* popup);

    static bool CreateImageFromFile(std::string const& path, Image*& img);

    void CreateTestMenu();

    static float FixPositionX(float x);
    static float FixPositionY(float y);
};

extern IMenuVSL* menuVSL;