#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif
#include "../util/debug.hpp"

#include "FullDynAspTuple.hpp"

#include <stack>

namespace dynasp
{
	using std::size_t;
	using std::unordered_set;
	using std::stack;

	using sharp::Hash;

	size_t FullDynAspTuple::DynAspCertificate::hash() const
	{
		Hash hash;

		for(atom_t atom : this->atoms)
			hash.addUnordered(atom);
		hash.incorporateUnordered();
		hash.add(this->atoms.size());

		for(const auto &pair : this->rules)
		{
			Hash ruleHash;
			ruleHash.add(pair.first);
			ruleHash.add(pair.second.minBodyWeight);
			ruleHash.add(pair.second.maxBodyWeight);
			ruleHash.add(pair.second.seenHeadAtoms);
			hash.addUnordered(ruleHash.get());
		}
		hash.incorporateUnordered();
		hash.add(this->rules.size());

		hash.add(this->same ? 1 : 0);

		return hash.get();
	}

	bool FullDynAspTuple::DynAspCertificate::operator==(
			const DynAspCertificate &other) const
	{
		return this->atoms == other.atoms
			&& this->rules == other.rules
			&& this->same == other.same;
	}

	FullDynAspTuple::FullDynAspTuple(bool leaf)
		: weight_(0), solutions_(1)
	{
		if(leaf) certificates_.insert({ { }, { }, true });
	}

	FullDynAspTuple::~FullDynAspTuple() { }

	bool FullDynAspTuple::merge(const IDynAspTuple &tuple)
	{
		const FullDynAspTuple &mergee = 
			static_cast<const FullDynAspTuple &>(tuple);

		DBG(std::endl); DBG("  merge\t");
		DBG_COLL(atoms_); DBG("=");  DBG_COLL(mergee.atoms_);
		DBG("\t");
		DBG_RULMAP(rules_); DBG("="); DBG_RULMAP(mergee.rules_);
		DBG("\t");
		DBG_CERT(certificates_); DBG("=");
		DBG_CERT(mergee.certificates_);

		if(atoms_ != mergee.atoms_) return false;
		if(rules_ != mergee.rules_) return false;

		//TODO: if certificates are in subset relation, we can sometimes merge
		if(certificates_ != mergee.certificates_) return false;

		if(weight_ > mergee.weight_)
		{
			solutions_ = mergee.solutions_;
			weight_ = mergee.weight_;
		}
		else if(weight_ == mergee.weight_)
		{
			solutions_ += mergee.solutions_;
		}

		return true;
	}

	bool FullDynAspTuple::isSolution() const
	{
		return certificates_.size() == 1 && certificates_.begin()->same;
	}

	size_t FullDynAspTuple::solutionCount() const
	{
		return solutions_;
	}

	size_t FullDynAspTuple::solutionWeight() const
	{
		return weight_;
	}

	size_t FullDynAspTuple::joinHash(const atom_vector &atoms) const
	{
		Hash h;
		size_t count = 0;
		for(atom_t atom : atoms)
			if(atoms_.find(atom) != atoms_.end())
			{
				h.addUnordered(atom);
				++count;
			}
		h.incorporateUnordered();
		h.add(count);
		return h.get();
	}

	size_t FullDynAspTuple::mergeHash() const
	{
		//TODO: update for certificates (?) depending on merge condition
		Hash h;

		for(atom_t atom : atoms_)
			h.addUnordered(atom);
		h.incorporateUnordered();
		h.add(atoms_.size());

		for(const auto entry : rules_)
		{
			Hash ruleHash;
			ruleHash.add(entry.first);
			ruleHash.add(entry.second.minBodyWeight);
			ruleHash.add(entry.second.maxBodyWeight);
			ruleHash.add(entry.second.seenHeadAtoms);
			h.addUnordered(ruleHash.get());
		}
		h.incorporateUnordered();
		h.add(rules_.size());

		return h.get();
	}

	size_t FullDynAspTuple::hash() const
	{
		//TODO: update for certificates, if not already in mergeHash (see above)
		Hash h;

		h.add(this->mergeHash());
		h.add(weight_);
		h.add(solutions_);

		return h.get();
	}

