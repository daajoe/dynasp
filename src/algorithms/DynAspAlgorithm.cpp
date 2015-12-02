#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif

#include <dynasp/DynAspAlgorithm.hpp>

#include "../instances/GroundAspInstance.hpp"

#include <dynasp/IDynAspTuple.hpp>

#include <unordered_set>

namespace dynasp
{
	using htd::vertex_t;
	using htd::vertex_container;
	using htd::ILabelingFunction;
	using htd::ITreeDecomposition;

	using sharp::IInstance;
	using sharp::INodeTupleSetMap;
	using sharp::ITupleSet;
	using sharp::ITuple;

	using std::vector;
	using std::unordered_set;
	using std::size_t;

	DynAspAlgorithm::DynAspAlgorithm() : impl(nullptr) { }

	DynAspAlgorithm::~DynAspAlgorithm()
	{
		if(impl) delete impl;
	}

	vector<const ILabelingFunction *> DynAspAlgorithm::preprocessOperations()
		const
	{
		return vector<const ILabelingFunction *>();
	}

	void DynAspAlgorithm::evaluateNode(
			vertex_t node,
			const ITreeDecomposition &decomposition,
			const INodeTupleSetMap &tuples,
			const IInstance &instance,
			ITupleSet &outputTuples) const
	{
		size_t childCount = decomposition.childrenCount(node);
		
		vertex_container join, intro;
		decomposition.rememberedVertices(node, join);
		decomposition.introducedVertices(node, intro);
		
		const IGroundAspInstance &aspInstance =
			static_cast<const IGroundAspInstance &>(instance);

		unordered_set<IDynAspTuple *, IDynAspTuple::join_hash>
			prev(1, IDynAspTuple::join_hash(join)),
			curr(1, IDynAspTuple::join_hash(join));

		bool firstChild = true;

		// find first vertex that is a rule
		vertex_container::const_iterator ruleBegin;
		for(ruleBegin = join.begin(); ruleBegin != join.end(); ++ruleBegin)
			if(aspInstance.isRule(*ruleBegin))
				break;

		// for each child, project and join tuples
		if(childCount == 0) prev.insert(nullptr); //FIXME
		else for(size_t childIndex = 0; childIndex < childCount; ++childIndex)
		{
			vertex_t child = decomposition.child(node, childIndex);
			const ITupleSet &childTuples = tuples[child];

			unordered_set<IDynAspTuple *, IDynAspTuple::merge_hash>
				merged(childTuples.size());

			// project out unneeded vertices
			for(const ITuple &tuple : childTuples)
			{
				const IDynAspTuple &childTuple =
					static_cast<const IDynAspTuple &>(tuple);

				IDynAspTuple *reducedTuple =
					childTuple.project(join.begin(), ruleBegin);

				// merge reduced tuples that are now the same
				size_t bucketIndex = merged.bucket(reducedTuple);

				bool isIn = false;
				for(auto mergeIter = merged.begin(bucketIndex);
						 mergeIter != merged.end(bucketIndex);
						 ++mergeIter)
					if((isIn = (*mergeIter)->merge(*reducedTuple)) == true)
						break;

				if(!isIn) merged.insert(reducedTuple);
			}

			// join the tuples to the ones of the previous child
			if(firstChild)
			{
				// if we are the first child, just swap our tuples into
				// the previous set and continue to the next child
				prev.reserve(merged.size());
				curr.reserve(merged.size());
				prev.insert(merged.begin(), merged.end());
				continue;
			}

			// elsewise, join our tuples to the current set
			for(IDynAspTuple *tuple : merged)
			{
				size_t bucketIndex = prev.bucket(tuple);
				IDynAspTuple *tmp = nullptr;
				for(auto joinIter = prev.begin(bucketIndex);
						 joinIter != prev.end(bucketIndex);
						 ++joinIter)
					if((tmp = (*joinIter)->join(join, *tuple)) != nullptr)
						curr.insert(tmp);

				delete tuple;
			}

			// delete old tuples in prev
			for(IDynAspTuple *tuple : prev)
				delete tuple;
			prev.clear();

			// move current tuples to prev for next iteration
			prev.swap(curr);

			// if after a join we have no tuples left, bail out
			if(prev.size() == 0)
				return;
		}

		// for each remaining tuple, 
		for(IDynAspTuple *tuple : prev)
		{
			tuple->introduce(intro, aspInstance, outputTuples);
			delete tuple;
		}
	}

} // namespace dynasp
