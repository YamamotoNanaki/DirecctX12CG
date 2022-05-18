#include "IFMath.h"

using namespace IF;

Float4 IF::SetFloat4(Float3 f, float w)
{
	return Float4(f.x, f.y, f.z, w);
}

Float4 IF::SetFloat4(Float2 f, float z, float w)
{
	return Float4(f.x, f.y, z, w);
}

Float3 IF::SetFloat3(Float2 f, float z)
{
	return Float3(f.x, f.y, z);
}

Float4 IF::SetFloat4(Vector3 v, float w)
{
	return Float4(v.x, v.y, v.z, w);
}

Float3 IF::SetFloat3(Vector3 v)
{
	return Float3(v.x, v.y, v.z);
}

Float2 IF::SetFloat3(Vector2 v)
{
	return Float2(v.x, v.y);
}

Vector3 IF::SetVector3(Float3 f)
{
	return Vector3(f.x, f.y, f.z);
}

Vector3 IF::SetVector3(Float2 f, float z)
{
	return Vector3(f.x, f.y, z);
}

Vector2 IF::SetVector2(Float2 f)
{
	return Vector2(f.x, f.y);
}

Matrix IF::SetMatrixX(Float4 f)
{
	Matrix m;
	m._1_1 = f.x;
	m._1_2 = f.y;
	m._1_3 = f.z;
	m._1_4 = f.w;

	return m;
}

Matrix IF::SetMatrixY(Float4 f)
{
	Matrix m;
	m._1_1 = f.x;
	m._1_2 = f.y;
	m._1_3 = f.z;
	m._1_4 = f.w;

	return m;
}

Matrix IF::SetMatrixZ(Float4 f)
{
	Matrix m;
	m._1_1 = f.x;
	m._1_2 = f.y;
	m._1_3 = f.z;
	m._1_4 = f.w;

	return m;
}

Matrix IF::SetMatrixW(Float4 f)
{
	Matrix m;
	m._1_1 = f.x;
	m._1_2 = f.y;
	m._1_3 = f.z;
	m._1_4 = f.w;

	return m;
}

Matrix IF::SetMatrixX(Float3 f, float w)
{
	Matrix m;
	m._1_1 = f.x;
	m._1_2 = f.y;
	m._1_3 = f.z;
	m._1_4 = w;

	return m;
}

Matrix IF::SetMatrixY(Float3 f, float w)
{
	Matrix m;
	m._1_1 = f.x;
	m._1_2 = f.y;
	m._1_3 = f.z;
	m._1_4 = w;

	return m;
}

Matrix IF::SetMatrixZ(Float3 f, float w)
{
	Matrix m;
	m._1_1 = f.x;
	m._1_2 = f.y;
	m._1_3 = f.z;
	m._1_4 = w;

	return m;
}

Matrix IF::SetMatrixW(Float3 f, float w)
{
	Matrix m;
	m._1_1 = f.x;
	m._1_2 = f.y;
	m._1_3 = f.z;
	m._1_4 = w;

	return m;
}

Matrix IF::SetMatrixX(Vector3 v, float w)
{
	Matrix m;
	m._1_1 = v.x;
	m._1_2 = v.y;
	m._1_3 = v.z;
	m._1_4 = w;

	return m;
}

Matrix IF::SetMatrixY(Vector3 v, float w)
{
	Matrix m;
	m._1_1 = v.x;
	m._1_2 = v.y;
	m._1_3 = v.z;
	m._1_4 = w;

	return m;
}

Matrix IF::SetMatrixZ(Vector3 v, float w)
{
	Matrix m;
	m._1_1 = v.x;
	m._1_2 = v.y;
	m._1_3 = v.z;
	m._1_4 = w;

	return m;
}

Matrix IF::SetMatrixW(Vector3 v, float w)
{
	Matrix m;
	m._1_1 = v.x;
	m._1_2 = v.y;
	m._1_3 = v.z;
	m._1_4 = w;

	return m;
}
