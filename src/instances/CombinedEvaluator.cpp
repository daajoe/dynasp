#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif
#include "../util/debug.hpp"

#include <dynasp/CombinedEvaluator.hpp>

extern htd::LibraryInstance* inst;

namespace dynasp
{
	using htd::vertex_t;
	using htd::IMultiHypergraph;

	double CombinedEvaluator::evaluateForgottenAtoms(const htd::IMultiHypergraph&,
			const htd::ITreeDecomposition &td) const
	{
		htd::PostOrderTreeTraversal fixJoin;
		int fitness = 0;
		fixJoin.traverse(td, [&](htd::vertex_t v, htd::vertex_t, size_t) {
			for (vertex_t v2 : td.forgottenVertices(v))
			{
				if (ainst.isAtom(v2))
					for (vertex_t ra : td.bagContent(v))
					{
						if (ainst.isRule(ra))
						{
							const IGroundAspRule& r = ainst.rule(ra);
							if (r.isHeadAtom(v2) || r.isPositiveBodyAtom(v2) || r.isNegativeBodyAtom(v2))
								fitness++;
						}
					}
			}
		});
		return -(double)fitness;
	}


	htd::FitnessEvaluation* CombinedEvaluator::evaluate(const htd::IMultiHypergraph& graph,
			const htd::ITreeDecomposition &td) const
	{
		switch (opt)
		{
			case 3:
				return new htd::FitnessEvaluation(2, de.evaluateValue(graph, td),
							je.evaluateValue(graph, td));
				break;
			case 4:		//removed atoms heuristic
				return new htd::FitnessEvaluation(1, evaluateForgottenAtoms(graph, td));
				break;
			case 5:
				return new htd::FitnessEvaluation(2, de.evaluateValue(graph, td), evaluateForgottenAtoms(graph, td));
				break;
			case 6:
				return new htd::FitnessEvaluation(2, evaluateForgottenAtoms(graph, td), de.evaluateValue(graph, td));
				break;
			case 8:
				return new htd::FitnessEvaluation(3, evaluateForgottenAtoms(graph, td), de.evaluateValue(graph, td), je.evaluateValue(graph, td));
				break;
			case 9:
				return new htd::FitnessEvaluation(3, de.evaluateValue(graph, td), evaluateForgottenAtoms(graph, td), je.evaluateValue(graph, td));
				break;
			case 10:
				return new htd::FitnessEvaluation(3, je.evaluateValue(graph, td), de.evaluateValue(graph, td), evaluateForgottenAtoms(graph, td));
				break;
			case 11:
				return new htd::FitnessEvaluation(3, je.evaluateValue(graph, td), evaluateForgottenAtoms(graph, td), de.evaluateValue(graph, td));
				break;




			case 7:	//reverse psychology?
			{
				return new htd::FitnessEvaluation(1, de.evaluateValue(graph, td, true));
			}
				break;
			default:
				return nullptr;
				break;
		}
	}

} // namespace dynasp
