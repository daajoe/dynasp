#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif
#include "../util/debug.hpp"

#include "LParseParser.hpp"

#include "LParseBisonParser.hpp"
#include "LParseLexer.hpp"

#include "../instances/GroundAspInstance.hpp"

#include <dynasp/create.hpp>

#include <iostream>

namespace dynasp
{
	using std::istream;
	using std::ostream;

	LParseParser::LParseParser() { }

	LParseParser::~LParseParser() { }

	IGroundAspInstance *LParseParser::parse(istream *in)
	{
		return this->parse(in, nullptr);
	}

	IGroundAspInstance *LParseParser::parse(istream *in, ostream *out)
	{
		IGroundAspInstance *result = create::instance();

		//TODO: proper error handling
		//if(!result) return nullptr;

		LParseLexer *lexer = new LParseLexer(in, out);
		LParseBisonParser *parser = new LParseBisonParser(*lexer, *result);

		bool error = parser->parse();

		delete parser;
		delete lexer;

		//TODO: proper error handling
		if(error)
		{
			delete result;
			return nullptr;
		}

		return result;
	}

} // namespace dynasp
