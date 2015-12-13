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
		constant_pool.push_back(3);
		constant_pool.push_back(2.5f);
		ByteCode byte_code
		({
			RepeatableVM::Instruction::LDC,
			0,
			RepeatableVM::Instruction::LDC,
			1,
			RepeatableVM::Instruction::F2I,
			RepeatableVM::Instruction::ISUB,
			RepeatableVM::Instruction::ISTORE,
			0,
			RepeatableVM::Instruction::EOC
		});
		RepeatableVM vm(10, 512, constant_pool, byte_code);
		for (int i = 0; i != 5; ++i)
		{
			vm.execute();
		}

		std::cout << BitCalculation::bit_cast<jint>(vm.variable(0)) << std::endl;
	}
	catch (const std::exception& _e)
	{
		std::cerr << _e.what() << std::endl;
	}
	
	std::system("pause");
	return 0;
}