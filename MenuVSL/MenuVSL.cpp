#include "MenuVSL.h"

#include <mod/amlmod.h>
#include <mod/logger.h>
#include <mod/config.h>

#include "Log.h"
#include "Item.h"
#include "Debug.h"
#include "Input.h"
#include "ModConfig.h"
#include "Vehicles.h"
#include "Localization.h"
#include "Messages.h"
#include "ScreenButton.h"
#include "TextureManager.h"

#include "rw/rwcore.h"
#include "rw/rpworld.h"
#include "menu/MenuStructs.h"
#include "menu/SimpleGTA.h"

extern CVector2D *m_vecCachedPos;
extern void (*CSprite2d_DrawRect)(CRect const& posn, CRGBA const& color);
extern void (*CSprite2d_DrawSprite)(CSprite2d*, CRect const&, CRGBA const&);
extern bool (*CSprite_CalcScreenCoors)(RwV3d const &posn, RwV3d *out, float *w, float *h, bool checkMaxVisible, bool checkMinVisible);
extern void (*CFont_PrintString)(float x, float y, unsigned short* text);
extern void (*CFont_SetOrientation)(eFontAlignment align);
extern void (*CFont_SetScale)(float w, float h);
extern void (*CFont_SetFontStyle)(short style);
extern void (*CFont_SetWrapx)(float value);
extern void (*CFont_SetProportional)(bool on);
extern void (*CFont_SetColor)(CRGBA const& color);
extern void (*CFont_SetDropShadowPosition)(short value);
extern void (*AsciiToGxtChar)(const char* txt, unsigned short* ret);
extern void (*RenderFontBuffer)(void);
extern bool *CTouchInterface_m_bTouchDown;

extern RwRaster* (*RwRasterCreate)(RwInt32 width, RwInt32 height, RwInt32 depth, RwInt32 flags);
extern RwRaster* (*RwRasterUnlock)(RwRaster* raster);
extern RwBool (*RwTextureSetMipmapping)(RwBool enable);
extern RwBool (*RwTextureSetAutoMipmapping)(RwBool enable);
extern RwTexture* (*RwTextureCreate)(RwRaster* raster);
extern RwUInt8* (*RwRasterLock)(RwRaster* raster, RwUInt8 level, RwInt32 lockMode);
extern RpMaterial* (*RpMaterialSetTexture)(RpMaterial* material, RwTexture* texture);
extern void (*RwImageFindRasterFormat)(RwImage*, int, int*, int*, int*, int*);
extern void (*RwImageDestroy)(RwImage*);
extern void (*RwRasterSetFromImage)(RwRaster*, RwImage*);
extern RwImage* (*RtPNGImageRead)(const char* filename);
extern RwImage* (*RtBMPImageRead)(const RwChar* imageName);
extern RwTexture* (*RwTextureRead)(const RwChar* name, const RwChar* maskname);

MenuVSL* MenuVSL::Instance = new MenuVSL();
std::vector<Window*> MenuVSL::m_Windows;
std::vector<Popup*> MenuVSL::m_Popups;
std::vector<ScreenButton*> MenuVSL::m_ScreenButtons;
CVector2D MenuVSL::m_DefaultFontScale = CVector2D(1.5f, 2.25f); //deafult: (1.5, 2.25)
CVector2D MenuVSL::m_FontScale = m_DefaultFontScale;
eFontStyle MenuVSL::m_DefaultFontStyle = eFontStyle::FONT_SUBTITLES;
eFontStyle MenuVSL::m_FontStyle = m_DefaultFontStyle;
CVector2D MenuVSL::m_GTAScreenSize = CVector2D(1080, 580);
CVector2D MenuVSL::m_ScreenSize = CVector2D(0, 0);

std::map<int, Window*> MenuVSL::m_CleoWindows;
std::map<int, Item*> MenuVSL::m_CleoItems;

std::map<std::string, int> MenuVSL::globalIntVariables;

bool MenuVSL::m_FirstUpdated = false;
bool MenuVSL::m_DrawWithFixedScale = false;

CSprite2d MenuVSL::m_WindowTitleSprite;
CSprite2d MenuVSL::m_WindowBgSprite;
CSprite2d MenuVSL::m_WindowSelectionSprite;

std::vector<ModCredits> MenuVSL::m_ModCredits;
bool MenuVSL::m_CanShownCredits = false;

std::vector<std::function<void()>> OnUpdateFunctions;
std::vector<std::function<void()>> OnProcessScriptsFunctions;
std::vector<std::function<void()>> OnRenderFunctions;
std::vector<std::function<void(void*)>> OnVehicleRenderBeforeFunctions;
std::vector<std::function<void(void*)>> OnVehicleRenderAfterFunctions;

char buffer[0xFF]; //char text[24];
unsigned short* textGxt = new unsigned short[0xFF];

int blockedTimeToClickMenu = 0;

bool firstTimeDraw = true;

void* LoadRwTextureFromPNG(const char* fn)
{
    //RwTexture* pTexture = NULL;

    MenuVSL::Instance->debug->AddLine("PNG: " + std::string(fn));

    RwImage* t2 = RtPNGImageRead(fn);
    MenuVSL::Instance->debug->AddLine("PNG: " + std::to_string((long long)t2));

    return NULL;

    /*
    if (RwImage* pImage = RtPNGImageRead(fn))
    {
        int width, height, depth, flags;
        RwImageFindRasterFormat(pImage, rwRASTERTYPETEXTURE, &width, &height, &depth, &flags);
        if (RwRaster* pRaster = RwRasterCreate(width, height, depth, flags))
        {
            RwRasterSetFromImage(pRaster, pImage);
            pTexture = RwTextureCreate(pRaster);
        }
        RwImageDestroy(pImage);
    }
    return pTexture;
    */
}

