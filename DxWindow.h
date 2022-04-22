#pragma once
#include <Windows.h>

class DxWindow
{
public:
	WNDCLASSEX w{}; // �E�B���h�E�N���X�̐ݒ�
	HWND hwnd;

public:
	DxWindow(int window_width, int window_height);
	~DxWindow();
	
	//�E�B���h�E�v���V�[�W��
	static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		// ���b�Z�[�W�ŕ���
		switch (msg)
		{
		case WM_DESTROY: //�E�B���h�[���j�󂳂ꂽ
			PostQuitMessage(0); // OS�ɑ΂��āA�A�v���̏I����`����
			return 0;
		}
		return DefWindowProc(hwnd, msg, wparam, lparam); // �W���̏������s��
	}
};