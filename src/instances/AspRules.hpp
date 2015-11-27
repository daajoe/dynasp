#ifndef DYNASP_ASP_RULES_HPP
#define DYNASP_ASP_RULES_HPP

#include <set>
#include <list>
#include <algorithm>

using namespace std;

namespace dynasp
{
	class AspRule;

        //typedef AspRule AspRule;
        typedef unsigned int atom_t;
        typedef unsigned int bound_t;
        typedef unsigned int weight_t;

        typedef set<AspRule> rule_set; // den datentyp kannst du gerne ändern wenn du willst (in ein unordered_set vllt? hab ich mir noch nicht überlegt was am besten ist)
        typedef set<atom_t> atom_set; // selbiges hier
        typedef list<weight_t> weight_list; // selbiges hier

	//returns whether s1 \subseteq s2
	template <class T>
	static bool isSubset(const set<T>& s1, const set<T>& s2) 
	{
		return std::includes(s2.begin(), s2.end(), s1.begin(), s1.end());
	}

	class AspRule
	{
	public:
		enum E_RULE_TYPE { ERT_ZERO = 0, ERT_BASIC, ERT_CONSTR, ERT_CHOICE, 
			ERT_FOUR, ERT_WEIGHT, ERT_MIN, ERT_SEVEN, ERT_EXT_BASIC, ERT_COUNT };

		AspRule(E_RULE_TYPE rtype);
		// implementieren, aber nicht verändern:
		virtual ~AspRule(); 
	
		const E_RULE_TYPE getType() const; 
		const atom_set& getPositiveBody() const;
		const atom_set& getNegativeBody() const;
		const atom_set& getHead() const;
		
		const atom_set& getBody() const; // die atome die im body von rule vorkommen
		const atom_set& getAtoms() const; // die atome die in rule vorkommen

		bool contains(const atom_set &atoms) const; //atoms ein subset von body(r)+head(r) ?;
		bool isCovered(const atom_set &atoms) const; //body(r)+head(r) ein subset von atoms=

		bool isSatisfied(const atom_set& positiveAtoms, const atom_set& negativeAtoms) const; // true wenn rule mit diesem assignment satisfied; false wenn nicht, oder wenn unbekannt/zu wenig information
		
		bool operator < (const AspRule& other) const;

	protected:
		E_RULE_TYPE type;

		// TODO: maybe use better, continuous representation without duplicate store, like:
		// [ h_1, h_2, ..., h_k, MARKER, p_1, p_2, ..., p_l, MARKER, n_1, n_2, ..., n_m ]
		//		            ^                          ^                        
			
		atom_set *pos, *neg, *head;
		atom_set *body, *atoms;

		void newHead(const atom_t& h);
		void computeAtoms();
	};

	class BasicRule : public AspRule 
	{
	public:
		BasicRule(atom_t& head, atom_set& atneg, atom_set& atpos);
	protected:
		BasicRule(atom_set& atneg, atom_set& atpos); 
	};

	class BoundRule : public AspRule 
	{
	public:
		BoundRule(E_RULE_TYPE rtype);
	protected:
		bound_t bound;
	};

	class ConstrRule : public BoundRule 
	{
	public:
		ConstrRule(const atom_t& head, bound_t upperbound, 
				atom_set& atneg, atom_set& atpos);
	};

	class ChoiceRule : public AspRule 
	{
	public:
		ChoiceRule(atom_set& athead, atom_set& atneg, atom_set& atpos);
	private:
	};

	class WeightRule : public BoundRule 
	{
	public:
		WeightRule(const atom_t& head, bound_t upperbound, 
				atom_set& atneg, atom_set& atpos, weight_list& wlist);
	
		virtual ~WeightRule();
	protected:
		WeightRule(bound_t upperbound, atom_set& atneg, 
				atom_set& atpos, weight_list& wlist);

		weight_list *weights;
	};

	class MinRule : public WeightRule
	{
	public:
		MinRule(unsigned int dummy, atom_set& atneg, 
				atom_set& atpos, weight_list& wlist);
	};

	class BasicExtRule : public BasicRule
	{
	public:
		BasicExtRule(atom_set& athead, atom_set& atneg, 
				atom_set &atpos);
	};

}

#endif // DYNASP_ASPINSTANCE_HPP_
