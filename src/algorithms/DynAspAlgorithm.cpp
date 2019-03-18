#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif
#include "../util/debug.hpp" 

#include <string>
#include <sstream>

#include <dynasp/DynAspAlgorithm.hpp>
#include <dynasp/CertificateDynAspTuple.hpp>

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

	DynAspAlgorithm::DynAspAlgorithm() : eval_(nullptr), further(0)  {} //: impl(nullptr) { }

	DynAspAlgorithm::~DynAspAlgorithm()
	{
		//if(impl) delete impl;
	}

	vector<const ILabelingFunction *> DynAspAlgorithm::preprocessOperations()
		const
	{
		return vector<const ILabelingFunction *>();
	}


	void DynAspAlgorithm::evaluateNode(
			vertex_t node,
			const ITreeDecomposition &decomposition,
			INodeTupleSetMap &tuples,
			const IInstance &instance,
			ITupleSet &outputTuples) const
	{
		TreeNodeInfo infoinst
		{
				//TODO: nasty
			static_cast<IGroundAspInstance &>(const_cast<IInstance &>(instance)),
			vertex_container(),
			vertex_container(),
			vertex_container(),
			vertex_container()
		#ifdef INT_ATOMS_TYPE
			,/*0,0,*/ AtomPositionMap()
			,/*0,0,*/ AtomPositionMap()
		#endif
		/*#ifdef NON_NORM_JOIN
			, vertex_container(),
			vertex_container()
		#endif*/
	    };
		TreeNodeInfo& info = further ? infoinst.instance->getNodeData(node) : infoinst;
		//TODO: FIXME!!! reduces parallelism!
		/*if (further)
			CertificateDynAspTuple::createPointerCache();*/

		size_t childCount = decomposition.childCount(node);


		DBG("preceeding tuples: "); DBG(node); DBG("("); DBG(childCount); DBG(") "); DBG(" ");
	#ifdef DEBUG
		for (size_t i = 0; i < childCount; ++i)
		{
			DBG("["); DBG(decomposition.childAtPosition(node, i)); DBG("]"); DBG(",");
		}
	#endif
		DBG(outputTuples.size()); DBG(std::endl);
		#ifdef DEBUG
			for (const auto& o: outputTuples)
			{
				DBG(&o); DBG(",");  DBG(static_cast<const CertificateDynAspTuple&>(o).solutions_); DBG("@:"); DBG(static_cast<const CertificateDynAspTuple&>(o).weight_); DBG(","); DBG_COLL(static_cast<const CertificateDynAspTuple&>(o).atoms_);  DBG(","); DBG_COLL(static_cast<const CertificateDynAspTuple&>(o).reductAtoms_); static_cast<const CertificateDynAspTuple&>(o).debug(); DBG(","); DBG(static_cast<const CertificateDynAspTuple&>(o).isPseudo()); DBG(", orig_size: "); DBG(static_cast<const CertificateDynAspTuple&>(o).origins_.size());   DBG(std::endl);
				
			}
		#endif



		if (!further)
		{

			//TODO: compute introduced / remembered vertices wrt one child node => safe resources
			const ConstCollection<vertex_t> remembered =
					decomposition.rememberedVertices(node);
			const ConstCollection<vertex_t> introduced =
					decomposition.introducedVertices(node);
			
#ifdef INT_ATOMS_TYPE
			unsigned int pos = 0, rpos = 0;
			//IGroundAspInstance::AtomPositionMap atomAtPosition;
#endif


			for (auto i = introduced.begin(); i != introduced.end(); ++i)
			{
				vertex_t vertex = *i;

				//FIXME: do this via labelling functions
				if (info.instance->isRule(vertex))
				{
					info.introducedRules.push_back(vertex);
				#ifdef INT_ATOMS_TYPE
					if (create::get() == create::INCIDENCEPRIMAL_RULESETTUPLE)
					{
						info.ruleAtPosition.emplace(vertex, rpos);
						info.int_introducedRules |= 1 << rpos++;
					}
				#endif
				}
				else
				{
					info.introducedAtoms.push_back(vertex);
#ifdef INT_ATOMS_TYPE
					info.atomAtPosition.emplace(vertex, pos);
					if (info.instance->isNegatedAtom(vertex))
						info.int_negatedAtoms |= 1 << pos;
					else if (info.instance->isNegatedChoiceAtom(vertex))
						info.int_negatedChoiceAtoms |= 1 << pos; 
					if (info.instance->isConstrainedFactAtom(vertex))
						info.int_constrainedFactAtoms |= 1 << pos;
					else if (info.instance->isConstrainedNFactAtom(vertex))
						info.int_constrainedNFactAtoms |= 1 << pos;
					if (info.instance->isProjectionAtom(vertex))
						info.int_projectionAtoms |= 1 << pos;
					info.int_introducedAtoms |= 1 << pos++;
#endif
				}
			}

			for (auto i = remembered.begin(); i != remembered.end(); ++i)
			{
				vertex_t vertex = *i;

				//FIXME: do this via labelling functions
				if (info.instance->isRule(vertex))
				{
					info.rememberedRules.push_back(vertex);
					if (create::get() == create::PRIMAL_SIMPLETUPLE)
					{
						//assert(false);
						//TODO: keep in MIND; works only for SimpleDynAspTuple
						info.introducedRules.push_back(vertex);
					}
				#ifdef INT_ATOMS_TYPE
					else if (create::get() == create::INCIDENCEPRIMAL_RULESETTUPLE)
					{
						info.ruleAtPosition.emplace(vertex, rpos);
						info.int_rememberedRules |= 1 << rpos++;
					}
				#endif
				}
				else
				{
					info.rememberedAtoms.push_back(vertex);
#ifdef INT_ATOMS_TYPE
					info.atomAtPosition.emplace(vertex, pos);
					if (info.instance->isNegatedAtom(vertex))
						info.int_negatedAtoms |= 1 << pos;
					else if (info.instance->isNegatedChoiceAtom(vertex))
						info.int_negatedChoiceAtoms |= 1 << pos;	
					if (info.instance->isConstrainedFactAtom(vertex))
						info.int_constrainedFactAtoms |= 1 << pos;
					else if (info.instance->isConstrainedNFactAtom(vertex))
						info.int_constrainedNFactAtoms |= 1 << pos;
					if (info.instance->isProjectionAtom(vertex))
						info.int_projectionAtoms |= 1 << pos;
					info.int_rememberedAtoms |= 1 << pos++;
#endif
				}
			}
			#ifdef INT_ATOMS_TYPE
				//update positions to the right one!
				info.int_rememberedRules <<= pos;
				info.int_introducedRules <<= pos;
				for (auto& it : info.ruleAtPosition)
				{
					it.second += pos;

					DBG("rule "); DBG(it.first); DBG(std::endl);
				}
			#endif

		#ifdef NON_NORM_JOIN
			
			if (childCount >= 2)
			{
				const std::vector<vertex_t> *joined = computeJoinVertices(decomposition, node);
						//decomposition.joinVertices(node);

				for (auto i = joined->begin(); i != joined->end(); ++i)
				{
					vertex_t vertex = *i;
					//FIXME: do this via labelling functions
					if (info.instance->isRule(vertex))
					{
						if (create::get() == create::INCIDENCEPRIMAL_RULESETTUPLE)
						{
							DBG("join rule "); DBG(vertex); DBG(std::endl);
							#ifdef INT_ATOMS_TYPE
							info.int_joinRules |= 1 << info.ruleAtPosition.find(vertex)->second;
							#else
							assert(false);
							#endif
							//info.joinedRules.push_back(vertex);
						}
					}
					else
					{
						DBG("join atom "); DBG(vertex); DBG(std::endl);
						#ifdef INT_ATOMS_TYPE
						info.int_joinAtoms |= 1 << info.atomAtPosition.find(vertex)->second;
						#else
						assert(false);
						#endif
						//info.joinedAtoms.push_back(vertex);
					}
				}
				delete joined;
				DBG("JOINED: "); DBG(info.int_joinAtoms);
				assert(info.introducedAtoms.size() == 0);
			}
		#endif

#ifdef USE_EXT_REDUCT_SPEEDUP
#ifdef INT_ATOMS_TYPE
			//assert(0);
			info.int_negatedAtoms = 0;
			pos = 0;
			for (const auto& i : info.introducedAtoms)
			{
				for (const auto& r : info.introducedRules)
					if (info.instance->rule(r).isNegativeBodyAtom(i))
					{
						info.int_negatedAtoms |= 1 << pos;
						break;
					}
				for (const auto& r : info.rememberedRules)
					if (info.instance->rule(r).isNegativeBodyAtom(i))
					{
						info.int_negatedAtoms |= 1 << pos;
						break;
					}

				++pos;
			}
			for (const auto& i : info.rememberedAtoms)
			{
				for (const auto& r : info.introducedRules)
					if (info.instance->rule(r).isNegativeBodyAtom(i))
					{
						info.int_negatedAtoms |= 1 << pos;
						break;
					}
				for (const auto& r : info.rememberedRules)
					if (info.instance->rule(r).isNegativeBodyAtom(i))
					{
						info.int_negatedAtoms |= 1 << pos;
						break;
					}

				++pos;
			}

#endif
#endif
			//info.int_negatedAtoms =0;

			//FIXME: remove bitvectors to calculate subsets, then this is not needed
			if (sizeof(size_t) * CHAR_BIT < (remembered.size() + introduced.size()))
				throw std::runtime_error("Treewidth too high.");

			DBG(node); DBG(": ra"); DBG_COLL(info.rememberedAtoms); DBG(" rr");
			DBG_COLL(info.rememberedRules); DBG(" ia");
			DBG_COLL(info.introducedAtoms); DBG(" ir");
			DBG_COLL(info.introducedRules); DBG(" children: ");

			//std::cout << POP_CNT(info.int_negatedAtoms) << " / " << POP_CNT(info.getAtoms()) << std::endl;
		}


#ifdef USE_LAME_BASE_JOIN
#ifndef INT_ATOMS_TYPE
		atom_vector baseVertices, nextBaseVertices;
		unordered_set<vertex_t> joinBase;
		// contains vertices already joined together
#else
		atom_vector baseVertices = 0, nextBaseVertices = 0;
		atom_vector tupleVertices = 0, nextTupleVertices = 0;
		atom_vector joinBase = 0;//, dummy;//, currJoin;
#endif
		vertex_t child;



		if(childCount != 0)
		{
			child = decomposition.childAtPosition(node, 0);

		#ifdef INT_ATOMS_TYPE
			/*currJoin = info.transform(child);
			joinBase |= currJoin;*/
			nextTupleVertices = info.transform(child);
			joinBase |= nextTupleVertices;
		#else
			const std::vector<vertex_t>& childBag =
				decomposition.bagContent(child);

			for(auto i = childBag.begin(); i != childBag.end(); ++i)
				joinBase.insert(*i);
		#endif
		}

		#ifdef INT_ATOMS_TYPE
		atom_vector v1 = 0, v2 = 0, *joinVertices = &v1, *nextJoinVertices = &v2;
		#else
		atom_vector v1, v2, *joinVertices = &v1, *nextJoinVertices = &v2;
		#endif
		//TODO: LEAF ugly stuff in case of INT_ATOMS_TYPE
		unordered_set<IDynAspTuple *, IDynAspTuple::join_hash>
			prev(1, IDynAspTuple::join_hash(*joinVertices, info)),
			curr(1, IDynAspTuple::join_hash(*nextJoinVertices, info));

		bool firstChild = true;
#else
		std::vector<IDynAspTuple*> prev;
#endif


		//assert(childCount <= 2);
		// for each child, project and join tuples
		if(childCount == 0)
		{
			IDynAspTuple *tt = create::tuple(true);
#ifdef USE_LAME_BASE_JOIN
			prev.insert(tt);
#else
			prev.push_back(tt);
#endif
			/*if (further)
				for (auto& t : outputTuples)
					static_cast<CertificateDynAspTuple*>(tt)->evolution_.push_back(&static_cast<CertificateDynAspTuple&>(t));*/
		}
#ifndef USE_LAME_BASE_JOIN
		else if (childCount >= 2)	//equijoin
		{
			std::vector<unsigned int> its(childCount, 0);
			std::vector<std::vector<IDynAspTuple *>*> begs(childCount, nullptr);
			std::vector<std::unordered_map<std::size_t, std::vector<IDynAspTuple *> > > joins;

			//its.reserve(childCount);
			//begs.reserve(childCount);
			joins.reserve(childCount);
			DBG("JOIN "); DBG(node); DBG(" with "); DBG(childCount); DBG(std::endl);

			//unsigned int hash = (unsigned int)-1;
			//bool firstOk = true;
			for (unsigned int childIndex = 0; childIndex < childCount; ++childIndex)
			{
				htd::vertex_t child = decomposition.childAtPosition(node, childIndex);
				joins.emplace_back(tuples[child].size());
				for (ITuple &tuple : tuples[child])
				{
					DBG("testing tuple: "); DBG(&tuple); DBG(std::endl);
					if (static_cast<IDynAspTuple *>(&tuple)->isClone())
						continue;
					assert(info.getJoinAtoms() == info.getAtoms() || dynasp::create::isNon());
					std::size_t h = static_cast<IDynAspTuple *>(&tuple)->joinHash(child,
#ifdef INT_ATOMS_TYPE
	#ifdef NON_NORM_JOIN
																				info.getJoinAtoms()
	#else
																				   info.getAtoms()
	#endif
#else

	#ifdef NON_NORM_JOIN
			assert(false)	//TODO
	#else
																			decomposition.bagContent(node)
	#endif
#endif
							, info);

					std::vector<IDynAspTuple*> &v = joins[childIndex].emplace(h, std::vector<IDynAspTuple*>()).first->second;
					v.push_back(static_cast<IDynAspTuple *>(&tuple));
					/*if (hash == (unsigned int)-1)
					{
						//its.emplace_back(0);
						begs.emplace_back(&(v));
						hash = h;

						std::cout << "found for " << 0 << " hash " << h << "," << &(v) << std::endl;
					}*/


					DBG("adding "); DBG(&tuple); DBG(" of child "); DBG(child); DBG(" with "); DBG_COLL(static_cast<CertificateDynAspTuple*>(&tuple)->atoms_); DBG("; "); DBG_COLL(static_cast<CertificateDynAspTuple*>(&tuple)->reductAtoms_); DBG(" to "); DBG(h); DBG(std::endl);
				#ifdef STRICT_MODE
					assert(v.size() <= 2);
				#endif

				}
				/*if (firstOk && childIndex > 0)
				{
					std::unordered_map<unsigned int, std::vector<IDynAspTuple *> >::iterator it = joins[childIndex].find(hash);
					if ((firstOk = it != joins[childIndex].end()))
					{
						std::cout << "found for " << childIndex  << " hash " << hash << "," << &(it->second) << std::endl;
						//its.emplace_back(0);
						begs.emplace_back(&(it->second));
					}
				}
				if (!firstOk)*/
				{
					//its.emplace_back();
					//begs.emplace_back();
				}
			}


			CertificateDynAspTuple *joined = static_cast<CertificateDynAspTuple*>(create::tuple(false));
			unsigned int idx = 0;// joins.size() - 1;
			//unsigned int res = 1;
			for (auto &pj : joins[0])
			{

				DBG("joining "); DBG(pj.first); DBG(","); DBG(&pj.second); DBG(std::endl);
				/*if (!firstOk)
				{
					firstOk = true;
					idx = 0;
					continue;
				}*/
				//if (!idx)
				{
					bool firstOk = false;
					for (unsigned int childIndex = 0; childIndex < childCount; ++childIndex)
					{
						std::unordered_map<std::size_t, std::vector<IDynAspTuple *> >::iterator it = joins[childIndex].find(pj.first);
						if (!(firstOk = it == joins[childIndex].end()))
						{
							//std::cout << "found for " << childIndex  << " hash " << pj.first << ","<< &(it->second) << std::endl;
							assert(begs[childIndex] != &(it->second));
							begs[childIndex] = &(it->second);
							its[childIndex] = 0;
							//res *= it->second.size();
						}
						else
							break;
					}
					if (firstOk)
					{
						idx = 0;
						continue;        //skip!
					}
					//else
					{
						//firstOk = true;
						idx = childCount - 1;
					}
				}

#define SIM
				std::unordered_set<CertificateDynAspTuple *, IDynAspTuple::merge_hash
#ifdef SIM
						, IDynAspTuple::ComputedCertificateTuplesEqual
#endif
				> generated;
				//generated.reserve(res);
				//(ends[0] - begs[0], IDynAspTuple::join_hash(*joinVertices, info)),
				//std::unordered_map<unsigned int, IDynAspTuple*> generated; //(tuples[decomposition.childAtPosition(node, 0)].size());
				//std::cout << "NEW MAP " << std::endl;
				/*for (unsigned int i = 0; i < childCount; ++i)
					std::cout << begs[i]->size() << std::endl;*/
				for (; its[0] != begs[0]->size(); ++its[idx])
				{
					if (its[idx] != begs[idx]->size())
					{
						idx = childCount - 1;
						CertificateDynAspTuple::ExtensionPointer p(childCount);
						CertificateDynAspTuple::EJoinResult pseudoJoin = joined->join(info, its, begs, decomposition, node, p);

						#ifdef BASE_JOIN_BOOST
						if (
#ifdef NOT_MERGE_JOIN
!create::isNon() &&
#endif
further && pseudoJoin != CertificateDynAspTuple::EJR_PSEUDO)
							continue;
						#endif

						if (pseudoJoin == CertificateDynAspTuple::EJR_NO)
							continue;
#ifndef SIM
						size_t bucketIndex = generated.bucket(joined);
						bool isIn = false;
						for(auto mergeIter = generated.begin(bucketIndex);
								 !isIn && mergeIter != generated.end(bucketIndex);
								 ++mergeIter)
							if ((isIn = *(*mergeIter) == *joined))
								(*mergeIter)->merge(*joined, &p);

						if(!isIn)
						{
							generated.insert(joined);
						#ifdef NON_NORM_JOIN
							//prev.push_back(joined);
							outputTuples.insert(joined);
						#else
							outputTuples.insert(joined);
						#endif
						#ifdef EXTENSION_POINTERS_SET_TYPE
							joined->origins_.insert(p);
						#else
							DBG("JOINTOX: "); DBG(p); DBG(std::endl);
							joined->origins_.emplace_back(p);
						#endif
							DBG("ORIGIN FOR JOIN "); DBG(joined); DBG(": "); DBG(p); DBG(std::endl);
							joined = static_cast<CertificateDynAspTuple*>(create::tuple(false));
						}
#else
						const auto itg = generated.emplace(joined);
						if (itg.second)
						{

							DBG("ORIGIN FOR JOIN "); DBG(joined); DBG(": "); DBG(p); DBG(std::endl);
#ifdef EXTENSION_POINTERS_SET_TYPE
							joined->origins_.insert(std::move(p));
#else
							joined->origins_.emplace_back(std::move(p));
#endif

							DBG("NEW NODE "); DBG(joined); DBG_COLL(joined->atoms_); DBG(","); DBG_COLL(joined->reductAtoms_); DBG(" with hash "); DBG(joined->mergeHash()); DBG(";");

#ifdef INT_ATOMS_TYPE
							DBG(joined->joinHash(
							info.getAtoms(), info));
#else
							DBG(joined->joinHash(
							decomposition.bagContent(node)
							, info));
#endif
							DBG(std::endl);
						#ifdef NON_NORM_JOIN
							//prev.push_back(joined);
							outputTuples.insert(joined);
						#else
							outputTuples.insert(joined);
						#endif
							joined = static_cast<CertificateDynAspTuple*>(create::tuple(false));
						}
						else
							(*itg.first)->merge(*joined, &p);
#endif
					}
					else
					{
						assert(idx > 0);
						its[idx] = 0;    //reset
						--idx;
					}
				}
				idx = 0;
			}
			delete joined;
		}
		else
		{
			//std::cout << "EXCHANGE " << node << std::endl;
			htd::vertex_t child = decomposition.childAtPosition(node, 0);
			ITupleSet &childTuples = tuples[child];

			DBG(child); //DBG_COLL(*joinVertices);
			DBG("("); DBG(childTuples.size()); DBG(",");

			unordered_set<IDynAspTuple *, IDynAspTuple::merge_hash>
				merged(childTuples.size());

			prev.reserve(childTuples.size());

					// project out unneeded vertices
			for (ITuple &tuple : childTuples)
			{
				//assert(!further ^ (static_cast<CertificateDynAspTuple&>(tuple).isPseudo()  || static_cast<CertificateDynAspTuple&>(tuple).evolution_.size()));
				if (static_cast<IDynAspTuple *>(&tuple)->isClone())
						continue;

				const IDynAspTuple &childTuple =
						static_cast<const IDynAspTuple &>(tuple);
			/*#ifdef REUSE_NON_SOLUTIONS
				if (!further && childTuple.solutionCount() == 0)
					continue;
			#endif*/
				IDynAspTuple *reducedTuple = childTuple.project(info, child);

				//childTuple_nc.projectPtrs(reducedTuple);

				if(!reducedTuple) continue;

				IDynAspTuple &childTuple_nc =
					static_cast<IDynAspTuple &>(tuple);
				reducedTuple->projectPtrs(childTuple_nc);

				// merge reduced tuples that are now the same
				size_t bucketIndex = merged.bucket(reducedTuple);

				bool isIn = false;
				for(auto mergeIter = merged.begin(bucketIndex);
						 !isIn && mergeIter != merged.end(bucketIndex);
						 ++mergeIter)
					if ((isIn = (*mergeIter)->merge(*reducedTuple)))
						(*mergeIter)->mergePtrs(*reducedTuple,  true); //(*mergeIter)->isNewProjectionOk(further));

				if(!isIn)
				{
					//if (reducedTuple->isNewProjectionOk(further))
					{
						merged.insert(reducedTuple);
						prev.push_back(reducedTuple);
					}
					/*else
						delete reducedTuple;*/
				}
				else
					delete reducedTuple;	//FIXED: MEMLEAK!
			}

		}
#else
		else for(size_t childIndex = 0; childIndex < childCount; ++childIndex)
		{
	#ifdef COMBINE_PSEUDO_PSEUDO_SOLUTIONS
		if (childCount >= 2) {
			std::unordered_map<unsigned int, unsigned int> tst;
			for (size_t childIndex = 0; childIndex < childCount; ++childIndex)
				for (ITuple &tuple : tuples[decomposition.childAtPosition(node, childIndex)])
					tst.emplace(((CertificateDynAspTuple&)tuple).joinHash(info.getAtoms(), info), 0).first->second++;
					//tst.emplace(((CertificateDynAspTuple&)tuple).mergeHash(), 0).first->second++;
					//tst.insert((CertificateDynAspTuple*)&tuple);
			for (auto it : tst)
			{
				/*std::cout << "same hash: " << it.first << std::endl;
				for (size_t childIndex = 0; childIndex < childCount; ++childIndex)
				{



					std::cout << "child " << childIndex << std::endl;
					for (ITuple &tuple : tuples[decomposition.childAtPosition(node, childIndex)])
						//if (	((CertificateDynAspTuple&)tuple).mergeHash() == it.first) {
						if (	((CertificateDynAspTuple&)tuple).joinHash(info.getAtoms(), info) == it.first) {
							std::cout << ((CertificateDynAspTuple&)tuple).atoms_ << "," << (((CertificateDynAspTuple&)tuple).reductAtoms_ & info.getAtoms()) << "," << (((CertificateDynAspTuple&)tuple).reductAtoms_)  << std::endl;
*/ {{




/*std::cout << "Hash(" <<   ((size_t(((CertificateDynAspTuple&)tuple).reductAtoms_)

                #ifdef COMBINE_PSEUDO_PSEUDO_SOLUTIONS
		                & (TO_INT((unsigned int)(-1)) & (0 << (INT_ATOMS_TYPE - 1)))
				                #endif
						                ) << (INT_ATOMS_TYPE)) << std::endl;

std::cout << "Hash(" <<   ((size_t(((CertificateDynAspTuple&)tuple).reductAtoms_)

                #ifdef COMBINE_PSEUDO_PSEUDO_SOLUTIONS
		                & (TO_INT((unsigned int)(-1)) & (0 << (INT_ATOMS_TYPE - 1)))
				                #endif
						                )) << std::endl;*/
//std::cout <<  & (TO_INT((unsigned int)(-1)))  << std::endl;


							
							}
				}
				assert(it.second <= childCount);
			}
			tst.clear();
			}
		#endif
			if (further)
			{
				DBG("EXCHANGE");
				DBG(std::endl);
			}
			child = decomposition.childAtPosition(node, childIndex);
#ifndef INT_ATOMS_TYPE
			vertex_t nextChild;

			if(childIndex + 1 < childCount)
			{
				nextChild = decomposition.childAtPosition(node, childIndex + 1);
				const std::vector<vertex_t>& nextBag =
					decomposition.bagContent(nextChild);

				//FIXME: tree decomposition should supply this
				nextJoinVertices->clear();
				nextBaseVertices.clear();
				nextBaseVertices.insert(
						nextBaseVertices.end(),
						joinBase.begin(),
						joinBase.end());

				for(auto i = nextBag.begin(); i != nextBag.end(); ++i)
					if(!joinBase.insert(*i).second)	//already inside the set
						nextJoinVertices->push_back(*i);
			}
#else

			swap(tupleVertices, nextTupleVertices);
			//assert(childCount <= 2);
			//atom_vector currJoin = info.transform(child);
			/*if (childIndex + 1 < childCount)
			{
				*nextJoinVertices = *joinVertices | (info.int_rememberedAtoms & currJoin);
				assert(*joinVertices == 0);
				atom_vector nextJoin = info.transform(decomposition.childAtPosition(node, childIndex + 1));
				assert(*nextJoinVertices == (joinBase & nextJoin));
			}*/

			if (childIndex + 1 < childCount)
			{
				nextTupleVertices = info.transform(decomposition.childAtPosition(node, childIndex + 1));
				*nextJoinVertices = joinBase & nextTupleVertices;
				nextBaseVertices = joinBase;
				joinBase |= nextTupleVertices;
			}
#endif

			ITupleSet &childTuples = tuples[child];

			DBG(child); DBG_COLL(*joinVertices);
			DBG("("); DBG(childTuples.size()); DBG(",");

			unordered_set<IDynAspTuple *, IDynAspTuple::merge_hash>
				merged(childTuples.size()), currmerged;

		
					// project out unneeded vertices
			for (ITuple &tuple : childTuples)
			{

				if (static_cast<IDynAspTuple *>(&tuple)->isClone())
						continue;

				//assert(!further ^ (static_cast<CertificateDynAspTuple&>(tuple).isPseudo()  || static_cast<CertificateDynAspTuple&>(tuple).evolution_.size()));

				const IDynAspTuple &childTuple =
						static_cast<const IDynAspTuple &>(tuple);
			/*#ifdef REUSE_NON_SOLUTIONS
				if (!further && childTuple.solutionCount() == 0)
					continue;
			#endif*/
				IDynAspTuple *reducedTuple = childTuple.project(info, child);

			//childTuple_nc.projectPtrs(reducedTuple);

			if(!reducedTuple) continue;

			IDynAspTuple &childTuple_nc =
				static_cast<IDynAspTuple &>(tuple);
			reducedTuple->projectPtrs(childTuple_nc);

			// merge reduced tuples that are now the same
			size_t bucketIndex = merged.bucket(reducedTuple);

			bool isIn = false;
			for(auto mergeIter = merged.begin(bucketIndex);
					 !isIn && mergeIter != merged.end(bucketIndex);
					 ++mergeIter)
				if ((isIn = (*mergeIter)->merge(*reducedTuple)))
					(*mergeIter)->mergePtrs(*reducedTuple, true);

			if(!isIn) merged.insert(reducedTuple);
			else
				delete reducedTuple;	//FIXED: MEMLEAK!
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
#ifndef INT_ATOMS_TYPE
#else
			/*atom_vector *jtmp = joinVertices;
			joinVertices = nextJoinVertices;
			nextJoinVertices = jtmp;*/
#endif
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

									*joinVertices, //info.int_rememberedAtoms,
									*tuple,
								#ifdef INT_ATOMS_TYPE
									tupleVertices  //dummy
								#else
									decomposition.bagContent(child)
								#endif	
									
									)))
					{
//#define MORE_WORK_JOIN
#ifdef MORE_WORK_JOIN
						tmp->joinPtrs((*joinIter), tuple, false);
#endif
						size_t bucket = currmerged.bucket(tmp);
						bool isIn = false;
						for(auto mergeIter = currmerged.begin(bucket);
								 !isIn && mergeIter != currmerged.end(bucket);
								 ++mergeIter)
							if ((isIn = (*mergeIter)->merge(*tmp)))
							{
#ifdef MORE_WORK_JOIN
								(*mergeIter)->mergePtrs(*tmp, false);
#else
								(*mergeIter)->joinPtrs((*joinIter), tuple, false);
#endif
							}

						if(isIn)
						{
							delete tmp;
							continue;
						}
#ifndef MORE_WORK_JOIN
						else
							//tmp->joinPtrs((*joinIter), tuple, false);
							tmp->joinPtrs((*joinIter), tuple, false);
#endif
						currmerged.insert(tmp);
						curr.insert(tmp);
					}
			/*#ifdef COMBINE_PSEUDO_PSEUDO_SOLUTIONS
				if (!static_cast<CertificateDynAspTuple*>(tuple)->evolution_.size())
			#endif*/
				delete tuple;
			}

			//TODO: make delete stuff again
			// delete old tuples in prev

			for(IDynAspTuple *tuple : prev)
			/*#ifdef COMBINE_PSEUDO_PSEUDO_SOLUTIONS
				if (!static_cast<CertificateDynAspTuple*>(tuple)->evolution_.size())
			#endif*/

				delete tuple;
			prev.clear();

			// move current tuples to prev for next iteration
			prev.swap(curr);


			swap(joinVertices, nextJoinVertices);
			swap(baseVertices, nextBaseVertices);
