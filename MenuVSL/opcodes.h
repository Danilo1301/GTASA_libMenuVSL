#include <stdint.h>
#include <string>

#include "cleo.h"

#include <mod/logger.h>

#define __decl_op(__name, __int)	const char* NAME_##__name = #__name; const uint16_t OP_##__name = __int;

#define __handler_params 			void *handle, uint32_t *ip, uint16_t opcode, const char *name

// Size of array
#define sizeofA(__aVar)  ((int)(sizeof(__aVar)/sizeof(__aVar[0])))

#define __reg_opcode				cleo->RegisterOpcode

#define __reg_func					cleo->RegisterOpcodeFunction

#define __op_name_match(x) 			opcode == OP_##x || strcmp(name, NAME_##x) == 0

#define __reg_op_func(x, h) 		__reg_opcode(OP_##x, h); __reg_func(NAME_##x, h);

#define __readParam(handle)         reinterpret_cast<cleo_ifs_t::data_t*>(cleo->ReadParam(handle))
#define __getPointerToScriptVar(handle)         reinterpret_cast<cleo_ifs_t::data_t*>(cleo->GetPointerToScriptVar(handle))

#include "Log.h"

#include "MenuVSL.h"

extern cleo_ifs_t* cleo;
extern void (*UpdateCompareFlag)(void*, uint8_t);

__decl_op(MENU_VSL_ADD_WINDOW, 0x0FE0); // 0FE0=4,%4d% = menu_vsl_add_window title %1g% x %2d% y %3d%
__decl_op(MENU_VSL_TEST, 0x0FE1); // 0FE1=3,%3d% = menu_vsl_test %1d% %2g%
__decl_op(MENU_VSL_ADD_TEXT, 0x0FE2); // 0FE2=3,%3d% = menu_vsl_add_text window %1d% text %2g%
__decl_op(MENU_VSL_ADD_BUTTON, 0x0FE3); // 0FE3=3,%3d% = menu_vsl_add_button window %1d% text %2g%
__decl_op(MENU_VSL_IS_BUTTON_JUST_PRESSED, 0x0FE4); //0FE4=1,menu_vsl_is_button_just_pressed %1d%
__decl_op(MENU_VSL_REMOVE_WINDOW, 0x0FE5); // 0FE5=1,menu_vsl_remove_window %1d%
__decl_op(MENU_VSL_ADD_INT_RANGE, 0x0FE6); // 0FE6=7,%7d% = menu_vsl_add_int_range window %1d% text %2g% value %3d% min %4d% max %5d% add_by %6d%
__decl_op(MENU_VSL_ADD_FLOAT_RANGE, 0x0FE7); // 0FE7=7,%7d% = menu_vsl_add_float_range window %1d% text %2g% value %3d% min %4d% max %5d% add_by %6d%
__decl_op(MENU_VSL_GET_INT_VALUE, 0x0FE8); // 0FE8=2,%2g% = menu_vsl_get_int_value item %1d%
__decl_op(MENU_VSL_GET_FLOAT_VALUE, 0x0FE9); // 0FE9=2,%2g% = menu_vsl_get_float_value item %1d%
__decl_op(MENU_VSL_SET_HOLD_TO_CHANGE, 0x0FEA); // 0FEA=2,menu_vsl_set_hold_to_change item %1d% value %2d%
__decl_op(MENU_VSL_SET_DEBUG_VISIBLE, 0x0FEB); // 0FEB=1,menu_vsl_set_debug_visible %1d%
__decl_op(MENU_VSL_ADD_DEBUG_INT, 0x0FEC); // 0FEC=2,menu_vsl_add_debug_int %1g% %2d%
__decl_op(MENU_VSL_ADD_DEBUG_FLOAT, 0x0FED); // 0FED=2,menu_vsl_add_debug_float %1g% %2d%
__decl_op(MENU_VSL_ADD_CHECKBOX, 0x0FEE); // 0FEE=4,%4d% = menu_vsl_add_checkbox window %1d% text %2g% value %3d%
__decl_op(MENU_VSL_SET_WINDOW_BACKGROUND, 0x0FEF); // 0FEF=5,menu_vsl_set_window_background window %1d% r %2d% g %3d% b %4d% a %5d%
__decl_op(SET_GLOBAL_INT_VARIABLE, 0x0FF0); // 0FF0=2,set_global_int_variable %1g% value %2d%
__decl_op(GET_GLOBAL_INT_VARIABLE, 0x0FF1); // 0FF1=2,%2d% = get_global_int_variable %1g%


