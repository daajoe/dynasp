#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif

#include "parsers/LParseParser.hpp"

#include <dynasp/Parser.hpp>

namespace dynasp
{
	IParser *Parser::get()
	{
		return Parser::get(Parser::DEFAULT);
	}

	IParser *Parser::get(ParserType parserType)
	{
		if(parserType == Parser::DEFAULT)
			return new LParseParser();
		return nullptr;
	}
} // namespace dynasp
