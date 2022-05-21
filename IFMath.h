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


	// 零ベクトルを返す
	const Vector3 Vector3Zero();
	// 2ベクトルが一致しているか調べる
	bool Vector3Equal(const Vector3& v1, const Vector3& v2);
	// ノルム(長さ)を求める
	float Vector3Length(const Vector3& v);
	// 正規化する
	Vector3& Vector3Normalize(Vector3& v);
	// 内積を求める
	float Vector3Dot(const Vector3& v1, const Vector3& v2);
	// 外積を求める
	Vector3 Vector3Cross(const Vector3& v1, const Vector3& v2);

	// ビュー行列の作成
	Matrix MatrixLookAtLH(const Vector3& eye, const Vector3& target, const Vector3& up);
	// 並行投影行列の作成
	Matrix MatrixOrthographic(
		float viewLeft, float viewRight, float viewBottom, float viewTop, float nearZ, float farZ);
	// 透視投影行列の作成
	Matrix MatrixPerspective(float fovAngleY, float aspectRatio, float nearZ, float farZ);

	// 座標変換（w除算なし）
	Vector3 Vector3Transform(const Vector3& v, const Matrix& m);
	// 座標変換（w除算あり）
	Vector3 Vector3TransformCoord(const Vector3& v, const Matrix& m);
	// ベクトル変換
	Vector3 Vector3TransformNormal(const Vector3& v, const Matrix& m);

	// 2項演算子オーバーロード
	Matrix operator*(const Matrix& m1, const Matrix& m2);
	Vector3 operator*(const Vector3& v, const Matrix& m);

};