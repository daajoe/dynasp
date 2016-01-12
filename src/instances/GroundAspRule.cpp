#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif
#include "../util/debug.hpp"

#include "GroundAspRule.hpp"

namespace dynasp
{
	using std::vector;
	using std::size_t;
	using std::unordered_map;

	GroundAspRule::GroundAspRule()
		: minimumSet_(false), choiceRule_(false),
		  minimumBodyWeight_(0), maximumBodyWeight_(0)
	{ }

	GroundAspRule::~GroundAspRule() { }

	void GroundAspRule::addHeadAtom(atom_t atom)
	{
		if(atom != Atom::FALSE) head_.insert(atom);
	}

	void GroundAspRule::makeChoiceHead()
	{
		choiceRule_ = true;
	}

	void GroundAspRule::addPositiveBodyAtom(atom_t atom)
	{
		this->addPositiveBodyAtom(atom, 1);
	}

	void GroundAspRule::addPositiveBodyAtom(atom_t atom, size_t weight)
	{
		positiveBody_[atom] = weight;
		maximumBodyWeight_ += weight;
		if(!minimumSet_) minimumBodyWeight_ += weight;
	}

	void GroundAspRule::addNegativeBodyAtom(atom_t atom)
	{
		this->addNegativeBodyAtom(atom, 1);
	}

	void GroundAspRule::addNegativeBodyAtom(atom_t atom, size_t weight)
	{
		negativeBody_[atom] = weight;
		maximumBodyWeight_ += weight;
		if(!minimumSet_) minimumBodyWeight_ += weight;
	}

	void GroundAspRule::setMinimumBodyWeight(size_t weight)
	{
		minimumSet_ = true;
		minimumBodyWeight_ = weight;
	}

	bool GroundAspRule::hasWeightedBody() const
	{
		return minimumBodyWeight_ != maximumBodyWeight_;
	}

	bool GroundAspRule::hasChoiceHead() const
	{
		return choiceRule_;
	}

	bool GroundAspRule::isHeadAtom(atom_t atom) const
	{
		return head_.find(atom) != head_.end();
	}

	bool GroundAspRule::isPositiveBodyAtom(atom_t atom) const
	{
		return positiveBody_.find(atom) != positiveBody_.end();
	}

	bool GroundAspRule::isNegativeBodyAtom(atom_t atom) const
	{
		return positiveBody_.find(atom) != positiveBody_.end();
	}

	GroundAspRule::SatisfiabilityInfo GroundAspRule::check(
			const atom_vector &trueAtoms,
			const atom_vector &falseAtoms,
			const atom_vector &reductFalseAtoms) const
	{
		return this->check(trueAtoms, falseAtoms, reductFalseAtoms,
				SatisfiabilityInfo
				{
					false,
					false,
					0,
					maximumBodyWeight_,
					0
				});
	}

