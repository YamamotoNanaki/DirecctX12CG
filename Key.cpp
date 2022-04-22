#include "Key.h"
#include <cassert>

#define DIRECTINPUT_VERSION	0x0800		//DirectInputのバージョン指定

Key::Key(HRESULT& result, HINSTANCE& hInstance, HWND& hwnd)
{
	result = DirectInput8Create(
		hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
	assert(SUCCEEDED(result));

	//キーボードデバイスの生成
	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));

	//入力データ形式のセット
	result = keyboard->SetDataFormat(&c_dfDIKeyboard);//標準形式
	assert(SUCCEEDED(result));

	//排他制御レベルのセット
	result = keyboard->SetCooperativeLevel(
		hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));

	for (int i = 0; i < 256; i++)
	{
		key[i] = 0;
		oldkey[i] = 0;
	}
}

void Key::Update(HRESULT& result)
{
	for (int i = 0; i < 256; i++)
	{
		oldkey[i] = key[i];
	}
	keyboard->Acquire();
	result = keyboard->GetDeviceState(sizeof(key), key);
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

bool Key::Judge(KeyCode a[],int max, int Type)
{
	if (Type != AND && Type != OR)return false;
	for (int i = 0; i < max; i++)
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
