#include <DirectXMath.h>
#include "Window.h"
#include "DirectX12.h"
#include "Key.h"
#include "Scene.h"
#include "Util.h"

using namespace DirectX;
using namespace IF;

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine,
	_In_ int nCmdShow) {

	// ウィンドウサイズ
	const int winWidth = 1280;  // 横幅
	const int winHeight = 720;  // 縦幅

	Window* win = new Window(winWidth, winHeight);

	Debug();

	HRESULT result;
	DirectX12* dx12 = new DirectX12(result, win->hwnd, winWidth, winHeight);
	Key* key = new Key(result, win->w.hInstance, win->hwnd);
	Scene scene(winWidth, winHeight, result, dx12->device.Get());

	while (!key->Down(key->ESC))
	{
		//メッセージ
		if (win->Message())break;

		scene.Update(key);

		dx12->DrawBefore(scene.graph.rootsignature.Get(), scene.cb.GetGPUAddress(), scene.tex.srvHeap);
		scene.Draw(dx12->commandList.Get(), dx12->viewport);
		dx12->DrawAfter();
	}
	delete key;
	delete dx12;
	delete win;
	return 0;
}