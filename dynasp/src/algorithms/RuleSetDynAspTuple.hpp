#ifndef DYNASP_ALGORITHMS_RULESETDYNASPTUPLE_H_
#define DYNASP_ALGORITHMS_RULESETDYNASPTUPLE_H_

#include <dynasp/global>

#include <dynasp/IDynAspTuple.hpp>
#include <dynasp/IGroundAspRule.hpp>

#include <unordered_set>
#include <unordered_map>
#include <vector>

namespace dynasp
{
	class DYNASP_LOCAL RuleSetDynAspTuple : public IDynAspTuple
	{
	public:
		RuleSetDynAspTuple(bool leaf);
		virtual ~RuleSetDynAspTuple();

		virtual bool merge(const IDynAspTuple &tuple);

		virtual bool isSolution() const;
		virtual mpz_class solutionCount() const;
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
				const htd::vertex_container &baseVertices,
				const htd::vertex_container &joinVertices,
				const IDynAspTuple &tuple,
				const std::vector<htd::vertex_t> &tupleVertices) const;

		virtual bool operator==(const ITuple &other) const;

	private:
		struct DYNASP_LOCAL DynAspCertificate
		{
			std::unordered_set<atom_t> atoms;
			std::unordered_set<rule_t> rules; // rules for which we don't know
			bool same; // certificate is the same as the model it belongs to

			std::size_t hash() const;
			bool operator==(const DynAspCertificate &other) const;
		};

		typedef std::unordered_set<atom_t> atom_set;
		typedef std::unordered_set<rule_t> rule_set;
		typedef std::unordered_set<
				DynAspCertificate,
				sharp::Hasher<DynAspCertificate> >
			certificate_set;

		std::size_t weight_;
		mpz_class solutions_;
		atom_set atoms_;
		rule_set rules_; // rules for which we don't know yet (sat/unsat)
		certificate_set certificates_;

		static bool checkRules(
				const IGroundAspInstance &instance,
				const atom_vector &trueAtoms,
				const atom_vector &falseAtoms,
				const atom_vector &reductFalseAtoms,
				const rule_vector &rules,
				rule_set &outputRules);

		static bool checkRules(
				const IGroundAspInstance &instance,
				const atom_vector &trueAtoms,
				const atom_vector &falseAtoms,
				const atom_vector &reductFalseAtoms,
				const rule_set &rules,
				rule_set &outputRules);

		static bool checkJoinRules(
				const TreeNodeInfo &info,
				const rule_vector &rules,
				const atom_vector &leftTrueAtoms,
				const atom_vector &leftFalseAtoms,
				const atom_vector &leftReductFalseAtoms,
				const rule_set &leftRules,
				const atom_vector &rightTrueAtoms,
				const atom_vector &rightFalseAtoms,
				const atom_vector &rightReductFalseAtoms,
				const rule_set &rightRules,
				rule_set &outputRules);

	}; // class RuleSetDynAspTuple

} // namespace dynasp

#endif // DYNASP_ALGORITHMS_RULESETDYNASPTUPLE_H_
