//ast.h

#pragma once

#include "vm.h"
#include <boost/fusion/include/adapt_struct.hpp>

namespace MyVM
{
	namespace AST
	{
		using Byte = MyVM::Byte;
		using ByteCode = std::vector<AST::Byte>;
		using Constant = MyVM::Unit;
		using ConstantPool = std::vector<Constant>;
		struct Code
		{
			ByteCode bytecode;
			ConstantPool constant_pool;
		};
	}
}

BOOST_FUSION_ADAPT_STRUCT
(
	MyVM::AST::Code,
	(MyVM::AST::ByteCode, bytecode)
	(MyVM::AST::ConstantPool, constant_pool)
)