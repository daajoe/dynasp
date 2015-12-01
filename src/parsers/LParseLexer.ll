%{
#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif

#include "LParseBisonParser.hpp"
#include "LParseLexer.hpp"

#include <string>

#include <cstddef>

#define yyterminate() return static_cast<token_type>(0);

static std::size_t 	cnt = 0, 
					pcnt = 0,
					ncnt = 0,
					hcnt = 0,
					state = dynasp::ERT_ZERO; 

static std::size_t decide_end();

%}

%option c++ batch noyywrap stack yylineno nounput debug
%option prefix="LParse"

blank [ \t\r]
eol \n
zero 0
delim ^{zero}\n

bp B\+
bm B-

pword [1-9][0-9]*
uword [0-9]+
id -?[a-zA-Z][a-zA-Z0-9(),]*


%{
#define YY_USER_ACTION yylloc->columns(yyleng);
%}

%x RULES_DONE

%x BASICS CONSTR WEIGHT CHOICE MIN EXT 
%x LITLIST_HEADS LITLIST_CNT LITLIST LITLIST_NCNT LITLIST_N LITLIST_WEIGHT LITLIST_BOUND LITLIST_END

%%

%{
yylloc->step();
%}

<*>{
	{blank} { yylloc->step(); }

	{eol} { 
		yylloc->lines(yyleng);
		if(YY_START == LITLIST_END) { BEGIN(INITIAL); }
		yylloc->step();
	}

	{delim} {
		BEGIN(RULES_DONE);
		return token::ZERO;
	}
}

<RULES_DONE>{
	{bp} { return token::BP; }
	
	{bm} { return token::BM; }

	{id} {
		yylval->string = new std::string(yytext, yyleng);
		return token::ID;
	}

	{uword} {
		yylval->number = static_cast<std::size_t>(std::stoull(yytext));
		return token::UWORD;
	}
}

<INITIAL>{
	[1] {
		BEGIN(BASICS);
		state = dynasp::ERT_BASIC;
		return token::BASIC_TYPE;
	}

	[2] {
		BEGIN(CONSTR);
		state = dynasp::ERT_CONSTR;
		return token::CONSTR_TYPE;
	}

	[3] {
		BEGIN(CHOICE);
		state = dynasp::ERT_CHOICE;
		return token::CHOICE_TYPE;
	}

	[5] {
		BEGIN(WEIGHT);
		state = dynasp::ERT_WEIGHT;
		return token::WEIGHT_TYPE;
	}

	[6] {
		BEGIN(MIN);
		state = dynasp::ERT_MIN;
		return token::MIN_TYPE;
	}

	[8] {
		BEGIN(EXT);
		state = dynasp::ERT_EXT_BASIC;
		return token::BASIC_EXT_TYPE;
	}
}

<BASICS,CONSTR,WEIGHT>{pword} { 
	if(state == dynasp::ERT_WEIGHT) { BEGIN(LITLIST_BOUND); }
	else { BEGIN(LITLIST_CNT); }
	yylval->number = static_cast<std::size_t>(std::stoull(yytext));
	return token::HEAD; 
}

<CHOICE,EXT>{uword} {
	BEGIN(LITLIST_HEADS);
	yylval-> number = hcnt = static_cast<std::size_t>(std::stoull(yytext));
	return token::HEAD_CNT;
}

<MIN>{zero} {
	BEGIN(LITLIST_CNT);
	yylval->number = static_cast<std::size_t>(std::stoull(yytext));
	return token::UWORD;
}

<LITLIST_HEADS>{pword} {
	if(--hcnt == 0) { BEGIN(LITLIST_CNT); }
	yylval->number = static_cast<std::size_t>(std::stoull(yytext));
	return token::HEAD;
}

<LITLIST_WEIGHT>{uword} {
	if(--cnt == 0) { BEGIN(LITLIST_END); }
	yylval->number = static_cast<std::size_t>(std::stoull(yytext));
	return token::LITLISTWEIGHT;
}

<LITLIST_CNT>{uword} {
	BEGIN(LITLIST_NCNT);
	yylval-> number = cnt = static_cast<std::size_t>(std::stoull(yytext));
	return token::LITLISTCNT;
}

<LITLIST_NCNT>{uword} { 
	yylval-> number = ncnt = static_cast<std::size_t>(std::stoull(yytext));
	pcnt = cnt - ncnt;  

	if(state != dynasp::ERT_CONSTR)
	{ 
		if(ncnt) { BEGIN(LITLIST_N); }
		else if(pcnt) { BEGIN(LITLIST); }
		else { BEGIN(decide_end()); } 
	}
	else { BEGIN(LITLIST_BOUND); } 

	return token::LITLISTNCNT; 
}

<LITLIST_N>{pword} { 
	if(--ncnt == 0)
	{ 
		if(pcnt) { BEGIN(LITLIST); }
		else { BEGIN(decide_end()); }
	}
	yylval->number = static_cast<std::size_t>(std::stoull(yytext));
	return token::LITLISTN; 
}

<LITLIST>{pword} {
	if(--pcnt == 0) { BEGIN(decide_end()); }
	yylval->number = static_cast<std::size_t>(std::stoull(yytext));
	return token::LITLISTP;
}

<LITLIST_BOUND>{uword} { 
	if(state == dynasp::ERT_WEIGHT) { BEGIN(LITLIST_CNT); }
	else if(ncnt) { BEGIN(LITLIST_N); }
	else if(pcnt) { BEGIN(LITLIST); }
	else { BEGIN(decide_end()); } 
	yylval->number = static_cast<std::size_t>(std::stoull(yytext));
	return token::BOUND; 
}

<*>. { yylval->string = new std::string(yytext, yyleng); return token::ERROR; }

%%

namespace dynasp
{
	LParseLexer::LParseLexer(FLEX_STD istream *in, FLEX_STD ostream *out)
		: LParseFlexLexer(in, out)
	{ }

	LParseLexer::~LParseLexer() { }

	void LParseLexer::set_debug(bool value)
	{
		yy_flex_debug = value;
	}
}

#ifdef yylex
#undef yylex
#endif
int LParseFlexLexer::yylex()
{
	std::cerr << "In LParseFlexLexer::yylex()!" << std::endl;
	return 0;
}

inline std::size_t decide_end()
{
	return (state == dynasp::ERT_WEIGHT
			|| state == dynasp::ERT_MIN)
		? LITLIST_WEIGHT : LITLIST_END;
}
