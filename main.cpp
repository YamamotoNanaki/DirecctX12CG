#include "Window.h"
#include "DirectX12.h"
#include "Input.h"
#include "Scene.h"
#include "FPS.h"
#include "Sound.h"
#ifdef _DEBUG
#include "Debug.h"
#endif // _DEBUG

using namespace IF;

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {
	// ウィンドウサイズ
	const int winWidth = 1280;  // 横幅
	const int winHeight = 720;  // 縦幅

	Window* win = Window::Instance();
	win->Initialize(winWidth, winHeight);
#ifdef _DEBUG
	Debug();
#endif // _DEBUG

	DirectX12* dx12 = DirectX12::Instance();
	dx12->Initialize(win->hwnd, winWidth, winHeight);
	Input::getInstance()->Initialize(win->w.hInstance, win->hwnd);
	LightManager::GetInstance()->SetDevice(dx12->device.Get());
	Sound::Instance()->Initialize();
	IScene* scene = new Scene(winWidth, winHeight, dx12->device.Get(), dx12->commandList.Get(), dx12->viewport);
	dx12->SetClearColor(0, 0, 0);
	scene->Initialize();
	FPS fps;
	fps.Initialize(60);

	while (!Input::getInstance()->KDown(KEY::ESC))
	{
		//メッセージ
		if (win->Message())break;

		scene->Update();

		dx12->DrawBefore();
		scene->Draw();
		dx12->DrawAfter();
		fps.FPSFixed();
	}
	delete scene;
	win->Unregister();
	return 0;
}