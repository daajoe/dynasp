#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif

#include "ClassicDynAspTuple.hpp"

#include <stack>

#include "../debug.cpp"

namespace dynasp
{
	using std::size_t;
	using std::unordered_set;
	using std::stack;

	ClassicDynAspTuple::ClassicDynAspTuple() : weight_(0), solutions_(1)  { }

	ClassicDynAspTuple::~ClassicDynAspTuple() { }

	bool ClassicDynAspTuple::merge(const IDynAspTuple &tuple)
	{
		const ClassicDynAspTuple &mergee = 
			static_cast<const ClassicDynAspTuple &>(tuple);

		if(atoms_ != mergee.atoms_) return false;
		if(rules_ != mergee.rules_) return false;
		if(certificates_ != mergee.certificates_) return false;

		solutions_ += mergee.solutions_;
		if(weight_ > mergee.weight_) weight_ = mergee.weight_;

		return true;
	}

	size_t ClassicDynAspTuple::joinHash(const atom_vector &atoms) const
	{
		//TODO: better hash function
		size_t hash = 0;
		for(atom_t atom : atoms)
			if(atoms_.find(atom) != atoms_.end())
				hash += (13 ^ atom) * 57;
		return hash;
	}

	size_t ClassicDynAspTuple::mergeHash() const
	{
		//TODO: better hash function
		//TODO: update for rules/certificates
		size_t hash = 0;
		for(atom_t atom : atoms_)
			hash += (13 ^ atom) * 57;
		return hash;
	}

	size_t ClassicDynAspTuple::hash() const
	{
		//TODO: update for weight_, solutions_ (?)
		return this->mergeHash();
	}

	void ClassicDynAspTuple::introduce(
			const TreeNodeInfo &info,
			sharp::ITupleSet &outputTuples) const
	{
		//TODO: adapt for certificates
		atom_vector trueAtoms(atoms_.begin(), atoms_.end());
		atom_vector falseAtoms, newTrueAtoms, newFalseAtoms;

		for(const atom_t atom : info.rememberedAtoms)
			if(atoms_.find(atom) == atoms_.end())
				falseAtoms.push_back(atom);

		//FIXME: take into account assignment knowledge of info.instance.
		//FIXME: is code below better with stack-based subset enumeration?
		//FIXME: code below only works if max number of subsets fits into size_t
		size_t numIntro = info.introducedAtoms.size();
		for(size_t subset = 0; subset < (1u << numIntro); ++subset)
		{
			//FIXME: debug code
			std::cout << "\ttuple ";
			//FIXME: end debug code

			for(size_t bit = 0; bit < numIntro; ++bit)
				if((subset >> bit) & 1)
				{
					trueAtoms.push_back(info.introducedAtoms[bit]);
					newTrueAtoms.push_back(info.introducedAtoms[bit]);
				}
				else
				{
					falseAtoms.push_back(info.introducedAtoms[bit]);
					newFalseAtoms.push_back(info.introducedAtoms[bit]);
				}

			//FIXME: debug
			std::cout << "t"; printColl(trueAtoms);
			std::cout << " f"; printColl(falseAtoms);
			//FIXME: end debug
			
			ClassicDynAspTuple *newTuple = new ClassicDynAspTuple();

			// check existing rules
			bool validTuple = true;
			for(const auto entry : rules_)
			{
				IGroundAspRule::SatisfiabilityInfo si =
					info.instance.rule(entry.first).check(
							newTrueAtoms,
							newFalseAtoms,
							entry.second);

				if(si.unsatisfied) { validTuple = false; break; }
				if(si.satisfied) continue;

				newTuple->rules_[entry.first] = si;
			}

			// check newly introduced rules
			for(const auto rule : info.introducedRules)
			{
				IGroundAspRule::SatisfiabilityInfo si =
					info.instance.rule(rule).check(trueAtoms, falseAtoms);

				if(si.unsatisfied) { validTuple = false; break; }
				if(si.satisfied) continue;

				newTuple->rules_[rule] = si;
			}

			//FIXME: debug
			std::cout << "\t" << (validTuple ? "yes" : "no");
			//FIXME: end debug
			
			if(validTuple)
			{
				newTuple->atoms_.insert(
						trueAtoms.begin(),
						trueAtoms.end());

				newTuple->weight_ = weight_ + info.instance.weight(
						newTrueAtoms,
						newFalseAtoms);

				newTuple->solutions_ = solutions_;

				//FIXME: debug
				std::cout << "\t" << newTuple->weight_ << " " << newTuple->solutions_;
				//FIXME: end debug

				outputTuples.insert(newTuple);
			}
			else
			{
				delete newTuple;
			}

			//FIXME: debug
			std::cout << std::endl;
			//FIXME: end debug

			newFalseAtoms.clear();
			newTrueAtoms.clear();
			for(size_t bit = 0; bit < numIntro; ++bit)
				if((subset >> bit) & 1) trueAtoms.pop_back();
				else falseAtoms.pop_back();
		}
	}

