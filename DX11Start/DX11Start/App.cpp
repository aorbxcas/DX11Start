#include "App.h"

#include <sstream>
#include <iomanip>

App::App()
    :
    wnd( 800,600,"The Donkey Fart Box" )
{}

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

void App::DoFrame()
{
    const float time = timer.Peek();
    std::ostringstream oss;
    oss << "Time elapsed: " << std::setprecision( 1 ) << std::fixed << time << "s";
    wnd.SetTitle( oss.str() );
}