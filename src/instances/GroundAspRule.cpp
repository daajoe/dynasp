#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif

#include "GroundAspRule.hpp"

#include "../debug.cpp"

namespace dynasp
{
	using std::vector;
	using std::size_t;

	GroundAspRule::GroundAspRule()
		: minimumSet_(false), choiceRule_(false), minimumTrueBodyAtoms_(0)
	{ }

	GroundAspRule::~GroundAspRule() { }

	void GroundAspRule::setHead(atom_t atom)
	{
		head_.clear();
		
		if(atom != Atom::FALSE) head_.insert(atom);
	}

	void GroundAspRule::setHead(const vector<atom_t> &atoms)
	{
		head_.clear();

		for(const atom_t &atom : atoms)
			if(atom != Atom::FALSE)
				head_.insert(atom);
	}

	void GroundAspRule::setPositiveBody(const vector<atom_t> &atoms)
	{
		positiveBody_.clear();
		positiveBody_.insert(atoms.begin(), atoms.end());
		this->updateMinimumTrueBodyAtomCount();
	}

	void GroundAspRule::setNegativeBody(const vector<atom_t> &atoms)
	{
		negativeBody_.insert(atoms.begin(), atoms.end());
		this->updateMinimumTrueBodyAtomCount();
	}

	void GroundAspRule::setMinimumTrueBodyAtoms(size_t count)
	{
		minimumSet_ = true;
		minimumTrueBodyAtoms_ = count;
	}

	void GroundAspRule::setChoiceHead()
	{
		choiceRule_ = true;
	}

	void GroundAspRule::updateMinimumTrueBodyAtomCount()
	{
		if(minimumSet_) return;
		minimumTrueBodyAtoms_ = positiveBody_.size() + negativeBody_.size();
	}

	int GroundAspRule::isTrue(
			const atom_vector &trueAtoms,
			const atom_vector &falseAtoms) const
	{
		if(choiceRule_) return true;

		size_t trueBodyAtoms = positiveBody_.size() + negativeBody_.size();

		size_t seenBodyAtoms = 0;
		size_t seenHeadAtoms = 0;

		for(const atom_t atom : trueAtoms)
			if(head_.find(atom) != head_.end())
				return 1; // known true
			else if(negativeBody_.find(atom) != negativeBody_.end()
					&& --trueBodyAtoms < minimumTrueBodyAtoms_)
				return 1; // known true
			else if(positiveBody_.find(atom) != positiveBody_.end())
				++seenBodyAtoms;

		for(const atom_t atom : falseAtoms)
			if(positiveBody_.find(atom) != positiveBody_.end()
			   && --trueBodyAtoms < minimumTrueBodyAtoms_)
				return 1; // known true
			else if(negativeBody_.find(atom) != negativeBody_.end())
				++seenBodyAtoms;
			else if(head_.find(atom) != head_.end())
				++seenHeadAtoms;

		if(seenHeadAtoms != head_.size()) return 0; // unknown
		if(seenBodyAtoms < minimumTrueBodyAtoms_) return 0; // unknown
		return -1; // known false
	}

	IGroundAspRule::const_iterator GroundAspRule::begin() const
	{
		//FIXME: debug
		std::cout << this;
		std::cout << " " << (choiceRule_ ? "c" : "h"); printColl(head_);
		std::cout << " p"; printColl(positiveBody_);
		std::cout << " n"; printColl(negativeBody_);
		std::cout << " >= " << minimumTrueBodyAtoms_;
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
			Iter hbegin, Iter hend,
			Iter pbegin, Iter pend,
			Iter nbegin, Iter nend)
		: hbegin_(hbegin), hend_(hend),
		  pbegin_(pbegin), pend_(pend),
		  nbegin_(nbegin), nend_(nend),
		  ended_(hbegin == hend && pbegin == pend && nbegin == nend)
	{ }

	GroundAspRule::ConstEnumerator::ConstEnumerator(
			Iter hend,
			Iter pend,
			Iter nend)
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
		else if(pbegin_ != pend_) return *pbegin_;
		else return *nbegin_;
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
