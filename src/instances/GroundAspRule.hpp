#ifndef DYNASP_INSTANCES_GROUNDASPRULE_H_
#define DYNASP_INSTANCES_GROUNDASPRULE_H_

#include <dynasp/global>

#include <dynasp/IGroundAspRule.hpp>

#include <unordered_set>

namespace dynasp
{
	class DYNASP_LOCAL GroundAspRule : public IGroundAspRule
	{
	public:
		GroundAspRule();
		virtual ~GroundAspRule();

		virtual void addHeadAtom(atom_t atom);
		virtual void makeChoiceHead();
		virtual void addPositiveBodyAtom(atom_t atom);
		virtual void addPositiveBodyAtom(atom_t atom, std::size_t weight);
		virtual void addNegativeBodyAtom(atom_t atom);
		virtual void addNegativeBodyAtom(atom_t atom, std::size_t weight);
		virtual void setMinimumBodyWeight(std::size_t weight);

		virtual bool hasWeightedBody() const;
		virtual bool hasChoiceHead() const;
		virtual bool isHeadAtom(atom_t atom) const;
		virtual bool isPositiveBodyAtom(atom_t atom) const;
		virtual bool isNegativeBodyAtom(atom_t atom) const;

		virtual SatisfiabilityInfo check(
				const atom_vector &trueAtoms,
				const atom_vector &falseAtoms,
				const atom_vector &reductFalseAtoms) const;

		virtual SatisfiabilityInfo check(
				const atom_vector &newTrueAtoms,
				const atom_vector &newFalseAtoms,
				const atom_vector &newReductFalseAtoms,
				SatisfiabilityInfo establishedInfo) const;

		virtual SatisfiabilityInfo check(
				const atom_vector &sharedTrueAtoms,
				const atom_vector &sharedFalseAtoms,
				const atom_vector &sharedReductFalseAtoms,
				SatisfiabilityInfo establishedInfo1,
				SatisfiabilityInfo establishedInfo2) const;

		virtual const_iterator begin() const;
		virtual const_iterator end() const;

	private:
		bool minimumSet_;
		bool choiceRule_;
		std::size_t minimumBodyWeight_, maximumBodyWeight_;
		std::unordered_set<atom_t> head_;
		std::unordered_map<atom_t, std::size_t> positiveBody_;
		std::unordered_map<atom_t, std::size_t> negativeBody_;

		//FIXME: remove, take sharp-provided templates
		class ConstEnumerator : public sharp::IConstEnumerator<atom_t>
		{
		private:
			typedef std::unordered_set<atom_t>::const_iterator SetIter;
			typedef std::unordered_map<atom_t, std::size_t>::const_iterator
				MapIter;

		public:
			ConstEnumerator();

			ConstEnumerator(
					SetIter hbegin, SetIter hend,
					MapIter pbegin, MapIter pend,
					MapIter nbegin, MapIter nend);

			ConstEnumerator(SetIter hend, MapIter pend, MapIter nend);

			virtual ~ConstEnumerator();

			virtual void next();

			virtual const atom_t &get() const;
			virtual bool valid() const;
			virtual ConstEnumerator *clone() const;

			virtual bool operator==(
					const sharp::IConstEnumerator<atom_t> &other) const;

		private:
			SetIter hbegin_, hend_;
			MapIter pbegin_, pend_, nbegin_, nend_;
			bool ended_;
		}; // class ConstEnumerator

	}; // class GroundAspRule

} // namespace dynasp

#endif // DYNASP_INSTANCES_GROUNDASPRULE_H_