void* LoadRwTextureFromBMP(const char* fn)
{
    MenuVSL::Instance->debug->AddLine("BMP: " + std::string(fn));

    RwImage* t2 = RtBMPImageRead(fn);
    MenuVSL::Instance->debug->AddLine("BMP: " + std::to_string((long long)t2));

    return NULL;

    /*
    if (RwImage* pImage = RtPNGImageRead(fn))
    {
        int width, height, depth, flags;
        RwImageFindRasterFormat(pImage, rwRASTERTYPETEXTURE, &width, &height, &depth, &flags);
        if (RwRaster* pRaster = RwRasterCreate(width, height, depth, flags))
        {
            RwRasterSetFromImage(pRaster, pImage);
            pTexture = RwTextureCreate(pRaster);
        }
        RwImageDestroy(pImage);
    }
    return pTexture;
    */
}

MenuVSL::MenuVSL()
{
    this->debug = new Debug();

    this->testSprite.m_pTexture = NULL;
}

IWindow* MenuVSL::AddWindow()
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "AddWindow" << std::endl;

    Window* window = new Window();
    //window->m_Position = Window::m_DefaultWindowPosition;

    m_Windows.push_back(window);

    m_ActiveWindow = window;

    blockedTimeToClickMenu = 500;

    return window;
}

IWindow* MenuVSL::AddWindow(IWindow* parent)
{
    IWindow* window = AddWindow();
    window->m_Parent = parent;
    window->m_ShowBackButton = true;

    return window;
}

std::vector<IWindow*> MenuVSL::GetWindows()
{
    std::vector<IWindow*> windows;
    for(auto window : m_Windows) windows.push_back(window);
    return windows;
}

void MenuVSL::DrawRect(CRect rect, CRGBA color)
{
    if(m_DrawWithFixedScale)
    {
        rect.top = FixPositionY(rect.top);
        rect.left = FixPositionX(rect.left);
        rect.bottom = FixPositionY(rect.bottom);
        rect.right = FixPositionX(rect.right);
    }

    CSprite2d_DrawRect(rect, color);
}

void MenuVSL::DrawRect(CVector2D position, CVector2D size, CRGBA color)
{
    CRect rect(position.x, position.y, position.x + size.x, position.y + size.y);
    DrawRect(rect, color);
}

void MenuVSL::DrawString(std::string text, CVector2D position, CRGBA color, eFontAlignment align)
{
    CVector2D scale = m_FontScale;

    if(m_DrawWithFixedScale)
    {
        position.x = FixPositionX(position.x);
        position.y = FixPositionY(position.y);

        scale.x = FixPositionX(scale.x);
        scale.y = FixPositionY(scale.y);
    }

    sprintf(buffer, "%s", text.c_str());
    AsciiToGxtChar(buffer, textGxt);
    
    // plugin-sdk
    //CFont::m_fWrapx = 100000.0f;
    //CFont::SetOrientation(m_FontAlign);
    //CFont::SetDropShadowPosition(0);
    //CFont::SetRightJustifyWrap(false);
    //CFont::SetJustify(false);
    //CFont::SetBackground(false, false);
    //CFont::SetScale(ScreenX(0.35f), ScreenY(0.95f));
    //CFont::SetFontStyle(m_FontStyle);
    //CFont::SetProportional(true);
    //CFont::SetColor(color);

    CFont_SetWrapx(10000.0f);
    CFont_SetOrientation(align);
    CFont_SetDropShadowPosition(1);
    //CFont::SetRightJustifyWrap(false);
    //CFont::SetJustify(false);
    //CFont::SetBackground(false, false);
    CFont_SetScale(scale.x, scale.y);
    CFont_SetFontStyle(m_FontStyle);
    CFont_SetProportional(true);
    CFont_SetColor(color);
    CFont_PrintString(position.x, position.y, textGxt);

    RenderFontBuffer();
}

void MenuVSL::DrawRectWithString(std::string text, CVector2D pos, CVector2D boxSize, CRGBA boxColor, CRGBA textColor, eFontAlignment align)
{
	DrawRect(pos, boxSize, boxColor);

	CVector2D textPos = pos;
	textPos.y += boxSize.y / 2;

    textPos.y -= MenuVSL::GetFontHeight() / 2;

	if (align == eFontAlignment::ALIGN_CENTER)
	{
		textPos.x += boxSize.x / 2;
	}

	if (align == eFontAlignment::ALIGN_RIGHT)
	{
		textPos.x += boxSize.x;
	}

	DrawString(text, textPos, textColor, align);
}

void MenuVSL::DrawSprite(CSprite2d* sprite, CVector2D pos, CVector2D size)
{   
    if(m_DrawWithFixedScale)
    {
        pos.x = FixPositionX(pos.x);
        pos.y = FixPositionY(pos.y);

        size.x = FixPositionX(size.x);
        size.y = FixPositionY(size.y);
    }

    CSprite2d_DrawSprite(
        sprite,
        CRect(
            pos.x,
            pos.y,
            pos.x + size.x,
            pos.y + size.y
        ),
        CRGBA(255, 255, 255, 255)
    );
}

