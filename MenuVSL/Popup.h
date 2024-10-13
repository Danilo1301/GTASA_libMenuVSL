#pragma once

#include "menu/IPopup.h"

class Popup : public IPopup {
public:
    int m_TimeElapsed = 0;
    int m_Time = 0;
    int m_FadeTime = 1000;

    Popup();
    ~Popup();

    void Update(int dt);
    void Draw();

    bool HasCompleted();
};