bool bool1 = true;
int int1 = 5;
float float1 = 0.35f;

static void replaceSubstring(std::string& str, const std::string& from, const std::string& to) {
    size_t startPos = str.find(from);
    if (startPos == std::string::npos) {
        //std::cerr << "Substring not found!" << std::endl;
        return;
    }
    str.replace(startPos, from.length(), to);
}

static void MENU_VSL_ADD_WINDOW(__handler_params)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "MENU_VSL_ADD_WINDOW" << std::endl;

    char param1[256];
    cleo->ReadStringLong(handle, param1, sizeof(param1));
    param1[sizeof(param1)-1] = 0; // I can't trust game scripting engine...
    int i = 0;
    while(param1[i] != 0) // A little hack
    {
        if(param1[i] == '\\') param1[i] = '/';
        ++i;
    }

    Log::Level(LOG_LEVEL::LOG_BOTH) << "param1=" << param1 << std::endl;

    int x = __readParam(handle)->i;
    int y = __readParam(handle)->i;

    Log::Level(LOG_LEVEL::LOG_BOTH) << "x: " << x << std::endl;
    Log::Level(LOG_LEVEL::LOG_BOTH) << "y: " << y << std::endl;

    auto menuVSL = MenuVSL::Instance;

    auto windowHandle = MenuVSL::AddCleoWindow();
    auto window = menuVSL->m_CleoWindows[windowHandle];
    window->m_Title = param1;
    window->m_Position = CVector2D((float)x, (float)y);

    menuVSL->debug->AddLine("Added window " + std::to_string(windowHandle));

    auto result = __getPointerToScriptVar(handle);

    result->i = windowHandle;
    
    Log::Level(LOG_LEVEL::LOG_BOTH) << "end" << std::endl;
}

static void MENU_VSL_TEST(__handler_params)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "MENU_VSL_TEST" << std::endl;

    int dt = __readParam(handle)->i;

    Log::Level(LOG_LEVEL::LOG_BOTH) << "dt=" << dt << std::endl;

    char param1[256];
    cleo->ReadStringLong(handle, param1, sizeof(param1));
    param1[sizeof(param1)-1] = 0; // I can't trust game scripting engine...
    int i = 0;
    while(param1[i] != 0) // A little hack
    {
        if(param1[i] == '\\') param1[i] = '/';
        ++i;
    }

    Log::Level(LOG_LEVEL::LOG_BOTH) << "param1=" << param1 << std::endl;

    auto menuVSL = MenuVSL::Instance;

    auto window = menuVSL->AddWindow();
    window->m_Title = param1;
    window->m_Position = CVector2D(200, 200);

    auto text1 = window->AddText("My text item outside", CRGBA(255, 255, 255));
    text1->m_FitInWindow = false;
    text1->m_Position = CVector2D(0, 500);

    auto text2 = window->AddText("My text item 2", CRGBA(255, 255, 255));
    auto text3 = window->AddText("My ~y~text item ~w~3 with ~r~colors~w~!", CRGBA(255, 255, 255));

    auto intrange1 = window->AddIntRange("Int range", &int1, 0, 100, 1);

    auto floatrange1 = window->AddFloatRange("Float range", &float1, 0.0f, 100.0f, 0.1f);
    floatrange1->m_HoldToChange = true;

    auto closebutton = window->AddButton("~r~Close", CRGBA(0, 0, 0, 0));
    closebutton->onClick = [window]() {
        window->SetToBeRemoved();
    };

    auto checkbox1 = window->AddCheckbox("Checkbox", &bool1);
    checkbox1->onClick = [window]() {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "onClick" << std::endl;

        window->AddText("Added text ~r~is this red?", CRGBA(255, 255, 0));
    };

    auto result = __getPointerToScriptVar(handle);

    result->i = 123;

    Log::Level(LOG_LEVEL::LOG_BOTH) << "end" << std::endl;
}

