#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif
#include "../util/debug.hpp"

#include "FullDynAspTuple.hpp"

#include <stack>

namespace dynasp
{
	using htd::vertex_t;
	using htd::ConstCollection;
	using htd::vertex_container;

	using sharp::Hash;

	using std::size_t;
	using std::unordered_set;
	using std::stack;

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

		hash.add(this->same ? (size_t)1 : (size_t)0);

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
		if (leaf)
		{
			DynAspCertificate c;
			c.same = true;
			certificates_.insert(std::move(c));
			//TODO: @GCC-BUG
			//certificates_.insert({ { }, { }, true });
		}
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

		DBG("\t=>\t"); DBG_COLL(atoms_); DBG("\t");
		DBG_RULMAP(rules_); DBG("\t");
		DBG_CERT(certificates_);


		return true;
	}

	bool FullDynAspTuple::isSolution() const
	{
		size_t reductModelCount = certificates_.size();
		for(auto &cert : certificates_)
			if(cert.same || !cert.rules.empty())
				--reductModelCount;

		return rules_.empty()
			&& !reductModelCount;
	}

	mpz_class FullDynAspTuple::solutionCount() const
	{
		return solutions_;
	}

	size_t FullDynAspTuple::solutionWeight() const
	{
		return weight_;
	}

	size_t FullDynAspTuple::joinHash(const htd::vertex_container &atoms) const
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
		//FIXME: support mpz_class
		//h.add(solutions_);

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
			const vertex_container &baseVertices,
			const vertex_container &joinVertices,
			const IDynAspTuple &tuple,
			const std::vector<vertex_t> tupleVertices) const
	{
		const FullDynAspTuple &other =
			static_cast<const FullDynAspTuple &>(tuple);

		DBG(std::endl); DBG("  join ");
		DBG_COLL(joinVertices); DBG("\t");
		DBG_COLL(atoms_); DBG("x"); DBG_COLL(other.atoms_);
		DBG("\t");
		DBG_RULMAP(rules_); DBG("x"); DBG_RULMAP(other.rules_);
		DBG("\t");
		DBG_CERT(certificates_); DBG("x");
		DBG_CERT(other.certificates_);

		atom_set joinSet(joinVertices.begin(), joinVertices.end());
		atom_vector trueAtoms, falseAtoms;
		atom_vector leftTrueAtoms, leftFalseAtoms;
		atom_vector rightTrueAtoms, rightFalseAtoms;

		// check that atom assignment matches on the join atoms
		for(atom_t atom : joinVertices)
			if(!info.instance.isAtom(atom)) continue;
			else if(atoms_.find(atom) == atoms_.end())
			{
				if(other.atoms_.find(atom) != other.atoms_.end())
					return nullptr;
				falseAtoms.push_back(atom);
			}
			else
			{
				if(other.atoms_.find(atom) == other.atoms_.end())
					return nullptr;
				trueAtoms.push_back(atom);
			}

		for(atom_t atom : baseVertices)
			if(!info.instance.isAtom(atom)) continue;
			else if(joinSet.find(atom) != joinSet.end()) continue;
			else if(atoms_.find(atom) != atoms_.end())
				leftTrueAtoms.push_back(atom);
			else
				leftFalseAtoms.push_back(atom);

		for(atom_t atom : tupleVertices)
			if(!info.instance.isAtom(atom)) continue;
			else if(joinSet.find(atom) != joinSet.end()) continue;
			else if(other.atoms_.find(atom) != other.atoms_.end())
				rightTrueAtoms.push_back(atom);
			else
				rightFalseAtoms.push_back(atom);

		FullDynAspTuple *newTuple = new FullDynAspTuple(false);
		
		// check and join rules
		if(!checkJoinRules(
					info,
					trueAtoms,
					falseAtoms,
					atom_vector(0),
					joinVertices,
					leftTrueAtoms,
					leftFalseAtoms,
					atom_vector(0),
					rules_,
					rightTrueAtoms,
					rightFalseAtoms,
					atom_vector(0),
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
		atom_vector certTrueAtoms, reductFalseAtoms;
		atom_vector leftCertTrueAtoms, leftReductFalseAtoms;
		atom_vector rightCertTrueAtoms, rightReductFalseAtoms;

		//TODO: use something more intelligent than a nested loop join
		for(const DynAspCertificate &cert1 : certificates_)
		for(const DynAspCertificate &cert2 : other.certificates_)
		{
			// cleanup
			certTrueAtoms.clear();
			reductFalseAtoms.clear();
			leftCertTrueAtoms.clear();
			leftReductFalseAtoms.clear();
			rightCertTrueAtoms.clear();
			rightReductFalseAtoms.clear();

			DBG(std::endl); DBG("    cert ");
			DBG_COLL(joinVertices); DBG("\t");
			DBG_COLL(cert1.atoms); DBG("x"); DBG_COLL(cert2.atoms);
			DBG("\t");
			DBG_RULMAP(cert1.rules); DBG("x"); DBG_RULMAP(cert2.rules);

			// check if join condition is fulfilled
			bool skip = false;
			for(atom_t atom : trueAtoms)
				if(cert1.atoms.find(atom) == cert1.atoms.end())
				{
					if(cert2.atoms.find(atom) != cert2.atoms.end())
					{
						skip = true;
						break;
					}
					reductFalseAtoms.push_back(atom);
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

			for(atom_t atom : leftTrueAtoms)
				if(cert1.atoms.find(atom) != cert1.atoms.end())
					leftCertTrueAtoms.push_back(atom);
				else
					leftReductFalseAtoms.push_back(atom);

			for(atom_t atom : rightTrueAtoms)
				if(cert2.atoms.find(atom) != cert2.atoms.end())
					rightCertTrueAtoms.push_back(atom);
				else
					rightReductFalseAtoms.push_back(atom);

			DynAspCertificate newCert;

			// join and check rules
			if(!checkJoinRules(
						info,
						certTrueAtoms,
						falseAtoms,
						reductFalseAtoms,
						joinVertices,
						leftCertTrueAtoms,
						leftFalseAtoms,
						leftReductFalseAtoms,
						cert1.rules,
						rightCertTrueAtoms,
						rightFalseAtoms,
						rightReductFalseAtoms,
						cert2.rules,
						newCert.rules))
			{
				continue;
			}

			newCert.atoms.insert(cert1.atoms.begin(), cert1.atoms.end());
			newCert.atoms.insert(cert2.atoms.begin(), cert2.atoms.end());
			newCert.same = cert1.same && cert2.same;

			DBG("\t>>\t"); DBG_COLL(newCert.atoms); DBG("\t");
			DBG_RULMAP(newCert.rules);

			newTuple->certificates_.insert(std::move(newCert));
		}
		// cleanup
		certTrueAtoms.clear();
		reductFalseAtoms.clear();
		leftCertTrueAtoms.clear();
		leftReductFalseAtoms.clear();
		rightCertTrueAtoms.clear();
		rightReductFalseAtoms.clear();

		DBG("\t=>\t"); DBG_COLL(newTuple->atoms_); DBG("\t");
		DBG_RULMAP(newTuple->rules_); DBG("\t");
		DBG_CERT(newTuple->certificates_);

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
			const TreeNodeInfo &info,
			const atom_vector &sharedTrueAtoms,
			const atom_vector &sharedFalseAtoms,
			const atom_vector &sharedReductFalseAtoms,
			const rule_vector &joinRules,
			const atom_vector &leftTrueAtoms,
			const atom_vector &leftFalseAtoms,
			const atom_vector &leftReductFalseAtoms,
			const rule_map &leftRules,
			const atom_vector &rightTrueAtoms,
			const atom_vector &rightFalseAtoms,
			const atom_vector &rightReductFalseAtoms,
			const rule_map &rightRules,
			rule_map &outputRules)
	{
		unordered_set<rule_t> rules;
		for(rule_t rule : joinRules)
			if(info.instance.isRule(rule)) //FIXME: do filtering outside
				rules.insert(rule);

		for(rule_t rule : info.rememberedRules)
		{
			auto leftIter = leftRules.find(rule);
			auto rightIter = rightRules.find(rule);

			if(leftIter == leftRules.end() && rightIter == rightRules.end())
				continue;

			const IGroundAspRule &ruleObject = info.instance.rule(rule);
			IGroundAspRule::SatisfiabilityInfo leftSi;
			IGroundAspRule::SatisfiabilityInfo rightSi;

			if(leftIter != leftRules.end() && rightIter != rightRules.end())
			{
				leftSi = leftIter->second;
				rightSi = rightIter->second;

				IGroundAspRule::SatisfiabilityInfo si = ruleObject.check(
							sharedTrueAtoms,
							sharedFalseAtoms,
							sharedReductFalseAtoms,
							leftSi,
							rightSi);

				if(si.unsatisfied) return false;
				if(si.satisfied) continue;

				outputRules[rule] = si;
			}
			else if(rules.find(rule) != rules.end())
			{
				continue; // if we have a join rule which was already true
			}
			else if(leftIter != leftRules.end())
			{
				leftSi = leftIter->second;

				IGroundAspRule::SatisfiabilityInfo si = ruleObject.check(
							rightTrueAtoms,
							rightFalseAtoms,
							rightReductFalseAtoms,
							leftSi);

				if(si.unsatisfied) return false;
				if(si.satisfied) continue;

				outputRules[rule] = si;
			}
			else // rightIter != rightRules.end()
			{
				rightSi = rightIter->second;

				IGroundAspRule::SatisfiabilityInfo si = ruleObject.check(
							leftTrueAtoms,
							leftFalseAtoms,
							leftReductFalseAtoms,
							rightSi);

				if(si.unsatisfied) return false;
				if(si.satisfied) continue;

				outputRules[rule] = si;
			}
		}

		return true;
	}
} // namespace dynasp
