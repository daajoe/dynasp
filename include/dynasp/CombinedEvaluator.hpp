#ifndef DYNASP_INSTANCES_COMBINED_EVALUTOR_H_
#define DYNASP_INSTANCES_COMBINED_EVALUTOR_H_

#include <dynasp/global>
#include <sharp/ITreeSolver.hpp>
#include <dynasp/JoinEvaluator.hpp>
#include <dynasp/DependencyGraphEvaluator.hpp>

namespace dynasp
{
	class DYNASP_API CombinedEvaluator
		: public sharp::ITreeDecompositionEvaluator
	{
	public:
		CombinedEvaluator(unsigned option, const dynasp::IGroundAspInstance& inst) : opt(option), de(inst), ainst(inst) {}
		virtual ~CombinedEvaluator() {}
		double evaluateForgottenAtoms(const htd::IMultiHypergraph& graph,
			const htd::ITreeDecomposition &td) const;

		virtual htd::FitnessEvaluation* evaluate(const htd::IMultiHypergraph&,
			const htd::ITreeDecomposition &) const;
	private:
		unsigned opt;	
		DependencyGraphEvaluator de;
		const dynasp::IGroundAspInstance& ainst;
		JoinEvaluator je;
	}; 
} // namespace dynasp

#endif
