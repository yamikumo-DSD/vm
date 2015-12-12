//main.cpp

#include "vm.h"
#include "debug.h"
#include <cstdlib>

#include "bit_calculation.h"

int main(void)
{
	try
	{
		using namespace MyVM;
		ConstantPool constant_pool;
		ByteCode byte_code
		({
			RepeatableVM::Instruction::ICONST_0,
			RepeatableVM::Instruction::ISTORE_1,
			RepeatableVM::Instruction::ILOAD_1,
			RepeatableVM::Instruction::ICONST_5,
			RepeatableVM::Instruction::IF_ICMP_GE,
			0x00,
			0x0e,
			RepeatableVM::Instruction::IINC,
			1,
			1,
			RepeatableVM::Instruction::USER_EXTENTION_5,
			RepeatableVM::Instruction::GOTO,
			0x00,
			0x02,
			RepeatableVM::Instruction::EOC
		});
		RepeatableVM vm(10, 512, constant_pool, byte_code);
		for (int i = 0; i != 5; ++i)
		{
			vm.execute();
		}

		BitCalculation::T2Byte<std::int32_t> result;
		result.field._0 = vm.variable(1);
		result.field._1 = 0;
		result.field._2 = 0;
		result.field._3 = 0;
		std::cout << result.t << std::endl;
	}
	catch (const std::exception& _e)
	{
		std::cerr << _e.what() << std::endl;
	}
	
	std::system("pause");
	return 0;
}