void MenuVSL::Update(int dt)
{
    if(!m_FirstUpdated)
    {
        m_FirstUpdated = true;
        OnFirstUpdate();
    }

    if(blockedTimeToClickMenu > 0)
    {
        blockedTimeToClickMenu -= dt;
    }

    if(GetGlobalIntVariable("show_test_menuVSL") == 1)
    {
        SetGlobalIntVariable("show_test_menuVSL", 0);

        CreateMainMenu();
    }

    Vehicles::TryFindNewVehicles();

    //

    IItem* pressedItem = NULL;

    for(auto window : MenuVSL::m_Windows)
    {
        if(!window->GetIsActive()) continue;

        window->Update();

        // check pressed item

        for(auto item : window->m_Items)
        {
            auto wItem = (Item*)item;

            if(blockedTimeToClickMenu > 0)
            {
                item->hasPressedThisFrame = false;
                continue;
            }

            if(item->hasPressedThisFrame)
            {
                pressedItem = item;
            }

            if(wItem->m_ButtonLeft)
            {
                if(wItem->m_ButtonLeft->hasPressedThisFrame) pressedItem = wItem->m_ButtonLeft;
            }

            if(wItem->m_ButtonRight)
            {
                if(wItem->m_ButtonRight->hasPressedThisFrame) pressedItem = wItem->m_ButtonRight;
            }
        }
    }

    if(pressedItem) ((Item*)pressedItem)->OnPress();

    //remove windows

    std::vector<Window*> windowsToRemove; //to fix a crash that i still cant believe it never happened before
    for (auto window : m_Windows)
    {
        if(window->m_CanBeRemoved) windowsToRemove.push_back(window);
    }
    for (auto window : windowsToRemove)
    {
        RemoveWindowNow(window);
    }
    if(windowsToRemove.size() > 0)
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "MenuVSL: Removed " << windowsToRemove.size() << " windows" << std::endl;
    }

    // screen button

    for(auto screenButton : m_ScreenButtons)
    {
        screenButton->Update(dt);
    }

    std::vector<ScreenButton*> screenButtonsToRemove; //to fix a crash that i still cant believe it never happened before
    for (auto screenButton : m_ScreenButtons)
    {
        if(screenButton->m_CanBeRemoved) screenButtonsToRemove.push_back(screenButton);
    }
    for (auto screenButton : screenButtonsToRemove)
    {
        RemoveScreenButtonNow(screenButton);
    }

    // credits

    if(m_CanShownCredits)
    {
        for(int i = 0; i < MenuVSL::m_ModCredits.size(); i++)
        {
            auto modCredits = &MenuVSL::m_ModCredits[i];

            if(modCredits->hasShownCredits) continue;
            modCredits->hasShownCredits = true;

            menuVSL->debug->AddLine("[Mod] " + modCredits->text);

            ShowCredits(modCredits->text, 4000);
        }
    }

    // popups

    for(int i = 0; i < m_Popups.size(); i++)
    {
        auto popup = m_Popups[i];
        if(i == 0) popup->Update(dt);
    }

    if(m_Popups.size() > 0)
    {
        if(m_Popups[0]->HasCompleted())
        {
            RemovePopup(m_Popups[0]);
        }
    }

    // messages

    Messages::Update(dt);

    //

    for(auto fn : OnUpdateFunctions) fn();
}

void MenuVSL::OnFirstUpdate()
{
    //CreateTestMenu();

    if(Localization::CurrentLanguage.size() == 0)
    {
        ShowSelectLanguageWindow();
    }

    m_CanShownCredits = true;
    

    auto menuVSL = MenuVSL::Instance;

    // TEST BUTTONS (SPAWN ON SCREEN)
    
    // char path[512];

    // sprintf(path, "%s/menuVSL/assets/button_info.png", aml->GetConfigPath());
    // auto buttonInfo = menuVSL->AddScreenButton(CVector2D(400, 200), path, CVector2D(80, 80));
    // buttonInfo->onClick = [menuVSL, buttonInfo]() {
    //     menuVSL->ShowMessage("Clicked info", 3000);
    // };

    // sprintf(path, "%s/modPolicia/assets/button_car.png", aml->GetConfigPath());
    // auto buttonCar = menuVSL->AddScreenButton(CVector2D(500, 200), path, CVector2D(80, 80));
    // buttonCar->m_Text = "Mais opcoes";
    // buttonCar->onClick = [menuVSL, buttonCar]() {
    //     menuVSL->ShowMessage("Clicked car", 3000);
    // };
}

void MenuVSL::ProcessScripts()
{
    //Log::Level(LOG_LEVEL::LOG_UPDATE) << "Processing other " << OnProcessScriptsFunctions.size() << " scripts..." << std::endl;

    for(auto fn : OnProcessScriptsFunctions) fn();
}

