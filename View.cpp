#include "View.h"

using namespace DirectX;
using namespace IF;

View::View(XMFLOAT3 eye, XMFLOAT3 target, XMFLOAT3 up)
{
	Initialze(eye, target, up);
}

void View::Initialze(XMFLOAT3 eye, XMFLOAT3 target, XMFLOAT3 up)
{
	this->eye = eye;
	this->target = target;
	this->up = up;
	Update();
}

void View::Update()
{
	matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
}

XMMATRIX View::Get() const
{
	return matView;
}
