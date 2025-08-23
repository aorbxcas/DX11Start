

#include "App.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(69);
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;

		case 'F':
			SetWindowText(hWnd, "FR");
			break;
		default:
			break;
		}
		break;
	case WM_CHAR: {
		static std::string title = "";
		title.push_back((char)wParam);
		SetWindowText(hWnd, title.c_str());
		break;
	}

	case WM_LBUTTONDOWN: {
		const POINTS pt = MAKEPOINTS(lParam);
		SetWindowText(hWnd, std::to_string(pt.x).c_str());
		break;
	}


	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//const auto pClassName = "zlWind32";
	//WNDCLASSEX wc = { 0 };
	//wc.cbSize = sizeof(wc);
	//wc.style = CS_OWNDC;
	//wc.lpfnWndProc = WndProc;
	//wc.cbClsExtra = 0;
	//wc.cbWndExtra = 0;
	//wc.hInstance = hInstance;

	//wc.hIcon = nullptr;

 //   wc.hCursor = nullptr;
 //   wc.hbrBackground = nullptr;
	//wc.hIconSm = nullptr;
	//wc.lpszClassName = pClassName;
	//RegisterClassEx(&wc);
	//HWND hWnd = CreateWindowEx(
	//	0, pClassName, "zlWind32", WS_OVERLAPPEDWINDOW, 200, 200, 640, 480, nullptr, nullptr, hInstance, nullptr);
	//ShowWindow(hWnd, SW_SHOW);

	//MSG msg;
	//BOOL bRet;
 //   while (bRet = GetMessage(&msg, nullptr, 0, 0)>0)
 //   {
 //       TranslateMessage(&msg);
 //       DispatchMessage(&msg);
 //   }
	//if (bRet == -1) {
 //       return -1;
	//}
	//else {
 //       return msg.wParam;
	//}
	//return 0;



	// Window wnd(800, 300, "Donkey Fart Box");
	// MSG msg;
	// BOOL gResult;
	// while ((gResult = GetMessage(&msg, nullptr, 0, 0)) > 0)
	// {
	// 	// TranslateMessage will post auxilliary WM_CHAR messages from key msgs
	// 	TranslateMessage(&msg);
	// 	DispatchMessage(&msg);
	// 	if (wnd.kbd.KeyIsPressed('F'))
	// 	{
	// 		MessageBoxA(nullptr,"f","s",MB_OK | MB_ICONEXCLAMATION);
	// 	}
	// }
	//
	// // check if GetMessage call itself borked
	// if (gResult == -1)
	// {
	// 	return -1;
	// }
	//
	// // wParam here is the value passed to PostQuitMessage
	// return msg.wParam;

	try
	{
		return App{}.Go();
	}
	catch( const ChiliException& e )
	{
		MessageBox( nullptr,e.what(),e.GetType(),MB_OK | MB_ICONEXCLAMATION );
	}
	catch( const std::exception& e )
	{
		MessageBox( nullptr,e.what(),"Standard Exception",MB_OK | MB_ICONEXCLAMATION );
	}
	catch( ... )
	{
		MessageBox( nullptr,"No details available","Unknown Exception",MB_OK | MB_ICONEXCLAMATION );
	}
	return -1;
}

