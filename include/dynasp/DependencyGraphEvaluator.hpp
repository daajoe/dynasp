#ifndef DYNASP_INSTANCES_DEPGRAPH_EVALUTOR_H_
#define DYNASP_INSTANCES_DEPGRAPH_EVALUTOR_H_

#include <dynasp/global>
#include <sharp/ITreeSolver.hpp>
#include <dynasp/IGroundAspInstance.hpp>

namespace dynasp
{
	class DYNASP_API DependencyGraphEvaluator
		: public sharp::ITreeDecompositionEvaluator
	{
	public:
		DependencyGraphEvaluator(const dynasp::IGroundAspInstance&);
		virtual ~DependencyGraphEvaluator() { delete &g; delete leafAt; delete linkedAt; delete leafAtb; delete linkedAtb; }

		double evaluateValue(const htd::IMultiHypergraph&,
			const htd::ITreeDecomposition &, bool rev = false) const;
		
		double evaluateDepth(const htd::IMultiHypergraph&,
			const htd::ITreeDecomposition &, bool rev = false);

		virtual htd::FitnessEvaluation* evaluate(const htd::IMultiHypergraph&,
			const htd::ITreeDecomposition &) const;

		virtual bool isLeaf(htd::vertex_t v) const { return leafAtb->count(v) == 1; }
		virtual bool isLinkedLeaf(htd::vertex_t v) const { return linkedAtb->count(v) == 1; }

		virtual void select() const; //{leafAttmp = leafAt; linkedAttmp = linkedAt; }
	private:
		const dynasp::IGroundAspInstance& ainst;
		htd::DirectedGraph& g;
		
		using VertexFound = std::unordered_set<htd::vertex_t>;
		/*std::unordered_map<htd::vertex_t, VertexFound*> inducAt;*/
		std::unordered_map<htd::vertex_t, unsigned int> depthAt;
		std::unordered_set<htd::vertex_t>* leafAt, *linkedAt;
		std::unordered_set<htd::vertex_t>* leafAtb, *linkedAtb;

	}; 
} // namespace dynasp

#endif
