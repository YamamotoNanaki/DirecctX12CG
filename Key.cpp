#include "Key.h"
#include <cassert>

using namespace IF;
using namespace KEY;

#define DIRECTINPUT_VERSION	0x0800		//DirectInput�̃o�[�W�����w��

Key::Key()
{
	for (int i = 0; i < 256; i++)
	{
		key[i] = 0;
		oldkey[i] = 0;
	}
}

HRESULT IF::Key::Initialize(HINSTANCE& hInstance, HWND& hwnd)
{
	HRESULT result = DirectInput8Create(
		hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
	assert(SUCCEEDED(result));

	//�L�[�{�[�h�f�o�C�X�̐���
	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));

	//���̓f�[�^�`���̃Z�b�g
	result = keyboard->SetDataFormat(&c_dfDIKeyboard);//�W���`��
	assert(SUCCEEDED(result));

	//�r�����䃌�x���̃Z�b�g
	result = keyboard->SetCooperativeLevel(
		hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));

	return result;
}

HRESULT Key::Update()
{
	for (int i = 0; i < 256; i++)
	{
		oldkey[i] = key[i];
	}
	keyboard->Acquire();
	HRESULT result = keyboard->GetDeviceState(sizeof(key), key);

	return result;
}

bool Key::Triggere(KeyCode keyCode)
{
	return key[keyCode] && !oldkey[keyCode];
}

bool Key::Down(KeyCode keyCode)
{
	return key[keyCode];
}

bool Key::Release(KeyCode keyCode)
{
	return !key[keyCode] && oldkey[keyCode];
}

bool Key::Judge(const KeyCode a[], int Type)
{
	if (a == nullptr)return false;
	if (Type != AND && Type != OR)return false;
	int b = sizeof(a);
	int c = sizeof(a[0]);
	for (int i = 0; i < b / c; i++)
	{
		if (Down(a[i]))
		{
			if (Type == OR)return true;
		}
		else
		{
			if (Type == AND)return false;
		}
	}
	if (Type == OR)return false;
	return true;
}
