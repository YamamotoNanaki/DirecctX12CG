#pragma once
#include <dinput.h>
#include "Window.h"

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")


namespace IF
{
	namespace KEY
	{
		typedef unsigned short KeyCode;
		enum Type
		{
			OR,
			AND,
		};
		enum KEYCODE
		{
			key0 = DIK_0,
			key1 = DIK_1,
			key2 = DIK_2,
			key3 = DIK_3,
			key4 = DIK_4,
			key5 = DIK_5,
			key6 = DIK_6,
			key7 = DIK_7,
			key8 = DIK_8,
			key9 = DIK_9,
			num0 = DIK_NUMPAD0,
			num1 = DIK_NUMPAD1,
			num2 = DIK_NUMPAD2,
			num3 = DIK_NUMPAD3,
			num4 = DIK_NUMPAD4,
			num5 = DIK_NUMPAD5,
			num6 = DIK_NUMPAD6,
			num7 = DIK_NUMPAD7,
			num8 = DIK_NUMPAD8,
			num9 = DIK_NUMPAD9,
			A = DIK_A,
			B = DIK_B,
			C = DIK_C,
			D = DIK_D,
			E = DIK_E,
			F = DIK_F,
			G = DIK_G,
			H = DIK_H,
			I = DIK_I,
			J = DIK_J,
			K = DIK_K,
			L = DIK_L,
			M = DIK_M,
			N = DIK_N,
			O = DIK_O,
			P = DIK_P,
			Q = DIK_Q,
			R = DIK_R,
			S = DIK_S,
			T = DIK_T,
			U = DIK_U,
			V = DIK_V,
			W = DIK_W,
			X = DIK_X,
			Y = DIK_Y,
			Z = DIK_Z,
			SPACE = DIK_SPACE,
			ENTER = DIK_RETURN,
			ESC = DIK_ESCAPE,
			SHIFT = DIK_LSHIFT,
			CTRL = DIK_LCONTROL,
			TAB = DIK_TAB,
			UP = DIK_UP,
			DOWN = DIK_DOWN,
			LEFT = DIK_LEFT,
			RIGHT = DIK_RIGHT,
			BACK = DIK_BACKSPACE,
			ALT = DIK_LALT,
			CLOCK = DIK_CAPSLOCK,
			RALT = DIK_RALT,
			RSHIFT = DIK_RSHIFT,
			RCTRL = DIK_RCONTROL,
			F1 = DIK_F1,
			F2 = DIK_F2,
			F3 = DIK_F3,
			F4 = DIK_F4,
			F5 = DIK_F5,
			F6 = DIK_F6,
			F7 = DIK_F7,
			F8 = DIK_F8,
			F9 = DIK_F9,
			F10 = DIK_F10,
			F11 = DIK_F11,
			F12 = DIK_F12
		};

		const KeyCode Arrow[4] = { LEFT,RIGHT,UP,DOWN };
		const KeyCode WASD[4] = { W,A,S,D };
	}

	class Key
	{
		using KeyCode = KEY::KeyCode;
	private:
		char key[256], oldkey[256];
		IDirectInput8* directInput = nullptr;
		IDirectInputDevice8* keyboard = nullptr;
	private:
		/// <summary>
		/// インストラクタ
		/// キーボード入力の初期化
		/// </summary>
		/// <param name="result"></param>
		/// <param name="w"></param>
		/// <param name="hwnd"></param>
		Key();
		Key(const Key&);
		Key& operator=(const Key&);
		~Key() {}
	public:
		static Key& getInstance()
		{
			static Key inst;
			return inst;
		}
		HRESULT Initialize(HINSTANCE& hInstance, HWND& hwnd);

		/// <summary>
		/// キーボード情報のアップデート
		/// </summary>
		/// <param name="result"></param>
		HRESULT Update();

		/// <summary>
		/// トリガー判定
		/// </summary>
		/// <param name="keyCode">判定を取りたいキーのマクロ</param>
		/// <returns></returns>
		bool Triggere(KeyCode keyCode);
		/// <summary>
		/// 入力判定
		/// </summary>
		/// <param name="keyCode">判定を取りたいキーのマクロ</param>
		/// <returns></returns>
		bool Down(KeyCode keyCode);
		/// <summary>
		/// リリース判定
		/// </summary>
		/// <param name="keyCode">判定を取りたいキーのマクロ</param>
		/// <returns></returns>
		bool Release(KeyCode keyCode);

		bool Judge(const KeyCode a[], int Type);
	};
}