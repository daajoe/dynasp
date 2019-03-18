#ifndef DYNASP_DYNASP_DYNASPPROJCERTIFICATEALGORITHM_H_
#define DYNASP_DYNASP_DYNASPPROJCERTIFICATEALGORITHM_H_

#include <dynasp/global>

#include <sharp/main>

#include <htd/main.hpp>
#include <dynasp/CertificateDynAspTuple.hpp>
#include <unordered_map>
#include <dynasp/DynAspAlgorithm.hpp>

//#ifdef SUPPORTED_CHECK

namespace dynasp
{

	class DYNASP_API DynAspProjectionAlgorithm : public DynAspAlgorithm // sharp::ITreeTupleAlgorithm
	{
	public:
		DynAspProjectionAlgorithm();
		virtual ~DynAspProjectionAlgorithm();
		virtual void onExit() const override;

		virtual void evaluateNode(
				htd::vertex_t node,
				const htd::ITreeDecomposition &decomposition,
				sharp::INodeTupleSetMap &tuples,
				const sharp::IInstance &instance,
				sharp::ITupleSet &outputTuples) const;

		virtual bool needAllTupleSets() const;

		

		///typedef std::unordered_map<atom_vector, CertificateDynAspTuple* > TupleTypes;
	private:

		struct TupleHash
		{
			size_t operator()(const CertificateDynAspTuple* tuple) const
			{
				assert(sizeof(std::size_t) >= 8);
				return	tuple->atoms_ & TupleHash::info->int_projectionAtoms;
			}

			size_t operator()(const CertificateDynAspTuple* t1, const CertificateDynAspTuple* t2) const
			{
				return (*this)(t1) == (*this)(t2);
			}

			static TreeNodeInfo* info;
		};

		struct ExtensionPointerHash
		{
			size_t operator()(const std::vector<CertificateDynAspTuple*>& exts) const
			{
				assert(exts.size() <= 2);
				assert(sizeof(std::size_t) >= 8);
				std::size_t key = 0;
				int pos = 0;
				for (const auto* downptr : exts)        //
				{
					key |= (downptr->atoms_ & ExtensionPointerHash::info[pos]->int_projectionAtoms) << (pos * INT_ATOMS_TYPE);
					++pos;
				}
				return key;
			}

			size_t operator()(const std::vector<CertificateDynAspTuple*>& e1, const std::vector<CertificateDynAspTuple*>& e2) const
			{
				return (*this)(e1) == (*this)(e2);
			}

			static TreeNodeInfo** info;
		};
		using Buckets = std::unordered_set<CertificateDynAspTuple*, TupleHash, TupleHash >;
		using ChildBuckets = std::unordered_set<std::vector<CertificateDynAspTuple*>, ExtensionPointerHash, ExtensionPointerHash >;
		//CertificateDynAspTuple* insertCompressed(TupleTypes& tupleTypes, atom_vector supp, CertificateDynAspTuple& me, CertificateDynAspTuple::ExtensionPointers::iterator& origin, const TreeNodeInfo& info, htd::vertex_t firstChild = 0) const;

	}; // class DynAspProjectionAlgorithm

} // namespace dynasp

//#endif

#endif // DYNASP_DYNASP_DYNASPCERTIFICATEALGORITHM_H_