void MenuVSL::Draw()
{
    if(firstTimeDraw)
    {
        firstTimeDraw = false;
        OnFirstDraw();
    }

    m_DrawWithFixedScale = true; // FIX SCALE
    
    for(auto fn : OnRenderFunctions) fn();

    m_DrawWithFixedScale = true;

    //Log::Level(LOG_LEVEL::LOG_UPDATE) << "Draw" << std::endl;

    if(!m_WindowTitleSprite.m_pTexture)
    {
        char path[512];

        sprintf(path, "%s/menuVSL/assets/menu_title_bg.png", aml->GetConfigPath());
        m_WindowTitleSprite.m_pTexture = (RwTexture*)LoadRwTextureFromFile(path, "title_bg", true);

        sprintf(path, "%s/menuVSL/assets/menu_bg.png", aml->GetConfigPath());
        m_WindowBgSprite.m_pTexture = (RwTexture*)LoadRwTextureFromFile(path, "menu_bg", true);

        sprintf(path, "%s/menuVSL/assets/menu_selection_bg.png", aml->GetConfigPath());
        m_WindowSelectionSprite.m_pTexture = (RwTexture*)LoadRwTextureFromFile(path, "selection_bg", true);
    }

    for(auto window : MenuVSL::m_Windows)
    {
        if(!window->GetIsActive()) continue;
        
        window->Draw();
    }

    for(auto screenButton : MenuVSL::m_ScreenButtons)
    {
        screenButton->Draw();
    }

    bool drawPosInfo = debug->visible;
    bool drawCursor = debug->visible;

    CRGBA red = CRGBA(255, 0, 0);
    CRGBA green = CRGBA(0, 255, 0);

    if(drawPosInfo)
    {
        auto pos = Input::GetTouchPos();

        sprintf(buffer, "TOUCH POS: %.2f, %.2f | dt: %d", pos.x, pos.y, deltaTime);
        DrawString(buffer, CVector2D(30, 30), red, eFontAlignment::ALIGN_LEFT);

        pos = Input::GetTouchPosFixed();

        sprintf(buffer, "FIXED POS: %.2f, %.2f", pos.x, pos.y);
        DrawString(buffer, CVector2D(30, 60), green, eFontAlignment::ALIGN_LEFT);
    }

    if(drawCursor)
    {
        

        bool anyDown = false;
        if(*CTouchInterface_m_bTouchDown) anyDown = true;

        /*
        for(int i = 1; i <= 9; i++)
        {
            bool down = CTouchInterface_IsTouchDown(i);

            if(down)
            {
                Log::Level(LOG_LEVEL::LOG_BOTH) << "Touch " << i << "is down" << std::endl;
                anyDown = true;
            } else {
                Log::Level(LOG_LEVEL::LOG_BOTH) << "Touch " << i << "is not down" << std::endl;
            }
        }
        */

        m_DrawWithFixedScale = false;
        DrawRect(Input::GetTouchPos(), CVector2D(5, 5), CRGBA(255, 0, 0));
        DrawRect(Input::GetTouchPosFixed(), CVector2D(5, 5), CRGBA(0, 255, 0));
        m_DrawWithFixedScale = true;
        
        //CSprite2d_DrawRect(CRect(m_vecCachedPos->x, m_vecCachedPos->y, m_vecCachedPos->x+20, m_vecCachedPos->y+20), anyDown ? green : red);
    }

    if(false)
    {
        CRGBA textColor = { 255, 255, 255, 255 };
        CRGBA boxColor = CRGBA(0, 0, 0, 100);

        DrawRectWithStringMultiline("Line 1", CVector2D(300, 200), 100, CVector2D(10, 10), boxColor, textColor, eFontAlignment::ALIGN_LEFT);
        DrawRectWithStringMultiline("Line 1", CVector2D(300, 300), 100, CVector2D(10, 10), boxColor, textColor, eFontAlignment::ALIGN_CENTER);
        
        DrawRectWithStringMultiline("Line 1~n~Multi line uau", CVector2D(500, 200), 100, CVector2D(10, 10), boxColor, textColor, eFontAlignment::ALIGN_LEFT);
        DrawRectWithStringMultiline("Line 1~n~Multi line uau", CVector2D(500, 300), 100, CVector2D(10, 10), boxColor, textColor, eFontAlignment::ALIGN_CENTER);
    }

    ((Debug*)debug)->Draw();

    bool drawStupidBaiacuSprite = false;

    if(drawStupidBaiacuSprite)
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "TestSprite texture: " << testSprite.m_pTexture << std::endl;

        if(!testSprite.m_pTexture)
        {
            char path[512];

            sprintf(path, "%s/menuVSL/test.bmp", aml->GetConfigPath());
            LoadRwTextureFromBMP(path);

            sprintf(path, "%s/menuVSL/test.png", aml->GetConfigPath());
            LoadRwTextureFromPNG(path);

            sprintf(path, "%s/menuVSL/test.png", aml->GetConfigPath());
            testSprite.m_pTexture = (RwTexture*)LoadRwTextureFromFile(path, "test", true);

            debug->AddLine("Loaded testSprite: " + std::to_string((int)testSprite.m_pTexture));
            if(testSprite.m_pTexture)
            {
                debug->AddLine("Texture name: " + std::string((char*)testSprite.m_pTexture->name));
            }
        }

        DrawSprite(&testSprite, CVector2D(0, 0), CVector2D(200, 200));
    }

    m_DrawWithFixedScale = false;

    // popups

    for(int i = 0; i < m_Popups.size(); i++)
    {
        auto popup = m_Popups[i];

        if(i == 0) popup->Draw();
    }

    // bottom message

    Messages::Draw();

    //
    
    m_DrawWithFixedScale = true;
}

void MenuVSL::OnFirstDraw()
{
    menuVSL->debug->AddLine("~w~- MenuVSL v" + ModConfig::GetModVersion() + " (by ~y~Danilo1301~w~)");

    for(int i = 0; i < MenuVSL::m_ModCredits.size(); i++)
    {
        auto modCredits = &MenuVSL::m_ModCredits[i];

        menuVSL->debug->AddLine(modCredits->text);
    }
}

void MenuVSL::VehicleRenderBefore(void* pVehicle)
{
    for(auto fn : OnVehicleRenderBeforeFunctions) fn(pVehicle);
}

void MenuVSL::VehicleRenderAfter(void* pVehicle)
{
    for(auto fn : OnVehicleRenderAfterFunctions) fn(pVehicle);
}

void MenuVSL::RemoveWindowNow(Window* window)
{
    //if (window == m_MainWindow) return;

    Log::Level(LOG_LEVEL::LOG_BOTH) << "Menu: RemoveWindow" << std::endl;

    if (m_ActiveWindow == window) m_ActiveWindow = NULL;
    if (window->m_Parent) m_ActiveWindow = (Window*)window->m_Parent;

    if (window->onCloseWindow) window->onCloseWindow();

    if(m_CleoWindows.count(window->m_Id)) m_CleoWindows.erase(window->m_Id);

    for(auto item : window->m_Items)
    {
        if(m_CleoItems.count(item->m_Id)) m_CleoItems.erase(item->m_Id);
    }

    auto it = std::find(m_Windows.begin(), m_Windows.end(), window);
    if (it != m_Windows.end())
    {
        m_Windows.erase(it);
        delete window;
    }
}

void MenuVSL::RemoveScreenButtonNow(ScreenButton* screenButton)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Menu: RemoveScreenButtonNow" << std::endl;

    auto it = std::find(m_ScreenButtons.begin(), m_ScreenButtons.end(), screenButton);
    if (it != m_ScreenButtons.end())
    {
        m_ScreenButtons.erase(it);
        delete screenButton;
    }
}

int MenuVSL::AddCleoWindow()
{
    auto window = Instance->AddWindow();

    int i = 1;
    while (m_CleoWindows.find(i) != m_CleoWindows.end())
    {
        //Log::Level(LOG_LEVEL::LOG_BOTH) << "CleoMenu: checked ID " << i << std::endl;
        i++;
    }

    Log::Level(LOG_LEVEL::LOG_BOTH) << "MenuVSL: Found free Window ID " << i << std::endl;

    window->m_Id = i;

    m_CleoWindows[i] = (Window*)window;

    return i;
}

