#ifndef DYNASP_DYNASP_IDYNASPTUPLE_H_
#define DYNASP_DYNASP_IDYNASPTUPLE_H_

#include <dynasp/global>

#include <dynasp/Atom.hpp>
#include <dynasp/Rule.hpp>
#include <dynasp/TreeNodeInfo.hpp>

#include <sharp/main>

#include <htd/main.hpp>

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

		virtual bool isSolution() const = 0;
		virtual std::size_t solutionCount() const = 0;
		virtual std::size_t solutionWeight() const = 0;
		virtual std::size_t joinHash(const atom_vector &atoms) const = 0;
		virtual std::size_t mergeHash() const = 0;

		virtual void introduce(
				const TreeNodeInfo &info,
				sharp::ITupleSet &outputTuples) const = 0;

		virtual IDynAspTuple *project(const TreeNodeInfo &info) const = 0;

		virtual IDynAspTuple *join(
				const TreeNodeInfo &info,
				const htd::vertex_container &baseVertices,
				const htd::vertex_container &joinVertices,
				const IDynAspTuple &tuple,
				const htd::ConstCollection<htd::vertex_t> tupleVertices) const = 0;

		//TODO: move this to cpp file(s)
		struct merge_hash
		{
			std::size_t operator()(const IDynAspTuple * const &tuple) const
			{
				return tuple->mergeHash();
			}
		};

		//TODO: move this to cpp file(s)
		struct join_hash
		{
			join_hash(const htd::vertex_container &joinVertices)
				: joinVertices(&joinVertices)
			{ }

			std::size_t operator()(const IDynAspTuple * const &tuple) const
			{
				return tuple->joinHash(*joinVertices);
			}

		//private:
			const htd::vertex_container *joinVertices;
		};

	}; // class IDynAspTuple

	inline IDynAspTuple::~IDynAspTuple() { }

} // namespace dynasp
#endif // DYNASP_DYNASP_IDYNASPTUPLE_H_
