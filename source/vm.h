//vm.h

#pragma once

#include "constant_pool.h"
#include <exception>
#include "debug.h"
#include "global_def.h"

#pragma warning(disable: 4369)

namespace MyVM
{
	using ByteCode = std::vector<Byte>;

	class StackOverFlow : public std::runtime_error
	{
	public:
		using std::runtime_error::runtime_error;
		~StackOverFlow(void) {}
	};

	class RepeatableVM
	{
	protected:
		std::vector<Word> operand_stack;
		std::vector<Word>::iterator sp; //A stack pointer.
		std::vector<Word> local_variables;
		const ConstantPool& constant_pool;
		const ByteCode& byte_code;
		ByteCode::const_iterator pc; //A program counter.
		bool continue_flag;
	public:
		enum Instruction : Byte //Original java code and extended code.
		{
			NEXT_FRAME = 203,        //0xcb: Extended code.
			EOC = 204,               //0xcc: Extended code.
			USER_EXTENTION_0 = 205,  //0xcd: Extended code.
			USER_EXTENTION_1 = 206,  //0xce: Extended code.
			USER_EXTENTION_2 = 207,  //0xcf: Extended code.
			USER_EXTENTION_3 = 208,  //0xd0: Extended code.
			USER_EXTENTION_4 = 209,  //0xd1: Extended code.
			USER_EXTENTION_5 = 210,  //0xd2: Extended code.

			LDC = 18,                //0x12: Push item from constant pool onto operand stack. pc[0]:LDC, pc[1]:index 
			ILOAD = 21,              //0x15: Load int from local variables. pc[0]:ILOAD, pc[1]:index
			ILOAD_0 = 26,            //0x1a: Load int from local variables. pc[0]:ILOAD_0
			ILOAD_1 = 27,            //0x1b: Load int from local variables. pc[0]:ILOAD_1
			ILOAD_2 = 28,            //0x1c: Load int from local variables. pc[0]:ILOAD_2
			ILOAD_3 = 29,            //0x1d: Load int from local variables. pc[0]:ILOAD_3
			ISTORE = 54,             //0x36: Store int into local variables. pc[0]:ISTORE, pc[1]:index
			ISTORE_0 = 59,           //0x3b: Store int into local variables. pc[0]:ISTORE_0
			ISTORE_1 = 60,           //0x3c: Store int into local variables. pc[0]:ISTORE_1
			ISTORE_2 = 61,           //0x3d: Store int into local variables. pc[0]:ISTORE_2
			ISTORE_3 = 62,           //0x3b: Store int into local variables. pc[0]:ISTORE_3
			FLOAD = 23,              //0x17: Load float from local variables. pc[0]:FLOAD, pc[1]:index
			FSTORE = 56,             //0x38: Store float into local variables. pc[0]:FSTORE, pc[1]:index
			IADD = 96,               //0x60: Add int on stack. pc[0]:IADD
			FADD = 98,               //0x62: Add float on stack. pc[0]:FADD
			ISUB = 100,              //0x64: Subtract int on stack(value on little address - value on big address). pc[0]:ISUB
			FSUB = 102,              //0x66: Subtract float on stack(value on little address - value on big address). pc[0]:FSUB
			I2F = 134,               //0x86: Convert int to float on stack. pc[0]:I2F
			F2I = 139,               //0x8b: Convert float to int on stack. pc[0]:F2I
			GOTO = 167,              //0xa7: Branch always. pc[0]:GOTO, pc[1]:branchbyte1, pc[2]:branchbyte2
			GOTO_W = 200,            //0xc8: Branch always. pc[0]:GOTO_@, pc[1]:branchbyte1, pc[2]:branchbyte2, pc[3]:branchbyte3, pc[4]:branchbyte4 
			IF_ICMP_EQ = 159,        //0x9f: Branch if int comparison succeeds. pc[0]:IF_ICMP_EQ, pc[1]:branchbyte1, pc[2]:branchbyte2
			IF_ICMP_NE = 160,        //0xa0: Branch if int comparison succeeds. pc[0]:IF_ICMP_NE, pc[1]:branchbyte1, pc[2]:branchbyte2
			IF_ICMP_LT = 161,        //0xa1: Branch if int comparison succeeds. pc[0]:IF_ICMP_LT, pc[1]:branchbyte1, pc[2]:branchbyte2
			IF_ICMP_GE = 162,        //0xa2: Branch if int comparison succeeds. pc[0]:IF_ICMP_GE, pc[1]:branchbyte1, pc[2]:branchbyte2
			IF_ICMP_GT = 163,        //0xa3: Branch if int comparison succeeds. pc[0]:IF_ICMP_GT, pc[1]:branchbyte1, pc[2]:branchbyte2
			IF_ICMP_LE = 164,        //0xa4: Branch if int comparison succeeds. pc[0]:IF_ICMP_LE, pc[1]:branchbyte1, pc[2]:branchbyte2
			IINC = 132,              //0x84: Increment local variable by constant. pc[0]:IINC, pc[1]:index, pc[2]:const
			ICONST_M1 = 2,           //0x02: Push int constant. pc[0]:ICONST_M1
			ICONST_0 = 3,            //0x03: Push int constant. pc[0]:ICONST_0
			ICONST_1 = 4,            //0x04: Push int constant. pc[0]:ICONST_1
			ICONST_2 = 5,            //0x05: Push int constant. pc[0]:ICONST_2
			ICONST_3 = 6,            //0x06: Push int constant. pc[0]:ICONST_3
			ICONST_4 = 7,            //0x07: Push int constant. pc[0]:ICONST_4
			ICONST_5 = 8,            //0x08: Push int constant. pc[0]:ICONST_5
			BIPUSH = 16,             //0x10: Push byte as sign-extended int value. pc[0]:BIPUSH, pc[1]:byte
			SIPUSH = 17,             //0x11: Push short. pc[0]:SIPUSH, pc[1]:byte1, pc[2]:byte2

		};
		RepeatableVM
		(
			std::size_t _size_of_local_variables,
			std::size_t _reserved_stack_size,
			const ConstantPool& _constant_pool,
			const std::vector<Byte>& _byte_code
		);
		void execute(void);
		Word variable(int _index) const;
		virtual void user_extention_0(void) { ++pc; }
		virtual void user_extention_1(void) { ++pc; }
		virtual void user_extention_2(void) { ++pc; }
		virtual void user_extention_3(void) { ++pc; }
		virtual void user_extention_4(void) { ++pc; }
		virtual void user_extention_5(void) { std::cout << "extention5" << std::endl; ++pc; }
		~RepeatableVM(void){}
	};
}