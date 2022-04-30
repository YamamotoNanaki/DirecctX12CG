#pragma once

namespace NA
{
	class Matrix
	{
	public:
		float _1_1, _1_2, _1_3, _1_4;
		float _2_1, _2_2, _2_3, _2_4;
		float _3_1, _3_2, _3_3, _3_4;
		float _4_1, _4_2, _4_3, _4_4;

	public:
		void IdentityMatrix();
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
};