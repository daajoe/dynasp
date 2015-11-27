#ifndef LOGICPROGRAMPROBLEM_H_
#define LOGICPROGRAMPROBLEM_H_


#include "AspInstance.hpp"
#include <iostream>
#include <map>
#include <list>
#include <vector>

//using namespace sharp;

namespace dynasp
{
	typedef const char* const CStr;

	class LParseParser;
	class LParseLexer;

	class LParseProblem 
	{

		friend class LParseParser;

	public:
		enum E_RULE_TYPE { ERT_ZERO = 0, ERT_BASIC, ERT_CONSTR, ERT_CHOICE, ERT_FOUR, ERT_WEIGHT, ERT_MIN, ERT_SEVEN, ERT_EXT_BASIC, ERT_COUNT };
		enum E_LIT_TYPE { ELT_HEAD, ELT_POS, ELT_NEG, ELT_COUNT };

		LParseProblem(std::istream *stream) ;
		virtual ~LParseProblem();
		
		virtual AspInstance* getInstance();
		LParseLexer *lexer();

	private:
	//	void addLitList(const E_LIT_TYPE type, const unsigned int n);
		//void addWeightList(const unsigned int w);

		//void setNumber(const unsigned int n);
		void addRule(AspRule& rule);

		void addMapping(const atom_t& v, const std::string& str); 

		//void addCompute(unsigned int c);
	//private:
	//	unsigned int ruleI(unsigned int idx, bool* res);

		//unsigned int nr;
		//std::map<std::string, unsigned int> mapping;
		//std::list<std::list<unsigned int> > rules; 
		
		//std::vector<unsigned int> currRule;
		//std::list<unsigned int> currWordlist;

		/*std::list<unsigned int> computePlus;
		std::list<unsigned int> computeMinus;
		
		std::list<unsigned int>* compute;*/
	
		LParseParser *lpparser;
		LParseLexer  *lplexer;
		AspInstance  *inst;
		//int rulenum;
	};
} // dynasp

#endif //DATALOGPROBLEM_H_
