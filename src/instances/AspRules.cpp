#include "AspRules.hpp"
#include <cassert>

using namespace dynasp;

AspRule::AspRule(E_RULE_TYPE rtype) : type(rtype), pos(nullptr), neg(nullptr), head(nullptr), 
					body(nullptr), atoms(nullptr)  { }
		
AspRule::~AspRule() 
{ 
	delete pos; 
	delete neg; 
	delete head;
	delete body;
	delete atoms; 
}
	
const AspRule::E_RULE_TYPE AspRule::getType() const 
{ 
	return type; 
}	

const atom_set& AspRule::getPositiveBody() const
{ 
	return *pos; 
}

const atom_set& AspRule::getNegativeBody() const
{ 
	return *neg; 
}

const atom_set& AspRule::getHead() const
{ 
	return *head; 
}
		
const atom_set& AspRule::getBody() const // die atome die im body von rule vorkommen
{ 
	return *body;
}

const atom_set& AspRule::getAtoms() const // die atome die in rule vorkommen
{ 
	return *atoms; 
}

bool AspRule::isSatisfied(const atom_set& positiveAtoms, const atom_set& negativeAtoms) const // true wenn rule mit diesem assignment satisfied; false wenn nicht, oder wenn unbekannt/zu wenig information
{
	for (const atom_t& t : *pos)
		if (positiveAtoms.find(t) == positiveAtoms.end())
			return false;
	for (const atom_t& t : *neg)
		if (negativeAtoms.find(t) == negativeAtoms.end())
			return false;
	return true; 
}

bool AspRule::contains(const atom_set &atoms) const //atoms ein subset von body(r)+head(r) ?;
{
	return isSubset(atoms, *this->atoms);
}

bool AspRule::isCovered(const atom_set &atoms) const //body(r)+head(r) ein subset von atoms=
{
	return isSubset(*this->atoms, atoms);
}
	
bool AspRule::operator < (const AspRule& other) const
{ 
	return this < &other; 
}

void AspRule::computeAtoms()
{
	assert(body == nullptr);
	assert(atoms == nullptr);
	body = new atom_set();
	for (const atom_t& t : *pos)
		body->insert(t);	
	for (const atom_t& t : *neg)
		body->insert(t);	

	atoms = new atom_set();
	for (const atom_t& t : *body)
		atoms->insert(t);	
	for (const atom_t& t : *head)
		atoms->insert(t);	

}

void AspRule::newHead(const atom_t& h)
{ 
	head = new atom_set(); 
	head->insert(h); 
}

BasicRule::BasicRule(atom_t& head, atom_set& atneg, atom_set& atpos) : 
			AspRule(ERT_BASIC) 
{ 
	newHead(head); 
	neg = &atneg; 
	pos = &atpos; 
	computeAtoms();
}

BasicRule::BasicRule(atom_set& atneg, atom_set& atpos) : 
			AspRule(ERT_BASIC) 
{ 
	neg = &atneg; 
	pos = &atpos;
	//computeAtoms(): 
}
		
BoundRule::BoundRule(E_RULE_TYPE rtype): AspRule(ERT_CONSTR) { }

ConstrRule::ConstrRule(const atom_t& head, bound_t upperbound, atom_set& atneg, atom_set& atpos) : 
		BoundRule(ERT_CONSTR) 
{ 
	newHead(head); 
	neg = &atneg; 
	pos = &atpos;
	bound = upperbound;
	computeAtoms();
}

ChoiceRule::ChoiceRule(atom_set& athead, atom_set& atneg, atom_set& atpos) : 
			AspRule(ERT_CHOICE) 
{ 
	head = &athead; 
	neg = &atneg; 
	pos = &atpos;
	computeAtoms();
}
	
WeightRule::WeightRule(const atom_t& head, bound_t upperbound, atom_set& atneg, atom_set& atpos, weight_list& wlist) : 
			BoundRule(ERT_WEIGHT), weights(&wlist) 
{ 
	neg = &atneg; 
	pos = &atpos; 
	bound = upperbound; 
	newHead(head);
	computeAtoms(); 
}
	
WeightRule::~WeightRule()
{
	delete weights; 
}

WeightRule::WeightRule(bound_t upperbound, atom_set& atneg, atom_set& atpos, weight_list& wlist) : 
			BoundRule(ERT_WEIGHT), weights(&wlist) 
{ 
	neg = &atneg; 
	pos = &atpos; 
	bound = upperbound;
	computeAtoms(); 
}

MinRule::MinRule(unsigned int dummy, atom_set& atneg, atom_set& atpos, weight_list& wlist) : 
			WeightRule(dummy, atneg, atpos, wlist)  
{
	type = ERT_MIN;
}

BasicExtRule::BasicExtRule(atom_set& athead, atom_set& atneg, atom_set &atpos) : 
			BasicRule(atneg, atpos) 
{ 
	type = ERT_EXT_BASIC; 
	head = &athead;
}

