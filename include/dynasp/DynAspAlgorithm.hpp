#ifndef DYNASP_DYNASP_DYNASPALGORITHM_H_
#define DYNASP_DYNASP_DYNASPALGORITHM_H_

#include <dynasp/global>

#include <sharp/main>

#include <htd/main>

namespace dynasp
{

	class DYNASP_API DynAspAlgorithm : public sharp::ITreeTupleAlgorithm
	{
	public:
		DynAspAlgorithm();
		virtual ~DynAspAlgorithm();

		virtual std::vector<const htd::ILabelingFunction *>
				preprocessOperations() const;

		virtual void evaluateNode(
				htd::vertex_t node,
				const htd::ITreeDecomposition &decomposition,
				const sharp::INodeTupleSetMap &tuples,
				const sharp::IInstance &instance,
				sharp::ITupleSet &outputTuples) const;

	private:
		struct Impl;
		Impl * const impl;

	}; // class DynAspAlgorithm

} // namespace dynasp

#endif // DYNASP_DYNASP_DYNASPALGORITHM_H_
