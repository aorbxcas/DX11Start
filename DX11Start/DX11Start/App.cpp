#include "App.h"

#include <sstream>
#include <iomanip>

#include "Drawable/Box.h"
#include <memory>
#include "Drawable/Pyramid.h"
#include "Drawable/Sheet.h"
#include "Drawable/SkinnedBox.h"
#include "Tools/ChiliMath.h"
#include "Drawable/Cylinder.h"

#include "Drawable/Surface.h"
#include "Tools/GDIPlusManager.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "VertexLayout.h"

#include "AssTest.h"

namespace dx = DirectX;


GDIPlusManager gdipm;
App::App()
    :
    wnd( 800,600,"The Donkey Fart Box" ),
    light( wnd.Gfx() )
{
    // 自行安装assimp.lib并链接
    Assimp::Importer imp;
    auto model = imp.ReadFile( "models\\suzanne.obj",
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices
    );

    class Factory
    {
    public:
        Factory( Graphics& gfx )
            :
            gfx( gfx )
        {}
        std::unique_ptr<Drawable> operator()()
        {
            const DirectX::XMFLOAT3 mat = { cdist( rng ),cdist( rng ),cdist( rng ) };
            switch( sdist( rng ) )
            {
            case 0:
                return std::make_unique<Box>(
                    gfx,rng,adist,ddist,
                    odist,rdist,bdist,mat
                );
            case 1:
                return std::make_unique<Cylinder>(
                    gfx,rng,adist,ddist,odist,
                    rdist,bdist,tdist
                );
            case 2:
                return std::make_unique<AssTest>(
                    gfx,rng,adist,ddist,
                    odist,rdist,mat,1.5f
                );
            // case 0:
            //     return std::make_unique<Pyramid>(
            //         gfx,rng,adist,ddist,
            //         odist,rdist
            //     );
            // case 1:
            //     return std::make_unique<Box>(
            //         gfx,rng,adist,ddist,
            //         odist,rdist,bdist
            //     );
            // case 2:
            //     // return std::make_unique<Melon>(
            //     //     gfx,rng,adist,ddist,
            //     //     odist,rdist,longdist,latdist
            //     // );
            //     return std::make_unique<SkinnedBox>(
            //         gfx,rng,adist,ddist,
            //         odist,rdist
            //     );
            default:
                //assert( false && "bad drawable type in factory" );
                return std::make_unique<Box>(
                    gfx,rng,adist,ddist,
                    odist,rdist,bdist,
                    mat
                );
                return {};
            }
        }
    private:
        Graphics& gfx;
        std::mt19937 rng{ std::random_device{}() };
        std::uniform_int_distribution<int> sdist{ 0,2 };
        std::uniform_real_distribution<float> adist{ 0.0f,PI * 2.0f };
        std::uniform_real_distribution<float> ddist{ 0.0f,PI * 0.5f };
        std::uniform_real_distribution<float> odist{ 0.0f,PI * 0.08f };
        std::uniform_real_distribution<float> rdist{ 6.0f,20.0f };
        std::uniform_real_distribution<float> bdist{ 0.4f,3.0f };
        std::uniform_int_distribution<int> latdist{ 5,20 };
        std::uniform_int_distribution<int> longdist{ 10,40 };
        std::uniform_int_distribution<int> typedist{ 0,2 };
        std::uniform_real_distribution<float> cdist{ 0.0f,1.0f };
        std::uniform_int_distribution<int> tdist{ 3,30 };
    };

    Factory f( wnd.Gfx() );
    drawables.reserve( nDrawables );
    std::generate_n( std::back_inserter( drawables ),nDrawables,f );
    
    wnd.Gfx().SetProjection( dx::XMMatrixPerspectiveLH( 1.0f,3.0f / 4.0f,0.5f,40.0f ) );
    //wnd.Gfx().SetCamera( dx::XMMatrixTranslation( 0.0f,0.0f,20.0f ) );
    
}

int App::Go()
{
    while( true )
    {
        // process all messages pending, but to not block for new messages
        if( const auto ecode = Window::ProcessMessages() )
        {
            // if return optional has value, means we're quitting so return exit code
            return *ecode;
        }
        if (wnd.kbd.KeyIsPressed('F'))
        {
            OutputDebugStringA("F key pressed\n");
        }
        DoFrame();
    }
}

App::~App()
{
}

void App::DoFrame()
{
    // const float time = timer.Peek();
    // std::ostringstream oss;
    // oss << "Time elapsed: " << std::setprecision( 1 ) << std::fixed << time << "s";
    // wnd.SetTitle( oss.str() );
    // const float c = sin( timer.Peek() ) / 2.0f + 0.5f;
    // wnd.Gfx().ClearBuffer( c,c,1.0f );
    // wnd.Gfx().DrawTriangle(timer.Peek());
    // wnd.Gfx().EndFrame();
    auto dt = timer.Mark() * speed_factor;
    static char buffer[1024];
    wnd.Gfx().SetCamera( cam.GetMatrix() );
    light.Bind( wnd.Gfx() ,cam.GetMatrix());
    //wnd.Gfx().ClearBuffer( 0.07f,0.0f,0.12f );
    
    if( wnd.kbd.KeyIsPressed( VK_SPACE ) )
    {
        wnd.Gfx().DisableImgui();
    }
    else
    {
        wnd.Gfx().EnableImgui();
    }
    wnd.Gfx().BeginFrame( 0.07f,0.0f,0.12f );
    
    for( auto& b : drawables )
    {
        b->Update( dt );
        b->Draw( wnd.Gfx() );
    }
    light.Draw(wnd.Gfx());
    if (wnd.Gfx().IsImguiEnabled() )
    {
        if( show_demo_window )
        {
            ImGui::ShowDemoWindow( &show_demo_window );
        }else
        {
            if( ImGui::Begin( "Simulation Speed" ) )
            {
		        ImGui::SliderFloat( "Speed Factor",&speed_factor,0.0f,6.0f,"%.4f",3.2f );                // ImGui::Text( "Application average %.3f ms/frame (%.1f FPS)",1000.0f / ImGui::GetIO().Framerate,ImGui::GetIO().Framerate );
                // ImGui::InputText( "Butts",buffer,sizeof( buffer ) );
                ImGui::Text( "%.3f ms/frame (%.1f FPS)",1000.0f / ImGui::GetIO().Framerate,ImGui::GetIO().Framerate );
                ImGui::Text( "Status: %s",wnd.kbd.KeyIsPressed( VK_SPACE ) ? "PAUSED" : "RUNNING" );
            }
            ImGui::End();
            // imgui window to control camera
            cam.SpawnControlWindow();
            light.SpawnControlWindow();
        }
    }

    // ImGui::Render();
    // ImGui_ImplDX11_RenderDrawData( ImGui::GetDrawData() );
    wnd.Gfx().EndFrame();
}
