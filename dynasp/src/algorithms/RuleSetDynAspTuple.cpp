#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif
#include "../util/debug.hpp"

#include "RuleSetDynAspTuple.hpp"

#include <stack>

namespace dynasp
{
	using htd::vertex_t;
	using htd::vertex_container;

	using sharp::Hash;

	using std::size_t;
	using std::unordered_set;
	using std::stack;
	using std::vector;

	size_t RuleSetDynAspTuple::DynAspCertificate::hash() const
	{
		Hash hash;

		for(atom_t atom : this->atoms)
			hash.addUnordered(atom);
		hash.incorporateUnordered();
		hash.add(this->atoms.size());

		for(rule_t rule : this->rules)
			hash.addUnordered(rule);
		hash.incorporateUnordered();
		hash.add(this->rules.size());

		hash.add(this->same ? (size_t)1 : (size_t)0);

		return hash.get();
	}

	bool RuleSetDynAspTuple::DynAspCertificate::operator==(
			const DynAspCertificate &other) const
	{
		return this->atoms == other.atoms
			&& this->rules == other.rules
			&& this->same == other.same;
	}

	RuleSetDynAspTuple::RuleSetDynAspTuple(bool leaf)
		: weight_(0), solutions_(1)
	{
		if (leaf) 
		{
			DynAspCertificate c;
			c.same = true;
			certificates_.insert(std::move(c));
		}
	}

	RuleSetDynAspTuple::~RuleSetDynAspTuple() { }

