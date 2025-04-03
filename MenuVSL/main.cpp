#include <mod/amlmod.h>
#include <mod/logger.h>
#include <mod/config.h>

//#include "SimpleGTA.h"

#include "utils.h"

#include "Log.h"
#include "Input.h"
#include "Localization.h"
#include "opcodes.h"

MYMODCFG(net.danilo1301.menuVSL, Menu VSL, 1.5.2, Danilo1301)
//MYMOD(net.rusjj.mymod.guid, AML Mod Template Without Config, 1.0, RusJJ)

//NEEDGAME(net.rusjj.mygame)

//BEGIN_DEPLIST()
//    ADD_DEPENDENCY_VER(net.rusjj.aml, 1.0)
//END_DEPLIST()

// CLEO 2.0.1.3
#include "cleo.h"
cleo_ifs_t* cleo = NULL;

// SAUtils 1.6 (but it says 1.5.2)
#include "isautils.h"
ISAUtils* sautils = NULL;

#include "MenuVSL.h"
#include "ModConfig.h"

#include "rw/rwcore.h"
#include "rw/rwplcore.h"
#include "rw/rpworld.h"

//

CVector2D *m_vecCachedPos;
void (*CSprite2d_DrawRect)(CRect const& posn, CRGBA  const& color);
void (*CSprite2d_DrawSprite)(CSprite2d*, CRect const&, CRGBA const&);
bool (*CSprite_CalcScreenCoors)(RwV3d const &posn, RwV3d *out, float *w, float *h, bool checkMaxVisible, bool checkMinVisible);
void (*CFont_PrintString)(float x, float y, unsigned short* text);
void (*CFont_SetOrientation)(eFontAlignment align);
void (*CFont_SetScale)(float w, float h);
void (*CFont_SetFontStyle)(short style);
void (*CFont_SetWrapx)(float value);
void (*CFont_SetProportional)(bool on);
void (*CFont_SetColor)(CRGBA const& color);
void (*CFont_SetDropShadowPosition)(short value);
void (*AsciiToGxtChar)(const char* txt, unsigned short* ret);
void (*RenderFontBuffer)(void);
bool (*CTouchInterface_IsTouchDown)(int param1);
bool *CTouchInterface_m_bTouchDown;
void (*UpdateCompareFlag)(void*, uint8_t);
int (*OS_ScreenGetWidth)(void);
int (*OS_ScreenGetHeight)(void);
unsigned int *m_snTimeInMilliseconds;

RwRaster* (*RwRasterCreate)(RwInt32 width, RwInt32 height, RwInt32 depth, RwInt32 flags);
RwRaster* (*RwRasterUnlock)(RwRaster* raster);
RwBool (*RwTextureSetMipmapping)(RwBool enable);
RwBool (*RwTextureSetAutoMipmapping)(RwBool enable);
RwTexture* (*RwTextureCreate)(RwRaster* raster);
RwUInt8* (*RwRasterLock)(RwRaster* raster, RwUInt8 level, RwInt32 lockMode);
void (*RwImageFindRasterFormat)(RwImage*, int, int*, int*, int*, int*);
void (*RwImageDestroy)(RwImage*);
void (*RwRasterSetFromImage)(RwRaster*, RwImage*);
RwImage* (*RtPNGImageRead)(const char* filename);
RwImage* (*RtBMPImageRead)(const RwChar* imageName);
RwTexture* (*RwTextureRead)(const RwChar* name, const RwChar* maskname);

uintptr_t* pVehiclePool = 0;
//uintptr_t* pPedPool = 0;

int (*GetVehicleRef)(int);
void* (*GetVehicleFromRef)(int);
//int (*GetPedRef)(int);
//void* (*GetPedFromRef)(int);

//

/*
char text[0xFF]; //char text[24];
unsigned short* textGxt = new unsigned short[0xFF];
*/

unsigned int prevTime = 0;