#ifndef INT_ATOMS_TYPE
#else
			/*atom_vector *jtmp = joinVertices;
			joinVertices = nextJoinVertices;
			nextJoinVertices = jtmp;*/
#endif
			DBG(prev.size()); DBG(") ");

			// if after a join we have no tuples left, bail out
			if(prev.size() == 0)
				goto bailout;
				//return;
		}
#endif
		DBG(std::endl);

		//unsigned int managedTuples = 0;
		//ITupleSet::iterator managedTuples = outputTuples.begin();
		// for each remaining tuple, 
		{

		IDynAspTuple::ComputedCertificateTuples certTuplesComputed;
//#ifdef SECOND_PASS_COMPRESSED
		//IDynAspTuple::EvaluatedTuples tupleIntroDone;
	#ifdef COMBINE_PSEUDO_PSEUDO_SOLUTIONS
		if (dynasp::create::isCompr())
		{
		certTuplesComputed.reserve(outputTuples.size());
		for (unsigned int out = 0; out < outputTuples.size(); ++out)
		{
			auto *ptr = static_cast<CertificateDynAspTuple *>(outputTuples[out]);
			auto it = certTuplesComputed.insert(ptr);
			assert(it.second);
		}
		}
	#endif
	//#endif
		size_t maxsize = outputTuples.size();
		for(IDynAspTuple *tuple : prev)
		{

			//assert(!further || (childCount == 0 || static_cast<CertificateDynAspTuple*>(tuple)->evolution_.size()));

			//sharp::TupleSet intermediateOutputTuples;
			assert (further || !tuple->isClone()); // && (static_cast<CertificateDynAspTuple *>(tuple)->isPseudo() || tuple->solutionCount() > 0))
			if (!tuple->isClone()) // && (static_cast<CertificateDynAspTuple *>(tuple)->isPseudo() || tuple->solutionCount() > 0))
			{
				if (further)
				{
					//#ifdef SECOND_PASS_COMPRESSED
					if (dynasp::create::isCompr())
					//assert(false);
						tuple->introduceFurtherCompressed(info, /*tupleIntroDone,*/ certTuplesComputed, outputTuples, maxsize);
					else //#else
					//assert(false);
						tuple->introduceFurther(info, outputTuples);
					//#endif
				}
				else
					tuple->introduce(info, certTuplesComputed, outputTuples);
				//TODO: FIXME, schircher shice, why copy?? CONST SCHMONST KA** CORRECTNESS
				//tuple->insertPtrs(intermediateOutputTuples);
	/*#ifdef COMBINE_PSEUDO_PSEUDO_SOLUTIONS
					if (!static_cast<CertificateDynAspTuple*>(tuple)->evolution_.size())
				#endif*/
			}
			delete tuple;
		}

		//#ifdef SECOND_PASS_COMPRESSED
		//IDynAspTuple::EvaluatedTuples tupleIntroDone;
			//const_cast<IDynAspTuple::ComputedCertificateTuples&>(certTuplesComputed).clear();
		//#endif

		}

		//std::cout << "POSTEVALUATE" << std::endl;
		#ifndef SINGLE_LAYER_
		//TODO: JOIN NODES?
		//if (childCount <= 1)			//JOIN nodes do not need reversepointers

		if (!create::isSatOnly())
			for(ITuple& tuple : outputTuples)
			{
				IDynAspTuple &dtuple = static_cast<IDynAspTuple &>(tuple);
				dtuple.postEvaluate(further);

			}
		#endif

		//std::cout << "POSTEVALUATE_FINISHED" << std::endl;

		bailout:
