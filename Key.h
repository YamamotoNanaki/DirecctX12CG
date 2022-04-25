#pragma once
#include <dinput.h>
#include "DxWindow.h"

typedef unsigned short KeyCode;

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
		Q = DIK_Q,
		W = DIK_W,
		E = DIK_E,
		R = DIK_R,
		A = DIK_A,
		S = DIK_S,
		D = DIK_D,
		F = DIK_F,
		Z = DIK_Z,
		X = DIK_X,
		C = DIK_C,
		V = DIK_V,
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
	/// �C���X�g���N�^
	/// �L�[�{�[�h���͂̏�����
	/// </summary>
	/// <param name="result"></param>
	/// <param name="w"></param>
	/// <param name="hwnd"></param>
	Key(HRESULT& result, HINSTANCE& hInstance, HWND& hwnd);
	/// <summary>
	/// �L�[�{�[�h���̃A�b�v�f�[�g
	/// </summary>
	/// <param name="result"></param>
	void Update(HRESULT& result);

	/// <summary>
	/// �g���K�[����
	/// </summary>
	/// <param name="keyCode">�������肽���L�[�̃}�N��</param>
	/// <returns></returns>
	bool Triggere(KeyCode keyCode);
	/// <summary>
	/// ���͔���
	/// </summary>
	/// <param name="keyCode">�������肽���L�[�̃}�N��</param>
	/// <returns></returns>
	bool Down(KeyCode keyCode);
	/// <summary>
	/// �����[�X����
	/// </summary>
	/// <param name="keyCode">�������肽���L�[�̃}�N��</param>
	/// <returns></returns>
	bool Release(KeyCode keyCode);

	enum Type
	{
		OR,
		AND,
	};

	bool Judge(const KeyCode a[], int Type);
};
