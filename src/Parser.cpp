#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif

#include <dynasp/Parser.hpp>

#include "parsers/LParseParser.hpp"

namespace dynasp
{
	namespace
	{
		IParserFactory *factory_ = nullptr;
	}

	void Parser::set(IParserFactory *factory)
	{
		if(factory_)
			delete factory_;
		factory_ = factory;
	}

	IParser *Parser::get()
	{
		return Parser::get(Parser::DEFAULT);
	}

	IParser *Parser::get(ParserType parserType)
	{
		switch(parserType)
		{
		case Parser::LPARSE:
			return new LParseParser();

		case Parser::GRINGO:
			return nullptr;

		case Parser::DEFAULT:
		default:
			if(factory_)
				return factory_->create();
			return new LParseParser();
		}
	}
} // namespace dynasp