static void MENU_VSL_ADD_TEXT(__handler_params)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "MENU_VSL_ADD_TEXT" << std::endl;

    int windowHandle = __readParam(handle)->i;

    Log::Level(LOG_LEVEL::LOG_BOTH) << "windowHandle=" << windowHandle << std::endl;

    char param1[256];
    cleo->ReadStringLong(handle, param1, sizeof(param1));
    param1[sizeof(param1)-1] = 0; // I can't trust game scripting engine...
    int i = 0;
    while(param1[i] != 0) // A little hack
    {
        if(param1[i] == '\\') param1[i] = '/';
        ++i;
    }

    Log::Level(LOG_LEVEL::LOG_BOTH) << "param1=" << param1 << std::endl;

    //auto menuVSL = MenuVSL::Instance;
    auto window = MenuVSL::m_CleoWindows[windowHandle];

    auto item = window->AddText(param1, CRGBA(255, 255, 255));
    auto itemHandle = MenuVSL::AddCleoItem((Item*)item);

    auto result = __getPointerToScriptVar(handle);

    result->i = itemHandle;
    
    Log::Level(LOG_LEVEL::LOG_BOTH) << "end" << std::endl;
}

static void MENU_VSL_ADD_BUTTON(__handler_params)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "MENU_VSL_ADD_BUTTON" << std::endl;

    int windowHandle = __readParam(handle)->i;

    Log::Level(LOG_LEVEL::LOG_BOTH) << "windowHandle=" << windowHandle << std::endl;

    char param1[256];
    cleo->ReadStringLong(handle, param1, sizeof(param1));
    param1[sizeof(param1)-1] = 0; // I can't trust game scripting engine...
    int i = 0;
    while(param1[i] != 0) // A little hack
    {
        if(param1[i] == '\\') param1[i] = '/';
        ++i;
    }

    Log::Level(LOG_LEVEL::LOG_BOTH) << "param1=" << param1 << std::endl;

    //auto menuVSL = MenuVSL::Instance;
    auto window = MenuVSL::m_CleoWindows[windowHandle];

    auto item = window->AddButton(param1, CRGBA(255, 255, 255));
    auto itemHandle = MenuVSL::AddCleoItem((Item*)item);

    auto result = __getPointerToScriptVar(handle);

    result->i = itemHandle;
    
    Log::Level(LOG_LEVEL::LOG_BOTH) << "end" << std::endl;
}

static void MENU_VSL_IS_BUTTON_JUST_PRESSED(__handler_params)
{
    //Log::Level(LOG_LEVEL::LOG_BOTH) << "MENU_VSL_IS_BUTTON_JUST_PRESSED" << std::endl;

    int itemHandle = __readParam(handle)->i;

    auto item = MenuVSL::m_CleoItems[itemHandle];

    UpdateCompareFlag(handle, item->hasPressedThisFrame);
}

static void MENU_VSL_REMOVE_WINDOW(__handler_params)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "MENU_VSL_REMOVE_WINDOW" << std::endl;

    int windowHandle = __readParam(handle)->i;

    menuVSL->debug->AddLine("Removing window " + std::to_string(windowHandle));

    auto window = MenuVSL::m_CleoWindows[windowHandle];
    window->SetToBeRemoved();
}