/*
DECL_HOOKv(CHud_Draw)
{
    Log::Level(LOG_LEVEL::LOG_UPDATE) << "CHud_Draw" << std::endl;

    Log::Level(LOG_LEVEL::LOG_UPDATE) << "1" << std::endl;
    CHud_Draw();
    Log::Level(LOG_LEVEL::LOG_UPDATE) << "2" << std::endl;

    MenuVSL::Instance->Draw();

    Log::Level(LOG_LEVEL::LOG_UPDATE) << "3" << std::endl;
    
    //CSprite2d_DrawRect(CRect(300, 300, 600, 600), CRGBA(255, 0, 255));
    //CSprite2d_DrawRect(CRect(0, 0, 200, 200), CRGBA(0, 0, 255));

    //Log::Level(LOG_LEVEL::LOG_UPDATE) << "vecCachedPos: x " << m_vecCachedPos->x << ", y " << m_vecCachedPos->y << std::endl;
    Log::Level(LOG_LEVEL::LOG_UPDATE) << "CHud_Draw end" << std::endl;
}
*/

DECL_HOOK(void, PreRenderEnd, void* self)
{
    Log::Level(LOG_LEVEL::LOG_UPDATE) << "PreRenderEnd" << std::endl;
    PreRenderEnd(self);

    MenuVSL::Instance->Draw();

    Log::Level(LOG_LEVEL::LOG_UPDATE) << "PreRenderEnd end" << std::endl;
}

DECL_HOOK(void*, UpdateGameLogic, uintptr_t a1)
{
    Log::Level(LOG_LEVEL::LOG_UPDATE) << "UpdateGameLogic" << std::endl;

    auto time = *m_snTimeInMilliseconds;
    if(prevTime == 0) prevTime = time;
    auto dt = time - prevTime;
    prevTime = time;

    menuVSL->deltaTime = dt;

    //Log::Level(LOG_LEVEL::LOG_UPDATE) << "dt: " << dt << std::endl;

    Input::Update();

    //Log::Level(LOG_LEVEL::LOG_UPDATE) << "vecCachedPos: x " << m_vecCachedPos->x << ", y " << m_vecCachedPos->y << std::endl;
    //Log::Level(LOG_LEVEL::LOG_UPDATE) << "touchDown: " << Input::IsTouchPressed() << std::endl;
    //Log::Level(LOG_LEVEL::LOG_UPDATE) << "isTouchPressed: " << Input::isTouchPressed << std::endl;

    MenuVSL::Instance->Update(dt);

    MenuVSL::Instance->ProcessScripts();
    
    Log::Level(LOG_LEVEL::LOG_UPDATE) << "UpdateGameLogic end" << std::endl;

    return UpdateGameLogic(a1);
}

DECL_HOOKv(RenderVehicle, void* self)
{
    MenuVSL::Instance->VehicleRenderBefore(self);
    RenderVehicle(self);
    MenuVSL::Instance->VehicleRenderAfter(self);
}

/*
CRASHES ON SAMP, MFUCKER, but ok, I have UpdateGameLogic
OK, NOW IT CRASHES ON SINGLEPLYAER, fuck it we ball
*/
// DECL_HOOKv(CTheScripts_Process)
// {
//     Log::Level(LOG_LEVEL::LOG_UPDATE) << "CTheScripts_Process" << std::endl;
//     CTheScripts_Process();
//     Log::Level(LOG_LEVEL::LOG_UPDATE) << "CTheScripts_Process fn called" << std::endl;
//     MenuVSL::Instance->ProcessScripts();
//     Log::Level(LOG_LEVEL::LOG_UPDATE) << "CTheScripts_Process end" << std::endl;
// }

extern "C" void OnModPreLoad()
{
    ModConfig::MakePaths();
    ModConfig::DefineVersions();

    char logPath[512];
    sprintf(logPath, "%s/menuVSL/", aml->GetConfigPath());
    Log::Open(logPath, "menuVSL");

    Log::Level(LOG_LEVEL::LOG_BOTH) << "Preload()" << std::endl;
    Log::Level(LOG_LEVEL::LOG_BOTH) << "AML headers: 1.0.3.1" << std::endl;
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Test #3" << std::endl;

    Log::Level(LOG_LEVEL::LOG_BOTH) << "Registering interface 'MenuVSL'..." << std::endl;

    RegisterInterface("MenuVSL", menuVSL);

    menuVSL->debug->visible = false;
    menuVSL->debug->AddLine("Interface MenuVSL registered");
    menuVSL->debug->AddLine("~w~MenuVSL v" + ModConfig::GetModVersion() + " (by ~y~Danilo1301~w~)");

    Log::Level(LOG_LEVEL::LOG_BOTH) << "test_value: " << menuVSL->GetGlobalIntVariable("test_value") << std::endl;

    menuVSL->SetGlobalIntVariable("test_value", 123);

    Log::Level(LOG_LEVEL::LOG_BOTH) << "test_value: " << menuVSL->GetGlobalIntVariable("test_value") << std::endl;
    
    Log::Level(LOG_LEVEL::LOG_BOTH) << "size: " << menuVSL->GetWindows().size() << std::endl;

    Log::Level(LOG_LEVEL::LOG_BOTH) << "Preload() END" << std::endl;

    menuVSL->debug->AddLine("Preloaded!");
}

