#include "Float4.h"
#include <assert.h>

using namespace IF;

Float4::Float4(float x, float y, float z, float w) :x(x), y(y), z(z), w(w) {}

Float4 Float4::operator+() const
{
	return *this;
}

Float4 Float4::operator-() const
{
	return *this;
}

Float4 Float4::operator*() const
{
	return *this;
}

Float4 Float4::operator+=(Float4 f)
{
	return Float4(this->x + f.x, this->y + f.y, this->z + f.z, this->w + f.w);
}

Float4 Float4::operator-=(Float4 f)
{
	return Float4(this->x - f.x, this->y - f.y, this->z - f.z, this->w - f.w);
}

Float4 Float4::operator*=(Float4 f)
{
	return Float4(this->x * f.x, this->y * f.y, this->z * f.z, this->w * f.w);
}

Float4 Float4::operator/=(Float4 f)
{
	assert(f.x == 0 && "—ëœŽZ");
	assert(f.y == 0 && "—ëœŽZ");
	assert(f.z == 0 && "—ëœŽZ");
	assert(f.w == 0 && "—ëœŽZ");
	return Float4(this->x / f.x, this->y / f.y, this->z / f.z, this->w / f.w);
}

Float4 Float4::operator*=(float f)
{
	return Float4(this->x * f, this->y * f, this->z * f, this->w + f);
}

Float4 Float4::operator/=(float f)
{
	assert(f == 0 && "—ëœŽZ");
	return Float4(this->x / f, this->y / f, this->z / f, this->w + f);
}
