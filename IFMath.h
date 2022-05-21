#pragma once
#include "Float2.h"
#include "Float3.h"
#include "Float4.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Matrix.h"


namespace IF
{
#define PI = 3.141592654f;

	Float4 SetFloat4(Float3 f, float z = 0);
	Float4 SetFloat4(Float2 f, float z = 0, float w = 0);
	Float3 SetFloat3(Float2 f, float z = 0);
	Float4 SetFloat4(Vector3 v, float w = 0);
	Float3 SetFloat3(Vector3 v);
	Float2 SetFloat3(Vector2 v);
	Vector3 SetVector3(Float3 f);
	Vector3 SetVector3(Float2 f, float z = 0);
	Vector2 SetVector2(Float2 f);
	Matrix SetMatrixX(Float4 f);
	Matrix SetMatrixY(Float4 f);
	Matrix SetMatrixZ(Float4 f);
	Matrix SetMatrixW(Float4 f);
	Matrix SetMatrixX(Float3 f, float w = 0);
	Matrix SetMatrixY(Float3 f, float w = 0);
	Matrix SetMatrixZ(Float3 f, float w = 0);
	Matrix SetMatrixW(Float3 f, float w = 1);
	Matrix SetMatrixX(Vector3 v, float w = 0);
	Matrix SetMatrixY(Vector3 v, float w = 0);
	Matrix SetMatrixZ(Vector3 v, float w = 0);
	Matrix SetMatrixW(Vector3 v, float w = 1);


	// ��x�N�g����Ԃ�
	const Vector3 Vector3Zero();
	// 2�x�N�g������v���Ă��邩���ׂ�
	bool Vector3Equal(const Vector3& v1, const Vector3& v2);
	// �m����(����)�����߂�
	float Vector3Length(const Vector3& v);
	// ���K������
	Vector3& Vector3Normalize(Vector3& v);
	// ���ς����߂�
	float Vector3Dot(const Vector3& v1, const Vector3& v2);
	// �O�ς����߂�
	Vector3 Vector3Cross(const Vector3& v1, const Vector3& v2);

	// �r���[�s��̍쐬
	Matrix MatrixLookAtLH(const Vector3& eye, const Vector3& target, const Vector3& up);
	// ���s���e�s��̍쐬
	Matrix MatrixOrthographic(
		float viewLeft, float viewRight, float viewBottom, float viewTop, float nearZ, float farZ);
	// �������e�s��̍쐬
	Matrix MatrixPerspective(float fovAngleY, float aspectRatio, float nearZ, float farZ);

	// ���W�ϊ��iw���Z�Ȃ��j
	Vector3 Vector3Transform(const Vector3& v, const Matrix& m);
	// ���W�ϊ��iw���Z����j
	Vector3 Vector3TransformCoord(const Vector3& v, const Matrix& m);
	// �x�N�g���ϊ�
	Vector3 Vector3TransformNormal(const Vector3& v, const Matrix& m);

	// 2�����Z�q�I�[�o�[���[�h
	Matrix operator*(const Matrix& m1, const Matrix& m2);
	Vector3 operator*(const Vector3& v, const Matrix& m);

};