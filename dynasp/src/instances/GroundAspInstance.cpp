#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif
#include "../util/debug.hpp"

#include "GroundAspInstance.hpp"

#include "GroundAspRule.hpp"

#include <dynasp/create.hpp>

#include <utility>
#include <stdexcept>

namespace dynasp
{
	using htd::IHypergraph;
	using htd::IMutableHypergraph;
	using htd::vertex_t;
		
	using std::string;
	using std::size_t;
	using std::make_pair;
	using std::pair;
	using std::unordered_map;
	using std::unique_ptr;

	GroundAspInstance::GroundAspInstance() : maxAtom_(1) { }

	GroundAspInstance::~GroundAspInstance()
	{
		for(IGroundAspRule *rule : rules_)
			delete rule;
	}

	void GroundAspInstance::addAtomName(atom_t atom, const string &name)
	{
		atomNames_[atom] = name;
	}

	void GroundAspInstance::addAtom(atom_t atom)
	{
		if(maxAtom_ < atom) maxAtom_ = atom;
	}

	void GroundAspInstance::addRule(IGroundAspRule *rule)
	{
		if(!rule)
			throw std::invalid_argument("Argument 'rule' cannot be null!");

		rules_.push_back(rule);
	}

	void GroundAspInstance::addWeight(atom_t atom, bool negated, size_t weight)
	{
		if(negated) negativeAtomWeights_[atom] += weight;
		else positiveAtomWeights_[atom] += weight;
	}

	IHypergraph *GroundAspInstance::toHypergraph() const
	{
		unique_ptr<IHypergraphConverter> converter(
				create::hypergraphConverter());

		return converter->convert(*this);
	}

	bool GroundAspInstance::isRule(vertex_t vertex) const
	{
		return vertex > maxAtom_ && vertex < maxAtom_ + 1 + rules_.size();
	}

	bool GroundAspInstance::isAtom(vertex_t vertex) const
	{
		return vertex <= maxAtom_;
	}

	const IGroundAspRule &GroundAspInstance::rule(rule_t rule) const
	{
		if(!isRule(rule))
			throw std::invalid_argument("Argument 'rule' is not a rule.");

		return *rules_[rule - maxAtom_ - 1];
	}

	size_t GroundAspInstance::weight(
			const atom_vector &trueAtoms,
			const atom_vector &falseAtoms) const
	{
		size_t weight = 0;
		unordered_map<atom_t, size_t>::const_iterator it;
		for(atom_t atom : trueAtoms)
			if((it = positiveAtomWeights_.find(atom))
					!= positiveAtomWeights_.end())
				weight += it->second;
		for(atom_t atom : falseAtoms)
			if((it = negativeAtomWeights_.find(atom))
					!= negativeAtomWeights_.end())
				weight += it->second;
		return weight;
	}

}// namespace dynasp
