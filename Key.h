#pragma once
#include <dinput.h>
#include "Window.h"

#pragma comment(lib,"dinput8.lib")

typedef unsigned short KeyCode;

namespace IF
{
	class Key
	{
	private:
		char key[256], oldkey[256];
		IDirectInput8* directInput = nullptr;
		IDirectInputDevice8* keyboard = nullptr;
	public:

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
			LSHIFT = DIK_LSHIFT,
			LCTRL = DIK_LCONTROL,
			TAB = DIK_TAB,
			UP = DIK_UP,
			DOWN = DIK_DOWN,
			LEFT = DIK_LEFT,
			RIGHT = DIK_RIGHT,
			BACK = DIK_BACKSPACE,
			LALT = DIK_LALT
		};

		const KeyCode Arrow[4] = { LEFT,RIGHT,UP,DOWN };
		const KeyCode WASD[4] = { W,A,S,D };

		/// <summary>
		/// インストラクタ
		/// キーボード入力の初期化
		/// </summary>
		/// <param name="result"></param>
		/// <param name="w"></param>
		/// <param name="hwnd"></param>
		Key(HRESULT& result, HINSTANCE& hInstance, HWND& hwnd);
		/// <summary>
		/// キーボード情報のアップデート
		/// </summary>
		/// <param name="result"></param>
		void Update(HRESULT& result);

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

		enum Type
		{
			OR,
			AND,
		};

		bool Judge(const KeyCode a[], int Type);
	};
}