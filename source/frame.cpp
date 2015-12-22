//frame.cpp

#include "frame.h"
#include "exception_def.h"

MyVM::Frame::Frame
(
	std::size_t _stack_size,
	std::size_t _maximum_local_variables_num,
	const ConstantPool& _constant_pool_ref
)
	:operand_stack(_stack_size + 1),
	local_variables(_maximum_local_variables_num),
	sp(),
	constant_pool(_constant_pool_ref)
{
	sp = operand_stack.begin();
}

void MyVM::Frame::advance_sp(int _n)
{
	if (_n > 0)
	{
		for (int i = 0; i != _n; ++i)
		{
			if (sp == operand_stack.end()) { throw StackOverFlow("Exception StackOverFlow is thrown."); }
			++sp;
		}
	}
	else if (_n < 0)
	{
		for (int i = 0; i != -_n; ++i)
		{
			if (sp == operand_stack.begin()) { throw StackUnderFlow("Exception StackUnderFlow is thrown."); }
			--sp;
		}
	}
}

MyVM::Word& MyVM::Frame::operator[](std::size_t _offset)
{
	return local_variables[_offset];
}