#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif
#include "../util/debug.hpp"

#include "InverseSimpleDynAspTuple.hpp"

#include <stack>
#include <cmath>

namespace dynasp
{
	using htd::vertex_t;
	using htd::ConstCollection;
	using htd::vertex_container;

	using sharp::Hash;

	using std::size_t;
	using std::unordered_set;
	using std::stack;

	size_t InverseSimpleDynAspTuple::DynAspCertificate::hash() const
	{
		Hash hash;

		for(atom_t atom : this->atoms)
			hash.addUnordered(atom);
		hash.incorporateUnordered();
		hash.add(this->atoms.size());

		return hash.get();
	}

	bool InverseSimpleDynAspTuple::DynAspCertificate::operator==(
			const DynAspCertificate &other) const
	{
		return this->atoms == other.atoms;
	}

	InverseSimpleDynAspTuple::InverseSimpleDynAspTuple(bool)
		: weight_(0), solutions_(1)
	{ }

	InverseSimpleDynAspTuple::~InverseSimpleDynAspTuple() { }

	bool InverseSimpleDynAspTuple::merge(const IDynAspTuple &tuple)
	{
		const InverseSimpleDynAspTuple &mergee = 
			static_cast<const InverseSimpleDynAspTuple &>(tuple);

		DBG(std::endl); DBG("  merge\t");
		DBG_COLL(atoms_); DBG("=");  DBG_COLL(mergee.atoms_);
		DBG("\t");
		DBG_ICERT(certificates_); DBG("=");
		DBG_ICERT(mergee.certificates_);

		if(atoms_ != mergee.atoms_) return false;

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

	bool InverseSimpleDynAspTuple::isSolution() const
	{
		return certificates_.size() == (std::pow(2, atoms_.size()) - 1);
	}

	size_t InverseSimpleDynAspTuple::solutionCount() const
	{
		return solutions_;
	}

	size_t InverseSimpleDynAspTuple::solutionWeight() const
	{
		return weight_;
	}

	size_t InverseSimpleDynAspTuple::joinHash(const atom_vector &atoms) const
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

	size_t InverseSimpleDynAspTuple::mergeHash() const
	{
		//TODO: update for certificates (?) depending on merge condition
		Hash h;

		for(atom_t atom : atoms_)
			h.addUnordered(atom);
		h.incorporateUnordered();
		h.add(atoms_.size());

		return h.get();
	}

	size_t InverseSimpleDynAspTuple::hash() const
	{
		//TODO: update for certificates, if not already in mergeHash (see above)
		Hash h;

		h.add(this->mergeHash());
		h.add(weight_);
		h.add(solutions_);

		return h.get();
	}

	void InverseSimpleDynAspTuple::introduce(
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

			bool validTuple = checkRules(
					info.instance,
					trueAtoms,
					falseAtoms,
					atom_vector(0),
					info.introducedRules);

			validTuple &= checkRules(
					info.instance,
					trueAtoms,
					falseAtoms,
					atom_vector(0),
					info.rememberedRules);

			DBG("\t"); DBG(validTuple ? "yes" : "no");
			
			if(validTuple)
			{
				InverseSimpleDynAspTuple *newTuple =
					new InverseSimpleDynAspTuple(false);

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
				size_t certIntro = trueAtoms.size();
				for(size_t certSubset = 0;
						certSubset < (1u << certIntro);
						++certSubset)
				{
					atom_vector certTrueAtoms, reductFalseAtoms;
					for(size_t bit = 0; bit < certIntro; ++bit)
						if((certSubset >> bit) & 1)
							certTrueAtoms.push_back(trueAtoms[bit]);
						else
							reductFalseAtoms.push_back(trueAtoms[bit]);

					DynAspCertificate newCert;
					newCert.atoms.insert(
							certTrueAtoms.begin(),
							certTrueAtoms.end());

					if(certificates_.find(newCert) != certificates_.end())
					{
						newTuple->certificates_.insert(std::move(newCert));
						continue;
					}

					// check rules						
					bool validCert = checkRules(
							info.instance,
							certTrueAtoms,
							falseAtoms,
							reductFalseAtoms,
							info.introducedRules);

					validCert &= checkRules(
							info.instance,
							certTrueAtoms,
							falseAtoms,
							reductFalseAtoms,
							info.rememberedRules);

					if(!validCert)
						newTuple->certificates_.insert(std::move(newCert));
				}

				DBG(std::endl); DBG("\t\t");
				DBG_ICERT(newTuple->certificates_);

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

	IDynAspTuple *InverseSimpleDynAspTuple::project(
			const TreeNodeInfo &info) const
	{
		InverseSimpleDynAspTuple *newTuple =
			new InverseSimpleDynAspTuple(false);

		// only keep remembered atoms
		atom_vector falseAtoms;
		for(atom_t atom : info.rememberedAtoms)
			if(atoms_.find(atom) != atoms_.end())
				newTuple->atoms_.insert(atom);
			else
				falseAtoms.push_back(atom);
		
		// keep weight and solution count
		newTuple->weight_ = weight_;
		newTuple->solutions_ = solutions_;
		
		// do the same for the certificates
		atom_vector tempAtoms(newTuple->atoms_.begin(), newTuple->atoms_.end());
		size_t certIntro = tempAtoms.size();
		for(size_t certSubset = 0;
				certSubset < (1u << certIntro);
				++certSubset)
		{
			atom_vector certTrueAtoms, reductFalseAtoms;
			for(size_t bit = 0; bit < certIntro; ++bit)
				if((certSubset >> bit) & 1)
					certTrueAtoms.push_back(tempAtoms[bit]);
				else
					reductFalseAtoms.push_back(tempAtoms[bit]);

			DynAspCertificate newCert;

			// check rules						
			bool validCert = checkRules(
					info.instance,
					certTrueAtoms,
					falseAtoms,
					reductFalseAtoms,
					info.introducedRules);

			validCert &= checkRules(
					info.instance,
					certTrueAtoms,
					falseAtoms,
					reductFalseAtoms,
					info.rememberedRules);

			if(!validCert)
			{
				newCert.atoms.insert(
						certTrueAtoms.begin(),
						certTrueAtoms.end());

				newTuple->certificates_.insert(std::move(newCert));
			}
		}

		return newTuple;
	}

	IDynAspTuple *InverseSimpleDynAspTuple::join(
			const TreeNodeInfo &info,
			const ConstCollection<vertex_t>,
			const vertex_container &joinVertices,
			const IDynAspTuple &tuple,
			const ConstCollection<vertex_t>) const
	{
		const InverseSimpleDynAspTuple &other =
			static_cast<const InverseSimpleDynAspTuple &>(tuple);

		DBG(std::endl); DBG("  join ");
		DBG_COLL(joinVertices); DBG("\t");
		DBG_COLL(atoms_); DBG("x"); DBG_COLL(other.atoms_);
		DBG("\t");
		DBG_ICERT(certificates_); DBG("x");
		DBG_ICERT(other.certificates_);

		atom_vector trueAtoms, falseAtoms;
		InverseSimpleDynAspTuple *newTuple =
			new InverseSimpleDynAspTuple(false);

		// check that atom assignment matches on the join atoms
		for(atom_t atom : joinVertices)
			if(!info.instance.isAtom(atom)) continue;
			else if(atoms_.find(atom) == atoms_.end())
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
		if(!checkRules(info.instance,
					trueAtoms,
					falseAtoms,
					atom_vector(0),
					info.rememberedRules))
		{
			delete newTuple;
			return nullptr;
		}

		// join atom assignment
		newTuple->atoms_ = atoms_;
		for(atom_t atom : other.atoms_)
			newTuple->atoms_.insert(atom);

		atom_vector allFalseAtoms;
		for(atom_t atom : info.rememberedAtoms)
			if(newTuple->atoms_.find(atom) == newTuple->atoms_.end())
				allFalseAtoms.push_back(atom);

		// join weights/counts
		newTuple->solutions_ = solutions_ * other.solutions_;
		newTuple->weight_ = weight_ + other.weight_ - info.instance.weight(
				trueAtoms,
				falseAtoms);

		// evaluate certificates
		atom_vector tempAtoms(newTuple->atoms_.begin(), newTuple->atoms_.end());
		size_t certIntro = tempAtoms.size();
		for(size_t certSubset = 0;
				certSubset < (1u << certIntro);
				++certSubset)
		{
			atom_vector certTrueAtoms, reductFalseAtoms;
			for(size_t bit = 0; bit < certIntro; ++bit)
				if((certSubset >> bit) & 1)
					certTrueAtoms.push_back(tempAtoms[bit]);
				else
					reductFalseAtoms.push_back(tempAtoms[bit]);

			DynAspCertificate newCert;

			// check rules						
			bool validCert = checkRules(
					info.instance,
					certTrueAtoms,
					allFalseAtoms,
					reductFalseAtoms,
					info.introducedRules);

			validCert &= checkRules(
					info.instance,
					certTrueAtoms,
					allFalseAtoms,
					reductFalseAtoms,
					info.rememberedRules);

			if(!validCert)
			{
				newCert.atoms.insert(
						certTrueAtoms.begin(),
						certTrueAtoms.end());

				newTuple->certificates_.insert(std::move(newCert));
			}
		}

		return newTuple;
	}

	bool InverseSimpleDynAspTuple::operator==(const ITuple &other) const
	{
		if(typeid(other) != typeid(*this))
			return false;

		const InverseSimpleDynAspTuple &tmpother =
			static_cast<const InverseSimpleDynAspTuple &>(other);

		return weight_ == tmpother.weight_
			&& solutions_ == tmpother.solutions_
			&& atoms_ == tmpother.atoms_
			&& certificates_ == tmpother.certificates_;
	}

	/***********************************\
	|* PRIVATE MEMBERS                 *|
	\***********************************/

	bool InverseSimpleDynAspTuple::checkRules(
			const IGroundAspInstance &instance,
			const atom_vector &trueAtoms,
			const atom_vector &falseAtoms,
			const atom_vector &reductFalseAtoms,
			const rule_vector &rules)
	{
		for(const auto rule : rules)
		{
			if(!instance.isRule(rule)) continue;

			IGroundAspRule::SatisfiabilityInfo si =
				instance.rule(rule).check(
						trueAtoms,
						falseAtoms,
						reductFalseAtoms);

			if(si.unsatisfied) return false;
		}

		return true;
	}

} // namespace dynasp