int MenuVSL::AddCleoItem(Item* item)
{
    int i = 1;
    while (m_CleoItems.find(i) != m_CleoItems.end())
    {
        //Log::Level(LOG_LEVEL::LOG_BOTH) << "CleoMenu: checked ID " << i << std::endl;
        i++;
    }

    Log::Level(LOG_LEVEL::LOG_BOTH) << "MenuVSL: Found free Item ID " << i << std::endl;

    item->m_Id = i;

    m_CleoItems[i] = item;

    return i;
}

float MenuVSL::GetFontHeight()
{
    return m_FontScale.y * 12.0f;
}

void MenuVSL::ShowCredits(std::string text, int time)
{
    auto screenSize = Input::GetCellphoneScreenSize();

    auto popupWidth = 400.0f;

    ShowPopup("Menu", text, CVector2D(screenSize.x/2 - popupWidth/2, screenSize.y - 200), time);
}

void MenuVSL::RemovePopup(Popup* popup)
{
    auto it = std::find(m_Popups.begin(), m_Popups.end(), popup);
    m_Popups.erase(it);
    delete popup;
}

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
//#include "stb/stb_image_write.h"

bool MenuVSL::CreateImageFromFile(std::string const& path, Image*& img)
{
    int32_t w, h, c;
    uint8_t* p = stbi_load(path.c_str(), &w, &h, &c, 4);

    if (p) {
        if (!img)
            img = new Image();

        img->width = w;
        img->height = h;
        img->channels = c;
        img->pixels = p;

        return true;
    }

    return false;
}

void* MenuVSL::LoadRwTextureFromFile(std::string file, std::string textureName, bool flipHorizontal)
{
    auto debug = MenuVSL::Instance->debug;

    //debug->AddLine("Loading image from stb: " + file);

    int width, height, channels;
    unsigned char* data = stbi_load(file.c_str(), &width, &height, &channels, STBI_rgb_alpha);

    if (!data) {
        debug->AddLine("Failed to load image");
        return nullptr;
    }

    // Create a raster
    RwRaster* raster = RwRasterCreate(width, height, 32, rwRASTERTYPETEXTURE | rwRASTERFORMAT8888);
    if (!raster) {
        debug->AddLine("Failed to create raster");
        stbi_image_free(data);
        return nullptr;
    }

    // Lock the raster to get access to its pixel data
    RwUInt8* rasterPixels = (RwUInt8*)RwRasterLock(raster, 0, rwRASTERLOCKWRITE);
    if (!rasterPixels) {
        debug->AddLine("Failed to lock raster");
        //RwRasterDestroy(raster);
        stbi_image_free(data);
        return nullptr;
    }

    if(flipHorizontal)
    {
        // Copy the image data to the raster, flipping vertically
        int rowSize = width * 4; // 4 bytes per pixel (RGBA)
        for (int y = 0; y < height; ++y) {
            memcpy(
                rasterPixels + y * rowSize,
                data + (height - 1 - y) * rowSize,
                rowSize
            );
        }
    } else {
        // Copy the image data to the raster (assuming tightly packed)
        memcpy(rasterPixels, data, width * height * 4);
    }
    
    // Unlock the raster
    RwRasterUnlock(raster);

    // Create a texture from the raster
    RwTexture* texture = RwTextureCreate(raster);
    if (!texture) {
        debug->AddLine("Failed to create texture");
        //RwRasterDestroy(raster);
        stbi_image_free(data);
        return nullptr;
    }

    // maybe sets the texture name
    memset(texture->name, 0, 32);
    textureName.copy(texture->name, 32);

    // Free the image data loaded by stb_image
    stbi_image_free(data);

    return texture;

    //original code
    Image* img = nullptr;
    if (CreateImageFromFile(file, img)) {
        uint32_t w = img->width;
        uint32_t h = img->height;
        uint8_t* p = img->pixels;

        bool mipMap = false;

        RwRaster* raster = RwRasterCreate(w, h, 0, rwRASTERTYPETEXTURE | rwRASTERFORMAT8888 | (mipMap ? rwRASTERFORMATMIPMAP | rwRASTERFORMATAUTOMIPMAP : 0));
        RwUInt32* pixels = (RwUInt32*)RwRasterLock(raster, 0, rwRASTERLOCKWRITE);

        for (uint32_t i = 0; i < w * h * 4; i += 4) {
            uint8_t r = p[i + 2];
            uint8_t g = p[i + 1];
            uint8_t b = p[i];

            p[i + 2] = b;
            p[i + 1] = g;
            p[i] = r;
        }

        memcpy(pixels, p, w * h * 4);
        RwRasterUnlock(raster);

        auto tex = RwTextureCreate(raster);
        //std::string fileNoExt = RemovePath(file);
        //fileNoExt = RemoveExtension(fileNoExt);

        if (mipMap)
            RwTextureSetFilterMode(tex, rwFILTERMIPLINEAR);
        else
            RwTextureSetFilterMode(tex, rwFILTERLINEAR);

        RwTextureSetMipmapping(mipMap);
        RwTextureSetAutoMipmapping(mipMap);
        memset(tex->name, 0, 32);
        textureName.copy(tex->name, 32);

        //spritesMap.insert({ fileNoExt, tex });
        //spritesMapIndex.insert({ Index++, tex });

        img->Release();

        return tex;
    }

    return nullptr;
}

IWindow* MenuVSL::AddWindowOptionsString(std::string title, IWindow* parent, std::string* pString, std::vector<std::string>* options)
{
    auto window = AddWindow();
    window->m_Parent = parent;
    window->m_Title = title;
    window->m_ShowBackButton = true;
    //window->m_Position = CVector2D(parent->m_Position.x + 10 + 50 + parent->m_Width, parent->m_Position.y);

    //TODO: separate options by letters A-Z or numbers
    
    //char characters[256] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    /*
    for(auto c : characters)
    {
        std::string firstChar = std::string(1, c);
        std::vector<std::string> options;

        for(auto option : *options)
        {

        }

        auto button = window->AddButton(std::string(1, c), CRGBA(255, 255, 255));
    }
    */

    for(auto option : *options)
    {
        auto button = window->AddButton(option, CRGBA(255, 255, 255));
        button->onClick = [option, pString, window]() {
            *pString = option;

            window->SetToBeRemoved();
        };
    }

    return window;
}