extern "C" void OnModLoad()
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Load()" << std::endl;

    cfg->Bind("Author", "", "About")->SetString("Danilo1301"); cfg->ClearLast();
    cfg->Bind("Discord", "", "About")->SetString("https://discord.gg/mkCDRf4zJA"); cfg->ClearLast();
    cfg->Bind("GitHub", "", "About")->SetString("https://github.com/Danilo1301/GTASA_libMenuVSL"); cfg->ClearLast();
    cfg->Save();

    //Interfaces

    LoadInterface(&cleo, "CLEO");
    Log::Level(LOG_LEVEL::LOG_BOTH) << "CLEO interface 2.0.1.3: (cleo_ifs_t)" << std::endl;

    if(!cleo)
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "You don't have the lib CLEO" << std::endl;
        return;
    }

    LoadInterface(&sautils, "SAUtils");

    if(sautils)
    {
        menuVSL->debug->AddLine("SAUtils loaded!");

        sautils->AddSliderItem(eTypeOfSettings::SetType_Mods, "MenuVSL (by Danilo1301)", 0, 0, 1);
    }

    Log::Level(LOG_LEVEL::LOG_BOTH) << "----------------------------" << std::endl;
    Log::Level(LOG_LEVEL::LOG_BOTH) << "MenuVSL: v" << ModConfig::GetModVersion() << std::endl;
    //Log::Level(LOG_LEVEL::LOG_BOTH) << "CLEO version: " << cleoVersion << " (recommended 2.0.1.3)" << std::endl;
    //Log::Level(LOG_LEVEL::LOG_BOTH) << "SAUtils version: " << sautilsVersion << " (recommended 1.3.1)" << std::endl;
    //Log::Level(LOG_LEVEL::LOG_BOTH) << "AML version: " << amlVersion << " (recommended 1.2.2)" << std::endl;
    Log::Level(LOG_LEVEL::LOG_BOTH) << "----------------------------" << std::endl;

    Log::Level(LOG_LEVEL::LOG_BOTH) << "Finding symbols..." << std::endl;

    void* hGTASA = dlopen("libGTASA.so", RTLD_LAZY);
    uintptr_t gameAddr = (uintptr_t)cleo->GetMainLibraryLoadAddress();

    SET_TO(m_vecCachedPos, aml->GetSym(hGTASA, "_ZN15CTouchInterface14m_vecCachedPosE"));
    SET_TO(CSprite2d_DrawRect, aml->GetSym(hGTASA, "_ZN9CSprite2d8DrawRectERK5CRectRK5CRGBA"));
    SET_TO(CSprite2d_DrawSprite, aml->GetSym(hGTASA, "_ZN9CSprite2d4DrawERK5CRectRK5CRGBA"));
    SET_TO(CSprite_CalcScreenCoors, aml->GetSym(hGTASA, "_ZN7CSprite15CalcScreenCoorsERK5RwV3dPS0_PfS4_bb"));
    SET_TO(CFont_PrintString, aml->GetSym(hGTASA, "_ZN5CFont11PrintStringEffPt"));
    SET_TO(CFont_SetOrientation, aml->GetSym(hGTASA, "_ZN5CFont14SetOrientationEh"));
    SET_TO(CFont_SetScale, aml->GetSym(hGTASA, "_ZN5CFont8SetScaleEf"));
    SET_TO(CFont_SetFontStyle, aml->GetSym(hGTASA, "_ZN5CFont12SetFontStyleEh"));
    SET_TO(CFont_SetWrapx, aml->GetSym(hGTASA, "_ZN5CFont8SetWrapxEf"));
    SET_TO(CFont_SetProportional, aml->GetSym(hGTASA, "_ZN5CFont15SetProportionalEh"));
    SET_TO(CFont_SetColor, aml->GetSym(hGTASA, "_ZN5CFont8SetColorE5CRGBA"));
    SET_TO(CFont_SetDropShadowPosition, aml->GetSym(hGTASA, "_ZN5CFont21SetDropShadowPositionEa"));
    SET_TO(AsciiToGxtChar, aml->GetSym(hGTASA, "_Z14AsciiToGxtCharPKcPt"));
    SET_TO(RenderFontBuffer, aml->GetSym(hGTASA, "_ZN5CFont16RenderFontBufferEv"));
    SET_TO(CTouchInterface_IsTouchDown, aml->GetSym(hGTASA, "_ZN15CTouchInterface11IsTouchDownEi"));
    SET_TO(CTouchInterface_m_bTouchDown, aml->GetSym(hGTASA, "_ZN15CTouchInterface12m_bTouchDownE"));
    SET_TO(UpdateCompareFlag, aml->GetSym(hGTASA, "_ZN14CRunningScript17UpdateCompareFlagEh")); //https://github.com/AndroidModLoader/GTASA_CLEOPlus/blob/main/externs.cpp
    SET_TO(OS_ScreenGetWidth, aml->GetSym(hGTASA, "_Z17OS_ScreenGetWidthv"));
    SET_TO(OS_ScreenGetHeight, aml->GetSym(hGTASA, "_Z18OS_ScreenGetHeightv"));
    SET_TO(m_snTimeInMilliseconds, aml->GetSym(hGTASA, "_ZN6CTimer22m_snTimeInMillisecondsE"));

    SET_TO(RwRasterCreate, aml->GetSym(hGTASA, "_Z14RwRasterCreateiiii"));
    SET_TO(RwRasterUnlock, aml->GetSym(hGTASA, "_Z14RwRasterUnlockP8RwRaster"));
    SET_TO(RwTextureSetMipmapping, aml->GetSym(hGTASA, "_Z22RwTextureSetMipmappingi"));
    SET_TO(RwTextureSetAutoMipmapping, aml->GetSym(hGTASA, "_Z26RwTextureSetAutoMipmappingi"));
    SET_TO(RwTextureCreate, aml->GetSym(hGTASA, "_Z15RwTextureCreateP8RwRaster"));
    SET_TO(RwRasterLock, aml->GetSym(hGTASA, "_Z12RwRasterLockP8RwRasterhi"));
    SET_TO(RwImageFindRasterFormat, aml->GetSym(hGTASA, "_Z23RwImageFindRasterFormatP7RwImageiPiS1_S1_S1_"));
    SET_TO(RwImageDestroy, aml->GetSym(hGTASA, "_Z14RwImageDestroyP7RwImage"));
    SET_TO(RwRasterSetFromImage, aml->GetSym(hGTASA, "_Z20RwRasterSetFromImageP8RwRasterP7RwImage"));
    SET_TO(RtPNGImageRead, aml->GetSym(hGTASA, "_Z14RtPNGImageReadPKc"));
    SET_TO(RtBMPImageRead, aml->GetSym(hGTASA, "_Z14RtBMPImageReadPKc"));

    SET_TO(RwTextureRead, aml->GetSym(hGTASA, "_Z13RwTextureReadPKcS0_"));

    SET_TO(pVehiclePool, aml->GetSym(hGTASA, "_ZN6CPools15ms_pVehiclePoolE"));
    //SET_TO(pPedPool, aml->GetSym(hGTASA, "_ZN6CPools11ms_pPedPoolE"));

    SET_TO(GetVehicleRef, aml->GetSym(hGTASA, "_ZN6CPools13GetVehicleRefEP8CVehicle"));
    SET_TO(GetVehicleFromRef, aml->GetSym(hGTASA, "_ZN6CPools10GetVehicleEi"));
    //SET_TO(GetPedRef, aml->GetSym(hGTASA, "_ZN6CPools9GetPedRefEP4CPed"));
    //SET_TO(GetPedFromRef, aml->GetSym(hGTASA, "_ZN6CPools6GetPedEi"));



    HOOKPLT(UpdateGameLogic, gameAddr + 0x66FE58);
    HOOK(RenderVehicle, aml->GetSym(hGTASA, "_ZN8CVehicle6RenderEv"));
    //HOOK(CTheScripts_Process, aml->GetSym(hGTASA, "_ZN11CTheScripts7ProcessEv"));
    HOOKPLT(PreRenderEnd, gameAddr + 0x674188);
    
    //HOOKPLT(PreRenderEnd, gameAddr + BYBIT(0x674188, 0x846E90));

    //HOOK(CHud_Draw, aml->GetSym(hGTASA, "_ZN4CHud4DrawEv"));

    MenuVSL::m_ScreenSize.x = OS_ScreenGetWidth();
    MenuVSL::m_ScreenSize.y = OS_ScreenGetHeight();
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Screen width: " << OS_ScreenGetWidth() << std::endl;
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Screen height: " << OS_ScreenGetHeight() << std::endl;
    Log::Level(LOG_LEVEL::LOG_BOTH) << "GTA Screen width: " << Input::GetGTAScreenSize().x << std::endl;
    Log::Level(LOG_LEVEL::LOG_BOTH) << "GTA Screen height: " << Input::GetGTAScreenSize().y << std::endl;

    Log::Level(LOG_LEVEL::LOG_BOTH) << "Registering opcodes..." << std::endl;

    __reg_op_func(MENU_VSL_ADD_WINDOW, MENU_VSL_ADD_WINDOW);
    __reg_op_func(MENU_VSL_TEST, MENU_VSL_TEST);
    __reg_op_func(MENU_VSL_ADD_TEXT, MENU_VSL_ADD_TEXT);
    __reg_op_func(MENU_VSL_ADD_BUTTON, MENU_VSL_ADD_BUTTON);
    __reg_op_func(MENU_VSL_IS_BUTTON_JUST_PRESSED, MENU_VSL_IS_BUTTON_JUST_PRESSED);
    __reg_op_func(MENU_VSL_REMOVE_WINDOW, MENU_VSL_REMOVE_WINDOW);
    __reg_op_func(MENU_VSL_ADD_INT_RANGE, MENU_VSL_ADD_INT_RANGE);
    __reg_op_func(MENU_VSL_ADD_FLOAT_RANGE, MENU_VSL_ADD_FLOAT_RANGE);
    __reg_op_func(MENU_VSL_GET_INT_VALUE, MENU_VSL_GET_INT_VALUE);
    __reg_op_func(MENU_VSL_GET_FLOAT_VALUE, MENU_VSL_GET_FLOAT_VALUE);
    __reg_op_func(MENU_VSL_SET_HOLD_TO_CHANGE, MENU_VSL_SET_HOLD_TO_CHANGE);
    __reg_op_func(MENU_VSL_SET_DEBUG_VISIBLE, MENU_VSL_SET_DEBUG_VISIBLE);
    __reg_op_func(MENU_VSL_ADD_DEBUG_INT, MENU_VSL_ADD_DEBUG_INT);
    __reg_op_func(MENU_VSL_ADD_DEBUG_FLOAT, MENU_VSL_ADD_DEBUG_FLOAT);
    __reg_op_func(MENU_VSL_ADD_CHECKBOX, MENU_VSL_ADD_CHECKBOX);
    __reg_op_func(MENU_VSL_SET_WINDOW_BACKGROUND, MENU_VSL_SET_WINDOW_BACKGROUND);

    __reg_op_func(SET_GLOBAL_INT_VARIABLE, SET_GLOBAL_INT_VARIABLE);
    __reg_op_func(GET_GLOBAL_INT_VARIABLE, GET_GLOBAL_INT_VARIABLE);

    ModConfig::DefineVersions();
    ModConfig::ProcessVersionChanges_PreConfigLoad();
    ModConfig::Load();

    char path[512];
    sprintf(path, "%s/menuVSL/languages/", aml->GetConfigPath());
    menuVSL->LoadLanguagesFolder(path);

    ModConfig::ProcessVersionChanges_PostConfigLoad();
    ModConfig::Save();

    Log::Level(LOG_LEVEL::LOG_BOTH) << "Load() END" << std::endl;

    menuVSL->debug->AddLine("Loaded!");

    /*
    menuVSL->AddOnProcessScripts([] () {
        menuVSL->debug->visible = true;
        menuVSL->debug->AddLine("AddOnProcessScripts works");
    });
    */

    /*
    menuVSL->AddOnUpdate([] () {
        menuVSL->debug->visible = true;
        menuVSL->debug->AddLine("AddOnUpdate");

        auto vehicles = menuVSL->GetVehicles();

        menuVSL->debug->AddLine(std::to_string(vehicles.size()) + " vehicles");
    });
    */
}   
