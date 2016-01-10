#ifndef DYNASP_DYNASP_PARSER_H_
#define DYNASP_DYNASP_PARSER_H_

#include <dynasp/global>

#include <dynasp/IParser.hpp>
#include <dynasp/IParserFactory.hpp>

#include <iostream>

namespace dynasp
{
	
	class DYNASP_API Parser
	{
	public:
		enum ParserType
		{
			DEFAULT = 0,
			LPARSE,
			GRINGO

		}; // enum ParserType

		static void set(IParserFactory *factory);

		static IParser *get();
		static IParser *get(ParserType parserType);

	private:
		Parser();

	}; // class Parser

} // namespace dynasp

#endif // DYNASP_DYNASP_PARSER_H_
