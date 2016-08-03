#ifndef DYNASP_DYNASP_IDYNASPTUPLE_H_
#define DYNASP_DYNASP_IDYNASPTUPLE_H_

#include <dynasp/global>

#include <dynasp/Atom.hpp>
#include <dynasp/Rule.hpp>
#include <dynasp/TreeNodeInfo.hpp>
#include <dynasp/SortedAtomVector.hpp>

#include <sharp/main>
#include <sharp/Hash.hpp>

#include <htd/main.hpp>

#include <gmpxx.h>
#include <vector>
#include <unordered_set>

#include <cstddef>


/*namespace dynasp
{
	class IDynAspTuple;
	//struct IDynAspTuple::DynAspCertificatePointer;
}*/

namespace dynasp
{
	class DYNASP_API IDynAspTuple : public sharp::ITuple
	{
	public:

		enum DeleteMode { DM_MARK, DM_DELETE, DM_ERASE };

		inline IDynAspTuple()  { }

		IDynAspTuple &operator=(IDynAspTuple &) { return *this; }

		virtual inline ~IDynAspTuple() { }

		virtual bool merge(const IDynAspTuple &tuple) = 0;

		virtual bool isSolution(const TreeNodeInfo& info/*size_t pass*/) const = 0;
		virtual bool isIntermediateSolution(const TreeNodeInfo& /*info*/) const { return true; }
		virtual bool cleanUp() { return false; };
		virtual bool cleanUp(htd::vertex_t /*node*/, const htd::ITreeDecomposition &/*decomposition*/, sharp::INodeTupleSetMap &/*tuples*/, bool /*doDelete*/) { return false; };
		virtual bool cleanUpRoot(htd::vertex_t /*node*/, const TreeNodeInfo&,  const htd::ITreeDecomposition &/*decomposition*/, sharp::INodeTupleSetMap &/*tuples*/, bool /*doDelete*/) { return false; };
		virtual bool cleanUpRoot(const TreeNodeInfo&) { return false; };
		virtual void cleanUpSecondLevel() { }

		virtual const mpz_class& solutionCount() const = 0;
		virtual std::size_t solutionWeight() const = 0;
		virtual std::size_t joinHash(const atom_vector &atoms, const TreeNodeInfo& info) const = 0;
		virtual std::size_t joinHash(htd::vertex_t child, const atom_vector &atoms, const TreeNodeInfo& info) const = 0;
		//virtual std::size_t joinHash(const vertex_container &atoms) const = 0;
		virtual std::size_t mergeHash() const = 0;

		//non-abstract methods
		//PRECONDITION: project() successfully has been called before
		virtual void projectPtrs(IDynAspTuple& /*newTuple*/) { };
		//PRECONDITION: merge() successfully has been called before
		virtual void mergePtrs(IDynAspTuple &/*tuple*/) { };
		//virtual void mergePtrs(ExtensionPointer &/*tuple*/) { };
		//PRECONDITION: join is feasible, i.e. join() successfully has been called before
		virtual void joinPtrs(IDynAspTuple* /*tupleleft*/, IDynAspTuple* /*tupleright*/, bool reuseMemory = true) { (void)reuseMemory; };
		virtual void postEvaluate() { };

		virtual void introduceFurther(
				const TreeNodeInfo &/*info*/,
				sharp::ITupleSet &/*outputTuples*/) const { };


		virtual IDynAspTuple* project(const TreeNodeInfo& info, size_t child) const = 0;

		virtual IDynAspTuple *join(
				const TreeNodeInfo &info,
				const atom_vector &baseVertices,
				const atom_vector &joinVertices,
				const IDynAspTuple &tuple,
				const atom_vector &tupleVertices) const = 0;

		//TODO: move this to cpp file(s)
		struct merge_hash
		{
			std::size_t operator()(const IDynAspTuple * const &tuple) const
			{
				return tuple->mergeHash();
			}
		};

		//TODO: move this to cpp file(s)
		struct join_hash
		{
			join_hash(const atom_vector &joinVertices, const TreeNodeInfo& info)
					: joinVertices(&joinVertices), info(&info)
			{ }

			std::size_t operator()(const IDynAspTuple * const &tuple) const
			{
				return tuple->joinHash(*joinVertices, *info);
			}

			//private:
			const atom_vector *joinVertices;
			const TreeNodeInfo *info;
		};


		struct ComputedCertificateTuplesEqual
		{
			bool operator()( const IDynAspTuple* lhs, const IDynAspTuple* rhs ) const
			{
				return *lhs == *rhs;
			}
		};


		//typedef std::vector<IDynAspTuple*> EvaluatedTupleResult;
		//typedef std::unordered_map<const IDynAspTuple*, EvaluatedTupleResult > EvaluatedTuples;
		typedef std::unordered_set<IDynAspTuple*, merge_hash, ComputedCertificateTuplesEqual > ComputedCertificateTuples;
		
		virtual void introduce(
				const TreeNodeInfo &/*info*/, /*EvaluatedTuples& tuplesDone,*/ ComputedCertificateTuples& /*tuplesComputed*/,
				sharp::ITupleSet &outputTuples) const = 0;


		virtual void introduceFurtherCompressed(
				const TreeNodeInfo &/*info*/, /*EvaluatedTuples& tuplesDone,*/ ComputedCertificateTuples& /*tuplesComputed*/,
				sharp::ITupleSet &/*outputTuples*/, unsigned int) const { };

		//virtual void introduceFurtherCompressed2(
		//		const TreeNodeInfo &/*info*/, EvaluatedTuples& /*tuplesDone*/, ComputedCertificateTuples& /*tuplesComputed*/,
		//		sharp::ITupleSet &/*outputTuples*/) const { };

	protected:

	}; // class IDynAspTuple

} // namespace dynasp

#endif // DYNASP_DYNASP_IDYNASPTUPLE_H_
