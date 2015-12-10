%{

#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif

#include "LParseBisonParser.hpp"
#include "LParseLexer.hpp"

#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include <cstddef>

#undef yylex
#define yylex lexer.lex

#define CHECK(name, pos, expect, actual)	\
	if((expect) != (actual))				\
	{										\
		std::ostringstream ss;				\
		ss << (name) << " was "	<< (expect)	\
		   << " but " << (actual)		\
		   << " in list";					\
		error((pos), ss.str());				\
		YYERROR;							\
	}

%}

%require "2.3"

%skeleton "lalr1.cc"

%define "parser_class_name" { LParseBisonParser } 
%name-prefix "dynasp"
%file-prefix "LParseBison"

%locations
%define parse.error verbose

%union
{
	std::string *string;
	std::size_t number;
	std::vector<std::size_t> *vector;
	IGroundAspRule *rule;
}

%token BP BM ZERO BASIC_TYPE CHOICE_TYPE CONSTR_TYPE WEIGHT_TYPE MIN_TYPE BASIC_EXT_TYPE
%token <string>	ID ERROR
%token <number> UWORD HEAD_CNT HEAD LITLISTCNT LITLISTNCNT LITLISTN LITLISTP LITLISTWEIGHT BOUND

%destructor { delete $$; } ID ERROR

%parse-param { LParseLexer &lexer } { IGroundAspInstance &instance }

%start lparse

%%

lparse	:	rules ZERO mapping ZERO BP compute ZERO BM compute ZERO UWORD
		;

rules	: 	rules rule
	  		{
				if($<rule>2) instance.addRule($<rule>2);
			}
		|
		;

hlist	: 	hlist head
	  		{
				$<vector>$->push_back($<number>2);
				instance.addAtom($<number>2);
			}
		|	{ $<vector>$ = new std::vector<std::size_t>(); }
		;

head	:	HEAD
	 		{
				$<number>$ = $<number>1 - 1;
				instance.addAtom($<number>$);
			}
		;

list	: 	list LITLISTP
	 		{
				$<vector>$->push_back($<number>2 - 1);
				instance.addAtom($<number>2 - 1);
			}
		|	{ $<vector>$ = new std::vector<std::size_t>(); }
		;

nlist	: 	nlist LITLISTN
	  		{
				$<vector>$->push_back($<number>2 - 1);
				instance.addAtom($<number>2 - 1);
			}
		|	{ $<vector>$ = new std::vector<std::size_t>(); }
		;

wlist	: 	wlist LITLISTWEIGHT
	  		{
				$<vector>$->push_back($<number>2);
			}
		|	{ $<vector>$ = new std::vector<std::size_t>(); }
		;

mapping	: 	mapping UWORD ID
			{
				instance.addAtomName($<number>2 - 1, *$<string>3);
			}
		| 
		;

compute	: 	compute UWORD
		|
		;

