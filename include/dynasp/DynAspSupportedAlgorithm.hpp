#ifndef DYNASP_DYNASP_DYNASPSUPPCERTIFICATEALGORITHM_H_
#define DYNASP_DYNASP_DYNASPSUPPCERTIFICATEALGORITHM_H_

#include <dynasp/global>

#include <sharp/main>

#include <htd/main.hpp>
#include <dynasp/CertificateDynAspTuple.hpp>
#include <unordered_map>
#include <dynasp/DynAspAlgorithm.hpp>

#ifdef SUPPORTED_CHECK

namespace dynasp
{

	class DYNASP_API DynAspSupportedAlgorithm : public DynAspAlgorithm // sharp::ITreeTupleAlgorithm
	{
	public:
		DynAspSupportedAlgorithm();
		virtual ~DynAspSupportedAlgorithm();
		virtual void onExit() const override;

		virtual void evaluateNode(
				htd::vertex_t node,
				const htd::ITreeDecomposition &decomposition,
				sharp::INodeTupleSetMap &tuples,
				const sharp::IInstance &instance,
				sharp::ITupleSet &outputTuples) const;

		virtual bool needAllTupleSets() const;
		typedef std::unordered_map<atom_vector, CertificateDynAspTuple* > TupleTypes;
	private:
		CertificateDynAspTuple* insertCompressed(TupleTypes& tupleTypes, atom_vector supp, CertificateDynAspTuple& me, CertificateDynAspTuple::ExtensionPointers::iterator& origin, const TreeNodeInfo& info, htd::vertex_t firstChild = 0) const;

	}; // class DynAspSupportedAlgorithm

} // namespace dynasp

#endif

#endif // DYNASP_DYNASP_DYNASPCERTIFICATEALGORITHM_H_
