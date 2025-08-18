#pragma once
#include "Window.h"
#include "ChiliTimer.h"
#include <string>

class App
{
public:
    App();
    // master frame / message loop
    int Go();
private:
    void DoFrame();
private:
    ChiliTimer timer;
    Window wnd;
};