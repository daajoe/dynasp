#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif
#include "../util/debug.hpp" 

#include <string>
#include <sstream>

#include <dynasp/ClaspAlgorithm.hpp>
//#include <dynasp/CertificateDynAspTuple.hpp>

#include "../instances/GroundAspInstance.hpp"

#include <dynasp/TreeNodeInfo.hpp>
#include <dynasp/create.hpp>

//#include <sharp/TupleSet.hpp>

#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <climits>
#include <vector>

//#define USE_LAME_BASE_JOIN

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

	ClaspAlgorithm::ClaspAlgorithm() : further(false), ext(0) {} //: impl(nullptr) { }

	ClaspAlgorithm::~ClaspAlgorithm()
	{
		//if(impl) delete impl;
	}

	vector<const ILabelingFunction *> ClaspAlgorithm::preprocessOperations()
		const
	{
		return vector<const ILabelingFunction *>();
	}


	void ClaspAlgorithm::evaluateNode(
			vertex_t node,
			const ITreeDecomposition &decomposition,
			INodeTupleSetMap &/*tuples*/,
			const IInstance &instance,
			ITupleSet &/*outputTuples*/) const
	{
		
	{
		const ConstCollection<vertex_t> introduced =
					decomposition.introducedVertices(node);
		bool found = false;	
		//#define INVERSE	
		//#define HEURISTIC
			for (auto i = introduced.begin(); i != introduced.end(); ++i)
			{
				vertex_t vertex = *i;

				//FIXME: do this via labelling functions
				if (static_cast<const IGroundAspInstance&>(instance).isRule(vertex))
				{
					const IGroundAspRule& r = static_cast<const IGroundAspInstance&>(instance).rule(vertex);
					if (((ClaspAlgorithm*)(this))->intros.insert(&r).second)
					{
				#ifdef HEURISTIC
						if (!r.isRule())
						{
							std::cerr << r << ":- e0" << " ." << std::endl;
							found = (ext == 0);
						}
						else
						{
							//std::cerr << r << " e0 ." << std::endl;
							std::cerr << r << " e0" << " ." << std::endl;
							found = (ext == 0);
						}
				#else
					#ifdef INVERSE
						if (!r.isRule())
						{
							std::cerr << r << ":- e" << ext << " ." << std::endl;
							found = true;
						}
						else
						{
							//std::cerr << r << " e0 ." << std::endl;
							std::cerr << r << " e" << ext << " ." << std::endl;
							found = true;
							//found = (ext == 0);
						}
					#else
						if (!r.isRule())
							std::cerr << r << " ." << std::endl;
						else
						{
							std::cerr << r << " e" << ext << " ." << std::endl;
							found = true;
						}
					#endif
				#endif
					}
				}
				else
				{
					{
				#ifdef HEURISTIC
						std::cerr << "_heuristic(a" << vertex << ",level," << ext << ")." << std::endl;
						found = true;
				#endif
					}
				}
			}
		if (found)
		{
			#ifndef HEURISTIC
			std::cerr << "#external " << " e" << ext << " ." << std::endl;
			#else
			if (ext == 0)
				std::cerr << "#external " << " e" << ext << " ." << std::endl;
			#endif
			const_cast<ClaspAlgorithm*>(this)->ext++;
		}
		if (node == decomposition.root())
		{
			std::cerr <<   static_cast<const IGroundAspInstance&>(instance).toString() << std::endl;
		#ifndef HEURISTIC
			std::cerr << "#script (python)." << std::endl << "MAX_EXT=" << (ext - 1) << std::endl << "#end." << std::endl;
		#else
			std::cerr << "#script (python)." << std::endl << "MAX_EXT=" << (0) << std::endl << "#end." << std::endl;
		#endif
		}
	}
	}

	bool ClaspAlgorithm::needAllTupleSets() const
	{
		return dynasp::create::passes() >= 2 ? true : false;
/*#ifdef SEVERAL_PASSES
				true
#else
			false
#endif*/
				;
	}

} // namespace dynasp
