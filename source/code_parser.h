//code_parser.h

#pragma once
#pragma warning(disable: 4819)

#include "bytecode_parser.h"
#include "constant_pool_parser.h"

namespace MyVM
{
	namespace Parser
	{
		namespace spirit = boost::spirit;
		namespace qi = spirit::qi;
		namespace ascii = qi::ascii;
		namespace phoenix = boost::phoenix;

		template<class Iterator>
		class CodeParser : public qi::grammar<Iterator, AST::Code(void), ascii::blank_type>
		{
		private:
			qi::rule<Iterator, AST::Code(void), ascii::blank_type> body;
			ConstantPoolParser<Iterator> constant_pool;
			ByteCodeParser<Iterator> bytecode;
		public:
			CodeParser(void) :base_type(body)
			{
				body =
					bytecode >> +qi::eol >> constant_pool;
			}
		};
	}
}
