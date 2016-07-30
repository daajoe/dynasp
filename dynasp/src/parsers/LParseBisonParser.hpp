#ifndef DYNASP_PARSERS_LPARSEBISONPARSER_H_
#define DYNASP_PARSERS_LPARSEBISONPARSER_H_

#include <dynasp/global>

namespace dynasp
{
	class LParseLexer;

	enum RuleType
	{ 
		ERT_ZERO = 0,
		ERT_BASIC,
		ERT_CONSTR,
		ERT_CHOICE,
		ERT_FOUR,
		ERT_WEIGHT,
		ERT_MIN,
		ERT_SEVEN,
		ERT_EXT_BASIC,
		ERT_COUNT 
	};

	enum LiteralType
	{
		ELT_HEAD,
		ELT_POS,
		ELT_NEG,
		ELT_COUNT 
	};
}

#include "stack.hh"
#include "position.hh"
#include "location.hh"

#include <dynasp/create.hpp>

#include "LParseBisonParser.hh"

#endif /* DYNASP_PARSERS_LPARSEBISONPARSER_H_ */
