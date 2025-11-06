#pragma once
#include "Window.h"
#include "ChiliTimer.h"
#include <string>
#include "ImguiManager.h"
#include "Camera.h"
#include "PointLight.h"
#include "Drawable/Mesh.h"

class App
{
public:
    App();
    // master frame / message loop
    int Go();
    ~App();
private:
    void DoFrame();
    void ShowRawInputWindow();
private:
    int x = 0;
    int y = 0;
    ImguiManager imgui;
    ChiliTimer timer;
    Window wnd;
    std::vector<std::unique_ptr<class Drawable>> drawables;
    static constexpr size_t nDrawables = 180;
    float speed_factor = 1.0f;
    Camera cam;
    bool show_demo_window = false;
    PointLight light;
    Model nano{ wnd.Gfx(),"Models\\nano.gltf" };

    
};
