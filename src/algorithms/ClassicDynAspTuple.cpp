#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif

#include "ClassicDynAspTuple.hpp"

#include <stack>

#include "../debug.cpp"

namespace dynasp
{
	using htd::vertex_t;

	using std::size_t;
	using std::unordered_set;
	using std::stack;

	ClassicDynAspTuple::ClassicDynAspTuple() { }

	ClassicDynAspTuple::~ClassicDynAspTuple() { }

	bool ClassicDynAspTuple::merge(const IDynAspTuple &tuple)
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
			const TreeNodeInfo &info,
			sharp::ITupleSet &outputTuples) const
	{
		atom_vector trueAtoms(trueAtoms_.begin(), trueAtoms_.end());
		atom_vector falseAtoms;

		for(const atom_t atom : info.rememberedAtoms)
			if(trueAtoms_.find(atom) == trueAtoms_.end())
				falseAtoms.push_back(atom);

		//FIXME: take into account assignment knowledge of info.instance
		//FIXME: can be much better with stack-based subset enumeration
		std::cout << "\t" << info.introducedAtoms.size() << std::endl;
		size_t numIntro = info.introducedAtoms.size();
		for(size_t subset = 0; subset < (1u << numIntro); ++subset)
		{
			//FIXME: debug code
			std::cout << "\ttuple t";
			//FIXME: end debug code

			for(size_t bit = 0; bit < numIntro; ++bit)
				if((subset >> bit) & 1)
					trueAtoms.push_back(info.introducedAtoms[bit]);
				else
					falseAtoms.push_back(info.introducedAtoms[bit]);

			//FIXME: debug
			printColl(trueAtoms); std::cout << " f";
			printColl(falseAtoms);
			//FIXME: end debug

			bool validTuple = true;
			for(const vertex_t rule : info.introducedRules)
				validTuple &= (-1 != info.instance.rule(rule).isTrue(
									trueAtoms,
									falseAtoms));
			//FIXME: debug
			std::cout << " " << (validTuple ? "yes" : "no") << std::endl;
			//FIXME: end debug
			
			if(validTuple)
			{
				ClassicDynAspTuple *newTuple = new ClassicDynAspTuple();
				newTuple->trueAtoms_.insert(
						trueAtoms.begin(),
						trueAtoms.end());
				outputTuples.insert(newTuple);
			}

			for(size_t bit = 0; bit < numIntro; ++bit)
				if((subset >> bit) & 1) trueAtoms.pop_back();
				else falseAtoms.pop_back();
		}
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
		for(const atom_t atom : other.trueAtoms_)
			newTuple->trueAtoms_.insert(atom);
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
