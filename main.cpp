#include "Window.h"
#include "DirectX12.h"
#include "Input.h"
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
	result = Input::getInstance()->Initialize(win->w.hInstance, win->hwnd);
	//����
	LightManager* light = LightManager::GetInstance();
	light->SetDevice(dx12->device.Get());
	Scene scene(winWidth, winHeight, result, dx12->device.Get());
	dx12->SetClearColor(0.25, 0.5, 1);


	while (!Input::getInstance()->KDown(KEY::ESC))
	{
		//���b�Z�[�W
		if (win->Message())break;

		scene.Update(dx12->device.Get());

		dx12->DrawBefore();
		scene.Draw(dx12->commandList.Get(), dx12->viewport);
		dx12->DrawAfter();
	}
	delete dx12;
	delete win;
	return 0;
}