static std::vector<char> multiOptionsVec;

IWindow* MenuVSL::AddWindowMultiOptionsString(std::string title, IWindow* parent, std::vector<std::string>* selectedOptions, std::vector<std::string>* allOptions)
{
    auto window = AddWindow();
    window->m_Parent = parent;
    window->m_Title = title;
    window->m_ShowBackButton = true;

    multiOptionsVec.clear();
    for(auto option : *allOptions)
    {
        bool contains = StringVectorContainsString(*selectedOptions, option);
        multiOptionsVec.push_back(contains ? 1 : 0);
    }

    int i = 0;
    for(auto option : *allOptions)
    {
        auto ptr = (bool*)(&multiOptionsVec[i]);

        auto checkbox = window->AddCheckbox(option, ptr);

        checkbox->onValueChange = [checkbox, selectedOptions, option]() {
            bool value = checkbox->GetCheckboxValue();

            if(value)
            {
                selectedOptions->push_back(option);
                MenuVSL::Instance->debug->AddLine("Added: " + option);
            } else {
                RemoveStringFromVector(*selectedOptions, option);
                MenuVSL::Instance->debug->AddLine("Removed: " + option);
            }

            for(auto option : *selectedOptions)
            {
                MenuVSL::Instance->debug->AddLine(option);
            }
        };

        i++;
    }

    return window;
}

static int colormenu_r = 0;
static int colormenu_g = 0;
static int colormenu_b = 0;
static int colormenu_a = 0;

IWindow* MenuVSL::AddColorWindow(IWindow* parent, CRGBA* color, std::function<void()> onValueChange)
{
    auto window = AddWindow();
    window->m_Parent = parent;
    window->m_Width = 300.0f;
    window->m_Position = { 200, 200 };
    window->m_ShowBackButton = true;

    colormenu_r = color->r;
    colormenu_g = color->g;
    colormenu_b = color->b;
    colormenu_a = color->a;

    auto option_r = window->AddIntRange("R", &colormenu_r, 0, 255, 1);
    auto option_g = window->AddIntRange("G", &colormenu_g, 0, 255, 1);
    auto option_b = window->AddIntRange("B", &colormenu_b, 0, 255, 1);
    auto option_a = window->AddIntRange("A", &colormenu_a, 0, 255, 1);

    option_r->m_HoldToChange = true;
    option_g->m_HoldToChange = true;
    option_b->m_HoldToChange = true;
    option_a->m_HoldToChange = true;

    option_r->onValueChange = [color, onValueChange]() { color->r = colormenu_r; onValueChange(); };
    option_g->onValueChange = [color, onValueChange]() { color->g = colormenu_g; onValueChange(); };
    option_b->onValueChange = [color, onValueChange]() { color->b = colormenu_b; onValueChange(); };
    option_a->onValueChange = [color, onValueChange]() { color->a = colormenu_a; onValueChange(); };

    return window;
}

IWindow* MenuVSL::AddVector2WindowEx(IWindow* parent, CVector2D* vec, float min, float max, float addBy, std::function<void()> onChange, std::function<void()> onBack)
{
    auto window = AddWindow();
    window->m_Parent = parent;
    window->m_Width = 400.0f;
    window->m_Position = { 200, 200 };
    window->m_ShowBackButton = true;

    auto option_x = window->AddFloatRange("X", &vec->x, min, max, addBy);
    option_x->m_HoldToChange = true;
    option_x->onValueChange = [onChange]() { onChange(); };

    auto option_y = window->AddFloatRange("Y", &vec->y, min, max, addBy);
    option_y->m_HoldToChange = true;
    option_y->onValueChange = [onChange]() { onChange(); };

    window->onCloseWindow = [onBack]()
    {
        onBack();
    };

    return window;
}

IWindow* MenuVSL::AddVector2Window(IWindow* parent, CVector2D* vec, float min, float max, float addBy)
{
    return AddVector2WindowEx(parent, vec, min, max, addBy, []() {}, []() {});
}

IWindow* MenuVSL::AddVectorWindowEx(IWindow* parent, CVector* vec, float min, float max, float addBy, std::function<void()> onChange, std::function<void()> onBack)
{
    auto window = AddWindow();
    window->m_Parent = parent;
    window->m_Width = 400.0f;
    window->m_Position = { 200, 200 };
    window->m_ShowBackButton = true;

    auto option_x = window->AddFloatRange("X", &vec->x, min, max, addBy);
    option_x->m_HoldToChange = true;
    option_x->onValueChange = [onChange]() { onChange(); };

    auto option_y = window->AddFloatRange("Y", &vec->y, min, max, addBy);
    option_y->m_HoldToChange = true;
    option_y->onValueChange = [onChange]() { onChange(); };

    auto option_z = window->AddFloatRange("Z", &vec->z, min, max, addBy);
    option_z->m_HoldToChange = true;
    option_z->onValueChange = [onChange]() { onChange(); };

    window->onCloseWindow = [onBack]()
    {
        onBack();
    };

    return window;
}

IWindow* MenuVSL::AddVectorWindow(IWindow* parent, CVector* vec, float min, float max, float addBy)
{
    return AddVectorWindowEx(parent, vec, min, max, addBy, []() {}, []() {});
}

IWindow* MenuVSL::ShowSelectLanguageWindow(IWindow* parent)
{
    auto window = AddWindow();
    window->m_Title = "Language";
    window->m_Parent = parent;
    if(parent) window->m_ShowBackButton = true;

    window->AddText("- Select your language:", CRGBA(255, 255, 255));

    for(auto language : Localization::AvailableLanguages)
    {
        auto button = window->AddButton("~y~" + language);

        button->onClick = [window, language]() {
            Localization::CurrentLanguage = language;
            ModConfig::Save();

            window->SetToBeRemoved();
        };
    }

    return window;
}

