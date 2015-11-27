#include "AspInstance.hpp"

using namespace dynasp;

const std::string AspInstance::ATOM_NAME_NOT_FOUND = "";
const atom_t AspInstance::ATOM_NOT_FOUND = 0;

AspInstance::~AspInstance()
{
}
		
const rule_set& AspInstance::getCoveredRules(const atom_set& atoms) const
{
	rule_set *result = new rule_set();
	for (const AspRule &r : rules)
		if (r.isCovered(atoms))
			result->insert(r);
	return *result;
}

const rule_set& AspInstance::getRulesContaining(const atom_set& atoms) const
{
	rule_set *result = new rule_set();
	for (const AspRule &r : rules)
		if (r.contains(atoms))
			result->insert(r);
	return *result;
}

const atom_set& AspInstance::getBody(const AspRule& rule) const
{
	return rule.getBody();
}

const atom_set& AspInstance::getPositiveBody(const AspRule& rule) const
{
	return rule.getPositiveBody();
}

const atom_set& AspInstance::getNegativeBody(const AspRule& rule) const
{
	return rule.getNegativeBody();
}

const atom_set& AspInstance::getHead(const AspRule& rule) const
{
	return rule.getHead();
}

const atom_set& AspInstance::getAtoms(const AspRule& rule) const
{
	return rule.getAtoms();
}

bool AspInstance::isRuleSatisfied(const AspRule& rule, const atom_set& positiveAtoms, const atom_set& negativeAtoms) const
{
	return rule.isSatisfied(positiveAtoms, negativeAtoms);
}

int AspInstance::compareAtoms(const atom_t &atom1, const atom_t &atom2) const
{
	//TODO
	return atom1 - atom2;
}

void AspInstance::addRule(AspRule& rule)
{
	rules.insert(rule);	
}

void AspInstance::addVariableMapping(const atom_t& atom, const std::string& str)
{
	vmap[str] = atom;	
}

const atom_t& AspInstance::getAtom(const std::string& name) const
{
	variable_map::const_iterator i = vmap.find(name);
	if (i != vmap.end())
		return i->second;
	else
		return AspInstance::ATOM_NOT_FOUND;	
}

const std::string& AspInstance::getAtomName(const atom_t& atom) const
{
	for (const pair<std::string, atom_t> &p: vmap)
		if (p.second == atom)
			return p.first;
	return AspInstance::ATOM_NAME_NOT_FOUND;	
}




