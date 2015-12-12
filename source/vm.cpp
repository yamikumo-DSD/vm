//vm.cpp

#include "vm.h" 
#include <typeindex>
#include <cstdlib>
#include "debug.h"
#include "bit_calculation.h"

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

void MyVM::RepeatableVM::execute(void)
{
	if (continue_flag)
	{
		while (*pc != Instruction::EOC && *pc != Instruction::NEXT_FRAME)
		{
			switch (*pc)
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
						std::type_index type = constant_pool[pc[1]].type();
						if (type == typeid(std::int32_t))
						{
							auto val = get<std::int32_t>(constant_pool[pc[1]]);
							for (int i = 0; i != sizeof(std::int32_t) / sizeof(Byte); ++i)
							{
								*sp++ = BitCalculation::get_byte(val, 0);
								val >>= 8;
							}
						}
						else if (type == typeid(float))
						{
							auto val = get<float>(constant_pool[pc[1]]);
							for (int i = 0; i != sizeof(float) / sizeof(Byte); ++i)
							{
								*sp++ = BitCalculation::get_byte(val, 0);
								auto temp = BitCalculation::bit_cast<int>(val);
								temp >>= 8;
								val = BitCalculation::bit_cast<float>(temp);
							}
						}
					}
					pc += 2;
					break;
				case Instruction::ILOAD:
					for (int i = 0; i != sizeof(std::int32_t) / sizeof(Byte); ++i)
					{
						*sp++ = local_variables[pc[1] + i];
					}
					pc += 2;
					break;
#ifndef STATIC_ILOAD
#define STATIC_ILOAD(N) \
					for (int i = 0; i != sizeof(std::int32_t) / sizeof(Byte); ++i)\
					{\
						*sp++ = local_variables[N + i];\
					}\
					++pc;\
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
				case Instruction::ISTORE:
					sp -= 4;
					for (int i = 0; i != sizeof(std::int32_t) / sizeof(Byte); ++i)
					{
						local_variables[pc[1] + i] = sp[i];
					}
					pc += 2;
					break;
#ifndef STATIC_ISTORE
#define STATIC_ISTORE(N) \
					sp -= 4;\
					for (int i = 0; i != sizeof(std::int32_t) / sizeof(Byte); ++i)\
					{\
						local_variables[N + i] = sp[i];\
					}\
					++pc;\
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
					for (int i = 0; i != sizeof(float) / sizeof(Byte); ++i)
					{
						*sp++ = local_variables[pc[1] + i];
					}
					pc += 2;
					break;
				case Instruction::FSTORE:
					sp -= 4;
					for (int i = 0; i != sizeof(float) / sizeof(Byte); ++i)
					{
						local_variables[pc[1] + i] = sp[i];
					}
					pc += 2;
					break;
				case Instruction::IADD:
				{
					BitCalculation::T2Byte<std::int32_t> value1(0), value2(0), result(0);
					const auto size = sizeof(std::int32_t) / sizeof(Byte);
					if (BitCalculation::endian_judge() == BitCalculation::Endian::LITTLE)
					{
						value1.field._0 = *(sp - size - 4);
						value1.field._1 = *(sp - size - 3);
						value1.field._2 = *(sp - size - 2);
						value1.field._3 = *(sp - size - 1);

						value2.field._0 = *(sp - 4);
						value2.field._1 = *(sp - 3);
						value2.field._2 = *(sp - 2);
						value2.field._3 = *(sp - 1);

						result.t = value1.t + value2.t;

						*(sp - size - 4) = result.field._0;
						*(sp - size - 3) = result.field._1;
						*(sp - size - 2) = result.field._2;
						*(sp - size - 1) = result.field._3;

						sp -= size;
					}
					else
					{
						value1.field._0 = *(sp - size - 1);
						value1.field._1 = *(sp - size - 2);
						value1.field._2 = *(sp - size - 3);
						value1.field._3 = *(sp - size - 4);

						value2.field._0 = *(sp - 1);
						value2.field._1 = *(sp - 2);
						value2.field._2 = *(sp - 3);
						value2.field._3 = *(sp - 4);

						result.t = value1.t + value2.t;

						*(sp - size - 1) = result.field._0;
						*(sp - size - 2) = result.field._1;
						*(sp - size - 3) = result.field._2;
						*(sp - size - 4) = result.field._3;

						sp -= size;
					}
				}
					++pc;
					break;
				case Instruction::FADD:
				{
					BitCalculation::T2Byte<float> value1(0), value2(0), result(0);
					const auto size = sizeof(float) / sizeof(Byte);
					if (BitCalculation::endian_judge() == BitCalculation::Endian::LITTLE)
					{
						value1.field._0 = *(sp - size - 4);
						value1.field._1 = *(sp - size - 3);
						value1.field._2 = *(sp - size - 2);
						value1.field._3 = *(sp - size - 1);

						value2.field._0 = *(sp - 4);
						value2.field._1 = *(sp - 3);
						value2.field._2 = *(sp - 2);
						value2.field._3 = *(sp - 1);

						result.t = value1.t + value2.t;

						*(sp - size - 4) = result.field._0;
						*(sp - size - 3) = result.field._1;
						*(sp - size - 2) = result.field._2;
						*(sp - size - 1) = result.field._3;

						sp -= size;
					}
					else
					{
						value1.field._0 = *(sp - size - 1);
						value1.field._1 = *(sp - size - 2);
						value1.field._2 = *(sp - size - 3);
						value1.field._3 = *(sp - size - 4);

						value2.field._0 = *(sp - 1);
						value2.field._1 = *(sp - 2);
						value2.field._2 = *(sp - 3);
						value2.field._3 = *(sp - 4);

						result.t = value1.t + value2.t;

						*(sp - size - 1) = result.field._0;
						*(sp - size - 2) = result.field._1;
						*(sp - size - 3) = result.field._2;
						*(sp - size - 4) = result.field._3;

						sp -= size;
					}
				}
					++pc;
					break;
				case Instruction::ISUB:
				{
					BitCalculation::T2Byte<std::int32_t> value1(0), value2(0), result(0);
					const auto size = sizeof(std::int32_t) / sizeof(Byte);
					if (BitCalculation::endian_judge() == BitCalculation::Endian::LITTLE)
					{
						value1.field._0 = *(sp - size - 4);
						value1.field._1 = *(sp - size - 3);
						value1.field._2 = *(sp - size - 2);
						value1.field._3 = *(sp - size - 1);

						value2.field._0 = *(sp - 4);
						value2.field._1 = *(sp - 3);
						value2.field._2 = *(sp - 2);
						value2.field._3 = *(sp - 1);

						result.t = value1.t - value2.t;

						*(sp - size - 4) = result.field._0;
						*(sp - size - 3) = result.field._1;
						*(sp - size - 2) = result.field._2;
						*(sp - size - 1) = result.field._3;

						sp -= size;
					}
					else
					{
						value1.field._0 = *(sp - size - 1);
						value1.field._1 = *(sp - size - 2);
						value1.field._2 = *(sp - size - 3);
						value1.field._3 = *(sp - size - 4);

						value2.field._0 = *(sp - 1);
						value2.field._1 = *(sp - 2);
						value2.field._2 = *(sp - 3);
						value2.field._3 = *(sp - 4);

						result.t = value1.t + value2.t;

						*(sp - size - 1) = result.field._0;
						*(sp - size - 2) = result.field._1;
						*(sp - size - 3) = result.field._2;
						*(sp - size - 4) = result.field._3;

						sp -= size;
					}
				}
					++pc;
					break;
				case Instruction::FSUB:
				{
					BitCalculation::T2Byte<float> value1(0), value2(0), result(0);
					const auto size = sizeof(float) / sizeof(Byte);
					if (BitCalculation::endian_judge() == BitCalculation::Endian::LITTLE)
					{
						value1.field._0 = *(sp - size - 4);
						value1.field._1 = *(sp - size - 3);
						value1.field._2 = *(sp - size - 2);
						value1.field._3 = *(sp - size - 1);

						value2.field._0 = *(sp - 4);
						value2.field._1 = *(sp - 3);
						value2.field._2 = *(sp - 2);
						value2.field._3 = *(sp - 1);

						result.t = value1.t - value2.t;

						*(sp - size - 4) = result.field._0;
						*(sp - size - 3) = result.field._1;
						*(sp - size - 2) = result.field._2;
						*(sp - size - 1) = result.field._3;

						sp -= size;
					}
					else
					{
						value1.field._0 = *(sp - size - 1);
						value1.field._1 = *(sp - size - 2);
						value1.field._2 = *(sp - size - 3);
						value1.field._3 = *(sp - size - 4);

						value2.field._0 = *(sp - 1);
						value2.field._1 = *(sp - 2);
						value2.field._2 = *(sp - 3);
						value2.field._3 = *(sp - 4);

						result.t = value1.t + value2.t;

						*(sp - size - 1) = result.field._0;
						*(sp - size - 2) = result.field._1;
						*(sp - size - 3) = result.field._2;
						*(sp - size - 4) = result.field._3;

						sp -= size;
					}
				}
					++pc;
					break;
				case Instruction::I2F:
				{
					BitCalculation::T2Byte<std::int32_t> value;
					BitCalculation::T2Byte<float> result;
					if (BitCalculation::endian_judge() == BitCalculation::Endian::LITTLE)
					{
						value.field._0 = *(sp - 4);
						value.field._1 = *(sp - 3);
						value.field._2 = *(sp - 2);
						value.field._3 = *(sp - 1);

						result.t = static_cast<float>(value.t);

						*(sp - 4) = result.field._0;
						*(sp - 3) = result.field._1;
						*(sp - 2) = result.field._2;
						*(sp - 1) = result.field._3;
					}
					else
					{
						value.field._0 = *(sp - 1);
						value.field._1 = *(sp - 2);
						value.field._2 = *(sp - 3);
						value.field._3 = *(sp - 4);

						result.t = static_cast<float>(value.t);

						*(sp - 1) = result.field._0;
						*(sp - 2) = result.field._1;
						*(sp - 3) = result.field._2;
						*(sp - 4) = result.field._3;
					}
				}
					++pc;
					break;
				case Instruction::F2I:
				{
					BitCalculation::T2Byte<float> value;
					BitCalculation::T2Byte<std::int32_t> result;
					if (BitCalculation::endian_judge() == BitCalculation::Endian::LITTLE)
					{
						value.field._0 = *(sp - 4);
						value.field._1 = *(sp - 3);
						value.field._2 = *(sp - 2);
						value.field._3 = *(sp - 1);

						//This palagraph needs revision to follow IEEE754 rounding rules.
						result.t = static_cast<std::int32_t>(value.t);

						*(sp - 4) = result.field._0;
						*(sp - 3) = result.field._1;
						*(sp - 2) = result.field._2;
						*(sp - 1) = result.field._3;
					}
					else
					{
						value.field._0 = *(sp - 1);
						value.field._1 = *(sp - 2);
						value.field._2 = *(sp - 3);
						value.field._3 = *(sp - 4);

						//This palagraph needs revision to follow IEEE754 rounding rules.
						result.t = static_cast<std::int32_t>(value.t);

						*(sp - 1) = result.field._0;
						*(sp - 2) = result.field._1;
						*(sp - 3) = result.field._2;
						*(sp - 4) = result.field._3;
					}
				}
					++pc;
					break;
				case Instruction::GOTO:
				{
					//Destination address(16bit). Branchoffset is (branchbyte1 << 8) | branchbyte2.
					BitCalculation::T2Byte<std::int16_t> branch;
					branch.field._0 = pc[2]; //branchbyte2
					branch.field._1 = pc[1]; //branchbyte1
					pc = byte_code.begin() + branch.t;
					break;
				}
				case Instruction::GOTO_W:
				{
					//Destination address(32bit). Branchoffset is (branchbyte1 << 24) | (branchbyte2 << 16) | (branchbyte3 << 8) | branchbyte4.
					BitCalculation::T2Byte<std::int32_t> branch;
					branch.field._0 = pc[4]; //branchbyte2
					branch.field._1 = pc[3]; //branchbyte1
					branch.field._2 = pc[2]; //branchbyte1
					branch.field._3 = pc[1]; //branchbyte1
					pc = byte_code.begin() + branch.t;
					break;
				}
#ifndef IF_ICMP_IMPLEMENT
#define IF_ICMP_IMPLEMENT(op)\
				{\
					BitCalculation::T2Byte<std::int32_t> value1, value2;\
					static constexpr auto size = sizeof(std::int32_t) / sizeof(Byte);\
					if (BitCalculation::endian_judge() == BitCalculation::Endian::LITTLE)\
					{\
						value1.field._0 = *(sp - size - 4);\
						value1.field._1 = *(sp - size - 3);\
						value1.field._2 = *(sp - size - 2);\
						value1.field._3 = *(sp - size - 1);\
\
						value2.field._0 = *(sp - 4);\
						value2.field._1 = *(sp - 3);\
						value2.field._2 = *(sp - 2);\
						value2.field._3 = *(sp - 1);\
\
						sp -= 2 * size;\
					}\
					else\
					{\
						value1.field._0 = *(sp - size - 1);\
						value1.field._1 = *(sp - size - 2);\
						value1.field._2 = *(sp - size - 3);\
						value1.field._3 = *(sp - size - 4);\
\
						value2.field._0 = *(sp - 1);\
						value2.field._1 = *(sp - 2);\
						value2.field._2 = *(sp - 3);\
						value2.field._3 = *(sp - 4);\
\
						sp -= 2 * size;\
					}\
					BitCalculation::T2Byte<std::int16_t> branch;\
					branch.field._0 = pc[2];\
					branch.field._1 = pc[1];\
					if(value1.t op value2.t){pc = byte_code.begin() + branch.t;} \
					else{pc += 3;}\
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
					IF_ICMP_IMPLEMENT(<=)
#undef IF_ICMP_IMPLEMENT
#endif
				case Instruction::IINC:
					// -------------------------------------- //
					// REVISION OF THIS SECTION NOW ONGOING!! //
					// -------------------------------------- // 
					{
						BitCalculation::T2Byte<std::int32_t> temp;
						if (BitCalculation::endian_judge() == BitCalculation::Endian::LITTLE)
						{
							temp.field._3 = local_variables[pc[1] + 3];
							temp.field._2 = local_variables[pc[1] + 2];
							temp.field._1 = local_variables[pc[1] + 1];
							temp.field._0 = local_variables[pc[1] + 0];

							auto constant = static_cast<std::int32_t>(pc[2]);
							temp.t += constant;

							local_variables[pc[1] + 3] = temp.field._3;
							local_variables[pc[1] + 2] = temp.field._2;
							local_variables[pc[1] + 1] = temp.field._1;
							local_variables[pc[1] + 0] = temp.field._0;
						}
						else
						{
							temp.field._0 = local_variables[pc[1] + 3];
							temp.field._1 = local_variables[pc[1] + 2];
							temp.field._2 = local_variables[pc[1] + 1];
							temp.field._3 = local_variables[pc[1] + 0];

							auto constant = static_cast<std::int32_t>(pc[2]);
							temp.t += constant;

							local_variables[pc[1] + 3] = temp.field._0;
							local_variables[pc[1] + 2] = temp.field._1;
							local_variables[pc[1] + 1] = temp.field._2;
							local_variables[pc[1] + 0] = temp.field._3;
						}

					}
					pc += 3;
					break;
#ifndef ICONST_IMPLE
#define ICONST_IMPLE(N)\
					{\
						BitCalculation::T2Byte<std::int32_t> i;\
						i.t = N;\
						if (BitCalculation::endian_judge() == BitCalculation::Endian::LITTLE)\
						{\
							sp[0] = i.field._0;\
							sp[1] = i.field._1;\
							sp[2] = i.field._2;\
							sp[3] = i.field._3;\
						}\
						else\
						{\
							sp[3] = i.field._0;\
							sp[2] = i.field._1;\
							sp[1] = i.field._2;\
							sp[0] = i.field._3;\
						}\
\
					}\
					sp += sizeof(std::int32_t);\
					++pc;\
					break;
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
				case Instruction::BIPUSH: 
					{
						BitCalculation::T2Byte<std::int32_t> i;
						i.t = static_cast<std::int32_t>(pc[1]);

						if (BitCalculation::endian_judge() == BitCalculation::Endian::LITTLE)
						{
							sp[0] = i.field._0;
							sp[1] = i.field._1;
							sp[2] = i.field._2;
							sp[3] = i.field._3;
						}
						else
						{
							sp[3] = i.field._0;
							sp[2] = i.field._1;
							sp[1] = i.field._2;
							sp[0] = i.field._3;
						}
					}
					sp += sizeof(std::int32_t);
					pc += 2;
					break;
				case Instruction::SIPUSH: 
					{
						BitCalculation::T2Byte<std::int16_t> intermediate_short;
						BitCalculation::T2Byte<std::int32_t> i;
						if (BitCalculation::endian_judge() == BitCalculation::Endian::LITTLE)
						{
							intermediate_short.field._1 = pc[1];
							intermediate_short.field._0 = pc[2];
							i.t = static_cast<std::int32_t>(intermediate_short.t);
							sp[0] = i.field._0;
							sp[1] = i.field._1;
							sp[2] = i.field._2;
							sp[3] = i.field._3;
						}
						else
						{
							intermediate_short.field._0 = pc[1];
							intermediate_short.field._1 = pc[2];
							i.t = static_cast<std::int32_t>(intermediate_short.t);
							sp[3] = i.field._0;
							sp[2] = i.field._1;
							sp[1] = i.field._2;
							sp[0] = i.field._3;
						}
					}
					sp += sizeof(std::int32_t);
					pc += 3;
					break;
				default:
					break;
				}
			}
	
		if (*pc == Instruction::NEXT_FRAME)
		{
			++pc;
		}
		else if (*pc == Instruction::EOC)
		{
			continue_flag = false;
		}
	}
	if (sp == operand_stack.end()) { throw StackOverFlow("Stack over flow."); }
}


MyVM::Byte MyVM::RepeatableVM::variable(int _index) const { return local_variables[_index]; }

