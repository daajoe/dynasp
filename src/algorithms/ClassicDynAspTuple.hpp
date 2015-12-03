#ifndef DYNASP_ALGORITHMS_CLASSICDYNASPTUPLE_H_
#define DYNASP_ALGORITHMS_CLASSICDYNASPTUPLE_H_

#include <dynasp/global>

#include <dynasp/IDynAspTuple.hpp>

#include <unordered_set>
#include <vector>

namespace dynasp
{
	class DYNASP_LOCAL ClassicDynAspTuple : public IDynAspTuple
	{
	public:
		ClassicDynAspTuple();
		virtual ~ClassicDynAspTuple();

		virtual bool merge(const IDynAspTuple &tuple);

		virtual std::size_t joinHash(const atom_vector &atoms) const;
		virtual std::size_t mergeHash() const;
		virtual std::size_t hash() const;

		virtual void introduce(
				const TreeNodeInfo &info,
				sharp::ITupleSet &outputTuples) const;

		virtual IDynAspTuple *project(const atom_vector &atoms) const;

		virtual IDynAspTuple *project(
				atom_vector::const_iterator begin,
				atom_vector::const_iterator end) const;

		virtual IDynAspTuple *join(
				const atom_vector &atoms,
				const IDynAspTuple &tuple) const;

		virtual bool operator==(const ITuple &other) const;

	private:
		std::unordered_set<atom_t> trueAtoms_;

		//TODO: certificates

	}; // class ClassicDynAspTuple

} // namespace dynasp

#endif // DYNASP_ALGORITHMS_CLASSICDYNASPTUPLE_H_
