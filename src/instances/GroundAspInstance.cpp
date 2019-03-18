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
#include <sstream>

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

	GroundAspInstance::GroundAspInstance() : maxAtom_(1), negativesComputed_(false), answers_(0), aset_(nullptr) { }

	GroundAspInstance::~GroundAspInstance()
	{
		for(IGroundAspRule *rule : rules_)
			delete rule;
	}

	unsigned int GroundAspInstance::getAtomCount() const
	{
		return maxAtom_;
	}

	#define DYNW "dynW("

	void GroundAspInstance::addAtomName(atom_t atom, const string &name)
	{
		//std::cout << atom << "," << name << std::endl;
	#ifdef USE_DYNW_WEIGHTS
		std::size_t found = std::string::npos;
		//std::cout << atom << "," << name << std::endl;
		if ((found = name.find(DYNW)) != std::string::npos)
		{
			//std::cout << "found dynW(" << std::endl;
			std::size_t found_end = name.find_last_of(",");
			if (found_end != std::string::npos)
			{
				float fval = 0;
				try
				{
					std::string val = name.substr(found_end + 1, name.length() - found_end - 2);
					val.replace(0, 1, 1, '.');
					fval = std::stof(val);
					//std::cout  << fval << std::endl;
				}
				catch (std::logic_error&)
				{
				}


				found += strlen(DYNW);
				std::string atname = name.substr(found, found_end - found);
				//const auto itatom = atomNames_.find(atname);

				//TODO!
				auto itatom = atomNames_.begin();
				for (; itatom != atomNames_.end(); ++itatom)
					if (itatom->second == atname)
						break;
			
				if (itatom != atomNames_.end())
				{
					std::cout << "now: " << itatom->first << ":" << fval << std::endl;
					costs_.emplace(itatom->first, fval);
				}
				else
				{
					tmpcosts_.emplace(atname, fval);
				}
			}
		}
		else
		{
			const auto itname = tmpcosts_.find(name);
			if (itname != tmpcosts_.end())
			{
				std::cout << "later: " << atom << ":" << itname->second << std::endl;
				costs_.emplace(atom, itname->second);
				tmpcosts_.erase(itname);
			}
			atomNames_.emplace(atom, name);
		}
	#else
		atomNames_.emplace(atom, name);
	#endif
	}

	

	void GroundAspInstance::addAtom(atom_t atom)
	{
		if(maxAtom_ < atom) maxAtom_ = atom;
	}

	void GroundAspInstance::addRule(IGroundAspRule *rule)
	{
		if(!rule)
			throw std::invalid_argument("Argument 'rule' cannot be null!");

		/*if (rule->isFact())
			std::cout << "fact" << std::endl;*/
		rules_.push_back(rule);
	}

	void GroundAspInstance::fetchDependencyGraph(htd::DirectedGraph& graph) const
	{
		graph.addVertices(maxAtom_);
		for(IGroundAspRule *rule : rules_)
		{
			rule->updateDependencyGraph(graph);
		}
	}

	void GroundAspInstance::cleanTmps()
	{
		tmpcosts_.clear();	
	}

	float GroundAspInstance::cost(const atom_vector &
	#ifdef USE_DYNW_WEIGHTS
		trueAtoms
	#endif
		,
				const atom_vector &
	#ifdef USE_DYNW_WEIGHTS			
		falseAtoms
	#endif	
		, const TreeNodeInfo& 
	#ifdef USE_DYNW_WEIGHTS	
		info
	#endif	
		) const
	{
		float weight = 1;
	#ifdef USE_DYNW_WEIGHTS

		unordered_map<atom_t, float>::const_iterator it;
#ifdef INT_ATOMS_TYPE
//TODO: make efficient
		for (size_t p = 0; p < info.introducedAtoms.size() + info.rememberedAtoms.size(); ++p)
		{
			const atom_t atom = p >= info.introducedAtoms.size() ? info.rememberedAtoms[p - info.introducedAtoms.size()] : info.introducedAtoms[p];
			if ((TO_INT(trueAtoms) >> p) & 1)		//true Atom
			{
				if ((it = costs_.find(atom))
						!= costs_.end())
					weight *= it->second;
			}
			else if ((TO_INT(falseAtoms) >> p) & 1)
			{
				if ((it = costs_.find(atom))
						!= costs_.end())
					weight *= (1-it->second);
			}
		}
#else
		for(atom_t atom : trueAtoms)
			if((it = costs_.find(atom))
					!= costs_.end())
				weight *= it->second;
		for(atom_t atom : falseAtoms)
			if((it = costs_.find(atom))
					!= costs_.end())
				weight *= (1-it->second);
#endif
	#endif
		//std::cout << weight << std::endl;
		return weight;

	}

	bool GroundAspInstance::isConstrainedFactAtom(htd::vertex_t vertex)
	{
		//return false;
		if (!dynasp::create::reductSpeedup()) //speed) // || create::get() == create::INCIDENCEPRIMAL_RULESETTUPLE)
			return false;
		//std::cout << "isNegatedChoiceAtom " << vertex << std::endl;
		if (!negativesComputed_)
			isNegatedAtom(vertex);
		return constrainedFactAtoms_.find(vertex) != constrainedFactAtoms_.end();
	}

	bool GroundAspInstance::isConstrainedNFactAtom(htd::vertex_t vertex)
	{
		//return false;
		if (!dynasp::create::reductSpeedup()) //speed) // || create::get() == create::INCIDENCEPRIMAL_RULESETTUPLE)
			return false;
		//std::cout << "isNegatedChoiceAtom " << vertex << std::endl;
		if (!negativesComputed_)
			isNegatedAtom(vertex);
		return constrainedNFactAtoms_.find(vertex) != constrainedNFactAtoms_.end();
	}

	bool GroundAspInstance::isProjectionAtom(htd::vertex_t vertex)
	{
		//TODO!
		return isNegatedChoiceAtom(vertex);
	}

	bool GroundAspInstance::isNegatedChoiceAtom(htd::vertex_t vertex)
	{
		//return false;
		if (!dynasp::create::reductSpeedup()) //speed) // || create::get() == create::INCIDENCEPRIMAL_RULESETTUPLE)
			return false;
		//std::cout << "isNegatedChoiceAtom " << vertex << std::endl;
		if (!negativesComputed_)
			isNegatedAtom(vertex);
		return negativeChoices_.find(vertex) != negativeChoices_.end();
	}



	bool GroundAspInstance::isNegatedAtom(htd::vertex_t vertex)
	{
		//return false;
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
			{
				if ((rule->hasChoiceHead() && rule->isHeadAtom(v) && !rule->isSimpleRule()) ||
				(/*false*/true && rule->isNegativeBodyAtom(v))
				/*||
				negativeAtomWeights_.find(v) != negativeAtomWeights_.end() ||
				((positiveAtomWeights_.find(v) != positiveAtomWeights_.end())
				|| rule->isNegWeightedAtom(v)
				|| rule->isPosWeightedAtom(v) )*/
				
				)
				{
					DBG("NEGATIVE: "); DBG(v); DBG(std::endl);

					//std::cout << "N: " << rule->toString() << std::endl;
					negatives_.emplace(v);
					tmpres = true;
				}
				else if (rule->hasChoiceHead() && rule->isHeadAtom(v) && rule->isSimpleRule())	//simple rule
				{
					DBG("NEG CHOICES: "); DBG(v); DBG(std::endl);
					//std::cout << rule->toString() << std::endl;
					//std::cout << "NEG CHOICES: " << (v) << (std::endl);
					negativeChoices_.emplace(v);
				}
				if (rule->isConstrainedFact() && rule->isNegativeBodyAtom(v))
				{
					//std::cout << rule->toString() << std::endl;
					DBG("CONS NEG FACT: "); DBG(v); DBG(std::endl);
					constrainedFactAtoms_.emplace(v);
				}
				else if (rule->isConstrainedNFact() && rule->isPositiveBodyAtom(v))
				{
					DBG("CONS FACT: "); DBG(v); DBG(std::endl);
					constrainedNFactAtoms_.emplace(v);
				}
				/*else
					std::cout << "OTH: " << rule->toString() << std::endl;*/
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

	bool GroundAspInstance::hasNodeData(size_t node) const 
	{
		return nodeData.count(node) >= 1;
	}

	TreeNodeInfo& GroundAspInstance::getNodeData(size_t node)
	{
		//std::cout << "get " << node << std::endl;
		auto it = nodeData.find(node);
		assert(it != nodeData.end());
		return it->second;
	}
//#endif


	std::string GroundAspInstance::toQBFString() const
	{
	//#define DBGV(X) X
	#define DBGV(X) ""
		unsigned int vcnt = 1;
		std::stringstream ss;
		ss << "#QCIR-G14" << std::endl;
		ss << "forall(";
		unsigned int i = 0, dupi = 0;
		for (i = 0; i < maxAtom_; ++i)
		{
			if (i > 0)
				ss << ",";
			ss << DBGV("a") << vcnt++;
		}
		ss << ")" << std::endl;

		dupi = vcnt + rules_.size() + 2 - 1;
		ss << "exists(";
		std::vector<atom_t> positives;
		for (i = 0; i < maxAtom_; ++i)
		{
			if (negatives_.count(i) == 0)
			{
				if (positives.size())
					ss << ",";
				ss << DBGV("aa") << dupi + (i + 1);
				positives.emplace_back(i + 1);
			}
		}
		ss << ")" << std::endl;
		ss << "output(" << DBGV("nm") << vcnt << ")" << std::endl;
		ss << DBGV("nm") << vcnt++;
		ss << " = or(" << DBGV("ex") << vcnt++ << ",";
		i = 0;
		for (IGroundAspRule *rule : rules_)
		{
			(void) rule; //struct rule;
			if (i > 0)
				ss << ",";
			ss << "-" << DBGV("r") << vcnt++;
			i++;
		}
		ss << ")" << std::endl;
		//dupi = vcnt - 1;
		for (IGroundAspRule *rule : rules_)
		{
			ss << DBGV("r") << (vcnt - i--) << " = or(" << rule->toQBF(false,dupi) << ")" << std::endl;	
		}
		ss << DBGV("ex") << (vcnt - rules_.size() - 1); //<< " = exists("; //<< std::endl;
		//std::vector<atom_t> positives;
		//unsigned int j = 1;
		vcnt += positives.size();
		/*for (i = 0; i < maxAtom_; ++i)
		{
			if (negatives_.count(i) == 0)
			{
				//if (positives.size())
				//	ss << ",";
				//ss << DBGV("aa") << vcnt++;
				vcnt++;
				positives.emplace_back(i + 1);
			}
		}*/
		//ss << "; " << DBGV("exf") << vcnt << ")" << std::endl;
		//ss << DBGV("exf") << vcnt++;
		ss << " = and(" << DBGV("impss") << vcnt++ << ",";
		for (i = 0; i < positives.size(); ++i)
		{
			if (i > 0)
				ss << ",";
			ss << DBGV("imp") << vcnt++; //positives[i]; // << "," << "imps" << positives[i];
		}
		ss << "," << DBGV("prog") << vcnt++ << ")" << std::endl;
		ss << DBGV("impss") << (vcnt - positives.size() - 2) << " = or(";
		for (i = 0; i < positives.size(); ++i)
		{
			if (i > 0)
				ss << ",";
			ss << DBGV("imps") << vcnt++; // << "," << "imps" << positives[i];
		}
		ss << ")" << std::endl;
		for (i = 0; i < positives.size(); ++i)
		{
			ss << DBGV("imp") << (vcnt - 2 * positives.size() - 1 + i)  << " = or(-" << DBGV("aa") << (dupi + positives[i]) << "," << DBGV("a") << positives[i] << ")" << std::endl;
			ss << DBGV("imps") << (vcnt - positives.size() + i) << " = and(-" << DBGV("aa") << (dupi + positives[i]) << "," << DBGV("a") << positives[i] << ")" << std::endl;
		}
		ss << DBGV("prog") << (vcnt - positives.size() - 1) << " = and(";
		i = 0;
		for (IGroundAspRule *rule : rules_)
		{
			(void) rule; //struct rule;
			if (i > 0)
				ss << ",";
			ss << DBGV("rr") << vcnt++;
			++i;
		}
		ss << ")" << std::endl;
		for (IGroundAspRule *rule : rules_)
		{
			ss << DBGV("rr") << (vcnt - i--) << " = or(" << rule->toQBF(true, dupi) << ")" << std::endl;	
		}
		return std::move(ss.str());
	}

	std::string GroundAspInstance::toString() const
	{
		std::stringstream ss;
		if (positiveAtomWeights_.size() || negativeAtomWeights_.size())
		{
			ss << "#minimize { ";
			bool first = true;
			for (auto tp : positiveAtomWeights_)
			{
				if (!first)
					ss << " ; ";
				ss << tp.second << ", a" << tp.first << " : a" << tp.first;
				first = false;
			}
			first = true;
			for (auto tp : negativeAtomWeights_)
			{
				if (!first)
					ss << " ; ";
				ss << tp.second << ", a" << tp.first << " : not a" << tp.first;
				first = false;
			}
			ss << " } .";
		}
		return ss.str();
	}

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