static void MENU_VSL_ADD_INT_RANGE(__handler_params)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "MENU_VSL_ADD_INT_RANGE" << std::endl;

    int windowHandle = __readParam(handle)->i;

    char param1[256];
    cleo->ReadStringLong(handle, param1, sizeof(param1));
    param1[sizeof(param1)-1] = 0; // I can't trust game scripting engine...
    int i = 0;
    while(param1[i] != 0) // A little hack
    {
        if(param1[i] == '\\') param1[i] = '/';
        ++i;
    }

    int value = __readParam(handle)->i;
    int min = __readParam(handle)->i;
    int max = __readParam(handle)->i;
    int addBy = __readParam(handle)->i;
    auto result = __getPointerToScriptVar(handle);

    auto window = MenuVSL::m_CleoWindows[windowHandle];

    auto item = window->AddIntRange(param1, NULL, min, max, addBy);
    auto itemHandle = MenuVSL::AddCleoItem((Item*)item);

    auto itemc = (Item*)item;
    itemc->tmpInt = value;

    result->i = itemHandle;
}

static void MENU_VSL_ADD_FLOAT_RANGE(__handler_params)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "MENU_VSL_ADD_FLOAT_RANGE" << std::endl;

    int windowHandle = __readParam(handle)->i;

    char param1[256];
    cleo->ReadStringLong(handle, param1, sizeof(param1));
    param1[sizeof(param1)-1] = 0; // I can't trust game scripting engine...
    int i = 0;
    while(param1[i] != 0) // A little hack
    {
        if(param1[i] == '\\') param1[i] = '/';
        ++i;
    }

    float value = __readParam(handle)->f;
    float min = __readParam(handle)->f;
    float max = __readParam(handle)->f;
    float addBy = __readParam(handle)->f;
    auto result = __getPointerToScriptVar(handle);

    auto window = MenuVSL::m_CleoWindows[windowHandle];

    auto item = window->AddFloatRange(param1, NULL, min, max, addBy);
    auto itemHandle = MenuVSL::AddCleoItem((Item*)item);

    auto itemc = (Item*)item;
    itemc->tmpFloat = value;

    result->i = itemHandle;
}

static void MENU_VSL_GET_INT_VALUE(__handler_params)
{
    //Log::Level(LOG_LEVEL::LOG_BOTH) << "MENU_VSL_GET_INT_VALUE" << std::endl;
    
    int itemHandle = __readParam(handle)->i;
    auto result = __getPointerToScriptVar(handle);

    auto item = (Item*)MenuVSL::m_CleoItems[itemHandle];

    if(item->m_Type == eItemType::ITEM_CHECKBOX)
    {
        result->i = *item->pCheckBoxBool ? 1 : 0;
        return;
    }

    result->i = *item->m_IntValueRange.value;
}

static void MENU_VSL_GET_FLOAT_VALUE(__handler_params)
{
    //Log::Level(LOG_LEVEL::LOG_BOTH) << "MENU_VSL_GET_FLOAT_VALUE" << std::endl;

    int itemHandle = __readParam(handle)->i;
    auto result = __getPointerToScriptVar(handle);

    auto item = (Item*)MenuVSL::m_CleoItems[itemHandle];

    result->f = *item->m_FloatValueRange.value;
}

static void MENU_VSL_SET_HOLD_TO_CHANGE(__handler_params)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "MENU_VSL_SET_HOLD_TO_CHANGE" << std::endl;

    int itemHandle = __readParam(handle)->i;
    bool value = __readParam(handle)->i > 0;

    auto item = (Item*)MenuVSL::m_CleoItems[itemHandle];
    item->m_HoldToChange = value;
}

static void MENU_VSL_SET_DEBUG_VISIBLE(__handler_params)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "MENU_VSL_SET_DEBUG_VISIBLE" << std::endl;

    bool value = __readParam(handle)->i > 0;

    Log::Level(LOG_LEVEL::LOG_BOTH) << "value: " << value << std::endl;

    MenuVSL::Instance->debug->visible = value;
}

