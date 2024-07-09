#pragma once

#include <iostream>
#include <vector>

#include "menu/IWindow.h"

class Window : public IWindow {
public:
    bool m_CanBeRemoved = false;
    
    ~Window();

    void SetPosition(CVector2D position);
    IItem* AddText(std::string text, CRGBA color);
    IItem* AddButton(std::string text, CRGBA color);
    IItem* AddCheckbox(std::string text, bool* value);
    IItem* AddIntRange(std::string text, int* value, int min, int max, int addBy);
    IItem* AddFloatRange(std::string text, float* value, float min, float max, float addBy);
    void SetToBeRemoved();

private:
    void AddItem(IItem* item);

public:
    void Update();
    void Draw();
};