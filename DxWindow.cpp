#include "DxWindow.h"

DxWindow::DxWindow(int window_width,int window_height)
{
	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProc; // �E�B���h�E�v���V�[�W����ݒ�
	w.lpszClassName = L"DirectX12Game"; // �E�B���h�E�N���X��
	w.hInstance = GetModuleHandle(nullptr); // �E�B���h�E�n���h��
	w.hCursor = LoadCursor(NULL, IDC_ARROW); // �J�[�\���w��

	// �E�B���h�E�N���X��OS�ɓo�^
	RegisterClassEx(&w);
	// �E�B���h�E�T�C�Y{ X���W Y���W ���� �c�� }
	RECT wrc = { 0, 0, window_width, window_height };
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false); // �����ŃT�C�Y�␳

	// �E�B���h�E�I�u�W�F�N�g�̐���
	hwnd = CreateWindow(w.lpszClassName, // �N���X��
		L"DirectXGame",         // �^�C�g���o�[�̕���
		WS_OVERLAPPEDWINDOW,        // �W���I�ȃE�B���h�E�X�^�C��
		CW_USEDEFAULT,              // �\��X���W�iOS�ɔC����j
		CW_USEDEFAULT,              // �\��Y���W�iOS�ɔC����j
		wrc.right - wrc.left,       // �E�B���h�E����
		wrc.bottom - wrc.top,   // �E�B���h�E�c��
		nullptr,                // �e�E�B���h�E�n���h��
		nullptr,                // ���j���[�n���h��
		w.hInstance,            // �Ăяo���A�v���P�[�V�����n���h��
		nullptr);               // �I�v�V����

	// �E�B���h�E�\��
	ShowWindow(hwnd, SW_SHOW);
}

DxWindow::~DxWindow()
{
	UnregisterClass(w.lpszClassName, w.hInstance);
}

HWND DxWindow::GetHwnd() const
{
	return hwnd;
}

HINSTANCE DxWindow::GetHinstance() const
{
	return w.hInstance;
}