rule	: 	BASIC_TYPE head LITLISTCNT LITLISTNCNT nlist list
	 		{
				CHECK("#neg", @4, $<number>4, $<vector>5->size());
				CHECK(	"#lit - #neg",
						@3,
						$<number>3 - $<number>4,
						$<vector>6->size());

				$<rule>$ = Factory::createRule();

				$<rule>$->addHeadAtom($<number>2);
				for(atom_t atom : *$<vector>6)
					$<rule>$->addPositiveBodyAtom(atom);
				for(atom_t atom : *$<vector>5)
					$<rule>$->addNegativeBodyAtom(atom);

				delete $<vector>5;
				delete $<vector>6;
			}
		| 	CONSTR_TYPE head LITLISTCNT LITLISTNCNT BOUND nlist list
			{
				CHECK("#neg", @4, $<number>4, $<vector>6->size());
				CHECK(	"#lit - #neg",
						@3,
						$<number>3 - $<number>4,
						$<vector>7->size());

				$<rule>$ = Factory::createRule();

				$<rule>$->addHeadAtom($<number>2);
				for(atom_t atom : *$<vector>7)
					$<rule>$->addPositiveBodyAtom(atom);
				for(atom_t atom : *$<vector>6)
					$<rule>$->addNegativeBodyAtom(atom);
				$<rule>$->setMinimumBodyWeight($<number>5);

				delete $<vector>6;
				delete $<vector>7;
			}
		| 	CHOICE_TYPE HEAD_CNT hlist LITLISTCNT LITLISTNCNT nlist list
			{
				CHECK("#head", @2, $<number>2, $<vector>3->size());
				CHECK("#neg", @5, $<number>5, $<vector>6->size());
				CHECK(	"#lit - #neg",
						@4,
						$<number>4 - $<number>5,
						$<vector>7->size());

				$<rule>$ = Factory::createRule();

				$<rule>$->makeChoiceHead();
				for(atom_t atom : *$<vector>3)
					$<rule>$->addHeadAtom(atom);
				for(atom_t atom : *$<vector>7)
					$<rule>$->addPositiveBodyAtom(atom);
				for(atom_t atom : *$<vector>6)
					$<rule>$->addNegativeBodyAtom(atom);
	
				delete $<vector>3;
				delete $<vector>6;
				delete $<vector>7;
			}

 		| 	WEIGHT_TYPE head BOUND LITLISTCNT LITLISTNCNT nlist list wlist
			{
				CHECK("#neg", @5, $<number>5, $<vector>6->size());
				CHECK("#weights", @4, $<number>4, $<vector>8->size());
				CHECK(	"#lit - #neg",
						@4,
						$<number>4 - $<number>5,
						$<vector>7->size());

				$<rule>$ = Factory::createRule();

				int pos = 0;
				$<rule>$->addHeadAtom($<number>2);
				for(atom_t atom : *$<vector>6)
					$<rule>$->addNegativeBodyAtom(atom, $<vector>8->at(pos++));
				for(atom_t atom : *$<vector>7)
					$<rule>$->addPositiveBodyAtom(atom, $<vector>8->at(pos++));
				$<rule>$->setMinimumBodyWeight($<number>3);

				delete $<vector>6;
				delete $<vector>7;
				delete $<vector>8;
			}
		| 	MIN_TYPE UWORD LITLISTCNT LITLISTNCNT nlist list wlist
			{
				CHECK("#neg", @4, $<number>4, $<vector>5->size());
				CHECK(	"#lit - #neg",
						@3,
						$<number>3 - $<number>4,
						$<vector>6->size());
				CHECK("#lit (weights)", @3, $<number>3, $<vector>7->size());

				std::size_t listIndex, weightIndex = 0;
				for(listIndex = 0; listIndex < $<vector>5->size(); ++listIndex)
					instance.addWeight(
							(*$<vector>5)[listIndex],
							true,
							(*$<vector>7)[weightIndex++]);

				for(listIndex = 0; listIndex < $<vector>6->size(); ++listIndex)
					instance.addWeight(
							(*$<vector>6)[listIndex],
							false,
							(*$<vector>7)[weightIndex++]);

				delete $<vector>5;
				delete $<vector>6;
				delete $<vector>7;

				$<rule>$ = nullptr;
			}
		| 	BASIC_EXT_TYPE HEAD_CNT hlist LITLISTCNT LITLISTNCNT nlist list
			{
				CHECK("#head", @2, $<number>2, $<vector>3->size());
				CHECK("#neg", @5, $<number>5, $<vector>6->size());
				CHECK(	"#lit - #neg",
						@4,
						$<number>4 - $<number>5,
						$<vector>7->size());

				$<rule>$ = Factory::createRule();
			
				for(atom_t atom : *$<vector>3)
					$<rule>$->addHeadAtom(atom);
				for(atom_t atom : *$<vector>7)
					$<rule>$->addPositiveBodyAtom(atom);
				for(atom_t atom : *$<vector>6)
					$<rule>$->addNegativeBodyAtom(atom);
	
				delete $<vector>3;
				delete $<vector>6;
				delete $<vector>7;
			}
		;

%%

namespace dynasp
{
	void LParseBisonParser::error(
		const LParseBisonParser::location_type &l,
		const std::string &m)
	{
		std::cerr << l << ": " << m << "; last parsed word: '"
			<< lexer.YYText() << "'" << std::endl;
	}
}
