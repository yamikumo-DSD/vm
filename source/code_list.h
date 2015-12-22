//code_list.h

#pragma once

#include "global_def.h"
#include <string>
#include <map>

namespace MyVM
{
	using CodeList = std::map<std::string, ByteCode>;
}