void MenuVSL::LoadLanguagesFolder(std::string folder)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Loading languages folder " << folder << std::endl;
    
    if (!std::filesystem::exists(folder))
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "Folder doesnt exists" << std::endl;
        return;
    }
    
    if(!std::filesystem::is_directory(folder))
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "Is not a directory" << std::endl;
        return;
    }

    for(auto& file : std::filesystem::directory_iterator(folder))
    {
        if(file.is_directory()) continue;

        std::string filePath = file.path().string();
        std::string filename = file.path().filename().string();

        std::string extension = ".json";

        std::string language = filename;
        language.replace(language.size() - extension.size(), extension.size(), "");

        Log::Level(LOG_LEVEL::LOG_BOTH) << "Loading language " << language << std::endl;

        Localization::RegisterLanguage(language, filePath); 
    }
}

std::string MenuVSL::GetLanguageLineFormatted(std::string key, ...)
{
    va_list args;
    va_start(args, key);

    // Chama a versão que usa va_list e a linha como formato
    std::string result = Localization::GetLineFormatted(key, args);

    va_end(args);

    return result;
}

void MenuVSL::ShowMessage(std::string key, int time)
{
    Messages::ShowMessage(key, time);
}

void MenuVSL::AddModCredits(std::string key)
{
    ModCredits menuCredits;
    menuCredits.text = key;
    m_ModCredits.push_back(menuCredits);
}

IScreenButton* MenuVSL::AddScreenButton(CVector2D position, std::string texture, CVector2D size)
{
    ScreenButton* screenButton = new ScreenButton();
    screenButton->m_Position = position;
    screenButton->m_Size = size;
    screenButton->m_Sprite.m_pTexture = TextureManager::LoadTexture(texture, "screen_button");

    m_ScreenButtons.push_back(screenButton);

    return (IScreenButton*)screenButton;
}

//

void MenuVSL::SetGlobalIntVariable(std::string key, int value)
{
    globalIntVariables[key] = value;
}

int MenuVSL::GetGlobalIntVariable(std::string key)
{
    if(!globalIntVariables.count(key)) return 0;
    return globalIntVariables[key];
}

CVector2D MenuVSL::ConvertWorldPositionToScreenPosition(CVector worldPosition)
{
    RwV3d rwp = { worldPosition.x, worldPosition.y, worldPosition.z };
	RwV3d screenCoors; float w, h;

	CSprite_CalcScreenCoors(rwp, &screenCoors, &w, &h, true, true);
    
    //screenCoors.x = FixPositionX(screenCoors.x);    
    //screenCoors.y = FixPositionY(screenCoors.y);

    return CVector2D(screenCoors.x, screenCoors.y);
}

void MenuVSL::DrawWorldText(std::string text, CVector position, CRGBA color, eFontAlignment align)
{
    auto screenPosition = ConvertWorldPositionToScreenPosition(position);

    DrawString(text, screenPosition, color, align);
}

void MenuVSL::AddOnRender(std::function<void()> fn)
{
    OnRenderFunctions.push_back(fn);
}

void MenuVSL::AddOnVehicleRenderBefore(std::function<void(void*)> fn)
{
    OnVehicleRenderBeforeFunctions.push_back(fn);
}

void MenuVSL::AddOnVehicleRenderAfter(std::function<void(void*)> fn)
{
    OnVehicleRenderAfterFunctions.push_back(fn);
}

void MenuVSL::AddOnUpdate(std::function<void()> fn)
{
    OnUpdateFunctions.push_back(fn);
}

void MenuVSL::AddOnProcessScripts(std::function<void()> fn)
{
    OnProcessScriptsFunctions.push_back(fn);
}

std::vector<MVehicle> MenuVSL::GetVehicles()
{
    auto vehicles = Vehicles::GetVehicles();
    std::vector<MVehicle> mvehicles;
    for(auto vehicle : vehicles)
    {
        MVehicle mv;
        mv.pVehicle = vehicle->pVehicle;
        mv.hVehicle = vehicle->hVehicle;

        mvehicles.push_back(mv);
    }
    return mvehicles;
}

void MenuVSL::DrawRectWithStringMultiline(std::string text, CVector2D pos, float width, CVector2D padding, CRGBA boxColor, CRGBA textColor, eFontAlignment align)
{
    int extraLines = countSubstrOccurrences(text, "~n~");

    float lineSizeY = MenuVSL::GetFontHeight();

    CVector2D boxSize = CVector2D(width, lineSizeY);
    boxSize.y += padding.y * 2;
    boxSize.y += extraLines * lineSizeY;

	DrawRect(pos, boxSize, boxColor);

	CVector2D textPos = pos;
    textPos.y += padding.y;

	if (align == eFontAlignment::ALIGN_CENTER)
	{
		textPos.x += boxSize.x / 2;
	} else if (align == eFontAlignment::ALIGN_RIGHT)
	{
		textPos.x += boxSize.x;
        textPos.x -= padding.x;
	} else {
        textPos.x += padding.x;
    }

	DrawString(text, textPos, textColor, align);
}

void MenuVSL::ShowPopup(std::string title, std::string text, CVector2D pos, int time)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "ShowPopup" << std::endl;

    Popup* popup = new Popup();
    popup->m_Title = title;
    popup->m_Text = text;
    popup->m_Position = pos;
    popup->m_Time = time;

    m_Popups.push_back(popup);
}

void MenuVSL::SetDrawWithFixedScale(bool enabled)
{
    m_DrawWithFixedScale = enabled;
}

CVector2D MenuVSL::GetFontScale()
{
    return m_FontScale;
}

void MenuVSL::SetFontScale(float sx, float sy)
{
    m_FontScale.x = sx;
    m_FontScale.y = sy;
}

