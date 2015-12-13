//constant_pool.h

#pragma once

#include <boost/variant.hpp>
#include <vector>
#include <cstdint>
#include "global_def.h"

namespace MyVM
{
	using boost::get;
	using Unit = boost::variant<jint, jfloat, jbool>;
	using ConstantPool = std::vector<Unit>;
}