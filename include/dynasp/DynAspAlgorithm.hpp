#ifndef DYNASP_DYNASP_DYNASPALGORITHM_H_
#define DYNASP_DYNASP_DYNASPALGORITHM_H_

#include <dynasp/global>

#include <sharp/main>

#include <htd/main.hpp>
#include <dynasp/Atom.hpp>
#include "IDynAspTuple.hpp"

#include <dynasp/DependencyGraphEvaluator.hpp>
namespace dynasp
{

	class TreeNodeInfo;
	class DYNASP_API DynAspAlgorithm : public sharp::ITreeTupleAlgorithm
	{
	public:
		virtual void setDepEval(DependencyGraphEvaluator* eval) { eval_ = eval; }
		DynAspAlgorithm();
		virtual ~DynAspAlgorithm();

		virtual std::vector<const htd::ILabelingFunction *>
				preprocessOperations() const;

		virtual void evaluateNode(
				htd::vertex_t node,
				const htd::ITreeDecomposition &decomposition,
				sharp::INodeTupleSetMap &tuples,
				const sharp::IInstance &instance,
				sharp::ITupleSet &outputTuples) const;

		virtual bool needAllTupleSets() const;

		void setFurther(unsigned char f) { further = f; }
		//virtual void onExit() const { }
	protected:
		DependencyGraphEvaluator* eval_;
		unsigned char further;

		/*struct Impl;
		Impl * const impl;*/

	}; // class DynAspAlgorithm

} // namespace dynasp

#endif // DYNASP_DYNASP_DYNASPALGORITHM_H_
