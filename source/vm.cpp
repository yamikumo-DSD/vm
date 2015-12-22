//vm.cpp

#include "vm.h" 
#include <typeindex>
#include <cstdlib>
#include "debug.h"
#include "bit_calculation.h"
#include "exception_def.h"

#define DUMP_STACK
#undef DUMP_STACK
#define DUMP_CODE
#undef DUMP_CODE

MyVM::RepeatableVM::RepeatableVM
(
	std::size_t _size_of_local_variables,
	std::size_t _reserved_stack_size,
	const ConstantPool& _constant_pool,
	const std::vector<Byte>& _byte_code
)
	:local_variables(_size_of_local_variables),
	constant_pool(_constant_pool),
	byte_code(_byte_code),
	pc(),
	operand_stack(_reserved_stack_size + 1),
	sp(),
	continue_flag(true)
{
	pc = byte_code.begin();
	sp = operand_stack.begin();
}

void MyVM::RepeatableVM::advance_sp(int _n)
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

void MyVM::RepeatableVM::advance_pc(unsigned int _n)
{
	for (unsigned int i = 0; i != _n; ++i)
	{
		if (pc == byte_code.end()) { throw ByteCodeOverRun("Exception ByteCodeOverRun is thrown."); }
		++pc;
	}
}


void MyVM::RepeatableVM::execute(void)
{
	if (continue_flag)
	{
		while (*get_pc() != Instruction::EOC && *get_pc() != Instruction::NEXT_FRAME)
		{
#ifdef DUMP_CODE
			//Dump code
			std::cout << "*********** Dump code ***********" << std::endl;
			std::printf("Instruction code: %x\n", *pc);
			std::cout << "**********************************" << std::endl;
#endif
#ifdef DUMP_STACK
			//Dump stack
			std::cout << "*********** Dump stack ***********" << std::endl;
			int i = 0;
			for (const jint& x : operand_stack)
			{
				std::printf("%x", x); if (i == std::distance(operand_stack.begin(), sp)) { std::cout << "<-sp"; }
				std::cout << std::endl;
				++i;
			}
			std::cout << "**********************************" << std::endl;
#endif
			switch (*get_pc())
			{
				case Instruction::USER_EXTENTION_0:
					user_extention_0();
					break;
				case Instruction::USER_EXTENTION_1:
					user_extention_1();
					break;
				case Instruction::USER_EXTENTION_2:
					user_extention_2();
					break;
				case Instruction::USER_EXTENTION_3:
					user_extention_3();
					break;
				case Instruction::USER_EXTENTION_4:
					user_extention_4();
					break;
				case Instruction::USER_EXTENTION_5:
					user_extention_5();
					break;
				case Instruction::LDC: 
					{
						std::type_index type = constant_pool[get_pc()[1]].type();
						if (type == typeid(jint))
						{
							auto val = get<jint>(constant_pool[get_pc()[1]]);
							*get_sp() = BitCalculation::bit_cast<Word>(val);
							advance_sp(1);
						}
						else if (type == typeid(float))
						{
							auto val = get<float>(constant_pool[get_pc()[1]]);
							*get_sp() = BitCalculation::bit_cast<Word>(val);
							advance_sp(1);
						}
					}
					advance_pc(2);
					break;
				case Instruction::ILOAD:
					*get_sp() = local_variables[get_pc()[1]];
					advance_sp(1);
					advance_pc(2);
					break;
#ifndef STATIC_ILOAD
#define STATIC_ILOAD(N) \
					*get_sp() = local_variables[N];\
					advance_sp(1);\
					advance_pc(1);\
					break;
				case Instruction::ILOAD_0:
					STATIC_ILOAD(0)
				case Instruction::ILOAD_1:
					STATIC_ILOAD(1)
				case Instruction::ILOAD_2:
					STATIC_ILOAD(2)
				case Instruction::ILOAD_3:
					STATIC_ILOAD(3)
#undef STATIC_ILOAD
#endif
#ifndef STATIC_FLOAD
#define STATIC_FLOAD(N) \
					*get_sp() = local_variables[N];\
					advance_sp(1);\
					advance_pc(1);\
					break;
				case Instruction::FLOAD_0:
					STATIC_FLOAD(0)
				case Instruction::FLOAD_1:
					STATIC_FLOAD(1)
				case Instruction::FLOAD_2:
					STATIC_FLOAD(2)
				case Instruction::FLOAD_3:
					STATIC_FLOAD(3)
#undef STATIC_FLOAD
#endif
				case Instruction::ISTORE:
					advance_sp(-1);
					local_variables[get_pc()[1]] = *get_sp();
					advance_pc(2);
					break;
#ifndef STATIC_ISTORE
#define STATIC_ISTORE(N) \
					advance_sp(-1);\
					local_variables[N] = *get_sp();\
					advance_pc(1);\
					break;
				case Instruction::ISTORE_0:
					STATIC_ISTORE(0)
				case Instruction::ISTORE_1:
					STATIC_ISTORE(1)
				case Instruction::ISTORE_2:
					STATIC_ISTORE(2)
				case Instruction::ISTORE_3:
					STATIC_ISTORE(3)
#undef STATIC_ITORE
#endif
				case Instruction::FLOAD:
					*get_sp() = local_variables[get_pc()[1]];
					advance_sp(1);
					advance_pc(2);
					break;
				case Instruction::FSTORE:
					advance_sp(-1);
					local_variables[get_pc()[1]] = *get_sp();
					advance_pc(2);
					break;
#ifndef STATIC_FSTORE
#define STATIC_FSTORE(N) \
					advance_sp(-1);\
					local_variables[N] = *get_sp();\
					advance_pc(1);\
					break;
				case Instruction::FSTORE_0:
					STATIC_ISTORE(0)
				case Instruction::FSTORE_1:
					STATIC_ISTORE(1)
				case Instruction::FSTORE_2:
					STATIC_ISTORE(2)
				case Instruction::FSTORE_3:
					STATIC_ISTORE(3)
#undef STATIC_FTORE
#endif
#ifndef MATHEMATICAL_BINARY_OPERATOR_WORD_SIZE
#define MATHEMATICAL_BINARY_OPERATOR_WORD_SIZE(op, FourByteType) \
				{ \
					advance_sp(-1); \
					auto value1 = BitCalculation::bit_cast<FourByteType>(get_sp()[-1]); \
					auto value2 = BitCalculation::bit_cast<FourByteType>(get_sp()[0]); \
					auto result = value1 op value2; \
					get_sp()[-1] = BitCalculation::bit_cast<Word>(result); \
					advance_pc(1); \
					break; \
				}
				case Instruction::IADD:
					MATHEMATICAL_BINARY_OPERATOR_WORD_SIZE(+, jint)
				case Instruction::FADD:
					MATHEMATICAL_BINARY_OPERATOR_WORD_SIZE(+, jfloat)
				case Instruction::ISUB:
					MATHEMATICAL_BINARY_OPERATOR_WORD_SIZE(-, jint)
				case Instruction::FSUB:
					MATHEMATICAL_BINARY_OPERATOR_WORD_SIZE(-, jfloat)
				case Instruction::IDIV:
					MATHEMATICAL_BINARY_OPERATOR_WORD_SIZE(/, jint)
				case Instruction::FDIV:
					MATHEMATICAL_BINARY_OPERATOR_WORD_SIZE(/, jfloat)
				case Instruction::IMUL:
					MATHEMATICAL_BINARY_OPERATOR_WORD_SIZE(*, jint)
				case Instruction::FMUL:
					MATHEMATICAL_BINARY_OPERATOR_WORD_SIZE(*, jfloat)
				case Instruction::IREM:
					MATHEMATICAL_BINARY_OPERATOR_WORD_SIZE(%, jint)
				case Instruction::FREM:
					throw std::runtime_error("FREM instruction hasn't implemented yet.");
#undef MATHEMATICAL_BINARY_OPERATOR_WORD_SIZE
#endif
				case Instruction::I2F:
				{
					auto value = BitCalculation::bit_cast<jint>(get_sp()[-1]);
					auto result = static_cast<jfloat>(value);
					get_sp()[-1] = BitCalculation::bit_cast<Word>(result);
					advance_pc(1);
					break;
				}
				case Instruction::F2I:
				{
					auto value = BitCalculation::bit_cast<jfloat>(get_sp()[-1]);
					auto result = static_cast<jint>(value);
					get_sp()[-1] = BitCalculation::bit_cast<Word>(result);
					advance_pc(1);
					break;
				}
				case Instruction::INEG:
				{
					auto value = BitCalculation::bit_cast<jint>(get_sp()[-1]);
					get_sp()[-1] = BitCalculation::bit_cast<Word>(-value);
					advance_pc(1);
					break;
				}
				case Instruction::FNEG:
				{
					auto value = BitCalculation::bit_cast<jfloat>(get_sp()[-1]);
					get_sp()[-1] = BitCalculation::bit_cast<Word>(-value);
					advance_pc(1);
					break;
				}
				case Instruction::GOTO:
				{
					//Destination address(16bit). Branchoffset is (branchbyte1 << 8) | branchbyte2.
					BitCalculation::T2Byte<Short> branch;
					branch.field._0 = get_pc()[2]; //branchbyte2
					branch.field._1 = get_pc()[1]; //branchbyte1
					pc = byte_code.begin() + branch.t;
					break;
				}
				case Instruction::GOTO_W:
				{
					//Destination address(32bit). Branchoffset is (branchbyte1 << 24) | (branchbyte2 << 16) | (branchbyte3 << 8) | branchbyte4.
					BitCalculation::T2Byte<Word> branch;
					branch.field._0 = get_pc()[4]; //branchbyte2
					branch.field._1 = get_pc()[3]; //branchbyte1
					branch.field._2 = get_pc()[2]; //branchbyte1
					branch.field._3 = get_pc()[1]; //branchbyte1
					pc = byte_code.begin() + branch.t;
					break;
				}
#ifndef IF_ICMP_IMPLEMENT
#define IF_ICMP_IMPLEMENT(op)\
				{\
					advance_sp(-2);\
					auto value1 = BitCalculation::bit_cast<jint>(get_sp()[0]);\
					auto value2 = BitCalculation::bit_cast<jint>(get_sp()[1]);\
					BitCalculation::T2Byte<Short> branch;\
					branch.field._0 = get_pc()[2];\
					branch.field._1 = get_pc()[1];\
					if (value1 op value2) { pc = byte_code.begin() + branch.t; }\
					else { advance_pc(3);}\
					break;\
				}
				case Instruction::IF_ICMP_EQ:
					IF_ICMP_IMPLEMENT(==)
				case Instruction::IF_ICMP_NE:
					IF_ICMP_IMPLEMENT(!=)
				case Instruction::IF_ICMP_LT:
					IF_ICMP_IMPLEMENT(<)
				case Instruction::IF_ICMP_GE:
					IF_ICMP_IMPLEMENT(>=)
				case Instruction::IF_ICMP_GT:
					IF_ICMP_IMPLEMENT(>)
				case Instruction::IF_ICMP_LE:
					IF_ICMP_IMPLEMENT(<= )
#undef IF_ICMP_IMPLEMENT
#endif
#ifndef IF_IMPLEMENT
#define IF_IMPLEMENT(op)\
				{\
					advance_sp(-1);\
					auto value = BitCalculation::bit_cast<jint>(*get_sp());\
					static constexpr jint zero = 0;\
					BitCalculation::T2Byte<Short> branch;\
					branch.field._0 = get_pc()[2];\
					branch.field._1 = get_pc()[1];\
					if (value op zero) { pc = byte_code.begin() + branch.t; }\
					else {advance_pc(3);}\
					break;\
				}
				case Instruction::IF_EQ:
					IF_IMPLEMENT(==)
				case Instruction::IF_NE:
					IF_IMPLEMENT(!=)
				case Instruction::IF_LT:
					IF_IMPLEMENT(<)
				case Instruction::IF_GE:
					IF_IMPLEMENT(>=)
				case Instruction::IF_GT:
					IF_IMPLEMENT(>)
				case Instruction::IF_LE:
					IF_IMPLEMENT(<= )
#undef IF_IMPLEMENT
#endif
#ifndef FCMP_IMPLEMENT
#define FCMP_IMPLEMENT(cmp) \
				{ \
					advance_sp(-1); \
					auto value1 = BitCalculation::bit_cast<jfloat>(get_sp()[-1]); \
					auto value2 = BitCalculation::bit_cast<jfloat>(get_sp()[0]); \
					jint result = (value1 cmp value2) ? 1 : 0; \
					get_sp()[-1] = BitCalculation::bit_cast<jint>(result); \
					advance_pc(1); \
					break; \
				}
				case Instruction::FCMPG:
					FCMP_IMPLEMENT(>)
				case Instruction::FCMPL:
					FCMP_IMPLEMENT(<)
#undef FCMP_IMPLEMENT
#endif
				case Instruction::IINC:
					{
						auto const_ = BitCalculation::bit_cast<jchar>(get_pc()[2]); //const is signed byte.
						auto result = BitCalculation::bit_cast<jint>(local_variables[get_pc()[1]]) + static_cast<jint>(const_); //const is signed-extended to int
						local_variables[get_pc()[1]] = BitCalculation::bit_cast<Word>(result);
						advance_pc(3);
						break;
					}
#ifndef ICONST_IMPLE
#define ICONST_IMPLE(N)\
					{\
						*get_sp() = BitCalculation::bit_cast<Word>(N);\
						advance_sp(1);\
						advance_pc(1); \
						break;\
					}
				case Instruction::ICONST_M1:
					ICONST_IMPLE(-1)
				case Instruction::ICONST_0:
					ICONST_IMPLE(0)
				case Instruction::ICONST_1:
					ICONST_IMPLE(1)
				case Instruction::ICONST_2:
					ICONST_IMPLE(2)
				case Instruction::ICONST_3:
					ICONST_IMPLE(3)
				case Instruction::ICONST_4:
					ICONST_IMPLE(4)
				case Instruction::ICONST_5:
					ICONST_IMPLE(5)
#undef ICONST_IMPLE
#endif
#ifndef FCONST_IMPLE
#define FCONST_IMPLE(N)\
					{\
						*get_sp() = BitCalculation::bit_cast<Word>(static_cast<jfloat>(N));\
						advance_sp(1);\
						advance_pc(1); \
						break;\
					}
				case Instruction::FCONST_0:
					FCONST_IMPLE(0)
				case Instruction::FCONST_1:
					FCONST_IMPLE(1)
				case Instruction::FCONST_2:
					FCONST_IMPLE(2)
#undef FCONST_IMPLE
#endif
				case Instruction::BIPUSH: 
					{
						auto immediate_byte = BitCalculation::bit_cast<jchar>(get_pc()[1]);
						*get_sp() = BitCalculation::bit_cast<Word>(static_cast<jint>(immediate_byte));
						advance_sp(1);
						advance_pc(2);
						break;
					}
				case Instruction::SIPUSH: 
					{
						BitCalculation::T2Byte<jshort> intermediate_value;
						intermediate_value.field._1 = get_pc()[1];
						intermediate_value.field._0 = get_pc()[2];
						*get_sp() = BitCalculation::bit_cast<Word>(static_cast<jint>(intermediate_value.t));
						advance_sp(1);
						advance_pc(3);
						break;
					}
				case Instruction::NOP:
					advance_pc(1);
					break;
				case Instruction::POP:
					advance_sp(-1);
					advance_pc(1);
					break;
				case Instruction::SWAP:
					std::swap(get_sp()[-1], get_sp()[-2]);
					advance_pc(1);
					break;
				default:
					throw InvalidInstruction("Exception InvalidInstruction is thrown.");
					break;
				}
			}
	
		if (*get_pc() == Instruction::NEXT_FRAME)
		{
			advance_pc(1);
		}
		else if (*get_pc() == Instruction::EOC)
		{
			continue_flag = false;
		}
	}
}


MyVM::Word MyVM::RepeatableVM::variable(int _index) const { return local_variables[_index]; }

