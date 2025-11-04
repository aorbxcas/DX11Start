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
    void ShowModelWindow();
private:
    ImguiManager imgui;
    ChiliTimer timer;
    Window wnd;
    std::vector<std::unique_ptr<class Drawable>> drawables;
    static constexpr size_t nDrawables = 180;
    float speed_factor = 1.0f;
    Camera cam;
    bool show_demo_window = false;
    PointLight light;
    Model nano{ wnd.Gfx(),"Models\\nanosuit.obj" };
    struct
    {
        float roll = 0.0f;
        float pitch = 0.0f;
        float yaw = 0.0f;
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
    } pos;
    
};
