#ifndef DYNASP_DYNASP_LPARSELEXER_H_
#define DYNASP_DYNASP_LPARSELEXER_H_

namespace dynasp
{
	class LParseProblem;
	class LParseLexer;
}

#include "LParseParser.hpp"

typedef dynasp::LParseParser::token token;
typedef dynasp::LParseParser::token_type token_type;
typedef dynasp::LParseParser::semantic_type semantic_type;
typedef dynasp::LParseParser::location_type location_type;

#ifndef YY_DECL
#ifdef VERBOSE
#define YY_DECL					\
	token_type				\
	dynasp::LParseLexer::verblex(	\
		semantic_type *yylval,		\
		location_type *yylloc		\
	)
#else // !VERBOSE
#define YY_DECL					\
	token_type				\
	dynasp::LParseLexer::lex(		\
		semantic_type *yylval,		\
		location_type *yylloc		\
	)
#endif // if VERBOSE
#endif // YY_DECL

#ifndef __FLEX_LEXER_H
#undef yyFlexLexer
#define yyFlexLexer LParseFlexLexer
#include <FlexLexer.h>
#undef yyFlexLexer
#endif // __FLEX_LEXER_H

namespace dynasp
{
	class LParseLexer : public LParseFlexLexer
	{
	public:
		LParseLexer(FLEX_STD istream *in = NULL, FLEX_STD ostream *out = NULL);
		virtual ~LParseLexer();

	public:
		virtual token_type lex(semantic_type *yylval, location_type *yylloc);

#ifdef VERBOSE
	private:
		token_type verblex(semantic_type *yylval, location_type *yylloc);
#endif // VERBOSE

	public:
		void set_debug(bool debug);

	}; // class LParseLexer
} // namespace dynasp

#undef yylex
#define yylex problem->lexer()->lex

#endif /* DYNASP_DYNASP_LPARSELEXER_H_ */
