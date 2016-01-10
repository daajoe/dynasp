#ifndef DYNASP_ALGORITHMS_FULLDYNASPTUPLE_H_
#define DYNASP_ALGORITHMS_FULLDYNASPTUPLE_H_

#include <dynasp/global>

#include <dynasp/IDynAspTuple.hpp>
#include <dynasp/IGroundAspRule.hpp>

#include <unordered_set>
#include <unordered_map>
#include <vector>

namespace dynasp
{
	class DYNASP_LOCAL FullDynAspTuple : public IDynAspTuple
	{
	public:
		FullDynAspTuple(bool leaf);
		virtual ~FullDynAspTuple();

		virtual bool merge(const IDynAspTuple &tuple);

		virtual bool isSolution() const;
		virtual std::size_t solutionCount() const;
		virtual std::size_t solutionWeight() const;
		virtual std::size_t joinHash(const atom_vector &atoms) const;
		virtual std::size_t mergeHash() const;
		virtual std::size_t hash() const;

		virtual void introduce(
				const TreeNodeInfo &info,
				sharp::ITupleSet &outputTuples) const;

		virtual IDynAspTuple *project(const TreeNodeInfo &info) const;

		virtual IDynAspTuple *join(
				const TreeNodeInfo &info,
				const atom_vector &joinAtoms,
				const rule_vector &joinRules,
				const IDynAspTuple &tuple) const;

		virtual bool operator==(const ITuple &other) const;

	private:
		struct DYNASP_LOCAL DynAspCertificate
		{
			std::unordered_set<atom_t> atoms;
			std::unordered_map<rule_t, IGroundAspRule::SatisfiabilityInfo>
				rules; // rules for which we don't know yet (sat/unsat)
			bool same; // certificate is the same as the model it belongs to

			std::size_t hash() const;
			bool operator==(const DynAspCertificate &other) const;
		}

		typedef std::unordered_set<atom_t> atom_set;
		typedef std::unordered_set<rule_t> rule_set;
		typedef std::unordered_set<
				DynAspCertificate,
				sharp::Hasher<DynAspCertificate> >
			certificate_set;
		typedef std::unordered_map<rule_t, IGroundAspRule::SatisfiabilityInfo>
			rule_map;

		std::size_t weight_;
		std::size_t solutions_;
		atom_set atoms_;
		rule_map rules_; // rules for which we don't know yet (sat/unsat)
		certificate_set certificates_;

		static bool checkExistingRules(
				const IGroundAspInstance &instance,
				const atom_vector &newTrueAtoms,
				const atom_vector &newFalseAtoms,
				const atom_vector &newReductFalseAtoms,
				const rule_map &existingRules,
				rule_map &outputRules);

		static bool checkNewRules(
				const IGroundAspInstance &instance,
				const atom_vector &trueAtoms,
				const atom_vector &falseAtoms,
				const atom_vector &reductFalseAtoms,
				const rule_vector &newRules,
				rule_map &outputRules);

		static bool checkJoinRules(
				const IGroundAspInstance &instance,
				const atom_vector &trueAtoms,
				const atom_vector &falseAtoms,
				const rule_vector &rules,
				const rule_map &leftRules,
				const rule_map &rightRules,
				rule_map &outputRules);

	}; // class FullDynAspTuple

} // namespace dynasp

#endif // DYNASP_ALGORITHMS_FULLDYNASPTUPLE_H_
