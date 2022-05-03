#include "Util.h"
#include <d3dx12.h>

float TypeConversionColor(int num)
{
    if (num > 255)num = 255;
    if (num < 0)num = 0;
    return (float)num / 255.0f;
}

void Debug()
{
#ifdef _DEBUG
	// デバックレイヤーをオンに
	ID3D12Debug* debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
	}
#endif
}
