#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif
#include "../util/debug.hpp" 

#include <dynasp/DynAspAlgorithm.hpp>

#include "../instances/GroundAspInstance.hpp"

#include <dynasp/TreeNodeInfo.hpp>
#include <dynasp/create.hpp>

#include <unordered_set>
#include <algorithm>
#include <climits>

namespace dynasp
{
	using htd::vertex_t;
	using htd::vertex_container;
	using htd::ILabelingFunction;
	using htd::ITreeDecomposition;
	using htd::ConstCollection;

	using sharp::IInstance;
	using sharp::INodeTupleSetMap;
	using sharp::ITupleSet;
	using sharp::ITuple;

	using std::vector;
	using std::unordered_set;
	using std::size_t;
	using std::set_intersection;
	using std::back_inserter;
	using std::swap;

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
		TreeNodeInfo info
		{ 
			static_cast<const IGroundAspInstance &>(instance),
			vertex_container(),
			vertex_container(),
			vertex_container(),
			vertex_container()
		};

		const ConstCollection<vertex_t> remembered =
			decomposition.rememberedVertices(node);
		const ConstCollection<vertex_t> introduced =
			decomposition.introducedVertices(node);

		for(auto i = remembered.begin(); i!= remembered.end(); ++i)
		{
			vertex_t vertex = *i;
			
			//FIXME: do this via labelling functions
			if(info.instance.isRule(vertex))
				info.rememberedRules.push_back(vertex);
			else info.rememberedAtoms.push_back(vertex);
		}

		for(auto i = introduced.begin(); i != introduced.end(); ++i)
		{
			vertex_t vertex = *i;
			
			//FIXME: do this via labelling functions
			if(info.instance.isRule(vertex))
				info.introducedRules.push_back(vertex);
			else info.introducedAtoms.push_back(vertex);
		}

		//FIXME: remove bitvectors to calculate subsets, then this is not needed
		if(sizeof(size_t) * CHAR_BIT < (remembered.size() + introduced.size()))
			throw std::runtime_error("Treewidth too high.");

		DBG(node); DBG(": ra"); DBG_COLL(info.rememberedAtoms); DBG(" rr");
		DBG_COLL(info.rememberedRules); DBG(" ia");
		DBG_COLL(info.introducedAtoms); DBG(" ir");
		DBG_COLL(info.introducedRules); DBG(" children: ");

		size_t childCount = decomposition.childCount(node);
		
		// contains vertices already joined together
		unordered_set<vertex_t> joinBase;
		vertex_container baseVertices, nextBaseVertices;
		vertex_t child;

		if(childCount != 0)
		{
			child = decomposition.childAtPosition(node, 0);

			const vector<vertex_t> &childBag =
				decomposition.bagContent(child);

			for(auto i = childBag.begin(); i != childBag.end(); ++i)
				joinBase.insert(*i);
		}

		vertex_container v1, v2, *joinVertices = &v1, *nextJoinVertices = &v2;
		unordered_set<IDynAspTuple *, IDynAspTuple::join_hash>
			prev(1, IDynAspTuple::join_hash(*joinVertices)),
			curr(1, IDynAspTuple::join_hash(*nextJoinVertices));

		bool firstChild = true;

		// for each child, project and join tuples
		if(childCount == 0) prev.insert(create::tuple(true));
		else for(size_t childIndex = 0; childIndex < childCount; ++childIndex)
		{
			child = decomposition.childAtPosition(node, childIndex);
			vertex_t nextChild;

			if(childIndex + 1 < childCount)
			{
				nextChild = decomposition.childAtPosition(node, childIndex + 1);
				const vector<vertex_t> &nextBag =
					decomposition.bagContent(nextChild);

				//FIXME: tree decomposition should supply this
				nextJoinVertices->clear();
				nextBaseVertices.clear();
				nextBaseVertices.insert(
						nextBaseVertices.end(),
						joinBase.begin(),
						joinBase.end());

				for(auto i = nextBag.begin(); i != nextBag.end(); ++i)
					if(!joinBase.insert(*i).second)
						nextJoinVertices->push_back(*i);
			}

			const ITupleSet &childTuples = tuples[child];

			DBG(child); DBG_COLL(*joinVertices);
			DBG("("); DBG(childTuples.size()); DBG(",");

			unordered_set<IDynAspTuple *, IDynAspTuple::merge_hash>
				merged(childTuples.size()), currmerged;

			// project out unneeded vertices
			for(const ITuple &tuple : childTuples)
			{
				const IDynAspTuple &childTuple =
					static_cast<const IDynAspTuple &>(tuple);

				IDynAspTuple *reducedTuple = childTuple.project(info);

				if(!reducedTuple) continue;

				// merge reduced tuples that are now the same
				size_t bucketIndex = merged.bucket(reducedTuple);

				bool isIn = false;
				for(auto mergeIter = merged.begin(bucketIndex);
						 !isIn && mergeIter != merged.end(bucketIndex);
						 ++mergeIter)
					isIn = (*mergeIter)->merge(*reducedTuple);

				if(!isIn) merged.insert(reducedTuple);
			}

			DBG(merged.size()); DBG(",");
			
			// join the tuples to the ones of the previous child
			if(firstChild)
			{
				// if we are the first child, just swap our tuples into
				// the previous set and continue to the next child
				prev.reserve(merged.size());
				curr.reserve(merged.size());
				curr.insert(merged.begin(), merged.end());
				prev.swap(curr);
				swap(joinVertices, nextJoinVertices);
				swap(baseVertices, nextBaseVertices);
	
				DBG(prev.size()); DBG(") ");
				
				firstChild = false;
				continue;
			}

			

			// elsewise, join our tuples to the current set
			currmerged.reserve(merged.size());
			for(IDynAspTuple *tuple : merged)
			{
				size_t bucketIndex = prev.bucket(tuple);
				IDynAspTuple *tmp;
				for(auto joinIter = prev.begin(bucketIndex);
						 joinIter != prev.end(bucketIndex);
						 ++joinIter)
					if(nullptr != (tmp = (*joinIter)->join(
									info,
									baseVertices,
									*joinVertices,
									*tuple,
									decomposition.bagContent(child))))
					{
						size_t bucket = currmerged.bucket(tmp);
						bool isIn = false;
						for(auto mergeIter = currmerged.begin(bucket);
								 !isIn && mergeIter != currmerged.end(bucket);
								 ++mergeIter)
							isIn = (*mergeIter)->merge(*tmp);

						if(isIn)
						{
							delete tmp;
							continue;
						}

						currmerged.insert(tmp);
						curr.insert(tmp);
					}

				delete tuple;
			}

			// delete old tuples in prev
			for(IDynAspTuple *tuple : prev)
				delete tuple;
			prev.clear();

			// move current tuples to prev for next iteration
			prev.swap(curr);
			swap(joinVertices, nextJoinVertices);
			swap(baseVertices, nextBaseVertices);

			DBG(prev.size()); DBG(") ");

			// if after a join we have no tuples left, bail out
			if(prev.size() == 0)
				return;
		}

		DBG(std::endl);

		// for each remaining tuple, 
		for(IDynAspTuple *tuple : prev)
		{
			tuple->introduce(info, outputTuples);
			delete tuple;
		}

		DBG("\ttuples: "); DBG(prev.size()); DBG(" ");
		DBG(outputTuples.size()); DBG(std::endl);
	}

	bool DynAspAlgorithm::needAllTupleSets() const
	{
		return false;
	}

} // namespace dynasp
