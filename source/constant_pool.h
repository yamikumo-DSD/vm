//constant_pool.h

#pragma once

#include <boost/variant.hpp>
#include <vector>
#include <cstdint>

namespace MyVM
{
	using boost::get;
	using Unit = boost::variant<std::int32_t, float, bool>;
	using ConstantPool = std::vector<Unit>;
}