	GroundAspRule::SatisfiabilityInfo GroundAspRule::check(
			const atom_vector &newTrueAtoms,
			const atom_vector &newFalseAtoms,
			const atom_vector &newReductFalseAtoms,
			GroundAspRule::SatisfiabilityInfo ei) const
	{
		unordered_map<atom_t, size_t>::const_iterator it;

		for(const atom_t atom : newReductFalseAtoms)
			if(!choiceRule_ && head_.find(atom) != head_.end())
				++ei.seenHeadAtoms;
			else if(choiceRule_ && head_.find(atom) != head_.end())
				ei.seenHeadAtoms = head_.size(); // h false
			else if((it = positiveBody_.find(atom)) != positiveBody_.end()
					&& (ei.maxBodyWeight -= it->second) < minimumBodyWeight_)
				return SatisfiabilityInfo { true, false, 0, 0, 0 }; // b false
			else if((it = negativeBody_.find(atom)) != negativeBody_.end()
					&& (ei.maxBodyWeight -= it->second) < minimumBodyWeight_)
				return SatisfiabilityInfo { true, false, 0, 0, 0 }; // b false
			
		for(const atom_t atom : newTrueAtoms)
			if(!choiceRule_ && head_.find(atom) != head_.end())
				return SatisfiabilityInfo { true, false, 0, 0, 0 }; // h true
			else if(choiceRule_ && ei.seenHeadAtoms != head_.size()
					&& head_.find(atom) != head_.end()
					&& ++ei.seenHeadAtoms == head_.size())
				return SatisfiabilityInfo { true, false, 0, 0, 0 }; // h true
			else if((it = negativeBody_.find(atom)) != negativeBody_.end()
					&& (ei.maxBodyWeight -= it->second) < minimumBodyWeight_)
				return SatisfiabilityInfo { true, false, 0, 0, 0 }; // b false
			else if((it = positiveBody_.find(atom)) != positiveBody_.end())
				ei.minBodyWeight += it->second;

		for(const atom_t atom : newFalseAtoms)
			if(!choiceRule_ && head_.find(atom) != head_.end())
				++ei.seenHeadAtoms;
			else if(choiceRule_ && ei.seenHeadAtoms != head_.size()
					&& head_.find(atom) != head_.end()
					&& ++ei.seenHeadAtoms == head_.size())
				return SatisfiabilityInfo { true, false, 0, 0, 0 }; // h true
			else if((it = positiveBody_.find(atom)) != positiveBody_.end()
					&& (ei.maxBodyWeight -= it->second) < minimumBodyWeight_)
				return SatisfiabilityInfo { true, false, 0, 0, 0 }; // b false
			else if((it = negativeBody_.find(atom)) != negativeBody_.end())
				ei.minBodyWeight += it->second;
		
		if(ei.seenHeadAtoms != head_.size()) return ei; // unknown
		if(ei.minBodyWeight < minimumBodyWeight_) return ei; // unknown
		return SatisfiabilityInfo { false, true, 0, 0, 0 }; // h false, b true
	}

	GroundAspRule::SatisfiabilityInfo GroundAspRule::check(
			const atom_vector &sharedTrueAtoms,
			const atom_vector &sharedFalseAtoms,
			SatisfiabilityInfo ei1,
			SatisfiabilityInfo ei2) const
	{
		SatisfiabilityInfo ei
		{
			false,
			false,
			ei1.minBodyWeight + ei2.minBodyWeight,
			ei1.maxBodyWeight + ei2.maxBodyWeight - maximumBodyWeight_,
			ei1.seenHeadAtoms + ei2.seenHeadAtoms
		};

		bool headFalse = choiceRule_ && (ei1.seenHeadAtoms == head_.size() 
					  				 ||  ei2.seenHeadAtoms == head_.size());

		if(headFalse) ei.seenHeadAtoms = head_.size();

		unordered_map<atom_t, size_t>::const_iterator it;
		for(const atom_t atom : sharedTrueAtoms)
			if((it = negativeBody_.find(atom)) != negativeBody_.end())
				ei.maxBodyWeight += it->second;
			else if((it = positiveBody_.find(atom)) != positiveBody_.end())
				ei.minBodyWeight -= it->second;
			else if((!choiceRule_ || !headFalse)
					&& head_.find(atom) != head_.end())
				--ei.seenHeadAtoms;

		for(const atom_t atom : sharedFalseAtoms)
			if((it = negativeBody_.find(atom)) != negativeBody_.end())
				ei.minBodyWeight -= it->second;
			else if((it = positiveBody_.find(atom)) != positiveBody_.end())
				ei.maxBodyWeight += it->second;
			else if((!choiceRule_ || !headFalse)
					&& head_.find(atom) != head_.end())
				--ei.seenHeadAtoms;

		if(choiceRule_ && !headFalse && ei.seenHeadAtoms == head_.size())
			return SatisfiabilityInfo { true, false, 0, 0, 0 }; // h true
		if(ei.maxBodyWeight < minimumBodyWeight_)
			return SatisfiabilityInfo { true, false, 0, 0, 0 }; // b false
		if(ei.seenHeadAtoms == head_.size()
				&& ei.minBodyWeight >= minimumBodyWeight_)
			return SatisfiabilityInfo { false, true, 0, 0, 0 }; // known false

		return ei; // still unknown
	}

