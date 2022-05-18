#include "Float2.h"
#include <assert.h>

using namespace IF;

Float2::Float2(float x, float y) :x(x), y(y) {}

Float2 Float2::operator+() const
{
	return *this;
}

Float2 Float2::operator-() const
{
	return *this;
}

Float2 Float2::operator*() const
{
	return *this;
}

Float2 Float2::operator+=(Float2 f)
{
	return Float2(this->x + f.x, this->y + f.y);
}

Float2 Float2::operator-=(Float2 f)
{
	return Float2(this->x - f.x, this->y - f.y);
}

Float2 Float2::operator*=(Float2 f)
{
	return Float2(this->x * f.x, this->y * f.y);
}

Float2 Float2::operator/=(Float2 f)
{
	assert(f.x == 0 && "—ëœŽZ");
	assert(f.y == 0 && "—ëœŽZ");
	return Float2(this->x / f.x, this->y / f.y);
}

Float2 Float2::operator*=(float f)
{
	return Float2(this->x * f, this->y * f);
}

Float2 Float2::operator/=(float f)
{
	assert(f == 0 && "—ëœŽZ");
	return Float2(this->x / f, this->y / f);
}
