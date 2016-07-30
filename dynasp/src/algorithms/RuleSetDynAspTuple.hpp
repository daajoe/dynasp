#ifndef DYNASP_ALGORITHMS_RULESETDYNASPTUPLE_H_
#define DYNASP_ALGORITHMS_RULESETDYNASPTUPLE_H_

#include <dynasp/global>

#include <dynasp/CertificateDynAspTuple.hpp>
#include <dynasp/IGroundAspRule.hpp>

#include <unordered_set>
#include <unordered_map>
#include <vector>

namespace dynasp
{
	class DYNASP_LOCAL RuleSetDynAspTuple : public CertificateDynAspTuple
	{
	public:
		RuleSetDynAspTuple(bool leaf);
		virtual ~RuleSetDynAspTuple();
		//virtual bool isPseudo() const { return false; }
		CertificateDynAspTuple* clone() const { return new RuleSetDynAspTuple(false); }
		//void mergeData(CertificateDynAspTuple* /*oldExtendedTuple*/, CertificateDynAspTuple::ExtensionPointers::iterator& /*origin*/) { }
		//TODO: implement checkRules
		CertificateDynAspTuple::ESubsetRelation checkRules(
			const rule_vector &rules, const CertificateDynAspTuple &other, const TreeNodeInfo& info, bool write = true);// { return CertificateDynAspTuple::ESR_NONE;  }

	/*#ifdef INT_ATOMS_TYPE
		CertificateDynAspTuple::ESubsetRelation checkRules(
			const atom_vector &rules, const CertificateDynAspTuple &other, const TreeNodeInfo& info, bool write = true);
	#endif*/
		//CertificateDynAspTuple::ESubsetRelation checkRelation(const CertificateDynAspTuple & /*other*/, bool /*write*/) { return CertificateDynAspTuple::ESR_NONE; }
		//void clean() { }
		//void prepareForBag(const atom_vector& /*atoms*/) {}

		virtual bool isIntermediateSolution(const TreeNodeInfo& info) const; // { return rules_.size() == 0; }
		virtual bool merge(const IDynAspTuple &tuple);

		virtual bool join(const TreeNodeInfo& info, const std::vector<unsigned int>& its, const std::vector<std::vector<IDynAspTuple *>*>&, const htd::ITreeDecomposition& td, htd::vertex_t node, ExtensionPointer& p);
		virtual bool isSolution(const TreeNodeInfo& info) const;
		//virtual mpz_class solutionCount() const;
		//virtual std::size_t solutionWeight() const;
		virtual void mergeHash(Hash& h) const;

		virtual void introduce(
				const TreeNodeInfo &info, IDynAspTuple::ComputedCertificateTuples& certTuplesComputed, 
				sharp::ITupleSet &outputTuples) const;


		virtual void introduceFurtherCompressed(
			const TreeNodeInfo &info, IDynAspTuple::ComputedCertificateTuples& certTuplesComputed,
			sharp::ITupleSet &outputTuples, unsigned int maxsize) const;


		virtual void introduceFurther(
			const TreeNodeInfo &info,
			sharp::ITupleSet &outputTuples) const;

		virtual IDynAspTuple *project(const TreeNodeInfo &info, size_t child) const;

		virtual IDynAspTuple *join(
				const TreeNodeInfo &info,
				const atom_vector &baseVertices,
				const atom_vector &joinVertices,
				const IDynAspTuple &tuple,
				const atom_vector &tupleVertices) const;

		virtual bool operator==(const ITuple &other) const;
		virtual void debug() const;

		/*virtual CertificateDynAspTuple::ESubsetRelation checkRelation(const CertificateDynAspTuple & other, bool subset, bool write)
		{
			atom_set_o atoms1(atoms_.begin(), atoms_.end()), atoms2(static_cast<const RuleSetDynAspTuple&>(other).atoms_.begin(), static_cast<const RuleSetDynAspTuple&>(other).atoms_.end());
			return CertificateDynAspTuple::checkRelation(other, atoms1, atoms2, subset, write);
		}*/