#ifdef CLEANUP_SOLUTIONS
		//return;
		/*#ifdef THREE_PASSES
		//if (further)
		#endif*/
		if (!create::isSatOnly())
		{
		for(size_t childIndex = 0; childIndex < childCount; ++childIndex)
		{
			htd::vertex_t child = decomposition.childAtPosition(node, childIndex);
			ITupleSet &childTuples = tuples[child];

			for (size_t pos = 0; pos < childTuples.size(); ++pos)
			{
				//if (!further)		//first pass
				{
					if (static_cast<IDynAspTuple *>(childTuples[pos])->cleanUp(child, decomposition, tuples,  dynasp::create::passes() >= 3 && !dynasp::create::isProjection() ?  (further & (dynasp::create::passes() < 4)) : true //!create::isSatOnly()
/*#ifdef THREE_PASSES
															   further
#else
																			   true
#endif*/
					))
						--pos;
				}
				//else if (static_cast<IDynAspTuple *>(childTuples[pos])->cleanUp())
				//	--pos;
				/*//TODO: make readable, cleanup!
				if (further && static_cast<CertificateDynAspTuple *>(childTuples[pos])->isPseudo())// && childCount == 1)	//NO JOIN NODE
					static_cast<CertificateDynAspTuple *>(childTuples[pos])->origins_.clear();*/
			}
		}

		//std::cout << "CLEANUP_FIN" << std::endl;
		/*return;*/

		if (((dynasp::create::isProjection()) || (create::get() == create::INCIDENCEPRIMAL_RULESETTUPLE)) && node == decomposition.root())		//cleanup after every pass
			for (size_t pos = 0; pos < outputTuples.size(); ++pos)
				/*if (further)
				{
					if (static_cast<IDynAspTuple *>(outputTuples[pos])->cleanUpRoot())
					{
						outputTuples.erase(pos);
						--pos;
					}
				}
				else*/ if (static_cast<IDynAspTuple *>(outputTuples[pos])->cleanUpRoot(node, info, decomposition, tuples, dynasp::create::passes() >= 3 && !dynasp::create::isProjection() ? (further & (dynasp::create::passes() < 4)) : true //!create::isSatOnly()
/*#ifdef THREE_PASSES
															   further
#else
																			   true
#endif*/
									))
				{
					outputTuples.erase(pos);
					--pos;
				}

		/*if (further)		//not needed any more
			tuples.clear();*/
			/*for (ITuple &tuple : outputTuples)
				static_cast<IDynAspTuple &>(tuple).cleanUpRoot();*/
		//TODO: remove the things from outputTuples?
		}
