//frame.h

#pragma once

#include "global_def.h"
#include "constant_pool.h"

namespace MyVM
{
	class Frame
	{
	private:
		std::vector<Word> operand_stack;
		std::vector<Word> local_variables;
		decltype(operand_stack.begin()) sp;
		const ConstantPool& constant_pool;
	public:
		Frame
		(
			std::size_t _stack_size,
			std::size_t _maximum_local_variables_num,
			const ConstantPool& _constant_pool_ref
		);
		void advance_sp(int _n);
		auto get_sp(void) const { return sp; }
		Word& operator[](std::size_t _offset);
		const Word& operator[](std::size_t _offset) const;
		Word top(void) const;
		DWord top2(void) const;
		void dump_stack(void) ; //to stdout
	};
}
