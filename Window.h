#pragma once
#include <Windows.h>

namespace IF
{
	class Window
	{
	public:
		WNDCLASSEX w{}; // �E�B���h�E�N���X�̐ݒ�
		HWND hwnd;
	private:
		MSG msg{}; // ���b�Z�[�W

	public:
		Window(int window_width, int window_height);
		~Window();
		bool Message();

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
}