#ifndef DYNASP_PARSERS_LPARSELEXER_H_
#define DYNASP_PARSERS_LPARSELEXER_H_

#include <dynasp/global>

#include "LParseBisonParser.hpp"

typedef dynasp::LParseBisonParser::token token;
typedef dynasp::LParseBisonParser::token_type token_type;
typedef dynasp::LParseBisonParser::semantic_type semantic_type;
typedef dynasp::LParseBisonParser::location_type location_type;

#ifndef YY_DECL
#define YY_DECL						\
	token_type						\
	dynasp::LParseLexer::lex(		\
		semantic_type *yylval,		\
		location_type *yylloc		\
	)
#endif // YY_DECL

#ifndef __FLEX_LEXER_H
#undef yyFlexLexer
#define yyFlexLexer LParseFlexLexer
#include <FlexLexer.h>
#undef yyFlexLexer
#endif // __FLEX_LEXER_H

#ifndef FLEX_STD
	#define FLEX_STD std::
#endif

namespace dynasp
{
	class DYNASP_LOCAL LParseLexer : public LParseFlexLexer
	{
	public:
		LParseLexer(FLEX_STD istream *in = NULL, FLEX_STD ostream *out = NULL);
		virtual ~LParseLexer();

		virtual token_type lex(semantic_type *yylval, location_type *yylloc);

		void set_debug(bool debug);

	}; // class LParseLexer

} // namespace dynasp

#endif /* DYNASP_PARSERS_LPARSELEXER_H_ */
