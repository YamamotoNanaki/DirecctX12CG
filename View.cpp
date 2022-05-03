#include "View.h"

using namespace DirectX;
using namespace IF;

XMMATRIX View::matBillBoard = XMMatrixIdentity();
XMMATRIX View::matBillBoardY = XMMatrixIdentity();

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
	XMVECTOR eyePosition = XMLoadFloat3(&eye);
	XMVECTOR targetPosition = XMLoadFloat3(&target);
	XMVECTOR upVector = XMLoadFloat3(&up);

	XMVECTOR cameraAxisZ = XMVectorSubtract(targetPosition, eyePosition);
	assert(!XMVector3Equal(cameraAxisZ, XMVectorZero()));
	assert(!XMVector3IsInfinite(cameraAxisZ));
	assert(!XMVector3Equal(upVector, XMVectorZero()));
	assert(!XMVector3IsInfinite(upVector));

	cameraAxisZ = XMVector3Normalize(cameraAxisZ);

	XMVECTOR cameraAxisX;
	cameraAxisX = XMVector3Cross(upVector, cameraAxisZ);
	cameraAxisX = XMVector3Normalize(cameraAxisX);

	XMVECTOR cameraAxisY;
	cameraAxisY = XMVector3Cross(cameraAxisZ, cameraAxisX);
	cameraAxisY = XMVector3Normalize(cameraAxisY);

	XMMATRIX matCameraRot;
	matCameraRot.r[0] = cameraAxisX;
	matCameraRot.r[1] = cameraAxisY;
	matCameraRot.r[2] = cameraAxisZ;
	matCameraRot.r[3] = XMVectorSet(0, 0, 0, 1);

	matView = XMMatrixTranspose(matCameraRot);

	XMVECTOR reverseEyePosition = XMVectorNegate(eyePosition);
	XMVECTOR tX = XMVector3Dot(cameraAxisX, reverseEyePosition);
	XMVECTOR tY = XMVector3Dot(cameraAxisY, reverseEyePosition);
	XMVECTOR tZ = XMVector3Dot(cameraAxisZ, reverseEyePosition);

	XMVECTOR translation = XMVectorSet(tX.m128_f32[0], tY.m128_f32[1], tZ.m128_f32[2], 1.0f);
	matView.r[3] = translation;

	matBillBoard.r[0] = cameraAxisX;
	matBillBoard.r[1] = cameraAxisY;
	matBillBoard.r[2] = cameraAxisZ;
	matBillBoard.r[3] = XMVectorSet(0, 0, 0, 1);
	XMVECTOR yBillAxisX, yBillAxisY, yBillAxisZ;

	yBillAxisX = cameraAxisX;
	yBillAxisY = XMVector3Normalize(upVector);
	yBillAxisZ = XMVector3Cross(yBillAxisX, yBillAxisY);

	matBillBoard.r[0] = yBillAxisX;
	matBillBoard.r[1] = yBillAxisY;
	matBillBoard.r[2] = yBillAxisZ;
	matBillBoard.r[3] = XMVectorSet(0, 0, 0, 1);
}

XMMATRIX View::Get() const
{
	return matView;
}
