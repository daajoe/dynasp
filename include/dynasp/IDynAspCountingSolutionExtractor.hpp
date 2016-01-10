#ifndef DYNASP_DYNASP_IDYNASPCOUNTINGSOLUTIONEXTRACTOR_H_
#define DYNASP_DYNASP_IDYNASPCOUNTINGSOLUTIONEXTRACTOR_H_

#include <dynasp/global>

#include <dynasp/IDynAspCountingSolution.hpp>

#include <sharp/main>

namespace dynasp
{
	class DYNASP_API IDynAspCountingSolutionExtractor
		: public sharp::ITreeTupleSolutionExtractor
	{
	protected:
		IDynAspCountingSolutionExtractor &operator=(
				IDynAspCountingSolutionExtractor &)
		{ return *this; }

	public:
		virtual ~IDynAspCountingSolutionExtractor() = 0;

		virtual IDynAspCountingSolution *extractSolution(
				htd::vertex_t node,
				const htd::ITreeDecomposition &decomposition,
				const sharp::INodeTupleSetMap &tuples,
				const sharp::IInstance &instance) const = 0;

		virtual IDynAspCountingSolution *emptySolution(
				const sharp::IInstance &instance) const = 0;

	}; // class IDynAspCountingSolutionExtractor

	inline IDynAspCountingSolutionExtractor::~IDynAspCountingSolutionExtractor()
	{ }

} // namespace dynasp

#endif // DYNASP_DYNASP_IDYNASPCOUNTINGSOLUTIONEXTRACTOR_H_