	private:
		/*struct DYNASP_LOCAL DynAspCertificate
		{
			std::unordered_set<atom_t> atoms;
			std::unordered_set<rule_t> rules; // rules for which we don't know
			bool same; // certificate is the same as the model it belongs to

			std::size_t hash() const;
			bool operator==(const DynAspCertificate &other) const;

		};*/

		//typedef std::unordered_set<atom_t> atom_set;
		//typedef std::set<atom_t> atom_set_o;
		typedef std::unordered_set<rule_t> rule_set;
		/*typedef std::unordered_set<
				DynAspCertificate,
				sharp::Hasher<DynAspCertificate> >
			certificate_set;*/

		//std::size_t weight_;
		//mpz_class solutions_;
		//atom_set atoms_;
#ifndef INT_ATOMS_TYPE
		rule_set rules_; // rules for which we don't know yet (sat/unsat)
#endif
		//certificate_set certificates_;

	#ifdef INT_ATOMS_TYPE
		static bool checkRules(
				const atom_vector &trueAtoms,
				const atom_vector &falseAtoms,
				const atom_vector &reductFalseAtoms,
				const atom_vector &rules,
				const TreeNodeInfo& info,
				atom_vector *outputRules = nullptr);
	#else
		static bool checkRules(
				const atom_vector &trueAtoms,
				const atom_vector &falseAtoms,
				const atom_vector &reductFalseAtoms,
				const rule_vector &rules,
				const TreeNodeInfo& info,
				rule_set *outputRules = nullptr);

		static bool checkRules(
				const atom_vector &trueAtoms,
				const atom_vector &falseAtoms,
				const atom_vector &reductFalseAtoms,
				const rule_set &rules,
				const TreeNodeInfo& info,
				rule_set *outputRules = nullptr);
	#endif
		static bool checkJoinRules(
				const TreeNodeInfo &info,
			#ifdef INT_ATOMS_TYPE
				const atom_vector &rules,
			#else
				const rule_vector &rules,
			#endif
				const atom_vector &leftTrueAtoms,
				const atom_vector &leftFalseAtoms,
				const atom_vector &leftReductFalseAtoms,
			#ifdef INT_ATOMS_TYPE
				const atom_vector &leftRules,
			#else
				const rule_set &leftRules,
			#endif
				const atom_vector &rightTrueAtoms,
				const atom_vector &rightFalseAtoms,
				const atom_vector &rightReductFalseAtoms,
			#ifdef INT_ATOMS_TYPE
				const atom_vector &rightRules,
				atom_vector &outputRules
			#else
				const rule_set &rightRules,
				rule_set &outputRules
			#endif
				);

	public:
#ifdef CHECK_CONSENSE
		typedef std::vector<const DynAspCertificatePointer*> RuleConsenseList;
		/*struct RuleConsenseList
		{
			RuleConsenseList() : pseudo(false) {}
			void update(const DynAspCertificatePointer& p) { pointers.push_back(&p); pseudo |= p.cert->isPseudo(); }

			std::vector<const DynAspCertificatePointer*> pointers;
			bool pseudo;
		};*/

		struct RuleConsenseData : public CertificateDynAspTuple::IConsenseData 
		{
			RuleConsenseData() : rules(~0), strict(false) { }
			atom_vector rules;
			bool strict;
			std::vector<RuleConsenseList> tuples;
		};
		
		virtual void setStrict(bool strict, CertificateDynAspTuple::IConsenseData& d) const { if (&d) static_cast<RuleConsenseData&>(d).strict = strict; }

		virtual void updateConsense(const DynAspCertificatePointer& p, CertificateDynAspTuple::IConsenseData *&, const TreeNodeInfo& info, unsigned int pos, htd::vertex_t id) const;

		virtual CertificateDynAspTuple::ESubsetRelation isConsense(CertificateDynAspTuple::IConsenseData &, const TreeNodeInfo& info, unsigned int cnt, const htd::ITreeDecomposition& td, htd::vertex_t node) const;
		
		virtual void clearConsense(CertificateDynAspTuple::IConsenseData &d) const { delete &d; }
#endif

	}; // class RuleSetDynAspTuple

} // namespace dynasp

#endif // DYNASP_ALGORITHMS_RULESETDYNASPTUPLE_H_
