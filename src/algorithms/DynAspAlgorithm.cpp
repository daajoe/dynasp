#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif

#include <dynasp/DynAspAlgorithm.hpp>

#include "../instances/GroundAspInstance.hpp"

#include <dynasp/TreeNodeInfo.hpp>
#include <dynasp/Factory.hpp>

#include <unordered_set>
#include <algorithm>
#include <climits>

#include "../debug.cpp" 

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
			vertex_container(),
			vertex_container(),
			vertex_container()
		};

		decomposition.bagContent(node, info.atoms);
		decomposition.rememberedVertices(node, info.rememberedAtoms);
		decomposition.introducedVertices(node, info.introducedAtoms);

		if(sizeof(size_t) * CHAR_BIT < info.atoms.size())
			throw std::runtime_error("Treewidth too high.");

		//FIXME: do this via labeling functions
		while(info.atoms.size() && info.instance.isRule(info.atoms.back()))
		{
			info.rules.push_back(info.atoms.back());
			info.atoms.pop_back();
		}

		//FIXME: do this via labeling functions
		while(info.rememberedAtoms.size()
				&& info.instance.isRule(info.rememberedAtoms.back()))
		{
			info.rememberedRules.push_back(info.rememberedAtoms.back());
			info.rememberedAtoms.pop_back();
		}

		//FIXME: do this via labeling functions
		while(info.introducedAtoms.size()
				&& info.instance.isRule(info.introducedAtoms.back()))
		{
			info.introducedRules.push_back(info.introducedAtoms.back());
			info.introducedAtoms.pop_back();
		}

		//FIXME: remove debug code below
		std::cout << node << ": a";
		printColl(info.atoms); std::cout << " r";
		printColl(info.rules); std::cout << " ra";
		printColl(info.rememberedAtoms); std::cout << " rr";
		printColl(info.rememberedRules); std::cout << " ia";
		printColl(info.introducedAtoms); std::cout << " ir";
		printColl(info.introducedRules); std::cout << " children: ";
		//FIXME: end debug code

		size_t childCount = decomposition.childrenCount(node);
		
		vertex_container childBag, nextChildBag;
		if(childCount != 0)
			decomposition.bagContent(
					decomposition.child(node, 0),
					nextChildBag);

		vertex_container v1, v2, *joinVertices = &v1, *nextJoinVertices = &v2;
		unordered_set<IDynAspTuple *, IDynAspTuple::join_hash>
			prev(1, IDynAspTuple::join_hash(*joinVertices)),
			curr(1, IDynAspTuple::join_hash(*nextJoinVertices));

		bool firstChild = true;

		// for each child, project and join tuples
		if(childCount == 0) prev.insert(Factory::createTuple());
		else for(size_t childIndex = 0; childIndex < childCount; ++childIndex)
		{
			vertex_t child = decomposition.child(node, childIndex);

			childBag.clear();
			nextChildBag.swap(childBag);

			if(childIndex + 1 < childCount)
			{
				decomposition.bagContent(
						decomposition.child(node, childIndex + 1),
						nextChildBag);

				//FIXME: Tree Decomposition should supply this
				nextJoinVertices->clear();
				set_intersection(childBag.begin(), childBag.end(),
						nextChildBag.begin(), nextChildBag.end(),
						back_inserter(*nextJoinVertices));
			}

			const ITupleSet &childTuples = tuples[child];

			//FIXME: remove debug code below
			std::cout << child << "(" << childTuples.size() << ",";
			//FIXME: end debug code

			unordered_set<IDynAspTuple *, IDynAspTuple::merge_hash>
				merged(childTuples.size());

			// project out unneeded vertices
			for(const ITuple &tuple : childTuples)
			{
				const IDynAspTuple &childTuple =
					static_cast<const IDynAspTuple &>(tuple);

				IDynAspTuple *reducedTuple =
					childTuple.project(info.rememberedAtoms);

				// merge reduced tuples that are now the same
				size_t bucketIndex = merged.bucket(reducedTuple);

				bool isIn = false;
				for(auto mergeIter = merged.begin(bucketIndex);
						 mergeIter != merged.end(bucketIndex);
						 ++mergeIter)
					if(true == (isIn = (*mergeIter)->merge(*reducedTuple)))
						break;

				if(!isIn) merged.insert(reducedTuple);
			}

			//FIXME: debug code
			std::cout << merged.size() << ",";
			//FIXME: end debug code
			
			
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
	
				//FIXME: remove debug code
				std::cout << prev.size() << ") ";
				//FIXME: end debug code
				
				firstChild = false;
				continue;
			}

			

			// elsewise, join our tuples to the current set
			for(IDynAspTuple *tuple : merged)
			{
				size_t bucketIndex = prev.bucket(tuple);
				IDynAspTuple *tmp;
				for(auto joinIter = prev.begin(bucketIndex);
						 joinIter != prev.end(bucketIndex);
						 ++joinIter)
					if(nullptr != (tmp = (*joinIter)->join(
									*joinVertices, *tuple)))
						curr.insert(tmp);

				delete tuple;
			}

			// delete old tuples in prev
			for(IDynAspTuple *tuple : prev)
				delete tuple;
			prev.clear();

			// move current tuples to prev for next iteration
			prev.swap(curr);
			swap(joinVertices, nextJoinVertices);

			//FIXME: remove debug code
			std::cout << prev.size() << ") ";
			//FIXME: end debug code

			// if after a join we have no tuples left, bail out
			if(prev.size() == 0)
				return;
		}

		//FIXME: remove debug code
		std::cout << std::endl;
		//FIXME: end debug code

		// for each remaining tuple, 
		for(IDynAspTuple *tuple : prev)
		{
			tuple->introduce(info, outputTuples);
			delete tuple;
		}

		//FIXME: remove debug code
		std::cout << "\ttuples: " << prev.size() << " " <<
			outputTuples.size() << std::endl;
		//FIXME: end debug code
	}

} // namespace dynasp
