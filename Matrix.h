#pragma once

namespace IF
{
	class Matrix
	{
	public:
		float _1_1, _1_2, _1_3, _1_4;
		float _2_1, _2_2, _2_3, _2_4;
		float _3_1, _3_2, _3_3, _3_4;
		float _4_1, _4_2, _4_3, _4_4;

	public:
		Matrix();
		Matrix(float _1_1,float _1_2,float _1_3,float _1_4,
			float _2_1,float _2_2,float _2_3,float _2_4,
			float _3_1,float _3_2,float _3_3,float _3_4,
			float _4_1,float _4_2,float _4_3,float _4_4);
		Matrix operator+()const;
		Matrix operator-()const;
		Matrix operator+(Matrix m)const;
		Matrix operator-(Matrix m)const;
		Matrix operator*(Matrix m)const;
		Matrix operator*(float s)const;
		Matrix operator/(float s)const;
		Matrix& operator+=(Matrix m);
		Matrix& operator-=(Matrix m);
		Matrix& operator*=(Matrix m);
		Matrix& operator*=(float s);
		Matrix& operator/=(float s);
	};

	// 単位行列を求める
	Matrix MatrixIdentity();
	// 転置行列を求める
	Matrix MatrixTranspose(const Matrix& m);

	// 拡大縮小行列の作成
	Matrix MatrixScaling(float sx, float sy, float sz);

	// 回転行列の作成
	Matrix MatrixRotationX(float angle);
	Matrix MatrixRotationY(float angle);
	Matrix MatrixRotationZ(float angle);
	Matrix MatrixRotation(float x, float y, float z, float angle);

	// 平行移動行列の作成
	Matrix MatrixTranslation(float tx, float ty, float tz);
};
