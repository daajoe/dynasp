#ifndef DYNASP_DYNASP_PARSER_H_
#define DYNASP_DYNASP_PARSER_H_

#include <dynasp/global>

#include <dynasp/IParser.hpp>

#include <iostream>

namespace dynasp
{
	
	class DYNASP_API Parser
	{
	private:
		Parser();
		Parser &operator=(Parser &);

	public:

		enum ParserType
		{
			DEFAULT = 0,
			LPARSE,
			GRINGO
		}; // enum ParserType

		static IParser *get();
		static IParser *get(ParserType parserType);
		
	}; // class Parser

} // namespace dynasp

#endif // DYNASP_DYNASP_PARSER_H_
