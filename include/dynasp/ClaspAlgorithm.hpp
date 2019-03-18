#ifndef CDYNASP_DYNASP_DYNASPALGORITHM_H_
#define CDYNASP_DYNASP_DYNASPALGORITHM_H_

#include <dynasp/global>

#include <sharp/main>

#include <unordered_set>

#include <htd/main.hpp>
#include <dynasp/Atom.hpp>
#include "IDynAspTuple.hpp"

#include <dynasp/IGroundAspRule.hpp>

namespace dynasp
{

	class TreeNodeInfo;
	class DYNASP_API ClaspAlgorithm : public sharp::ITreeTupleAlgorithm
	{
	public:
		ClaspAlgorithm();
		virtual ~ClaspAlgorithm();

		virtual std::vector<const htd::ILabelingFunction *>
				preprocessOperations() const;

		virtual void evaluateNode(
				htd::vertex_t node,
				const htd::ITreeDecomposition &decomposition,
				sharp::INodeTupleSetMap &tuples,
				const sharp::IInstance &instance,
				sharp::ITupleSet &outputTuples) const;

		virtual bool needAllTupleSets() const;

		void setFurther(bool f) { further = f; }

	private:
		bool further;
		unsigned ext;
		std::unordered_set<const IGroundAspRule*> intros;
		/*struct Impl;
		Impl * const impl;*/

	}; // class DynAspAlgorithm

} // namespace dynasp

#endif // DYNASP_DYNASP_DYNASPALGORITHM_H_
