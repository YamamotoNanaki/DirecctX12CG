#include "Window.h"
#include "DirectX12.h"
#include "Key.h"
#include "Scene.h"
#include "Util.h"

using namespace IF;

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine,
	_In_ int nCmdShow) {

	// �E�B���h�E�T�C�Y
	const int winWidth = 1280;  // ����
	const int winHeight = 720;  // �c��

	Window* win = new Window(winWidth, winHeight);

	Debug();

	HRESULT result;
	DirectX12* dx12 = new DirectX12(result, win->hwnd, winWidth, winHeight);
	result = Key::getInstance().Initialize(win->w.hInstance, win->hwnd);
	Scene scene(winWidth, winHeight, result, dx12->device.Get());
	dx12->SetClearColor(0, 0, 0);

	while (!Key::getInstance().Down(KEY::ESC))
	{
		//���b�Z�[�W
		if (win->Message())break;

		scene.Update();

		dx12->DrawBefore(scene.graph.rootsignature.Get(), scene.tex.srvHeap, scene.cb.GetGPUAddress(),D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
		scene.Draw(dx12->commandList.Get(), dx12->viewport);
		dx12->DrawAfter();
	}
	delete dx12;
	delete win;
	return 0;
}