	IDynAspTuple *ClassicDynAspTuple::project(const TreeNodeInfo &info) const
	{
		//TODO: verify if this check is needed, given our rule->check() impl
		for(rule_t rule : info.forgottenRules)
			if(rules_.find(rule) != rules_.end())
				return nullptr;

		ClassicDynAspTuple *newTuple = new ClassicDynAspTuple();

		// only keep remembered atoms
		for(atom_t atom : info.rememberedAtoms)
			if(atoms_.find(atom) != atoms_.end())
				newTuple->atoms_.insert(atom);
		
		// keep all rules
		// (check above already takes care of unsat forgotten rules)
		newTuple->rules_ = rules_;

		// keep weight and solution count
		newTuple->weight_ = weight_;
		newTuple->solutions_ = solutions_;
		
		// do the same for the certificates
		for(const DynAspCertificate &cert : certificates_)
		{
			//TODO: verify if this check is needed, same as above
			bool remove = false;
			for(rule_t rule : info.forgottenRules)
				if((remove = (cert.rules.find(rule) != cert.rules.end())))
					break;
			if(remove) continue;
			
			// copy atoms and rules from old certificate
			// (check above already takes care of unsat forgotten rules)
			DynAspCertificate newCert = cert;

			// only keep remembered atoms
			for(atom_t atom : info.forgottenAtoms)
				newCert.atoms.erase(atom); 

			// add the new certificate
			newTuple->certificates_.insert(newCert);
		}

		return newTuple;
	}

	IDynAspTuple *ClassicDynAspTuple::join(
			const TreeNodeInfo &info,
			const atom_vector &joinAtoms,
			const IDynAspTuple &tuple) const
	{
		const ClassicDynAspTuple &other =
			static_cast<const ClassicDynAspTuple &>(tuple);

		atom_vector trueAtoms, falseAtoms;
		ClassicDynAspTuple *newTuple = new ClassicDynAspTuple();

		// check that atom assignment matches on the join atoms
		for(atom_t atom : joinAtoms)
			if(atoms_.find(atom) == atoms_.end())
			{
				if(other.atoms_.find(atom) != other.atoms_.end())
				{
					delete newTuple;
					return nullptr;
				}
				falseAtoms.push_back(atom);
			}
			else
			{
				if(other.atoms_.find(atom) == other.atoms_.end())
				{
					delete newTuple;
					return nullptr;
				}
				trueAtoms.push_back(atom);
			}
		
		// check and join rules
		for(rule_t rule : info.rememberedRules)
		{
			auto thisIter = rules_.find(rule);
			auto otherIter = other.rules_.find(rule);

			if(thisIter != rules_.end() && otherIter != other.rules_.end())
			{
				IGroundAspRule::SatisfiabilityInfo si = 
					info.instance.rule(rule).check(
							trueAtoms,
							falseAtoms,
							thisIter->second,
							otherIter->second);

				if(si.unsatisfied)
				{
					delete newTuple;
					return nullptr;
				}
				if(si.satisfied) continue;

				newTuple->rules_[thisIter->first] = si;
			}
		}

		// join atom assignment
		newTuple->atoms_ = atoms_;
		for(atom_t atom : other.atoms_)
			newTuple->atoms_.insert(atom);

		// join weights/counts
		newTuple->solutions_ = solutions_ * other.solutions_;
		newTuple->weight_ = weight_ + other.weight_ - info.instance.weight(
				trueAtoms,
				falseAtoms);

		// join certificates
		//TODO

		return newTuple;
	}

	bool ClassicDynAspTuple::operator==(const ITuple &other) const
	{
		if(typeid(other) != typeid(*this))
			return false;

		const ClassicDynAspTuple &tmpother =
			static_cast<const ClassicDynAspTuple &>(other);

		return weight_ == tmpother.weight_
			&& solutions_ == tmpother.solutions_
			&& atoms_ == tmpother.atoms_
			&& rules_ == tmpother.rules_
			&& certificates_ == tmpother.certificates_;
	}

} // namespace dynasp