#endif

		//bailout:
	//#ifdef THREE_PASSES
		if (!further)
	//#endif
		{
			info.instance->setNodeData(node, std::move(infoinst));
			//std::cout << "setNode " << node << std::endl;
			/*IGroundAspInstance::NodeData (info.int_introducedAtoms, info.int_rememberedAtoms, std::move(info.introducedAtoms), std::move(info.rememberedAtoms), std::move(atomAtPosition))
		);*/
		}

		DBG(std::endl);
		DBG("resulting tuples: "); DBG(node); DBG("("); DBG(childCount); DBG(") "); DBG(" with "); DBG(prev.size()); DBG(" ");
	#ifdef DEBUG
		for (size_t i = 0; i < childCount; ++i)
		{
			DBG("["); DBG(decomposition.childAtPosition(node, i)); DBG("]"); DBG(",");
		}
	#endif
		DBG(outputTuples.size()); DBG(std::endl);
		#ifdef DEBUG
			for (const auto& o: outputTuples)
			{
				DBG(&o); DBG(",");  DBG(static_cast<const CertificateDynAspTuple&>(o).solutions_); DBG("@:"); DBG(static_cast<const CertificateDynAspTuple&>(o).weight_); DBG(","); DBG_COLL(static_cast<const CertificateDynAspTuple&>(o).atoms_);  DBG(","); DBG_COLL(static_cast<const CertificateDynAspTuple&>(o).reductAtoms_); static_cast<const CertificateDynAspTuple&>(o).debug(); DBG(","); DBG(static_cast<const CertificateDynAspTuple&>(o).isPseudo()); DBG(", orig_size: "); DBG(static_cast<const CertificateDynAspTuple&>(o).origins_.size()); DBG(" supp: "); 
			#ifdef SUPPORTED_CHECK	
				DBG(static_cast<const CertificateDynAspTuple&>(o).supp_);   
			#endif	
				DBG(std::endl);
				
			}
		#endif

		DBG("resulting tuples: "); DBG(node); DBG(" with "); DBG(outputTuples.size()); DBG(" ");
		/*if (further)
		{
			std::stringstream timefor;
			timefor << "PASS 2 / Node " << node << " with " << childCount << " and " << outputTuples.size() << " outputs";
			sharp::Benchmark::registerTimestamp(timefor.str().c_str());
			std::cout << timefor.str().c_str() << std::endl;
			//assert(childCount <= 4);
		}*/
	}


	bool DynAspAlgorithm::needAllTupleSets() const
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
