//global_def.h

#pragma once

#include <cstdint>

namespace MyVM
{
	using jint = std::int32_t;
	using jfloat = float;
	using jchar = char;
	using jbool = bool;

	using Byte = char;
	using Short = std::int16_t;
	using Word = std::int32_t;
	using DWord = std::int64_t;
}