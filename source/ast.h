//ast.h

#pragma once

#include "vm.h"
#include <boost/fusion/include/adapt_struct.hpp>
#include <string>

namespace MyVM
{
	namespace AST
	{
		using Byte = MyVM::Byte;
		using ByteCode = std::vector<AST::Byte>;
		using Constant = MyVM::Unit;
		using SymbolicReference = MyVM::SymbolicReference;
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

BOOST_FUSION_ADAPT_STRUCT
(
	MyVM::AST::SymbolicReference,
	(std::string, method_name)
	(std::string, type_of_returned_value)
	(std::vector<std::string>, type_of_arguments)
)