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
