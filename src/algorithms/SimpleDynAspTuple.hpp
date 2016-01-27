#ifndef DYNASP_ALGORITHMS_SIMPLEDYNASPTUPLE_H_
#define DYNASP_ALGORITHMS_SIMPLEDYNASPTUPLE_H_

#include <dynasp/global>

#include <dynasp/IDynAspTuple.hpp>
#include <dynasp/IGroundAspRule.hpp>

#include <unordered_set>
#include <unordered_map>
#include <vector>

namespace dynasp
{
	class DYNASP_LOCAL SimpleDynAspTuple : public IDynAspTuple
	{
	public:
		SimpleDynAspTuple(bool leaf);
		virtual ~SimpleDynAspTuple();

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
				const htd::ConstCollection<htd::vertex_t> tupleVertices) const;

		virtual bool operator==(const ITuple &other) const;

	private:
		struct DYNASP_LOCAL DynAspCertificate
		{
			std::unordered_set<atom_t> atoms;
			bool same; // certificate is the same as the model it belongs to

			std::size_t hash() const;
			bool operator==(const DynAspCertificate &other) const;
		};

		typedef std::unordered_set<atom_t> atom_set;
		typedef std::unordered_set<
				DynAspCertificate,
				sharp::Hasher<DynAspCertificate> >
			certificate_set;

		std::size_t weight_;
		mpz_class solutions_;
		atom_set atoms_;
		certificate_set certificates_;

		static bool checkRules(
				const IGroundAspInstance &instance,
				const atom_vector &trueAtoms,
				const atom_vector &falseAtoms,
				const atom_vector &reductFalseAtoms,
				const rule_vector &rules);

	}; // class SimpleDynAspTuple

} // namespace dynasp

#endif // DYNASP_ALGORITHMS_SIMPLEDYNASPTUPLE_H_
