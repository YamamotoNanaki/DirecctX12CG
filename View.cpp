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
	//視点座標、注視点座標、上方向
	Vector3 eyePosition = SetVector3(eye);
	Vector3 targetPosition = SetVector3(target);
	Vector3 upVector = SetVector3(up);

	//カメラZ軸
	Vector3 cameraAxisZ;
	cameraAxisZ.x = targetPosition.x - eyePosition.x;
	cameraAxisZ.y = targetPosition.y - eyePosition.y;
	cameraAxisZ.z = targetPosition.z - eyePosition.z;
	//ゼロベクトルを除外
	assert(!Vector3Equal(cameraAxisZ, Vector3Zero()));
	assert(!Vector3Equal(upVector, Vector3Zero()));
	//ベクトル正規化
	cameraAxisZ.Normalize();

	//カメラX軸Y軸
	Vector3 cameraAxisX;
	cameraAxisX = Vector3Cross(upVector, cameraAxisZ);
	cameraAxisX.Normalize();
	Vector3 cameraAxisY;
	cameraAxisY = Vector3Cross(cameraAxisZ, cameraAxisX);
	cameraAxisY.Normalize();

	//回転行列の作成
	Matrix matCameraRot;
	matCameraRot.SetX(cameraAxisX);
	matCameraRot.SetY(cameraAxisY);
	matCameraRot.SetZ(cameraAxisZ);
	matCameraRot.SetZ({ 0, 0, 0 }, 1);

	//逆行列を代入
	matView = MatrixTranspose(matCameraRot);

	//カメラの位置から原点へのベクトルを生成
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

	//Y軸ビルボード
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
