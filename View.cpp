#include "View.h"
#include <cassert>

using namespace IF;

Matrix View::matBillBoard = MatrixIdentity();
Matrix View::matBillBoardY = MatrixIdentity();

View::View(Float3 eye, Float3 target, Float3 up)
{
	Initialze(eye, target, up);

}

void View::Initialze(Float3 eye, Float3 target, Float3 up)
{
	this->eye = eye;
	this->target = target;
	this->up = up;
	Update();
}

void View::Update()
{
	//���_���W�A�����_���W�A�����
	Vector3 eyePosition = SetVector3(eye);
	Vector3 targetPosition = SetVector3(target);
	Vector3 upVector = SetVector3(up);

	//�J����Z��
	Vector3 cameraAxisZ;
	cameraAxisZ.x = targetPosition.x - eyePosition.x;
	cameraAxisZ.y = targetPosition.y - eyePosition.y;
	cameraAxisZ.z = targetPosition.z - eyePosition.z;
	//�[���x�N�g�������O
	assert(!Vector3Equal(cameraAxisZ, Vector3Zero()));
	assert(!Vector3Equal(upVector, Vector3Zero()));
	//�x�N�g�����K��
	cameraAxisZ.Normalize();

	//�J����X��Y��
	Vector3 cameraAxisX;
	cameraAxisX = Vector3Cross(upVector, cameraAxisZ);
	cameraAxisX.Normalize();
	Vector3 cameraAxisY;
	cameraAxisY = Vector3Cross(cameraAxisZ, cameraAxisX);
	cameraAxisY.Normalize();

	//��]�s��̍쐬
	Matrix matCameraRot;
	matCameraRot.SetX(cameraAxisX);
	matCameraRot.SetY(cameraAxisY);
	matCameraRot.SetZ(cameraAxisZ);
	matCameraRot.SetZ({ 0, 0, 0 }, 1);

	//�t�s�����
	matView = MatrixTranspose(matCameraRot);

	//�J�����̈ʒu���猴�_�ւ̃x�N�g���𐶐�
	Vector3 reverseEyePosition = VectorNegate(eyePosition);
	float dotX = Vector3Dot(cameraAxisX, reverseEyePosition);
	float dotY = Vector3Dot(cameraAxisY, reverseEyePosition);
	float dotZ = Vector3Dot(cameraAxisZ, reverseEyePosition);
	Vector3 translation(dotX, dotY, dotZ);
	matView.SetW(translation);

	matBillBoard.SetX(cameraAxisX);
	matBillBoard.SetY(cameraAxisY);
	matBillBoard.SetZ(cameraAxisZ);
	matBillBoard.SetW({ 0,0,0 });

	//Y���r���{�[�h
	Vector3 yBillAxisX, yBillAxisY, yBillAxisZ;

	yBillAxisX = cameraAxisX;
	yBillAxisY = Vector3Normalize(upVector);
	yBillAxisZ = Vector3Cross(yBillAxisX, yBillAxisY);

	matBillBoardY.SetX(yBillAxisX);
	matBillBoardY.SetY(yBillAxisY);
	matBillBoardY.SetZ(yBillAxisZ);
	matBillBoardY.SetW({ 0,0,0 });
}

Matrix View::Get() const
{
	return matView;
}
