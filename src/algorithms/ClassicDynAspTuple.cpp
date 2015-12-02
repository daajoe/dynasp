#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif

#include "ClassicDynAspTuple.hpp"

namespace dynasp
{
	using std::size_t;
	using std::unordered_set;

	ClassicDynAspTuple::ClassicDynAspTuple() { }

	ClassicDynAspTuple::~ClassicDynAspTuple() { }

	virtual bool ClassicDynAspTuple::merge(const IDynAspTuple &tuple)
	{
		const ClassicDynAspTuple &mergee = 
			static_cast<const ClassicDynAspTuple &>(tuple);

		//TODO: update for certificates
		return trueAtoms_ == mergee.trueAtoms_;
	}

	size_t ClassicDynAspTuple::joinHash(const atom_vector &atoms) const
	{
		size_t hash = 0;
		for(atom_t atom : atoms)
			if(trueAtoms_.find(atom) != trueAtoms_.end())
				hash += atom * 13;
		return hash;
	}

	size_t ClassicDynAspTuple::mergeHash() const
	{
		size_t hash = 0;
		for(atom_t atom : trueAtoms_)
			hash += atom * 13;
		return hash;
	}

	size_t ClassicDynAspTuple::hash() const
	{
		//TODO: update for certificates
		size_t hash = 0;
		for(atom_t atom : trueAtoms_)
			hash += atom * 13;
		return hash;
	}

	void ClassicDynAspTuple::introduce(
			const atom_vector &atoms,
			const IGroundAspInstance &instance,
			sharp::ITupleSet &outputTuples) const
	{
		//TODO
	}

	IDynAspTuple *ClassicDynAspTuple::project(const atom_vector &atoms) const
	{
		return this->project(atoms.begin(), atoms.end());
	}

	IDynAspTuple *ClassicDynAspTuple::project(
			atom_vector::const_iterator begin,
			atom_vector::const_iterator end) const
	{
		ClassicDynAspTuple *newTuple = new ClassicDynAspTuple();

		atom_t atom;
		while(begin != end)
			if(trueAtoms_.find(atom = *begin++) != trueAtoms_.end())
				newTuple->trueAtoms_.insert(atom);

		return newTuple;
	}

	IDynAspTuple *ClassicDynAspTuple::join(
			const atom_vector &atoms,
			const IDynAspTuple &tuple) const
	{
		const ClassicDynAspTuple &other =
			static_cast<const ClassicDynAspTuple &>(tuple);

		bool join = true;
		for(const atom_t atom : atoms)
			if(!join)
				return nullptr;
			else
				join &= (trueAtoms_.find(atom) != trueAtoms_.end()) ==
						(other.trueAtoms_.find(atom) != other.trueAtoms_.end());

		if(!join) return nullptr;

		//TODO: adapt for certificates
		ClassicDynAspTuple *newTuple = new ClassicDynAspTuple();
		newTuple->trueAtoms_ = trueAtoms_;
		return newTuple;
	}

	bool ClassicDynAspTuple::operator==(const ITuple &other) const
	{
		if(typeid(other) != typeid(*this))
			return false;

		const ClassicDynAspTuple &tmpother =
			static_cast<const ClassicDynAspTuple &>(other);

		//TODO: update for certificates
		return trueAtoms_ == tmpother.trueAtoms_;
	}

} // namespace dynasp
