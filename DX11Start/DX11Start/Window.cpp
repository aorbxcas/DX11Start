#include "Window.h"


// Window Class Stuff
Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass() noexcept
	:
	hInst( GetModuleHandle( nullptr ) )
{
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof( wc );
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMsgSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetInstance();
	wc.hIcon = nullptr;
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = GetName();
	wc.hIconSm = nullptr;
	RegisterClassEx( &wc );
}

Window::WindowClass::~WindowClass()
{
	UnregisterClass( wndClassName,GetInstance() );
}

const char* Window::WindowClass::GetName() noexcept
{
	return wndClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
	return wndClass.hInst;
}


// Window Stuff
Window::Window( int width,int height,const char* name ) noexcept
{
	// calculate window size based on desired client region size
	RECT wr;
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = height + wr.top;
	AdjustWindowRect( &wr,WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,FALSE );
	// create window & get hWnd
	hWnd = CreateWindow(
		WindowClass::GetName(),name,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT,CW_USEDEFAULT,wr.right - wr.left,wr.bottom - wr.top,
		nullptr,nullptr,WindowClass::GetInstance(),this
	);
	// show window
	ShowWindow( hWnd,SW_SHOWDEFAULT );
	pGfx = std::make_unique<Graphics>( hWnd );
}

Window::~Window()
{
	DestroyWindow( hWnd );
}

void Window::SetTitle( const std::string& title )
{
	if( SetWindowText( hWnd,title.c_str() ) == 0 )
	{
		return;
	}
}

LRESULT CALLBACK Window::HandleMsgSetup( HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam ) noexcept
{
	// use create parameter passed in from CreateWindow() to store window class pointer at WinAPI side
	if( msg == WM_NCCREATE )
	{
		// extract ptr to window class from creation data
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
		// set WinAPI-managed user data to store ptr to window class
		SetWindowLongPtr( hWnd,GWLP_USERDATA,reinterpret_cast<LONG_PTR>(pWnd) );
		// set message proc to normal (non-setup) handler now that setup is finished
		SetWindowLongPtr( hWnd,GWLP_WNDPROC,reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk) );
		// forward message to window class handler
		return pWnd->HandleMsg( hWnd,msg,wParam,lParam );
	}
	// if we get a message before the WM_NCCREATE message, handle with default handler
	return DefWindowProc( hWnd,msg,wParam,lParam );
}

LRESULT CALLBACK Window::HandleMsgThunk( HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam ) noexcept
{
	// retrieve ptr to window class
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr( hWnd,GWLP_USERDATA ));
	// forward message to window class handler
	return pWnd->HandleMsg( hWnd,msg,wParam,lParam );
}
std::optional<int> Window::ProcessMessages()
{
	MSG msg;
	// while queue has messages, remove and dispatch them (but do not block on empty queue)
	while( PeekMessage( &msg,nullptr,0,0,PM_REMOVE ) )
	{
		// check for quit because peekmessage does not signal this via return val
		if( msg.message == WM_QUIT )
		{
			// return optional wrapping int (arg to PostQuitMessage is in wparam) signals quit
			return msg.wParam;
		}
		// TranslateMessage will post auxilliary WM_CHAR messages from key msgs
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}

	// return empty optional when not quitting app
	return {};
}

Graphics& Window::Gfx()
{
	return *pGfx;
}

LRESULT Window::HandleMsg( HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam ) noexcept
{
	switch( msg )
	{
	// we don't want the DefProc to handle this message because
	// we want our destructor to destroy the window, so return 0 instead of break
	// 在这里发送消息
	case WM_CLOSE:
		PostQuitMessage( 0 );
		return 0;
	case WM_KEYDOWN:
		if (!(lParam & 0x40000000)||kbd.AutorepeatIsEnabled())
		{
			kbd.OnKeyPressed( static_cast<unsigned char>(wParam) );
		}
		break;
	case WM_KEYUP:
		kbd.OnKeyReleased( static_cast<unsigned char>(wParam) );
		break;
	case WM_CHAR:
		kbd.OnChar( static_cast<char>(wParam) );
		break;
	case WM_SYSKEYDOWN:
		kbd.OnKeyPressed( static_cast<unsigned char>(wParam) );
		break;
	case WM_KILLFOCUS:
		kbd.ClearState();
		break;	
	case WM_MOUSEMOVE:
		{
			const POINTS pt = MAKEPOINTS( lParam );
			// in client region -> log move, and log enter + capture mouse (if not previously in window)
			if( pt.x >= 0 && pt.x < width && pt.y >= 0 && pt.y < height )
			{
				mouse.OnMouseMove( pt.x,pt.y );
				if( !mouse.IsInWindow() )
				{
					SetCapture( hWnd );
					mouse.OnMouseEnter();
				}
			}
			// not in client -> log move / maintain capture if button down
			else
			{
				if( wParam & (MK_LBUTTON | MK_RBUTTON) )
				{
					mouse.OnMouseMove( pt.x,pt.y );
				}
				// button up -> release capture / log event for leaving
				else
				{
					ReleaseCapture();
					mouse.OnMouseLeave();
				}
			}
			break;
		}
	case WM_LBUTTONDOWN:
		{
			const POINTS pt = MAKEPOINTS( lParam );
			mouse.OnLeftPressed( pt.x,pt.y );
			break;
		}
	case WM_RBUTTONDOWN:
		{
			const POINTS pt = MAKEPOINTS( lParam );
			mouse.OnRightPressed( pt.x,pt.y );
			break;
		}
	case WM_LBUTTONUP:
		{
			const POINTS pt = MAKEPOINTS( lParam );
			mouse.OnLeftReleased( pt.x,pt.y );
			// release mouse if outside of window
			if( pt.x < 0 || pt.x >= width || pt.y < 0 || pt.y >= height )
			{
				ReleaseCapture();
				mouse.OnMouseLeave();
			}
			break;
		}
	case WM_RBUTTONUP:
		{
			const POINTS pt = MAKEPOINTS( lParam );
			mouse.OnRightReleased( pt.x,pt.y );
			// release mouse if outside of window
			if( pt.x < 0 || pt.x >= width || pt.y < 0 || pt.y >= height )
			{
				ReleaseCapture();
				mouse.OnMouseLeave();
			}
			break;
		}
	case WM_MOUSEWHEEL:
		{
			const POINTS pt = MAKEPOINTS( lParam );
			const int delta = GET_WHEEL_DELTA_WPARAM( wParam );
			mouse.OnWheelDelta( pt.x,pt.y,delta );
			break;
		}
	}

	return DefWindowProc( hWnd,msg,wParam,lParam );
}
