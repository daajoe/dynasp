#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif
#include "../util/debug.hpp"

#include <dynasp/DependencyGraphEvaluator.hpp>
#include <htd/main.hpp>

extern htd::LibraryInstance* inst;

namespace dynasp
{
	using htd::vertex_t;
	using htd::IMultiHypergraph;

	DependencyGraphEvaluator::DependencyGraphEvaluator(const dynasp::IGroundAspInstance& aainst) : ainst(aainst), g(*new htd::DirectedGraph(inst)), leafAt(nullptr), linkedAt(nullptr), leafAtb(nullptr), linkedAtb(nullptr)
	{
		ainst.fetchDependencyGraph(g);
		leafAt = new std::unordered_set<htd::vertex_t>();
		linkedAt = new std::unordered_set<htd::vertex_t>();
	}


	void DependencyGraphEvaluator::select() const
	{
		DependencyGraphEvaluator* me = const_cast<DependencyGraphEvaluator*>(this);
		delete me->leafAtb;
		delete me->linkedAtb;
		me->leafAtb = leafAt;
		me->linkedAtb = linkedAt;
		me->leafAt = new std::unordered_set<htd::vertex_t>();
		me->linkedAt = new std::unordered_set<htd::vertex_t>();
	}

	double DependencyGraphEvaluator::evaluateValue(const htd::IMultiHypergraph&,
			const htd::ITreeDecomposition &td, bool reverse) const
	{
	#define CLEAN_INDUCT
		std::unordered_map<htd::vertex_t, VertexFound*> inducAt;
		VertexFound* atomsfound = new VertexFound();
		htd::PostOrderTreeTraversal fixJoin;
		int fitness = 0;
		htd::vertex_t vpre = 0;
		fixJoin.traverse(td, [&](htd::vertex_t v, htd::vertex_t, size_t) {
			assert(v);
			assert(atomsfound != nullptr);
			if (vpre && !td.isChild(v, vpre))	//new branch, remember previous one!
			{
				inducAt.emplace(vpre, atomsfound);
				atomsfound = new VertexFound();
			}
			for (vertex_t v2 : td.introducedVertices(v))
				if (ainst.isAtom(v2))
					atomsfound->insert(v2);
			if (vpre && td.childCount(v) >= 2) // && vpre == td.childAtPosition(v, td.childCount(v) - 1))	//we encountered previously the last children
			{
				assert(td.isChild(v, vpre));
				assert(td.childAtPosition(v, 0) == vpre);
				//I am childcount - 1, so start with childCount - 1 (gets -1 again next line!)
				for (unsigned int child = 1; child < td.childCount(v); ++child) //td.childCount(v) - 1; child > 0; --child)
				{
					htd::vertex_t childN =  td.childAtPosition(v, child);
					auto it = inducAt.find(childN);
					assert(it != inducAt.end());
					atomsfound->insert(it->second->begin(), it->second->end());
				#ifdef CLEAN_INDUCT
					delete it->second;
					inducAt.erase(it);
				#endif
				}
			}	
			//dist = it->second;
			//assert(dist==(int)dst);
			for (vertex_t v2 : td.introducedVertices(v))
			{
				if (!ainst.isAtom(v2))
					continue;
				for (vertex_t n = 0; n < (reverse ? g.outgoingNeighborCount(v2) : g.incomingNeighborCount(v2)); ++n)
				{
					fitness += 1 - (atomsfound->count(reverse ? g.outgoingNeighborAtPosition(v2,n) : g.incomingNeighborAtPosition(v2,n)));
					/*if ((atomsfound->count(reverse ? g.outgoingNeighborAtPosition(v2,n) : g.incomingNeighborAtPosition(v2,n))))
					{
						static_cast<htd::IMutableLabeledMultiHypergraph&>(const_cast<htd::ITreeDecomposition&>(td)).setVertexLabel("l", v, nullptr);
					}*/
				}
			}
			vpre = v;
		});
	#ifdef CLEAN_INDUCT
		assert(inducAt.size() == 0);
		delete atomsfound;
	#endif
		std::cout << "value: " << fitness << std::endl;
		return -(double)fitness;
	}

