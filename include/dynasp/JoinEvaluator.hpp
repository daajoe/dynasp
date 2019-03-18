#ifndef DYNASP_INSTANCES_JOIN_EVALUTOR_H_
#define DYNASP_INSTANCES_JOIN_EVALUTOR_H_

#include <dynasp/global>
#include <sharp/ITreeSolver.hpp>

namespace dynasp
{
	class DYNASP_API JoinEvaluator
		: public sharp::ITreeDecompositionEvaluator
	{
	public:
		JoinEvaluator() {}
		virtual ~JoinEvaluator() {  }

		double evaluateValue(const htd::IMultiHypergraph&,
			const htd::ITreeDecomposition &td) const;

		virtual htd::FitnessEvaluation* evaluate(const htd::IMultiHypergraph&,
			const htd::ITreeDecomposition &) const;
	private:
	}; 
} // namespace dynasp

#endif
