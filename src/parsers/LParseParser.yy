%{

#include <cassert>
#include <cstring>
#include <iostream>
#include <string>

using namespace std;

#include "location.hh"
#include "position.hh"
#include "LParseParser.hh"

using namespace dynasp;

typedef set<int> atom_set;
typedef int atom_t;
typedef list<int> weight_list;

enum E_RULE_TYPE { ERT_ZERO = 0, ERT_BASIC, ERT_CONSTR, ERT_CHOICE, ERT_FOUR, ERT_WEIGHT, ERT_MIN, ERT_SEVEN, ERT_EXT_BASIC, ERT_COUNT };
enum E_LIT_TYPE { ELT_HEAD, ELT_POS, ELT_NEG, ELT_COUNT };

static atom_set *nlist = new atom_set(), *plist = new atom_set(), *hlist = new atom_set();
static weight_list *wlist = new weight_list(); 

static void clean()
{
	plist = new atom_set();
	nlist = new atom_set();
	hlist = new atom_set();
	wlist = new weight_list();
}

/*static void pyerror(bool ass, const char* const cmsg)
{
	if (ass)
		return;
	position *p = new position(new std::string("stdin"), pline, 1); 
	location *l = new location(*p); 
	std::string* msg = new std::string(cmsg); 
	throw *new LParseParser::syntax_error(*l, *msg); 
}

static void pyerror(const char* const cmsg)
{	
	pyerror(cmsg == NULL, cmsg);
}*/
%}

%require "2.3"

%skeleton "lalr1.cc"

%defines
%define "parser_class_name" { LParseParser } 
%name-prefix "dynasp"
%file-prefix "LParse"
%locations

%debug
%error-verbose

%union
{
	std::string *value;
	unsigned int ivalue;
}

%token 		BP BM BN DELIM
%token <value>	ID ERROR
%token <ivalue>	BASIC_TYPE CHOICE_TYPE CONSTR_TYPE WEIGHT_TYPE MIN_TYPE BASIC_EXT_TYPE UWORD HEAD_CNT HEAD LITLISTCNT LITLISTNCNT LITLISTN LITLISTP LITLISTWEIGHT BOUND

%destructor { delete $$; }	ID

%parse-param { LParseProblem *problem }

%start lparse

%%

lparse		: rules DELIM mapping DELIM BP compute DELIM BM compute DELIM UWORD BN	{ /*(problem->setNumber($<ivalue>11));*/ }
		;

rules		: rules rule BN 	{ /*(problem->addRule($<ivalue>2));*/ }
		| 		 	{ }
		;

lithlist	: lithlist HEAD		{ hlist->insert($<ivalue>2); /*(problem->addLitList(LParseProblem::ELT_HEAD, $<ivalue>2));*/ }
		| 			{ }
		;

litlist		: litlist LITLISTP	{ plist->insert($<ivalue>2); /*(problem->addLitList(LParseProblem::ELT_POS, $<ivalue>2));*/ }
		| 			{ }
		;

litnlist	: litnlist LITLISTN	{ nlist->insert($<ivalue>2); /*problem->addLitList(LParseProblem::ELT_NEG, $<ivalue>2));*/ }
		| 			{ }
		;

weightlist	: weightlist LITLISTWEIGHT	{ wlist->push_back($<ivalue>2);/*(problem->addWeightList($<ivalue>2));*/ }
		| 				{ }
		;

mapping		: mapping UWORD ID BN 	{ (problem->addMapping($<ivalue>2, *$3)); }
		| 			{ }
		;

compute		: compute UWORD BN	{ /*(problem->setCompute($<ivalue>2));*/ }
		| 			{ }
		;

rule		: BASIC_TYPE HEAD LITLISTCNT LITLISTNCNT litnlist litlist 
			{ problem->addRule(*new BasicRule($<ivalue>2, *nlist, *plist)); clean(); }
		| CONSTR_TYPE HEAD LITLISTCNT LITLISTNCNT BOUND litnlist litlist 
			{ problem->addRule(*new ConstrRule($<ivalue>2, $<ivalue>5, *nlist, *plist)); clean(); }
		| CHOICE_TYPE HEAD_CNT lithlist LITLISTCNT LITLISTNCNT litnlist litlist
			{ problem->addRule(*new ChoiceRule(*hlist, *nlist, *plist)); clean(); }
 		| WEIGHT_TYPE HEAD BOUND LITLISTCNT LITLISTNCNT litnlist litlist weightlist
			{ problem->addRule(*new WeightRule($<ivalue>2, $<ivalue>3, *nlist, *plist, *wlist)); clean(); }
		| MIN_TYPE UWORD LITLISTCNT LITLISTNCNT litnlist litlist weightlist 
			{ problem->addRule(*new MinRule($<ivalue>2, *nlist, *plist, *wlist)); clean(); }
		| BASIC_EXT_TYPE HEAD_CNT lithlist LITLISTCNT LITLISTNCNT litnlist litlist	
			{ problem->addRule(*new BasicExtRule(*hlist, *nlist, *plist)); clean(); }
		;

%%

namespace dynasp
{
	void LParseParser::error(const LParseParser::location_type &l, const std::string &m)
	{
		cerr << l << ": " << m << "; last parsed word: '" << this->problem->lexer()->YYText() << "'" << endl;
	}


}
