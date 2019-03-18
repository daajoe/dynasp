#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif
#include "../util/debug.hpp"

#include <dynasp/JoinEvaluator.hpp>

extern htd::LibraryInstance* inst;

namespace dynasp
{
	using htd::vertex_t;
	using htd::IMultiHypergraph;

	double JoinEvaluator::evaluateValue(const htd::IMultiHypergraph&,
			const htd::ITreeDecomposition &td) const
	{
		int fitness = 0;
		htd::PostOrderTreeTraversal fixJoin;
		fixJoin.traverse(td, [&](htd::vertex_t v, htd::vertex_t, size_t) {
			if (v != htd::Vertex::UNKNOWN && td.childCount(v) == 2)
			{
				fitness += td.bagContent(td.childAtPosition(v,0)).size() * td.bagContent(td.childAtPosition(v,1)).size();
			}
		});
		// Here we specify the fitness evaluation for a given decomposition. In this case, we select the maximum bag size and the height.
		return -(double)fitness;
	}

	htd::FitnessEvaluation* JoinEvaluator::evaluate(const htd::IMultiHypergraph& graph,
			const htd::ITreeDecomposition &td) const 
	{
		// Here we specify the fitness evaluation for a given decomposition. In this case, we select the maximum bag size and the height.
		return new htd::FitnessEvaluation(1, evaluateValue(graph, td)); //-(double)(fitness));
	}

} // namespace dynasp