	double DependencyGraphEvaluator::evaluateDepth(const htd::IMultiHypergraph&,
			const htd::ITreeDecomposition &td, bool reverse)
	{
	#ifndef CLEAN_INDUCT
		for (const auto i : inducAt)
			delete i.second;
		inducAt.clear();
	#else
		//using VertexFound = std::unordered_set<vertex_t>;
		std::unordered_map<htd::vertex_t, VertexFound*> inducAt;
	#endif
		std::cout << "TD: " << &td << std::endl;
		depthAt.clear();
		leafAt->clear();
		linkedAt->clear();
		bool leaf = false;
		VertexFound* atomsfound = new VertexFound();
		//atomsfound.reserve(ainst.getAtomCount());
		htd::PostOrderTreeTraversal fixJoin;
		htd::PreOrderTreeTraversal depthPass;
		int fitness = 0, maxdepth = 0, dist = 0;
		htd::vertex_t vpre = 0;
		depthPass.traverse(td, [&](htd::vertex_t v, htd::vertex_t, size_t) {
			if (vpre && !td.isChild(vpre, v))
			{
				auto it = depthAt.find(td.parent(v));
				assert(it != depthAt.end());
				dist = it->second + 1;
			}
			depthAt.emplace(v, dist);
			maxdepth = std::max(maxdepth, dist);
			if (vpre && td.isChild(vpre, v))
				dist++;
			vpre = v;
		});
		vpre = 0;
		fixJoin.traverse(td, [&](htd::vertex_t v, htd::vertex_t, size_t) {
			assert(v);
			assert(atomsfound != nullptr);
			/*std::cout << "visit: " << v << "/" << vpre << std::endl;
			for (unsigned int child = 0; child < td.childCount(v); ++child) //td.childCount(v) - 1; child > 0; --child)
				std::cout << "chld: " << td.childAtPosition(v, child);
			std::cout << std::endl;*/

			if (vpre && !td.isChild(v, vpre))	//new branch, remember previous one!
			{
				//std::cout << "emplace: " << vpre << std::endl;
				inducAt.emplace(vpre, atomsfound);
				atomsfound = new VertexFound();
				leaf = false;
				//std::cout << "reset leaf" << std::endl;
			}
			//if (!vpre || td.isChild(v, vpre))
			{
				for (vertex_t v2 : td.introducedVertices(v))
						if (ainst.isAtom(v2))
							atomsfound->insert(v2);
				/*if (leaf)
					linkedAt.emplace(v);*/
			}
			//next time do not insert
			//vlock = vpre && (!td.childCount(vpre) || !td.isChild(vpre, v));
			/*if (td.childCount(v))
			{
				std::cout << vpre << "," << td.childAtPosition(v, td.childCount(v) - 1) << std::endl;
			}*/
			//assert(v != 6);
			/*if (td.childCount(v) == 2)
				std::cout << v << "," << td.childAtPosition(v, 0) << "," << td.childAtPosition(v,1) << std::endl;*/
			//assert(td.childCount(v) != 2);
			//if (vpre && td.childCount(v) >= 2) //&& vpre == td.childAtPosition(v, td.childCount(v) - 1))	//we encountered previously the last children
			//	assert(false);
			/*if (vpre && td.childCount(v) >= 2 && vpre == td.childAtPosition(v, 0))	//we encountered previously the last children
				assert(false);*/

			if (vpre && td.childCount(v) >= 2) // && vpre == td.childAtPosition(v, td.childCount(v) - 1))	//we encountered previously the last children
			{
				//std::cout << td.childCount(v) << std::endl;
				assert(td.isChild(v, vpre));
				assert(td.childAtPosition(v, 0) == vpre);
				//I am childcount - 1, so start with childCount - 1 (gets -1 again next line!)
				//bool linkedLeaf = leaf;
				for (unsigned int child = 1; !leaf && child < td.childCount(v); ++child) //td.childCount(v) - 1; child > 0; --child)
				{
					htd::vertex_t childN =  td.childAtPosition(v, child);
					leaf = /*leafAt.count(childN) ||*/ linkedAt->count(childN) == 1;
				}
				htd::vertex_t child0 = td.childAtPosition(v, 0);
				if (leaf && linkedAt->count(child0) == 0)
				{
					//std::cout << "0leaf " << child0 << " leaf by propagation" << std::endl;
					leafAt->emplace(child0);
					linkedAt->emplace(child0);
				}
				for (unsigned int child = 1; child < td.childCount(v); ++child) //td.childCount(v) - 1; child > 0; --child)
				{
					htd::vertex_t childN =  td.childAtPosition(v, child);
					auto it = inducAt.find(childN);
					assert(it != inducAt.end());
					atomsfound->insert(it->second->begin(), it->second->end());
				#ifdef CLEAN_INDUCT
					delete it->second;
					inducAt.erase(it);
				#endif
					if (leaf && linkedAt->count(childN) == 0)
					{
						//std::cout << child << "leaf " << childN << " leaf by propagation" << std::endl;
						leafAt->emplace(childN);
						linkedAt->emplace(childN);
					}
					//std::cout << "erase: " << childN << std::endl;
				}
				//I could be now became a new linked leaf
			}	
			if (leaf)
			{
				//std::cout << "node " << v << "leaf by propagation" << std::endl;
				linkedAt->emplace(v);
			}

			//std::cout << "atoms found" << std::endl;
			//std::cout << td.introducedVertices(v).size() << ":" << td.introducedVertices(v) << std::endl;
			auto it = depthAt.find(v);
			assert(it != depthAt.end());
			dist = it->second;
			//assert(dist==(int)dst);
			//for (vertex_t v2 : td.bagVertices(v)) //introducedVertices(v))
			for (vertex_t v2 : td.introducedVertices(v))
			//for (vertex_t v2 : td.forgottenVertices(v))
			{
				if (!ainst.isAtom(v2))
					continue;
				//std::cout << "now at " << v2 << std::endl;
				//std::cout << g.incomingNeighborCount(v2) << ";" << g.incomingNeighbors(v2) << std::endl;
				for (vertex_t n = 0; n < (reverse ? g.outgoingNeighborCount(v2) : g.incomingNeighborCount(v2)); ++n)
					
				//for (htd::Hyperedge h : g.hyperedges(v2))
				{
					//for (vertex_t vx : h.elements())
					{
						//if (vx != v2 && g.isIncomingNeighbor(v2, vx))
						{
							//maxdepth = std::max((int)dist, maxdepth);
							/*htd::vertex_t found = 0;
							for (auto v : *atomsfound)
								if (v == reverse ? g.outgoingNeighborAtPosition(v2,n) : g.incomingNeighborAtPosition(v2,n))
								{
									found = v;
									break;
								}*/
							if (atomsfound->count(reverse ? g.outgoingNeighborAtPosition(v2,n) : g.incomingNeighborAtPosition(v2,n)) == 0)
							{
								if (!leaf)
								{
									//std::cout  << "firstleaf: " << v << std::endl;
									leafAt->emplace(v);
									linkedAt->emplace(v);
									leaf = true;
								}
								fitness = std::max((int)dist, fitness);
							}
							//fitness += 1 - atomsfound.count(reverse ? g.outgoingNeighborAtPosition(v2,n) : g.incomingNeighborAtPosition(v2,n));
							//break;
						}
					}
				}
			}
			vpre = v;
			//fitness += td.bagContent(td.childAtPosition(v,0)).size() * td.bagContent(td.childAtPosition(v,1)).size();
		});
		//std::cout << inducAt.size() << std::endl;

	#ifdef CLEAN_INDUCT
		assert(inducAt.size() == 0);
		delete atomsfound;
	#else
		inducAt.emplace(td.root(), atomsfound);
	#endif
		std::cout << "depth: " << fitness << "/" << maxdepth << std::endl;
		return -(double)fitness;
	}

	htd::FitnessEvaluation* DependencyGraphEvaluator::evaluate(const htd::IMultiHypergraph& graph,
			const htd::ITreeDecomposition &td) const
	{
			//fitness += td.bagContent(td.childAtPosition(v,0)).size() * td.bagContent(td.childAtPosition(v,1)).size();
		//return new htd::FitnessEvaluation(2,  (const_cast<DependencyGraphEvaluator*>(this))->evaluateDepth(graph, td), evaluateValue(graph, td)); //-(double)fitness);
		return new htd::FitnessEvaluation(1, (const_cast<DependencyGraphEvaluator*>(this))->evaluateDepth(graph, td)); //, evaluateValue(graph, td)); //-(double)fitness);
	}

} // namespace dynasp
