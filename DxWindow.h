#pragma once
#include <Windows.h>

class DxWindow
{
public:
	WNDCLASSEX w{}; // ウィンドウクラスの設定
	HWND hwnd;
private:
	MSG msg{}; // メッセージ

public:
	DxWindow(int window_width, int window_height);
	~DxWindow();
	bool Message();
	
	//ウィンドウプロシージャ
	static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		// メッセージで分岐
		switch (msg)
		{
		case WM_DESTROY: //ウィンドーが破壊された
			PostQuitMessage(0); // OSに対して、アプリの終了を伝える
			return 0;
		}
		return DefWindowProc(hwnd, msg, wparam, lparam); // 標準の処理を行う
	}
};