#include <fstream>
#include "LParseProblem.hpp"
#include "LParseLexer.hpp"
#include <iostream>

using namespace std;
using namespace dynasp;

LParseProblem::LParseProblem(istream *stream)  //nr(0)//, compute(&computePlus)
{
	this->lpparser = new LParseParser(this);
	this->lplexer = new LParseLexer(stream, &cout);
	this->inst = nullptr;
}

LParseProblem::~LParseProblem()
{
	delete lpparser;
	delete lplexer;
}

LParseLexer *LParseProblem::lexer()
{
	return this->lplexer;
}

AspInstance *LParseProblem::getInstance()
{
	inst = new AspInstance();
	if (this->lpparser->parse())
		std::cerr << "parsing failed" << std::endl;
	return inst;
}

/*void LParseProblem::setNumber(const unsigned int n)
{
	nr = n;
} */

#define RULE_I(_idx, _r) _r = ruleI(_idx, &err); if (err) return "illegal rule, unexpected end";
#define RULE_I1(_idx) RULE_I(_idx, r);

#define CHECK(_s, _i) if (_i != _s) return "comparison within rule failed";
#define CHECK_NOT(_s, _i) if (_i == _s) return "incomparison within rule failed";

#define READ_ALL_NOT(_c, _v) for (unsigned int i = 0; i < _c; ++i) { RULE_I1(pos++); CHECK_NOT(r, _v); }

void LParseProblem::addRule(AspRule& rule)
{
	inst->addRule(rule);
	//std::cout << "type " << rule.getType() << std::endl;
//	return; 
/*
	//TODO: check rule
	unsigned int pos = 0;
	unsigned int r = 0, cnt = 0, head = 0, neg = 0;
	bool err = false;
	switch (type)
	{
		case ERT_BASIC:
			RULE_I(pos++, head);
			CHECK_NOT(head, ERT_ZERO);
			RULE_I(pos++, cnt);
			RULE_I(pos++, neg);
			READ_ALL_NOT(cnt, ERT_ZERO);
			CHECK(cnt + 3, currRule.size());
			break;
		case ERT_CONSTR:
			RULE_I(pos++, head);
			CHECK_NOT(head, ERT_ZERO);
			RULE_I(pos++, cnt);
			RULE_I(pos++, neg);
			RULE_I1(pos++);
			READ_ALL_NOT(cnt, ERT_ZERO);
			CHECK(cnt + 4, currRule.size());
			break;
		case ERT_CHOICE:
			RULE_I(pos++, head);
			READ_ALL_NOT(head, ERT_ZERO);
			RULE_I(pos++, cnt);
			RULE_I(pos++, neg);
			READ_ALL_NOT(cnt, ERT_ZERO);
			CHECK(cnt + head + 3, currRule.size());
			break;
		case ERT_WEIGHT:
			RULE_I(pos++, head);
			RULE_I1(pos++);
			RULE_I(pos++, cnt);
			RULE_I(pos++, neg);
			READ_ALL_NOT(cnt, ERT_ZERO);
			CHECK(cnt * 2 + 4, currRule.size());
			break;	
		case ERT_MIN:
			RULE_I(pos++, head);
			RULE_I(pos++, cnt);
			RULE_I(pos++, neg);
			READ_ALL_NOT(cnt, ERT_ZERO);
			RULE_I(pos++, cnt);
			RULE_I(pos++, neg);
			READ_ALL_NOT(cnt, ERT_ZERO);
			CHECK(cnt * 2 + 3, currRule.size());
			break;
		case ERT_EXT_BASIC:
			RULE_I(pos++, head);
			READ_ALL_NOT(head, ERT_ZERO);
			RULE_I(pos++, cnt);
			RULE_I(pos++, neg);
			READ_ALL_NOT(cnt, ERT_ZERO);
			CHECK(cnt + head + 3, currRule.size());
			break;
		default:
			return "illegal rule type";	
	}*/
	//TODO: store rule
	//rules.push_back(type, currRule);
	//currRule.clear();
}

/*void LParseProblem::addLitList(const E_LIT_TYPE type, const unsigned int n)
{
	std::cout << "addLit " << type << "," << n << std::endl;
}*/

/*void LParseProblem::beginLitList(const E_LIT_TYPE type)
{
	std::cout << "begLit " << type << std::endl;
}*/

/*void LParseProblem::addWeightList(const unsigned int w)
{
	std::cout << "addWgt " << "," << w << std::endl;
}*/

/*void LParseProblem::beginWeightList()
{
	std::cout << "begWgt " << std::endl;
}*/
/*
unsigned int LParseProblem::ruleI(unsigned int idx, bool* res)
{
	*res = idx >= currRule.size();
	if (*res)
		return ERT_ZERO;
	return currRule[idx];
}*/

void LParseProblem::addMapping(const atom_t& v, const std::string& str)
{
	//TODO: performance improve
	//if (mapping.find(str) != mapping.end())
	//	return "duplicate mapping entry";
	//mapping[str] = v;
	inst->addVariableMapping(v, str);
	//std::cout << str << " -> " << v << std::endl;
}

/*
CStr LParseProblem::addWordlist(unsigned int w)
{
	std::cout << "word: " << w << std::endl;
	currRule.push_back(w);
	return nullptr;
}

CStr LParseProblem::beginWordlist(unsigned int w)
{
	std::cout << "word: " << w << " beg" << std::endl;
	currRule.push_back(w);
	return nullptr;
}*/

/*void LParseProblem::addCompute(unsigned int c)
{
	std::cout << "comp " << c << std::endl;
	compute->push_back(c);
}*/

/*void LParseProblem::beginCompute()
{
	compute = &computeMinus;
	//if (compute->back() != ERT_ZERO)
	//	return "zero line required";
	std::cout << "begcomp" << std::endl;
}*/

