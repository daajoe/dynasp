#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif

#include "GroundAspRule.hpp"

#include "../debug.cpp"

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

	GroundAspRule::SatisfiabilityInfo GroundAspRule::check(
			const atom_vector &trueAtoms,
			const atom_vector &falseAtoms) const
	{
		return this->check(trueAtoms, falseAtoms, 
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
			GroundAspRule::SatisfiabilityInfo ei) const
	{
		//FIXME: fix next line for certificates
		if(choiceRule_) return SatisfiabilityInfo { true, false, 0, 0, 0 };

		unordered_map<atom_t, size_t>::const_iterator it;
		for(const atom_t atom : newTrueAtoms)
			if(head_.find(atom) != head_.end())
				return SatisfiabilityInfo { true, false, 0, 0, 0 }; // h true
			else if((it = negativeBody_.find(atom)) != negativeBody_.end()
					&& (ei.maxBodyWeight -= it->second) < minimumBodyWeight_)
				return SatisfiabilityInfo { true, false, 0, 0, 0 }; // b false
			else if((it = positiveBody_.find(atom)) != positiveBody_.end())
				ei.minBodyWeight += it->second;

		for(const atom_t atom : newFalseAtoms)
			if((it = positiveBody_.find(atom)) != positiveBody_.end()
			   && (ei.maxBodyWeight -= it->second) < minimumBodyWeight_)
				return  SatisfiabilityInfo { true, false, 0, 0, 0 }; // b false
			else if((it = negativeBody_.find(atom)) != negativeBody_.end())
				ei.minBodyWeight += it->second;
			else if(head_.find(atom) != head_.end())
				++ei.seenHeadAtoms;

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
			maximumBodyWeight_ - ei1.maxBodyWeight - ei2.maxBodyWeight,
			ei1.seenHeadAtoms + ei2.seenHeadAtoms
		};

		unordered_map<atom_t, size_t>::const_iterator it;
		for(const atom_t atom : sharedTrueAtoms)
			if((it = negativeBody_.find(atom)) != negativeBody_.end())
				ei.maxBodyWeight += it->second;
			else if((it = positiveBody_.find(atom)) != positiveBody_.end())
				ei.minBodyWeight -= it->second;

		for(const atom_t atom : sharedFalseAtoms)
			if((it = negativeBody_.find(atom)) != negativeBody_.end())
				ei.minBodyWeight -= it->second;
			else if((it = positiveBody_.find(atom)) != positiveBody_.end())
				ei.maxBodyWeight += it->second;
			else if(head_.find(atom) != head_.end())
				--ei.seenHeadAtoms;

		if(ei.maxBodyWeight < minimumBodyWeight_)
			return SatisfiabilityInfo { true, false, 0, 0, 0 }; // b false
		if(ei.seenHeadAtoms == head_.size()
				&& ei.minBodyWeight >= minimumBodyWeight_)
			return SatisfiabilityInfo { false, true, 0, 0, 0 }; // known false

		return ei; // still unknown
	}

	IGroundAspRule::const_iterator GroundAspRule::begin() const
	{
		//FIXME: debug
		std::cout << this;
		std::cout << " " << (choiceRule_ ? "c" : "h"); printColl(head_);
		std::cout << " p"; printMap(positiveBody_);
		std::cout << " n"; printMap(negativeBody_);
		std::cout << " >= " << minimumBodyWeight_;
		std::cout << std::endl;
		//FIXME: end debug

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
