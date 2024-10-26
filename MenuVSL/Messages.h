#pragma once

#include "pch.h"

class Messages {
public:
    static std::string m_Message;
    static int m_TimeLeft;

private:
    static void SetMessage(std::string message);

public:
    static void ShowMessage(std::string message, int time);

    static void Draw();
    static void Update(int dt);
};