#pragma once
#include "Float2.h"
#include "Float3.h"
#include "Float4.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Matrix.h"


namespace IF
{
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
};