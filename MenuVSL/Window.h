#pragma once

#include <iostream>
#include <vector>

#include "menu/IWindow.h"
//#include "Item.h"

class Window : public IWindow {
public:
    bool m_CanBeRemoved = false;

    IItem* m_LeftButton = NULL;
    IItem* m_RightButton = NULL;
    IItem* m_BackButton = NULL;
    
    Window();
    ~Window();

    /*1.0.0*/
    void SetPosition(CVector2D position);
    IItem* AddText(std::string text, CRGBA color);
    IItem* AddButton(std::string text, CRGBA color);
    IItem* AddButton(std::string text);
    IItem* AddCheckbox(std::string text, bool* value);
    IItem* AddIntRange(std::string text, int* value, int min, int max, int addBy);
    IItem* AddFloatRange(std::string text, float* value, float min, float max, float addBy);
    void SetToBeRemoved();

    /*1.1.0*/
    bool GetIsActive();
    IItem* AddOptions(std::string text);

private:
    void AddItem(IItem* item);

public:
    void Update();
    void Draw();

    std::vector<IItem*> GetItemsInPage();
    std::vector<IItem*> GetFloatingItems();
    int GetMaxPages();
};