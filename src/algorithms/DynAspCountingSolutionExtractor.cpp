#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif

#include "DynAspCountingSolutionExtractor.hpp"

#include <dynasp/IDynAspTuple.hpp>
#include <dynasp/IGroundAspInstance.hpp>

namespace dynasp
{
	using sharp::INodeTupleSetMap;
	using sharp::IInstance;
	using sharp::ITuple;

	using htd::vertex_t;
	using htd::ITreeDecomposition;

	using std::size_t;

	DynAspCountingSolutionExtractor::DynAspCountingSolutionExtractor() { }

	DynAspCountingSolutionExtractor::~DynAspCountingSolutionExtractor() { }

	DynAspCountingSolution *DynAspCountingSolutionExtractor::extractSolution(
			vertex_t node,
			const ITreeDecomposition &td,
			const INodeTupleSetMap &tuples,
			const IInstance &inst) const
	{
		mpz_class currentCount = 0;
		size_t currentOptimalWeight = (size_t)-1; // maximal value of size_t

		//TODO: JSON OUTPUT
		std::cout << "SIZE: " << tuples[node].size() << std::endl;

		/*for(const ITuple &entry : tuples[node])
			static_cast<const IDynAspTuple &>(entry).isSolution();*/

		for(const ITuple &entry : tuples[node])
		{
			const auto &tuple = static_cast<const IDynAspTuple &>(entry);
			if(!tuple.isSolution(static_cast<const IGroundAspInstance&>(inst).getNodeData(td.root()))) continue;

			if(currentOptimalWeight > tuple.solutionWeight())
			{
				currentOptimalWeight = tuple.solutionWeight();
				currentCount = tuple.solutionCount();
			}
			else if(currentOptimalWeight == tuple.solutionWeight())
			{
				currentCount += tuple.solutionCount();
			}
		}

		return new DynAspCountingSolution(currentCount, currentOptimalWeight);
	}

	DynAspCountingSolution *DynAspCountingSolutionExtractor::emptySolution(
			const IInstance &) const
	{
		return new DynAspCountingSolution(0, (size_t)-1);
	}



} // namespace dynasp
