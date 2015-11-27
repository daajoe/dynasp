%{
#include <string>

#include <cassert>
//#include "LParseProblem.hpp"
#include "LParseLexer.hpp"

#define yyterminate() return static_cast<token_type>(0);

static unsigned int cnt = 0, pcnt = 0, ncnt = 0, hcnt = 0, state = LParseProblem::ERT_ZERO; 

#define S2I(_s)		(atoi(_s))
#define READ_INT	yylval->ivalue = (unsigned int)(S2I(yytext));
#define READ_INT1(_c)	_c = (unsigned int)(S2I(yytext));

#define YY_NO_UNISTD_H

static unsigned int decide_end();

%}

%option c++ batch noyywrap stack yylineno nounput debug
%option prefix="LParse"

blank [ \t]
eol \n
zero 0
delim ^0\n

bp B\+
bm B-

pword [1-9]*
uword [0-9]*
id -?[a-zA-Z][a-zA-Z0-9]*


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
	{blank}		{ yylloc->step(); }
	{eol}		{ yylloc->lines(yyleng); if (YY_START == LITLIST_END) BEGIN(INITIAL); return token::BN; }
	{delim}		{ BEGIN(RULES_DONE); return token::DELIM; }
}

<RULES_DONE>{
	{bp}{eol}	{ return token::BP; }
	{bm}{eol}	{ return token::BM; }
	{id}		{ yylval->value = new std::string(yytext, yyleng); return token::ID; }
	{uword}		{ READ_INT; return token::UWORD; }
}

<INITIAL>{
	[1] { BEGIN(BASICS); state = yylval->ivalue = LParseProblem::ERT_BASIC; return token::BASIC_TYPE; }
	[2] { BEGIN(CONSTR); state = yylval->ivalue = LParseProblem::ERT_CONSTR; return token::CONSTR_TYPE; }
	[3] { BEGIN(CHOICE); state = yylval->ivalue = LParseProblem::ERT_CHOICE; return token::CHOICE_TYPE; }
	[5] { BEGIN(WEIGHT); state = yylval->ivalue = LParseProblem::ERT_WEIGHT; return token::WEIGHT_TYPE; }
	[6] { BEGIN(MIN); state = yylval->ivalue = LParseProblem::ERT_MIN; return token::MIN_TYPE; }
	[8] { BEGIN(EXT); state = yylval->ivalue = LParseProblem::ERT_EXT_BASIC; return token::BASIC_EXT_TYPE; }
}

<BASICS,CONSTR,WEIGHT>{pword} { 
	if (state == LParseProblem::ERT_WEIGHT) { 
		BEGIN(LITLIST_BOUND); 
	} else { 
		BEGIN(LITLIST_CNT); 
	} 
	READ_INT; 
	return token::HEAD; 
}
<CHOICE,EXT>{uword} { BEGIN(LITLIST_HEADS); READ_INT1(hcnt); return token::HEAD_CNT; }

<MIN>zero { BEGIN(LITLIST_HEADS); }

<LITLIST_HEADS>{pword} { if (--hcnt == 0) { BEGIN(LITLIST_CNT); } READ_INT; return token::HEAD; }
<LITLIST_WEIGHT>{uword} { if (--cnt == 0) { BEGIN(LITLIST_END); } READ_INT; return token::LITLISTWEIGHT; }

<LITLIST_CNT>{uword} { BEGIN(LITLIST_NCNT); READ_INT1(cnt); return token::LITLISTCNT; }

<LITLIST_NCNT>{uword} { 
	READ_INT1(ncnt); 
	pcnt = cnt - ncnt;  
	if (state != LParseProblem::ERT_CONSTR) { 
		if (ncnt) { 
			BEGIN(LITLIST_N); 
		} else if (pcnt) { 
			BEGIN(LITLIST); 
		} else { 
			BEGIN(decide_end()); 
		} 
	} else { 
		BEGIN(LITLIST_BOUND); 
	} 
	return token::LITLISTNCNT; 
}

<LITLIST_N>{pword} { 
	if (--ncnt == 0) { 
		if (pcnt) { 
			BEGIN(LITLIST);
		} else
			BEGIN(decide_end()); 
	} READ_INT; 
	return token::LITLISTN; 
}
<LITLIST>{pword} { if (--pcnt == 0) { BEGIN(decide_end()); } READ_INT; return token::LITLISTP; }

<LITLIST_BOUND>{uword} { 
	if (state == LParseProblem::ERT_WEIGHT) { 
		BEGIN(LITLIST_CNT); 
	} else { 
		BEGIN(LITLIST_N); 
	} 
	READ_INT; 
	return token::BOUND; 
}

<*>.	{ yylval->value = new std::string(yytext, yyleng); return token::ERROR; }

%%

dynasp::LParseLexer::LParseLexer(FLEX_STD istream *in, FLEX_STD ostream *out)
	: LParseFlexLexer(in, out)
{ }

dynasp::LParseLexer::~LParseLexer() { }

void dynasp::LParseLexer::set_debug(bool value)
{
	yy_flex_debug = value;
}

#ifdef yylex
#undef yylex
#endif
int LParseFlexLexer::yylex()
{
	std::cerr << "In LParseFlexLexer::yylex()!" << std::endl;
	return 0;
}

unsigned int decide_end() {
	return (state == LParseProblem::ERT_WEIGHT || state == LParseProblem::ERT_MIN) ? LITLIST_WEIGHT : LITLIST_END;
}
