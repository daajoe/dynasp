#ifndef DYNASP_DYNASP_IGROUNDASPRULE_H_
#define DYNASP_DYNASP_IGROUNDASPRULE_H_

#include <dynasp/global>

#include <dynasp/Atom.hpp>

#include <sharp/main>

#include <vector>
#include <cstddef>

#include <dynasp/TreeNodeInfo.hpp>
/*
namespace dynasp
{
	struct TreeNodeInfo;
}*/

namespace dynasp
{
	class DYNASP_API IGroundAspRule
	{
	protected:
		IGroundAspRule &operator=(IGroundAspRule &) { return *this; }

	public:
		typedef sharp::IConstEnumerator<atom_t>::Iterator const_iterator;

		struct SatisfiabilityInfo
		{
			bool satisfied;
			bool unsatisfied;
			std::size_t minBodyWeight;
			std::size_t maxBodyWeight;
			std::size_t seenHeadAtoms;

			bool operator==(const SatisfiabilityInfo &other) const
			{
				return satisfied == other.satisfied
					&& unsatisfied == other.unsatisfied
					&& minBodyWeight == other.minBodyWeight
					&& maxBodyWeight == other.maxBodyWeight
					&& seenHeadAtoms == other.seenHeadAtoms;
			}
		};

		virtual ~IGroundAspRule() = 0;

		virtual const std::string toString() const = 0;

		virtual void addHeadAtom(atom_t atom) = 0;
		virtual void makeChoiceHead() = 0;
		virtual void addPositiveBodyAtom(atom_t atom) = 0;
		virtual void addPositiveBodyAtom(atom_t atom, std::size_t weight) = 0;
		virtual void addNegativeBodyAtom(atom_t atom) = 0;
		virtual void addNegativeBodyAtom(atom_t atom, std::size_t weight) = 0;
		virtual void setMinimumBodyWeight(std::size_t weight) = 0;

		virtual void updateDependencyGraph(htd::DirectedGraph& graph) const = 0;

		virtual std::string toQBF(bool reduct, unsigned int id) const = 0;
		virtual bool isSimpleRule() const = 0;
		virtual bool isFact() const = 0;
		virtual bool isConstrainedFact() const = 0;
		virtual bool isConstrainedNFact() const = 0;
		virtual bool isRule() const = 0;
		virtual bool isPosWeightedAtom(atom_t atom) const = 0;
		virtual bool isNegWeightedAtom(atom_t atom) const = 0;
		virtual bool hasWeightedBody() const = 0;
		virtual bool hasChoiceHead() const = 0;
		virtual bool isHeadAtom(atom_t atom) const = 0;
		virtual bool isPositiveBodyAtom(atom_t atom) const = 0;
		virtual bool isNegativeBodyAtom(atom_t atom) const = 0;

		virtual SatisfiabilityInfo check(
				const atom_vector &trueAtoms,
				const atom_vector &falseAtoms,
				const atom_vector &reductFalseAtoms,
	#ifdef SUPPORTED_CHECK
			atom_vector *supp,

		#endif
				const TreeNodeInfo& info
				) const = 0;

		virtual SatisfiabilityInfo check(
				const atom_vector &newTrueAtoms,
				const atom_vector &newFalseAtoms,
				const atom_vector &newReductFalseAtoms,
	#ifdef SUPPORTED_CHECK
			atom_vector *supp,

		#endif
				SatisfiabilityInfo establishedInfo,
				const TreeNodeInfo& info
				) const = 0;

		virtual SatisfiabilityInfo check(
				const atom_vector &sharedTrueAtoms,
				const atom_vector &sharedFalseAtoms,
				const atom_vector &sharedReductFalseAtoms,
				SatisfiabilityInfo establishedInfo1,
				SatisfiabilityInfo establishedInfo2,
				const TreeNodeInfo& info
				) const = 0;

		virtual const_iterator begin() const = 0;
		virtual const_iterator end() const = 0;

	}; // class IGroundAspRule

	inline IGroundAspRule::~IGroundAspRule() { }

} // namespace dynasp

std::ostream& operator<<(std::ostream& os, const dynasp::IGroundAspRule& obj);

#endif // DYNASP_DYNASP_IGROUNDASPRULE_H_
