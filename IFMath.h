#pragma once
#include "Float2.h"
#include "Float3.h"
#include "Float4.h"
#include "Matrix.h"


namespace IF
{
	Float4 SetFloat4(Float3 f, float z);
	Float4 SetFloat4(Float2 f, float z, float w);
	Float3 SetFloat3(Float2 f, float z);
};