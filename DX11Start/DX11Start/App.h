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
    ~App();
private:
    void DoFrame();
private:
    ChiliTimer timer;
    Window wnd;
    std::vector<std::unique_ptr<class Box>> boxes;
};