#pragma once
#include <dinput.h>
#include "DxWindow.h"

#pragma region マクロ
#define k0		DIK_0
#define k1		DIK_1
#define k2		DIK_2
#define k3		DIK_3
#define k4		DIK_4
#define k5		DIK_5
#define k6		DIK_6
#define k7		DIK_7
#define k8		DIK_8
#define k9		DIK_9
#define kQ		DIK_Q
#define kW		DIK_W
#define kE		DIK_E
#define kR		DIK_R
#define kA		DIK_A
#define kS		DIK_S
#define kD		DIK_D
#define kF		DIK_F
#define kZ		DIK_Z
#define kX		DIK_X
#define kC		DIK_C
#define kV		DIK_V
#define SPACE	DIK_SPACE
#define ENTER	DIK_RETURN
#define ESC		DIK_ESCAPE
#define LSHIFT	DIK_LSHIFT
#define LCTRL	DIK_LCONTROL
#define TAB		DIK_TAB
#define UP		DIK_UP
#define DOWN	DIK_DOWN
#define LEFT	DIK_LEFT
#define RIGHT	DIK_RIGHT
#define BACK	DIK_BACKSPACE
#define LALT	DIK_LALT
#pragma endregion マクロ

typedef unsigned short KeyCode;

class Key
{
private:
	char key[256], oldkey[256];
	IDirectInput8* directInput = nullptr;
	IDirectInputDevice8* keyboard = nullptr;
public:
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

	bool Judge(KeyCode a[], int max,int Type);
};