	bool RuleSetDynAspTuple::merge(const IDynAspTuple &tuple)
	{
		const RuleSetDynAspTuple &mergee = 
			static_cast<const RuleSetDynAspTuple &>(tuple);

		DBG(std::endl); DBG("  merge\t");
		DBG_COLL(atoms_); DBG("=");  DBG_COLL(mergee.atoms_);
		DBG("\t");
		DBG_COLL(rules_); DBG("="); DBG_COLL(mergee.rules_);
		DBG("\t");
		DBG_RSCERT(certificates_); DBG("=");
		DBG_RSCERT(mergee.certificates_);

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

	bool RuleSetDynAspTuple::isSolution() const
	{
		size_t reductModelCount = certificates_.size();
		for(auto &cert : certificates_)
			if(cert.same || !cert.rules.empty())
				--reductModelCount;

		return rules_.empty()
			&& !reductModelCount;
	}

	mpz_class RuleSetDynAspTuple::solutionCount() const
	{
		return solutions_;
	}

	size_t RuleSetDynAspTuple::solutionWeight() const
	{
		return weight_;
	}

	size_t RuleSetDynAspTuple::joinHash(const atom_vector &atoms) const
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

	size_t RuleSetDynAspTuple::mergeHash() const
	{
		//TODO: update for certificates (?) depending on merge condition
		Hash h;

		for(atom_t atom : atoms_)
			h.addUnordered(atom);
		h.incorporateUnordered();
		h.add(atoms_.size());

		for(rule_t rule : rules_)
			h.addUnordered(rule);
		h.incorporateUnordered();
		h.add(rules_.size());

		return h.get();
	}

	size_t RuleSetDynAspTuple::hash() const
	{
		//TODO: update for certificates, if not already in mergeHash (see above)
		Hash h;

		h.add(this->mergeHash());
		h.add(weight_);
		//FIXME: support mpz_class
		//h.add(solutions_);

		return h.get();
	}

	void RuleSetDynAspTuple::introduce(
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
			
			RuleSetDynAspTuple *newTuple = new RuleSetDynAspTuple(false);

			bool validTuple = checkRules(
					info.instance,
					trueAtoms,
					falseAtoms,
					atom_vector(0),
					rules_,
					newTuple->rules_);

			validTuple &= checkRules(
					info.instance,
					trueAtoms,
					falseAtoms,
					atom_vector(0),
					info.introducedRules,
					newTuple->rules_);

			DBG(" r"); DBG_COLL(newTuple->rules_);
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
							reductFalseAtoms;
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
								certTrueAtoms.push_back(newTrueAtoms[bit]);
							else
								reductFalseAtoms.push_back(newTrueAtoms[bit]);

						DynAspCertificate newCert;

						// check rules						
						bool validCert = checkRules(
								info.instance,
								certTrueAtoms,
								falseAtoms,
								reductFalseAtoms,
								cert.rules,
								newCert.rules);

						validCert &= checkRules(
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
				DBG_RSCERT(newTuple->certificates_);

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

	IDynAspTuple *RuleSetDynAspTuple::project(const TreeNodeInfo &info) const
	{
		size_t ruleOkCount = 0;
		for(rule_t rule : info.rememberedRules)
			if(rules_.find(rule) != rules_.end())
				++ruleOkCount;
		if(ruleOkCount < rules_.size())
			return nullptr;

		RuleSetDynAspTuple *newTuple = new RuleSetDynAspTuple(false);

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
			ruleOkCount = 0;
			for(rule_t rule : info.rememberedRules)
				if(cert.rules.find(rule) != cert.rules.end())
					++ruleOkCount;
			if(ruleOkCount < cert.rules.size())
				continue;
			
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

	IDynAspTuple *RuleSetDynAspTuple::join(
			const TreeNodeInfo &info,
			const vertex_container &baseVertices,
			const vertex_container &joinVertices,
			const IDynAspTuple &tuple,
			const vector<vertex_t> &tupleVertices) const
	{
		const RuleSetDynAspTuple &other =
			static_cast<const RuleSetDynAspTuple &>(tuple);

		DBG(std::endl); DBG("  join ");
		DBG_COLL(joinVertices); DBG("\t");
		DBG_COLL(atoms_); DBG("x"); DBG_COLL(other.atoms_);
		DBG("\t");
		DBG_COLL(rules_); DBG("x"); DBG_COLL(other.rules_);
		DBG("\t");
		DBG_RSCERT(certificates_); DBG("x");
		DBG_RSCERT(other.certificates_);

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
		RuleSetDynAspTuple *newTuple = new RuleSetDynAspTuple(false);
		
		// check and join rules
		if(!checkJoinRules(
					info,
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
			DBG_COLL(cert1.rules); DBG("x"); DBG_COLL(cert2.rules);

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
			DBG_COLL(newCert.rules);

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
		DBG_COLL(newTuple->rules_); DBG("\t");
		DBG_RSCERT(newTuple->certificates_);

		return newTuple;
	}

	bool RuleSetDynAspTuple::operator==(const ITuple &other) const
	{
		if(typeid(other) != typeid(*this))
			return false;

		const RuleSetDynAspTuple &tmpother =
			static_cast<const RuleSetDynAspTuple &>(other);

		return weight_ == tmpother.weight_
			&& solutions_ == tmpother.solutions_
			&& atoms_ == tmpother.atoms_
			&& rules_ == tmpother.rules_
			&& certificates_ == tmpother.certificates_;
	}

	/***********************************\
	|* PRIVATE MEMBERS                 *|
	\***********************************/
	bool RuleSetDynAspTuple::checkRules(
			const IGroundAspInstance &instance,
			const atom_vector &trueAtoms,
			const atom_vector &falseAtoms,
			const atom_vector &reductFalseAtoms,
			const rule_vector &rules,
			rule_set &outputRules)
	{
		for(rule_t rule : rules)
		{
			IGroundAspRule::SatisfiabilityInfo si =
				instance.rule(rule).check(
						trueAtoms,
						falseAtoms,
						reductFalseAtoms);

			if(si.unsatisfied) return false;
			if(si.satisfied) continue;

			outputRules.insert(rule);
		}

		return true;
	}

	bool RuleSetDynAspTuple::checkRules(
			const IGroundAspInstance &instance,
			const atom_vector &trueAtoms,
			const atom_vector &falseAtoms,
			const atom_vector &reductFalseAtoms,
			const rule_set &rules,
			rule_set &outputRules)
	{
		for(rule_t rule : rules)
		{
			IGroundAspRule::SatisfiabilityInfo si =
				instance.rule(rule).check(
						trueAtoms,
						falseAtoms,
						reductFalseAtoms);

			if(si.unsatisfied) return false;
			if(si.satisfied) continue;

			outputRules.insert(rule);
		}

		return true;
	}

	bool RuleSetDynAspTuple::checkJoinRules(
			const TreeNodeInfo &info,
			const rule_vector &joinRules,
			const atom_vector &leftTrueAtoms,
			const atom_vector &leftFalseAtoms,
			const atom_vector &leftReductFalseAtoms,
			const rule_set &leftRules,
			const atom_vector &rightTrueAtoms,
			const atom_vector &rightFalseAtoms,
			const atom_vector &rightReductFalseAtoms,
			const rule_set &rightRules,
			rule_set &outputRules)
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

			IGroundAspRule::SatisfiabilityInfo si;
			const IGroundAspRule &ruleObject = info.instance.rule(rule);

			if(leftIter != leftRules.end() && rightIter != rightRules.end())
			{
				outputRules.insert(rule);
				continue;
			}
			else if(rules.find(rule) != rules.end())
			{
				continue; // if we have a join rule which was already true
			}
			else if(leftIter != leftRules.end())
			{
				si = ruleObject.check(
						rightTrueAtoms,
						rightFalseAtoms,
						rightReductFalseAtoms);
			}
			else // rightIter != rightRules.end()
			{
				si = ruleObject.check(
						leftTrueAtoms,
						leftFalseAtoms,
						leftReductFalseAtoms);
			}

			if(si.unsatisfied) return false;
			if(si.satisfied) continue;

			outputRules.insert(rule);
		}

		return true;
	}
} // namespace dynasp
