#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif
#include "../util/debug.hpp"

#include "GroundAspInstance.hpp"

#include "GroundAspRule.hpp"

#include <dynasp/create.hpp>
#include <dynasp/TreeNodeInfo.hpp>

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

	GroundAspInstance::GroundAspInstance() : maxAtom_(1), negativesComputed_(false) { }

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

    std::size_t GroundAspInstance::getNumAtoms(){
        return atomNames_.size();
    }

    std::size_t GroundAspInstance::getNumRules(){
        return rules_.size();
    }

    std::size_t GroundAspInstance::getNumConstraints(){
        int i = 0;
        for (auto &&rule :rules_) {
            if(rule->hasConstraint()){
                i++;
            }
        }
        return i;
    }
    std::size_t GroundAspInstance::getNumBinaryConstraints(){
        int i = 0;
        for (auto &&rule :rules_) {
            if(rule->hasBinaryConstraint()){
                i++;
            }
        }
        return i;
    }
    std::size_t GroundAspInstance::getNumTernaryConstraints(){
        int i = 0;
        for (auto &&rule :rules_) {
            if(rule->hasTernaryConstraint()){
                i++;
            }
        }
        return i;
    }
    std::size_t GroundAspInstance::getNumOtherConstraints(){
        int i = 0;
        for (auto &&rule :rules_) {
            if(rule->hasOtherConstraint()){
                i++;
            }
        }
        return i;
    }


    std::size_t GroundAspInstance::getNumCardinalityRules(){
        int i = 0;
        for (auto &&rule :rules_) {
            if(rule->hasCardinalityBody()){
                i++;
            }
        }
        return i;
    }

    std::size_t GroundAspInstance::getNumChoiceRules() {
        int i = 0;
        for (auto &&rule :rules_) {
            if (rule->hasChoiceHead()) {
                i++;
            }
        }
        return i;
    }

    std::size_t GroundAspInstance::getNumDisjunctiveRules() {
        int i = 0;
        for (auto &&rule :rules_) {
            if (rule->hasDisjunctiveHead()) {
                i++;
            }
        }
        return i;
    }

    std::size_t GroundAspInstance::getNumNormalRules(){
        int i = 0;
        for (auto &&rule :rules_) {
            if(rule->hasNormalHead()){
                i++;
            }
        }
        return i;
    }

    std::size_t GroundAspInstance::getNumWeightedRules(){
        int i = 0;
        for (auto &&rule :rules_) {
            if(rule->hasWeightedBody()){
                i++;
            }
        }
        return i;
    }

    bool GroundAspInstance::isNegatedAtom(htd::vertex_t vertex)
	{
		if (!dynasp::create::reductSpeedup()) //speed) // || create::get() == create::INCIDENCEPRIMAL_RULESETTUPLE)
			return true;
		//std::cout << "isNegatedAtom " << vertex << std::endl;
		if (negativesComputed_)
			return negatives_.find(vertex) != negatives_.end();

		//std::cout << "MAXATOM " << maxAtom_ << std::endl;
		bool result = false;
		for (unsigned int v = 1; v <= maxAtom_; ++v)
		{
			
			//negatives_.emplace(v);
			
			bool tmpres = false;
			for(IGroundAspRule *rule : rules_)
				if ((rule->hasChoiceHead() && rule->isHeadAtom(v)) ||
				(rule->isNegativeBodyAtom(v))
				/*||
				negativeAtomWeights_.find(v) != negativeAtomWeights_.end() ||
				((positiveAtomWeights_.find(v) != positiveAtomWeights_.end())
				|| rule->isNegWeightedAtom(v)
				|| rule->isPosWeightedAtom(v) )*/
				
				)
				{
					DBG("NEGATIVE: "); DBG(v); DBG(std::endl);
					negatives_.emplace(v);
					tmpres = true;
				}
			if (v == vertex)
				result = tmpres;
		}
		negativesComputed_ = true;
		return result;
	}

	void GroundAspInstance::setSpeedup(bool)
	{
		//speed = s;
		isNegatedAtom(1);
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

 //#ifdef INT_ATOMS_TYPE
	void GroundAspInstance::setNodeData(size_t node, TreeNodeInfo&& data)
	{
		//std::cout << "set " << node << std::endl;
		nodeData.emplace(node, data);
	}

	/*htd::atom_vector GroundAspInstance::getNodeBag(size_t node)
	{
		NodeData& nd = nodeData[node];
		return nd.int_introducedAtoms | nd.int_rememberedAtoms;
	}*/

	const TreeNodeInfo& GroundAspInstance::getNodeData(size_t node) const
	{
		//std::cout << "get " << node << std::endl;
		const auto it = nodeData.find(node);
		assert(it != nodeData.end());
		return it->second;
	}

	TreeNodeInfo& GroundAspInstance::getNodeData(size_t node)
	{
		//std::cout << "get " << node << std::endl;
		auto it = nodeData.find(node);
		assert(it != nodeData.end());
		return it->second;
	}
//#endif


	size_t GroundAspInstance::weight(
			const atom_vector &trueAtoms,
			const atom_vector &falseAtoms, const TreeNodeInfo& info) const
	{
		size_t weight = 0;
		unordered_map<atom_t, size_t>::const_iterator it;
#ifdef INT_ATOMS_TYPE
//TODO: make efficient
		for (size_t p = 0; p < info.introducedAtoms.size() + info.rememberedAtoms.size(); ++p)
		{
			const atom_t atom = p >= info.introducedAtoms.size() ? info.rememberedAtoms[p - info.introducedAtoms.size()] : info.introducedAtoms[p];
			if ((TO_INT(trueAtoms) >> p) & 1)		//true Atom
			{
				if ((it = positiveAtomWeights_.find(atom))
						!= positiveAtomWeights_.end())
					weight += it->second;
			}
			else if ((TO_INT(falseAtoms) >> p) & 1)
			{
				if ((it = negativeAtomWeights_.find(atom))
						!= negativeAtomWeights_.end())
					weight += it->second;
			}
		}
#else
		for(atom_t atom : trueAtoms)
			if((it = positiveAtomWeights_.find(atom))
					!= positiveAtomWeights_.end())
				weight += it->second;
		for(atom_t atom : falseAtoms)
			if((it = negativeAtomWeights_.find(atom))
					!= negativeAtomWeights_.end())
				weight += it->second;
#endif
		return weight;
	}



}// namespace dynasp
