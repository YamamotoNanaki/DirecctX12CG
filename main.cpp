#include <Windows.h>

//ウィンドウプロシージャ
LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{

	//--- ウインドウの生成処理 -----------------------------------------------------//

// コンソールヘの文字出力
//OutputDebugStringA("Hello,DirectX!!\n");

// ウィンドウの生成（前処理）
#pragma region ウィンドウの生成（前処理）

	// ウィンドウサイズ
	const int window_width = 1280;  // 横幅
	const int window_height = 720;  // 縦幅

	WNDCLASSEX w{}; // ウィンドウクラスの設定
	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProc; // ウィンドウプロシージャを設定
	w.lpszClassName = L"DirectX12Game"; // ウィンドウクラス名
	w.hInstance = GetModuleHandle(nullptr); // ウィンドウハンドル
	w.hCursor = LoadCursor(NULL, IDC_ARROW); // カーソル指定

	// ウィンドウクラスをOSに登録
	RegisterClassEx(&w);
	// ウィンドウサイズ{ X座標 Y座標 横幅 縦幅 }
	RECT wrc = { 0, 0, window_width, window_height };
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false); // 自動でサイズ補正

	// ウィンドウオブジェクトの生成
	HWND hwnd = CreateWindow(w.lpszClassName, // クラス名
		L"DirectXGame",         // タイトルバーの文字
		WS_OVERLAPPEDWINDOW,        // 標準的なウィンドウスタイル
		CW_USEDEFAULT,              // 表示X座標（OSに任せる）
		CW_USEDEFAULT,              // 表示Y座標（OSに任せる）
		wrc.right - wrc.left,       // ウィンドウ横幅
		wrc.bottom - wrc.top,   // ウィンドウ縦幅
		nullptr,                // 親ウィンドウハンドル
		nullptr,                // メニューハンドル
		w.hInstance,            // 呼び出しアプリケーションハンドル
		nullptr);               // オプション

	// ウィンドウ表示
	ShowWindow(hwnd, SW_SHOW);

#pragma endregion ウィンドウの生成（前処理）

	//------------------------------------------------------------------------------//

	MSG msg{}; // メッセージ

#pragma region DirectX初期化処理
#pragma endregion DirectX初期化処理



	while (true)
	{
#pragma region メッセージ
		// メッセージがある？
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg); // キー入力メッセージの処理
			DispatchMessage(&msg);  //プロシージャにメッセージを送る
		}

		//Xボタンで終了メッセージが来たらゲームループを抜ける
		if (msg.message == WM_QUIT)
		{
			break;
		}

#pragma endregion メッセージ

#pragma region DirectX毎フレームの処理
#pragma endregion DirectX毎フレームの処理


	}

	UnregisterClass(w.lpszClassName, w.hInstance);

	return 0;
}

//ウィンドウプロシージャ
LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
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