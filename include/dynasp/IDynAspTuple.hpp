#ifndef DYNASP_DYNASP_IDYNASPTUPLE_H_
#define DYNASP_DYNASP_IDYNASPTUPLE_H_

#include <dynasp/global>

#include <sharp/main>

#include <htd/main>

#include <cstddef>

namespace dynasp
{

	class DYNASP_API IDynAspTuple : public sharp::ITuple
	{
	protected:
		IDynAspTuple &operator=(IDynAspTuple &) { return *this; }

	public:
		virtual ~IDynAspTuple() = 0;

		
		virtual bool merge(const IDynAspTuple &tuple) = 0;

		virtual std::size_t joinHash(const atom_vector &atoms) const = 0;
		virtual std::size_t mergeHash() const = 0;

		virtual void introduce(
				const atom_vector &atoms,
				const IGroundAspInstance &instance,
				sharp::ITupleSet &outputTuples) const = 0;

		virtual IDynAspTuple *project(const atom_vector &atoms) const = 0;

		virtual IDynAspTuple *project(
				atom_vector::const_iterator begin,
				atom_vector::const_iterator end) const = 0;

		virtual IDynAspTuple *join(
				const atom_vector &atoms,
				const IDynAspTuple &tuple) const = 0;

		struct merge_hash
		{
			std::size_t operator()(const IDynAspTuple * const &tuple) const
			{
				return tuple->mergeHash();
			}
		};

		struct join_hash
		{
			join_hash(const atom_vector &joinAtoms) : joinAtoms(joinAtoms) { }

			std::size_t operator()(const IDynAspTuple * const &tuple) const
			{
				return tuple->joinHash(joinAtoms);
			}

		private:
			atom_vector joinAtoms;
		};

	}; // class IDynAspTuple

	inline IDynAspTuple::~IDynAspTuple() { }

} // namespace dynasp

#endif // DYNASP_DYNASP_IDYNASPTUPLE_H_