static void MENU_VSL_ADD_DEBUG_INT(__handler_params)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "MENU_VSL_ADD_DEBUG_INT" << std::endl;

    char param1[256];
    cleo->ReadStringLong(handle, param1, sizeof(param1));
    param1[sizeof(param1)-1] = 0; // I can't trust game scripting engine...
    int i = 0;
    while(param1[i] != 0) // A little hack
    {
        if(param1[i] == '\\') param1[i] = '/';
        ++i;
    }

    int int1 = __readParam(handle)->i;

    std::string text = param1;
    replaceSubstring(text, "~1~", std::to_string(int1));

    MenuVSL::Instance->debug->AddLine(text);
}

static void MENU_VSL_ADD_DEBUG_FLOAT(__handler_params)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "MENU_VSL_ADD_DEBUG_FLOAT" << std::endl;

    char param1[256];
    cleo->ReadStringLong(handle, param1, sizeof(param1));
    param1[sizeof(param1)-1] = 0; // I can't trust game scripting engine...
    int i = 0;
    while(param1[i] != 0) // A little hack
    {
        if(param1[i] == '\\') param1[i] = '/';
        ++i;
    }

    float float1 = __readParam(handle)->f;

    std::string text = param1;
    replaceSubstring(text, "~1~", std::to_string(float1));

    MenuVSL::Instance->debug->AddLine(text);
}

static void MENU_VSL_ADD_CHECKBOX(__handler_params)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "MENU_VSL_ADD_CHECKBOX" << std::endl;

    int windowHandle = __readParam(handle)->i;

    char param1[256];
    cleo->ReadStringLong(handle, param1, sizeof(param1));
    param1[sizeof(param1)-1] = 0; // I can't trust game scripting engine...
    int i = 0;
    while(param1[i] != 0) // A little hack
    {
        if(param1[i] == '\\') param1[i] = '/';
        ++i;
    }

    bool value = __readParam(handle)->i > 0;
    auto result = __getPointerToScriptVar(handle);

    auto window = MenuVSL::m_CleoWindows[windowHandle];

    auto item = window->AddCheckbox(param1, NULL);
    auto itemHandle = MenuVSL::AddCleoItem((Item*)item);

    auto itemc = (Item*)item;
    itemc->tmpBool = value;

    result->i = itemHandle;
}

static void MENU_VSL_SET_WINDOW_BACKGROUND(__handler_params)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "MENU_VSL_SET_WINDOW_BACKGROUND" << std::endl;

    int windowHandle = __readParam(handle)->i;
    int r = __readParam(handle)->i;
    int g = __readParam(handle)->i;
    int b = __readParam(handle)->i;
    int a = __readParam(handle)->i;

    auto window = MenuVSL::m_CleoWindows[windowHandle];
    window->m_TitleBackgroundColor = CRGBA(r, g, b, a);
}

static void SET_GLOBAL_INT_VARIABLE(__handler_params)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "SET_GLOBAL_INT_VARIABLE" << std::endl;

    char param1[256];
    cleo->ReadStringLong(handle, param1, sizeof(param1));
    param1[sizeof(param1)-1] = 0; // I can't trust game scripting engine...
    int i = 0;
    while(param1[i] != 0) // A little hack
    {
        if(param1[i] == '\\') param1[i] = '/';
        ++i;
    }

    int value = __readParam(handle)->i;

    MenuVSL::Instance->SetGlobalIntVariable(param1, value);
}

static void GET_GLOBAL_INT_VARIABLE(__handler_params)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "SET_GLOBAL_INT_VARIABLE" << std::endl;

    char param1[256];
    cleo->ReadStringLong(handle, param1, sizeof(param1));
    param1[sizeof(param1)-1] = 0; // I can't trust game scripting engine...
    int i = 0;
    while(param1[i] != 0) // A little hack
    {
        if(param1[i] == '\\') param1[i] = '/';
        ++i;
    }

    auto result = __getPointerToScriptVar(handle);

    result->i = MenuVSL::Instance->GetGlobalIntVariable(param1);
}