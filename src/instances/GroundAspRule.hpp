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

		virtual void setHead(atom_t atom);
		virtual void setHead(const std::vector<atom_t> &atoms);
		virtual void setPositiveBody(const std::vector<atom_t> &atoms);
		virtual void setNegativeBody(const std::vector<atom_t> &atoms);
		virtual void setMinimumTrueBodyAtoms(std::size_t count);
		virtual void setChoiceHead();

		virtual int isTrue(
				const atom_vector &trueAtoms,
				const atom_vector &falseAtoms) const;

		virtual const_iterator begin() const;
		virtual const_iterator end() const;

	private:
		bool minimumSet_;
		bool choiceRule_;
		std::size_t minimumTrueBodyAtoms_;
		std::unordered_set<atom_t> head_;
		std::unordered_set<atom_t> positiveBody_;
		std::unordered_set<atom_t> negativeBody_;

		void updateMinimumTrueBodyAtomCount();

		//FIXME: remove, take sharp-provided templates
		class ConstEnumerator : public sharp::IConstEnumerator<atom_t>
		{
		private:
			typedef std::unordered_set<atom_t>::const_iterator Iter;

		public:
			ConstEnumerator();

			ConstEnumerator(
					Iter hbegin, Iter hend,
					Iter pbegin, Iter pend,
					Iter nbegin, Iter nend);

			ConstEnumerator(Iter hend, Iter pend, Iter nend);

			virtual ~ConstEnumerator();

			virtual void next();

			virtual const atom_t &get() const;
			virtual bool valid() const;
			virtual ConstEnumerator *clone() const;

			virtual bool operator==(
					const sharp::IConstEnumerator<atom_t> &other) const;

		private:
			Iter hbegin_, hend_, pbegin_, pend_, nbegin_, nend_;
			bool ended_;
		}; // class ConstEnumerator

	}; // class GroundAspRule

} // namespace dynasp

#endif // DYNASP_INSTANCES_GROUNDASPRULE_H_
