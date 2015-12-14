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

	ClassicDynAspTuple::ClassicDynAspTuple(bool initial)
		: weight_(0), solutions_(1)
	{
		if(initial)
			certificates_.insert({ { }, { }, true });
	}

	ClassicDynAspTuple::~ClassicDynAspTuple() { }

	bool ClassicDynAspTuple::merge(const IDynAspTuple &tuple)
	{
		const ClassicDynAspTuple &mergee = 
			static_cast<const ClassicDynAspTuple &>(tuple);

		//FIXME: debug
		std::cout << std::endl << "  merge\t";
		printColl(atoms_); std::cout << "=";  printColl(mergee.atoms_);
		std::cout << "\t";
		printRules(rules_); std::cout << "="; printRules(mergee.rules_);
		std::cout << "\t";
		printCert(certificates_); std::cout << "=";
		printCert(mergee.certificates_);
		//FIXME: end debug

		if(atoms_ != mergee.atoms_) return false;
		if(rules_ != mergee.rules_) return false;
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
			
			ClassicDynAspTuple *newTuple = new ClassicDynAspTuple(false);

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

			//FIXME: debug
			std::cout << " r"; printRules(newTuple->rules_);
			std::cout << "\t" << (validTuple ? "yes" : "no");
			//FIXME: end debug
			
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

				//FIXME: debug
				std::cout << "\t" << newTuple->weight_ << " ";
				std::cout << newTuple->solutions_;
				//FIXME: end debug
				
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

				//FIXME: debug
				std::cout << std::endl << "\t\t";
				printCert(newTuple->certificates_);
				//FIXME: end debug

				outputTuples.insert(newTuple);
			}

			//FIXME: debug
			std::cout << std::endl;
			//FIXME: end debug

			// reset to state before big loop
			//FIXME: can this be optimized (i.e. no loop again at end)?
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

		ClassicDynAspTuple *newTuple = new ClassicDynAspTuple(false);

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
			newTuple->certificates_.insert(std::move(newCert));
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

		//FIXME: debug
		std::cout << std::endl << "  join ";
		printColl(joinAtoms); std::cout << "\t";
		printColl(atoms_); std::cout << "x";  printColl(other.atoms_);
		std::cout << "\t";
		printRules(rules_); std::cout << "x"; printRules(other.rules_);
		std::cout << "\t";
		printCert(certificates_); std::cout << "x";
		printCert(other.certificates_);
		//FIXME: end debug


		atom_vector trueAtoms, falseAtoms;
		ClassicDynAspTuple *newTuple = new ClassicDynAspTuple(false);

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
					info.rememberedRules,
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
						info.rememberedRules,
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

	/***********************************\
	|* PRIVATE MEMBERS                 *|
	\***********************************/

	bool ClassicDynAspTuple::checkExistingRules(
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

	bool ClassicDynAspTuple::checkNewRules(
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

	bool ClassicDynAspTuple::checkJoinRules(
			const IGroundAspInstance &instance,
			const atom_vector &trueAtoms,
			const atom_vector &falseAtoms,
			const rule_vector &rules,
			const rule_map &leftRules,
			const rule_map &rightRules,
			rule_map &outputRules)
	{
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
		}

		return true;
	}
} // namespace dynasp