	IGroundAspRule::const_iterator GroundAspRule::begin() const
	{
		DBG(this); DBG(" "); DBG(choiceRule_ ? "c" : "h"); DBG_COLL(head_);
		DBG(" p"); DBG_MAP(positiveBody_); DBG(" n"); DBG_MAP(negativeBody_);
		DBG(" >= "); DBG(minimumBodyWeight_); DBG(std::endl);

		return IGroundAspRule::const_iterator(new ConstEnumerator(
					head_.begin(), head_.end(),
					positiveBody_.begin(), positiveBody_.end(),
					negativeBody_.begin(), negativeBody_.end()));
	}

	IGroundAspRule::const_iterator GroundAspRule::end() const
	{
		return IGroundAspRule::const_iterator(new ConstEnumerator(
					head_.end(),
					positiveBody_.end(),
					negativeBody_.end()));
	}

	GroundAspRule::ConstEnumerator::ConstEnumerator() { ended_ = true; }

	GroundAspRule::ConstEnumerator::ConstEnumerator(
			SetIter hbegin, SetIter hend,
			MapIter pbegin, MapIter pend,
			MapIter nbegin, MapIter nend)
		: hbegin_(hbegin), hend_(hend),
		  pbegin_(pbegin), pend_(pend),
		  nbegin_(nbegin), nend_(nend),
		  ended_(hbegin == hend && pbegin == pend && nbegin == nend)
	{ }

	GroundAspRule::ConstEnumerator::ConstEnumerator(
			SetIter hend,
			MapIter pend,
			MapIter nend)
		: hbegin_(hend), hend_(hend),
		  pbegin_(pend), pend_(pend),
		  nbegin_(nend), nend_(nend),
		  ended_(true)
	{ }

	GroundAspRule::ConstEnumerator::~ConstEnumerator() { }

	void GroundAspRule::ConstEnumerator::next()
	{	
		if(hbegin_ != hend_) ++hbegin_;
		else if(pbegin_ != pend_) ++pbegin_;
		else ++nbegin_;

		ended_ |= (hbegin_ == hend_ && pbegin_ == pend_ && nbegin_ == nend_);
	}

	const atom_t &GroundAspRule::ConstEnumerator::get() const
	{	
		if(hbegin_ != hend_) return *hbegin_;
		else if(pbegin_ != pend_) return pbegin_->first;
		else return nbegin_->first;
	}

	bool GroundAspRule::ConstEnumerator::valid() const
	{
		return !ended_;
	}

	GroundAspRule::ConstEnumerator *GroundAspRule::ConstEnumerator::clone()
		const
	{
		ConstEnumerator *ret = new ConstEnumerator();
		ret->hbegin_ = hbegin_;
		ret->hend_ = hend_;
		ret->pbegin_ = pbegin_;
		ret->pend_ = pend_;
		ret->nbegin_ = nbegin_;
		ret->nend_ = nend_;
		ret->ended_ = ended_;
		return ret;
	}

	bool GroundAspRule::ConstEnumerator::operator==(
			const sharp::IConstEnumerator<atom_t> &other) const
	{
		if(typeid(other) != typeid(*this)) return false;

		const ConstEnumerator &tmpother =
			static_cast<const ConstEnumerator &>(other);

		return tmpother.hbegin_ == hbegin_
			&& tmpother.hend_ == hend_
			&& tmpother.pbegin_ == pbegin_
			&& tmpother.pend_ == pend_
			&& tmpother.nbegin_ == nbegin_
			&& tmpother.nend_ == nend_;
	}

} // namespace dynasp
