#ifndef DYNASP_ALGORITHMS_SIMPLEDYNASPTUPLE_H_
#define DYNASP_ALGORITHMS_SIMPLEDYNASPTUPLE_H_

#include <dynasp/global>

#include <dynasp/CertificateDynAspTuple.hpp>
#include <dynasp/IGroundAspRule.hpp>
#include <dynasp/SortedAtomVector.hpp>

#include <unordered_set>
#include <unordered_map>
#include <vector>
#include "../../include/dynasp/IDynAspTuple.hpp"

namespace dynasp
{
	class DYNASP_LOCAL SimpleDynAspTuple : public CertificateDynAspTuple
	{
	public:
		SimpleDynAspTuple(/*bool leaf*/);
		virtual ~SimpleDynAspTuple();
		CertificateDynAspTuple* clone() const;

		//virtual bool merge(const IDynAspTuple &tuple);

		virtual void introduce(
				const TreeNodeInfo &info, /*EvaluatedTuples& tuplesDone,*/ ComputedCertificateTuples& certTuplesComputed,
				sharp::ITupleSet &outputTuples) const;

		virtual void introduceFurther(
				const TreeNodeInfo &info,
				sharp::ITupleSet &outputTuples) const;

		virtual void introduceFurtherCompressed(
				const TreeNodeInfo &info, /*EvaluatedTuples& tuplesDone,*/ ComputedCertificateTuples& certTuplesComputed,
				sharp::ITupleSet &outputTuples, unsigned int maxsize) const;

		/*virtual void introduceFurtherCompressed2(
				const TreeNodeInfo &info, EvaluatedTuples& tuplesDone, ComputedCertificateTuples& certTuplesComputed,
				sharp::ITupleSet &outputTuples) const;*/

		virtual CertificateDynAspTuple::ESubsetRelation checkRules(const rule_vector & rules, const CertificateDynAspTuple & other, const TreeNodeInfo& info, bool write = true);
	/*#ifdef INT_ATOMS_TYPE
		virtual CertificateDynAspTuple::ESubsetRelation checkRules(const atom_vector & rules, const CertificateDynAspTuple & other, const TreeNodeInfo& info, bool write = true);
	#endif*/

		//virtual IDynAspTuple *project(const TreeNodeInfo &info) const;

		virtual IDynAspTuple *join(
				const TreeNodeInfo &info,
				const atom_vector &baseVertices,
				const atom_vector &joinVertices,
				const IDynAspTuple &tuple,
				const atom_vector &tupleVertices) const;


	private:
		/*struct DYNASP_LOCAL DynAspCertificate
		{
			std::unordered_set<atom_t> atoms;
			//std::unordered_set<atom_t> pseudoatoms;
			//bool same; // certificate is the same as the model it belongs to

			std::size_t hash() const;
			bool operator==(const DynAspCertificate &other) const;

		};*/

		//typedef std::unordered_set<atom_t> atom_set;
		//typedef std::set<atom_t> atom_set_o;
		/*typedef std::unordered_set<
				DynAspCertificate,
				sharp::Hasher<DynAspCertificate> >
			certificate_set;*/

		//certificate_set certificates_;

		static bool checkRules(
				const atom_vector &trueAtoms,
				const atom_vector &falseAtoms,
				const atom_vector &reductFalseAtoms,
				const rule_vector &rules,
				const TreeNodeInfo& info
				);

	}; // class SimpleDynAspTuple

} // namespace dynasp

#endif // DYNASP_ALGORITHMS_SIMPLEDYNASPTUPLE_H_
