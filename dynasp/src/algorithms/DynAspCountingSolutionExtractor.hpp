#ifndef DYNASP_ALGORITHMS_DYNASPCOUNTINGSOLUTIONEXTRACTOR_H_
#define DYNASP_ALGORITHMS_DYNASPCOUNTINGSOLUTIONEXTRACTOR_H_

#include <dynasp/global>

#include "DynAspCountingSolution.hpp"

#include <dynasp/IDynAspCountingSolutionExtractor.hpp>

namespace dynasp
{
	class DYNASP_LOCAL DynAspCountingSolutionExtractor
		: public IDynAspCountingSolutionExtractor
	{
	public:
		DynAspCountingSolutionExtractor();
		virtual ~DynAspCountingSolutionExtractor();

		virtual DynAspCountingSolution *extractSolution(
				htd::vertex_t node,
				const htd::ITreeDecomposition &decomposition,
				const sharp::INodeTupleSetMap &tuples,
				const sharp::IInstance &instance) const;

		virtual DynAspCountingSolution *emptySolution(
				const sharp::IInstance &instance) const;

	}; // class DynAspCountingSolutionExtractor

} // namespace dynasp

#endif // DYNASP_ALGORITHMS_DYNASPCOUNTINGSOLUTIONEXTRACTOR_H_
