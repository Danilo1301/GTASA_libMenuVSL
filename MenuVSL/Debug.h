#pragma once

#include <iostream>
#include <vector>

#include "menu/IDebug.h"

class Debug : public IDebug {
public:
    void AddLine(std::string text);
    void Clear();

    void Draw();
};