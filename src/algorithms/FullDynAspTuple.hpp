#ifndef DYNASP_ALGORITHMS_CLASSICDYNASPTUPLE_H_
#define DYNASP_ALGORITHMS_CLASSICDYNASPTUPLE_H_

#include <dynasp/global>

#include <dynasp/IDynAspTuple.hpp>
#include <dynasp/IGroundAspRule.hpp>

#include <unordered_set>
#include <unordered_map>
#include <vector>

namespace dynasp
{
	struct DYNASP_LOCAL DynAspCertificate
	{
		std::unordered_set<atom_t> atoms;
		std::unordered_map<rule_t, IGroundAspRule::SatisfiabilityInfo>
			rules; // rules for which we don't know yet (sat/unsat)
		bool same; // certificate is the same as the model it belongs to

		bool operator==(const DynAspCertificate &other) const
		{
			return atoms == other.atoms
				&& rules == other.rules
				&& same == other.same;
		}
	}; // struct DynAspCertificate

} // namespace dynasp

namespace std
{
	template<>
	struct hash<dynasp::DynAspCertificate>
	{
		size_t operator()(
				const dynasp::DynAspCertificate &cert) const
		{
			//TODO: better hash function
			//TODO: move to cpp file
			size_t hash = 0;
			for(dynasp::atom_t atom : cert.atoms)
				hash += (13 ^ atom) * 57;
			for(auto pair : cert.rules)
				hash += ((((((((13 
						^ pair.first)
						* 57)
						^ pair.second.minBodyWeight)
						* 57)
						^ pair.second.maxBodyWeight)
						* 57)
						^ pair.second.seenHeadAtoms)
						* 57);
			if(cert.same) hash += 13 * 57;
			return hash;
		}
	};

} // namespace std

namespace dynasp
{
	class DYNASP_LOCAL ClassicDynAspTuple : public IDynAspTuple
	{
	public:
		ClassicDynAspTuple(bool leaf);
		virtual ~ClassicDynAspTuple();

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
		typedef std::unordered_set<atom_t> atom_set;
		typedef std::unordered_set<rule_t> rule_set;
		typedef std::unordered_set<DynAspCertificate> certificate_set;
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

	}; // class ClassicDynAspTuple

} // namespace dynasp

#endif // DYNASP_ALGORITHMS_CLASSICDYNASPTUPLE_H_