	void FullDynAspTuple::introduce(
			const TreeNodeInfo &info,
			sharp::ITupleSet &outputTuples) const
	{
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
			DBG("\ttuple ");

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

			DBG("t"); DBG_COLL(trueAtoms); DBG(" f"); DBG_COLL(falseAtoms);
			
			FullDynAspTuple *newTuple = new FullDynAspTuple(false);

			bool validTuple = checkExistingRules(
					info.instance,
					newTrueAtoms,
					newFalseAtoms,
					atom_vector(0),
					rules_,
					newTuple->rules_);

			validTuple &= checkNewRules(
					info.instance,
					trueAtoms,
					falseAtoms,
					atom_vector(0),
					info.introducedRules,
					newTuple->rules_);

			DBG(" r"); DBG_RULMAP(newTuple->rules_);
			DBG("\t"); DBG(validTuple ? "yes" : "no");
			
			if(!validTuple) delete newTuple;
			else
			{
				newTuple->atoms_.insert(
						trueAtoms.begin(),
						trueAtoms.end());

				newTuple->weight_ = weight_ + info.instance.weight(
						newTrueAtoms,
						newFalseAtoms);

				newTuple->solutions_ = solutions_;

				DBG("\t"); DBG(newTuple->weight_); DBG(" ");
				DBG(newTuple->solutions_);
				
				// introduce atoms into certificates
				atom_vector certTrueAtoms,
							certNewTrueAtoms,
							reductFalseAtoms,
							newReductFalseAtoms;
				for(const DynAspCertificate &cert : certificates_)
				{
					for(atom_t atom : atoms_)
						if(cert.atoms.find(atom) == cert.atoms.end())
							reductFalseAtoms.push_back(atom);
						else
							certTrueAtoms.push_back(atom);
					
					size_t certIntro = newTrueAtoms.size();
					for(size_t certSubset = 0;
							certSubset < (1u << certIntro);
							++certSubset)
					{
						for(size_t bit = 0; bit < certIntro; ++bit)
							if((certSubset >> bit) & 1)
							{
								certTrueAtoms.push_back(newTrueAtoms[bit]);
								certNewTrueAtoms.push_back(newTrueAtoms[bit]);
							}
							else
							{
								reductFalseAtoms.push_back(newTrueAtoms[bit]);
								newReductFalseAtoms.push_back(
										newTrueAtoms[bit]);
							}

						DynAspCertificate newCert;

						// check rules						
						bool validCert = checkExistingRules(
								info.instance,
								certNewTrueAtoms,
								newFalseAtoms,
								newReductFalseAtoms,
								cert.rules,
								newCert.rules);

						validCert &= checkNewRules(
								info.instance,
								certTrueAtoms,
								falseAtoms,
								reductFalseAtoms,
								info.introducedRules,
								newCert.rules);

						if(validCert)
						{
							newCert.atoms.insert(
									certTrueAtoms.begin(),
									certTrueAtoms.end());

							newCert.same = cert.same
								&& certSubset == (1u << certIntro) - 1u;

							newTuple->certificates_.insert(std::move(newCert));
						}
		
						// cleanup
						certNewTrueAtoms.clear();
						newReductFalseAtoms.clear();
						for(size_t bit = 0; bit < certIntro; ++bit)
							if((certSubset >> bit) & 1)
								certTrueAtoms.pop_back();
							else
								reductFalseAtoms.pop_back();
					}

					// cleanup
					reductFalseAtoms.clear();
					certTrueAtoms.clear();
				}

				DBG(std::endl); DBG("\t\t");
				DBG_CERT(newTuple->certificates_);

				outputTuples.insert(newTuple);
			}

			DBG(std::endl);

			// reset to state before big loop
			//FIXME: can this be optimized (i.e. no loop again at end)?
			newFalseAtoms.clear();
			newTrueAtoms.clear();
			for(size_t bit = 0; bit < numIntro; ++bit)
				if((subset >> bit) & 1) trueAtoms.pop_back();
				else falseAtoms.pop_back();
		}
	}

	IDynAspTuple *FullDynAspTuple::project(const TreeNodeInfo &info) const
	{
		//TODO: verify if this check is needed, given our rule->check() impl
		//for(rule_t rule : info.forgottenRules)
		//	if(rules_.find(rule) != rules_.end())
		//		return nullptr;

		FullDynAspTuple *newTuple = new FullDynAspTuple(false);

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
			//bool remove = false;
			//for(rule_t rule : info.forgottenRules)
			//	if((remove = (cert.rules.find(rule) != cert.rules.end())))
			//		break;
			//if(remove) continue;
			
			DynAspCertificate newCert;

			// only keep remembered atoms
			for(atom_t atom : info.rememberedAtoms)
				if(cert.atoms.find(atom) != cert.atoms.end())
					newCert.atoms.insert(atom); 

			// copy rules from old certificate
			// (check above already takes care of unsat forgotten rules)
			newCert.rules = cert.rules;

			// copy whether cert represents the tuple
			newCert.same = cert.same;

			// add the new certificate
			newTuple->certificates_.insert(std::move(newCert));
		}

		return newTuple;
	}

	IDynAspTuple *FullDynAspTuple::join(
			const TreeNodeInfo &info,
			const atom_vector &joinAtoms,
			const rule_vector &joinRules,
			const IDynAspTuple &tuple) const
	{
		const FullDynAspTuple &other =
			static_cast<const FullDynAspTuple &>(tuple);

		DBG(std::endl); DBG("  join ");
		DBG_COLL(joinAtoms); DBG("\t");
		DBG_COLL(atoms_); DBG("x"); DBG_COLL(other.atoms_);
		DBG("\t");
		DBG_RULMAP(rules_); DBG("x"); DBG_RULMAP(other.rules_);
		DBG("\t");
		DBG_CERT(certificates_); DBG("x");
		DBG_CERT(other.certificates_);

		atom_vector trueAtoms, falseAtoms;
		FullDynAspTuple *newTuple = new FullDynAspTuple(false);

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
		if(!checkJoinRules(
					info.instance,
					trueAtoms,
					falseAtoms,
					joinRules,
					rules_,
					other.rules_,
					newTuple->rules_))
		{
			delete newTuple;
			return nullptr;
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
		//TODO: use something more intelligent than a nested loop join
		atom_vector certTrueAtoms, certFalseAtoms;
		for(const DynAspCertificate &cert1 : certificates_)
		for(const DynAspCertificate &cert2 : other.certificates_)
		{
			// check if join condition is fulfilled
			bool skip = false;
			for(atom_t atom : joinAtoms)
				if(cert1.atoms.find(atom) == cert1.atoms.end())
				{
					if(cert2.atoms.find(atom) != cert2.atoms.end())
					{
						skip = true;
						break;
					}
					certFalseAtoms.push_back(atom);
				}
				else
				{
					if(cert2.atoms.find(atom) == cert2.atoms.end())
					{
						skip = true;
						break;
					}
					certTrueAtoms.push_back(atom);
				}
			if(skip) continue;

			DynAspCertificate newCert;

			// join and check rules
			if(!checkJoinRules(
						info.instance,
						certTrueAtoms,
						certFalseAtoms,
						joinRules,
						cert1.rules,
						cert2.rules,
						newCert.rules))
			{
				continue;
			}

			newCert.atoms.insert(cert1.atoms.begin(), cert1.atoms.end());
			newCert.atoms.insert(cert2.atoms.begin(), cert2.atoms.end());
			newCert.same = cert1.same && cert2.same;

			newTuple->certificates_.insert(std::move(newCert));

			// cleanup
			certTrueAtoms.clear();
			certFalseAtoms.clear();
		}

		return newTuple;
	}

	bool FullDynAspTuple::operator==(const ITuple &other) const
	{
		if(typeid(other) != typeid(*this))
			return false;

		const FullDynAspTuple &tmpother =
			static_cast<const FullDynAspTuple &>(other);

		return weight_ == tmpother.weight_
			&& solutions_ == tmpother.solutions_
			&& atoms_ == tmpother.atoms_
			&& rules_ == tmpother.rules_
			&& certificates_ == tmpother.certificates_;
	}

	/***********************************\
	|* PRIVATE MEMBERS                 *|
	\***********************************/

	bool FullDynAspTuple::checkExistingRules(
			const IGroundAspInstance &instance,
			const atom_vector &newTrueAtoms,
			const atom_vector &newFalseAtoms,
			const atom_vector &newReductFalseAtoms,
			const rule_map &existingRules,
			rule_map &outputRules)
	{
		for(const auto entry : existingRules)
		{
			IGroundAspRule::SatisfiabilityInfo si =
				instance.rule(entry.first).check(
						newTrueAtoms,
						newFalseAtoms,
						newReductFalseAtoms,
						entry.second);

			if(si.unsatisfied) return false;
			if(si.satisfied) continue;

			outputRules[entry.first] = si;
		}

		return true;
	}

	bool FullDynAspTuple::checkNewRules(
			const IGroundAspInstance &instance,
			const atom_vector &trueAtoms,
			const atom_vector &falseAtoms,
			const atom_vector &reductFalseAtoms,
			const rule_vector &newRules,
			rule_map &outputRules)
	{
		for(const auto rule : newRules)
		{
			IGroundAspRule::SatisfiabilityInfo si =
				instance.rule(rule).check(
						trueAtoms,
						falseAtoms,
						reductFalseAtoms);

			if(si.unsatisfied) return false;
			if(si.satisfied) continue;

			outputRules[rule] = si;
		}

		return true;
	}

	bool FullDynAspTuple::checkJoinRules(
			const IGroundAspInstance &instance,
			const atom_vector &trueAtoms,
			const atom_vector &falseAtoms,
			const rule_vector &rules,
			const rule_map &leftRules,
			const rule_map &rightRules,
			rule_map &outputRules)
	{
		rule_set mergedRules(rules.size());

		for(rule_t rule : rules)
		{
			auto leftIter = leftRules.find(rule);
			auto rightIter = rightRules.find(rule);

			if(leftIter != leftRules.end() && rightIter != rightRules.end())
			{
				IGroundAspRule::SatisfiabilityInfo si = 
					instance.rule(rule).check(
							trueAtoms,
							falseAtoms,
							leftIter->second,
							rightIter->second);

				if(si.unsatisfied) return false;
				if(si.satisfied) continue;

				outputRules[leftIter->first] = si;
			}
			mergedRules.insert(rule);
		}

		for(const auto &entry : leftRules)
		{
			if(mergedRules.find(entry.first) == mergedRules.end())
				outputRules[entry.first] = entry.second;
		}
		
		for(const auto &entry : rightRules)
		{
			if(mergedRules.find(entry.first) == mergedRules.end())
				outputRules[entry.first] = entry.second;
		}

		return true;
	}
} // namespace dynasp
