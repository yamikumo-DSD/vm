//constant_pool_parser.h

#pragma once
#pragma warning(disable: 4819)

#include "ast.h"
#include <boost/spirit/include/qi.hpp>
#include <boost/phoenix.hpp>
#include <iostream>

namespace MyVM
{
	namespace Parser
	{
		namespace spirit = boost::spirit;
		namespace qi = spirit::qi;
		namespace ascii = qi::ascii;
		namespace phoenix = boost::phoenix;

		template<class Iterator>
		class ConstantPoolParser : public qi::grammar<Iterator, AST::ConstantPool(void), ascii::blank_type>
		{
		private:
			qi::rule<Iterator, AST::ConstantPool(void), ascii::blank_type> body;
			qi::rule<Iterator, AST::Constant(void), ascii::blank_type> constant;
			qi::rule<Iterator, jbool(void), ascii::blank_type> boolean;
		public:
			ConstantPoolParser(void) :base_type(body)
			{
				boolean = qi::lit("true")[qi::_val = true] | qi::lit("false")[qi::_val = false ];
				constant =
					"Float" >> qi::float_ | 
					"Integer" >> qi::int_ |
					"Boolean" >> boolean;
				body =
					qi::lit("Constant") >> qi::lit("pool") >> qi::lit(':') >> qi::eol >>
					constant % qi::eol;
			}
		};
	}
}
