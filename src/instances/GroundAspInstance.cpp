#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif

#include "GroundAspInstance.hpp"

#include "GroundAspRule.hpp"

#include <utility>
#include <stdexcept>

namespace dynasp
{
	using htd::IHypergraph;
	using htd::IMutableHypergraph;
	using htd::hyperedge_t;
		
	using std::string;
	using std::size_t;
	using std::make_pair;

	using htd::vertex_t;

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
		//FIXME: implement dependency injection light
		IMutableHypergraph *ret = this->createEmptyHypergraph();

		//FIXME: use IInstanceToHypergraphConverter, instead of hard-coded
		//		 graph structure
		for(IGroundAspRule * const &rule : rules_)
		{
			hyperedge_t edge;
			edge.push_back(ret->addVertex());

			for(const atom_t &atom : *rule)
				edge.push_back(atom);

			ret->addEdge(edge);
		}

		return ret;
	}

	IGroundAspRule *GroundAspInstance::createEmptyRule(
			IGroundAspRule::Type type) const
	{
		//FIXME: Implement dependency injection light
		return new GroundAspRule(type);
	}

	bool GroundAspInstance::isRule(vertex_t vertex) const
	{
		return vertex > maxAtom_;
	}

	bool GroundAspInstance::isAtom(vertex_t vertex) const
	{
		return vertex <= maxAtom_;
	}

	IMutableHypergraph *GroundAspInstance::createEmptyHypergraph() const
	{
		//FIXME: implement dependency injection light
		return new htd::MutableHypergraphImpl(maxAtom_);
	}

}// namespace dynasp