void MenuVSL::ResetFontScale()
{
    m_FontScale = m_DefaultFontScale;
}

static std::vector<std::string> testStrVec;
static std::string testStr;
static CRGBA testColor(255, 0, 0);
static std::vector<std::string> testAllOptions;
static std::vector<std::string> testSelectedOptions;
static CVector2D vec2(1.0f, 2.0f);
static CVector vec3(1.0f, 2.0f, 3.0f);

void MenuVSL::CreateMainMenu()
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Creating test menu" << std::endl;

    auto menuVSL = this;

    auto window = AddWindow();

    auto button_language = window->AddButton("Change Language");
    button_language->onClick = [menuVSL, window]() {
        menuVSL->ShowSelectLanguageWindow(window);
    };

    auto button_debug = window->AddButton("Toggle Debug");
    button_debug->onClick = [menuVSL]() {
        menuVSL->debug->visible = !menuVSL->debug->visible;

        if(menuVSL->debug->visible)
        {
            menuVSL->ShowMessage("Debug~n~~g~ON", 3000);
        } else {
            menuVSL->ShowMessage("Debug~n~~r~OFF", 3000);
        }
    };

    auto button_cleardebug = window->AddButton("Clear Debug");
    button_cleardebug->onClick = [menuVSL]() {
        menuVSL->debug->Clear();
        menuVSL->ShowMessage("Debug cleared", 2000);
    };

    auto button_test = window->AddButton("Tests");
    button_test->onClick = [menuVSL, window]() {
        menuVSL->CreateTestMenu(window);
    };

    auto close = window->AddButton("~r~Close");
    close->onClick = [window]() {
        window->SetToBeRemoved();
    };
}

void MenuVSL::CreateTestMenu(IWindow* parent)
{
    auto menuVSL = this;

    auto window = AddWindow(parent);

    testStrVec.clear();
    testStr = "none";
    testAllOptions.clear();
    testAllOptions.push_back("Op 1");
    testAllOptions.push_back("Op 2");
    testAllOptions.push_back("Op 3");
    testSelectedOptions.clear();
    testSelectedOptions.push_back(testAllOptions[1]);
    
    for(int i = 0; i < 20; i++)
    {
        testStrVec.push_back("String " + std::to_string(i));
    }

    auto button_screenButton = window->AddButton("Screen buttons");
    button_screenButton->onClick = [menuVSL]() {

        char path[512];

        sprintf(path, "%s/menuVSL/test.png", aml->GetConfigPath());
        auto screenButton = menuVSL->AddScreenButton(CVector2D(400, 200), path, CVector2D(80, 80));
        screenButton->m_Text = "Test button";
        screenButton->onClick = [screenButton]() {
            screenButton->SetToBeRemoved();
        };
    };

    auto color = window->AddButton("Select color");
    color->AddColorIndicator(&testColor);
    color->AddColorIndicator(&testColor);
    color->AddColorIndicator(&testColor);
    color->onClick = [menuVSL, window]() {
        menuVSL->AddColorWindow(window, &testColor, []() {});
    };

    auto button_message1 = window->AddButton("Show message 1");
    button_message1->onClick = [menuVSL, window]() {
        Messages::ShowMessage("THis is the~n~bottom message~n~num 1 ok?", 3000);
    };

    auto button_message2 = window->AddButton("Show message 2");
    button_message2->onClick = [menuVSL, window]() {
        Messages::ShowMessage("THis is the~n~bottom ~r~message ~w~num2", 3000);
    };

    auto button_extraText = window->AddButton("Test extra text:");
    button_extraText->m_StringAtRight = "EXTRA TEXT";
    button_extraText->onClick = [menuVSL, window]() {

    };

    auto button_vec2 = window->AddButton("Vector2 XY");
    button_vec2->onClick = [menuVSL, window]() {
        menuVSL->AddVector2Window(window, &vec2, -10.0f, 10.0f, 0.1f);
    };

    auto button_vec3 = window->AddButton("Vector3 XYZ");
    button_vec3->onClick = [menuVSL, window]() {
        menuVSL->AddVectorWindow(window, &vec3, -10.0f, 10.0f, 0.1f);
    };

    auto selectItem = window->AddButton("Select item");
    selectItem->m_StringPtrAtRight = &testStr;
    selectItem->onClick = [menuVSL, window] () {
        auto newWindow = menuVSL->AddWindowOptionsString("Select string", window, &testStr, &testStrVec);
    };

    auto selectMultipleItem = window->AddButton("Select multiple items");
    selectMultipleItem->onClick = [menuVSL, window] () {
        auto newWindow = menuVSL->AddWindowMultiOptionsString("Select multiple", window, &testSelectedOptions, &testAllOptions);
        newWindow->onCloseWindow = [menuVSL] () {
            menuVSL->debug->AddLine("OnCloseWindow");
        };
    };

    auto options = window->AddOptions("Options");
    options->AddOption(35, "Value 35");
    options->AddOption(17, "Value 17");
    options->AddOption(112, "Value 112");
    options->SetCurrentOption(17);
    
    auto currentItem = window->AddText("Current: ???", CRGBA(255, 255, 255));
    currentItem->onUpdate = [currentItem, options]() {
        currentItem->m_Text = options->GetCurrentOption().name + ", " + std::to_string(options->GetCurrentOption().value);
    };

    window->AddText("Select the item:", CRGBA(255, 255, 255));

    for(int i = 0; i < 5; i++)
    {
        window->AddButton("> ~y~Test item " + std::to_string(i));
    }

    auto close = window->AddButton("~r~Back");
    close->onClick = [window]() {
        window->SetToBeRemoved();
    };
}

float MenuVSL::FixPositionX(float x)
{
    return MenuVSL::m_ScreenSize.x / MenuVSL::m_GTAScreenSize.x * x;
}

float MenuVSL::FixPositionY(float y)
{
    return MenuVSL::m_ScreenSize.y / MenuVSL::m_GTAScreenSize.y * y;
}

IMenuVSL* menuVSL = MenuVSL::Instance;
