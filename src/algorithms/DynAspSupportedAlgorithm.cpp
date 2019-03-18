#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif
#include "../util/debug.hpp" 

#include <dynasp/DynAspSupportedAlgorithm.hpp>
#include <dynasp/CertificateDynAspTuple.hpp>

#include "../instances/GroundAspInstance.hpp"
#include "../../include/dynasp/IDynAspTuple.hpp"
#include "../../include/dynasp/DynAspSupportedAlgorithm.hpp"

#include <dynasp/TreeNodeInfo.hpp>
#include <dynasp/create.hpp>

#include <unordered_set>
#include <algorithm>
#include <climits>
#include <vector>

//FIXME: use config.h info here, build check into autoconf
#include <sys/times.h>

#ifdef HAVE_UNISTD_H //FIXME: what if not def'd?
	#include <unistd.h>
#endif

#ifdef SUPPORTED_CHECK

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

#ifdef DEBUG_INFO
	static unsigned long join = 0, leaf = 0, exchange = 0, splitup = 0, reuse = 0, first = 0;
#endif


	DynAspSupportedAlgorithm::DynAspSupportedAlgorithm() { } //: impl(nullptr) { }

	DynAspSupportedAlgorithm::~DynAspSupportedAlgorithm() {}
	
	void DynAspSupportedAlgorithm::onExit() const
	{
		std::cout << "exit support generation" << std::endl;
	#ifdef DEBUG_INFO
		std::cout << "LEAF: " << leaf << std::endl;
		std::cout << "EXCHANGE: " << exchange << std::endl;
		std::cout << "JOIN: " << join << std::endl;

		std::cout << "FIRSTS: " << first << std::endl;
		std::cout << "REUSES: " << reuse << std::endl;
		std::cout << "SPLITUPS: " << splitup << std::endl;

	#endif
	}

	//TODO: change first layer in order to produce ALL models (including pseudo solutions), modify first layer algorithm, remove true second layer algorithm
	//TODO: further increase compression due to recovering certificates among ALL tuples, not only those where we originated from
	//TODO: data structure refactoring, search stuff performance increase

	CertificateDynAspTuple* DynAspSupportedAlgorithm::insertCompressed(TupleTypes& tupleTypes, atom_vector tupleType, CertificateDynAspTuple& me, CertificateDynAspTuple::ExtensionPointers::iterator& origin, const TreeNodeInfo& info, htd::vertex_t firstChild) const
	{
		//std::cout << "INSERT_COMPRESS " << &tupleType << " @ " << tupleType.size() << std::endl;
		DBG("INSERT_COMPRESS"); DBG("\n");
		DBG(&tupleType); 
		if 
		#ifdef USE_N_PASSES
			(me.certsdone_ > further)
		#else
			(me.supp_ != (~((atom_vector)0)) >> 1) //certificate_pointer_set_ != nullptr) //tupleTypes.size() > 0)	//me is still empty
		#endif
		{
			CertificateDynAspTuple* found = nullptr;
			DBG("SEARCHING "); DBG(tupleTypes.size()); DBG("\n");
			//tupleTypes.find(tupleType);

		#ifdef VEC_CERTS_TYPE
			for (CertificateDynAspTuple* t : tupleTypes)
			{
				//assert(t->certificate_pointer_set_ != nullptr);
				//TODO: @BUG here (operator ==) [if both are vectors]
				//TODO: BUG here (operator==) [if both are vectors]
				if (*t->supp_ == tupleType)
				{
					found = t;
					break;
				}
			}
		#else
			//std::cout << "SEARCHING FOR " << &tupleType << " @ " << tupleType.size() << std::endl;
			auto it = tupleTypes.find(tupleType);
			if (it != tupleTypes.end())
				found = it->second;
		#endif
			if (found)
			{
#ifdef DEBUG_INFO
				++reuse;
#endif
				DBG("FOUND REORG");
				if (found != &me)
				{
					//TODO: merge with WEIGHTS/COUNTS -> see DynASP2
					//mpz_class count, weight;
					//removeExtensionPointer(count, weight);
					//keep * in mind for join nodes
					//TODO: move semantics!
					DBG("BEFORE REORG "); DBG(found->atoms_); DBG(" "); DBG(found->reductAtoms_); DBG(" "); DBG(found->solutions_); DBG(" @ "); DBG(found->weight_); DBG(std::endl);
					DBG("BEFORE REORG "); DBG(found->atoms_); DBG(" "); DBG(found->reductAtoms_); DBG(" "); DBG(me.solutions_); DBG(" @ "); DBG(me.weight_); DBG(std::endl);
					if (!me.isPseudo())
					found->mergeData(&me, origin, info, firstChild);		//* vs min.

					DBG("AFTER REORG "); DBG(found->atoms_); DBG(" "); DBG(found->reductAtoms_); DBG(" "); DBG(found->solutions_); DBG(" @ "); DBG(found->weight_); DBG(std::endl);
					DBG("AFTER REORG "); DBG(found->atoms_); DBG(" "); DBG(found->reductAtoms_); DBG(" "); DBG(me.solutions_); DBG(" @ "); DBG(me.weight_); DBG(std::endl);

				#ifdef EXTENSION_POINTERS_SET_TYPE
					found->origins_.insert(*origin);
				#else
					found->origins_.emplace_back(*origin);
				#endif
					origin = me.origins_.erase(origin);
				}
				else
					++origin;
			}
			else	//split up, make new tuple since the certificates differ
			{
#ifdef DEBUG_INFO
				++splitup;
#endif
				DBG("NEW/SPLITTING UP"); DBG("\n");
				//DONE: insert, split up, remanage pointers, store compressed stuff for further references
				//CertificateDynAspTuple* newTuple = me.clone(tupleType, std::move(me.origins_[meOrigin]));
				//TODO: FIXME, why not working?
				CertificateDynAspTuple* newTuple = me.clone(nullptr, *origin);
				newTuple->supp_ = tupleType;	
				for (auto* up : me.getClone()->evolution_)
				{
					unsigned int mePos = (unsigned int)-1;
				#if defined(EXTENSION_POINTERS_SET_TYPE) || defined(EXTENSION_POINTERS_LIST_TYPE)
					std::vector<CertificateDynAspTuple::ExtensionPointer> tmp;
					for (const auto & e : up->origins_)
					{
						
				#else
					unsigned int sz = up->origins_.size();
					for (unsigned int p = 0; p < sz; ++p)
					{
						const CertificateDynAspTuple::ExtensionPointer& e = up->origins_[p];
				#endif
						if (mePos == (unsigned int)-1)
						{
							unsigned int pos = 0;
							for (const auto* tuple : e)
							{
								if (tuple == &me)
								{
									mePos = pos;
									break;
								}
								++pos;
							}
						}
						if (mePos != (unsigned int)-1 && e[mePos] == &me)
						{
							CertificateDynAspTuple::ExtensionPointer cp = e;
							cp[mePos] = newTuple;

					#if defined(EXTENSION_POINTERS_SET_TYPE) || defined(EXTENSION_POINTERS_LIST_TYPE)
						tmp
					#else
						up->origins_
					#endif
							.emplace_back(std::move(cp));
						}
					}
				
				#ifdef EXTENSION_POINTERS_SET_TYPE
					up->origins_.reserve(up->origins_.size() + tmp.size());
					for (const auto& t : tmp)
						up->origins_.emplace(std::move(t));
				#else
				#ifdef EXTENSION_POINTERS_LIST_TYPE
					up->origins_.insert(up->origins_.end(), make_move_iterator(tmp.begin()), make_move_iterator(tmp.end()));
				#endif
				#endif
				}
				DBG("BEFORE NEW REORG "); DBG(newTuple->atoms_); DBG(" "); DBG(newTuple->reductAtoms_); DBG(" ");  DBG(newTuple->solutions_); DBG(" @ "); DBG(newTuple->weight_); DBG(std::endl);
				DBG("BEFORE NEW REORG "); DBG(me.atoms_); DBG(" "); DBG(me.reductAtoms_); DBG(" ");  DBG(me.solutions_); DBG(" @ "); DBG(me.weight_); DBG(std::endl);

				//Done: manage counter, weights
				if (!me.isPseudo())
				newTuple->mergeData(&me, origin, info, firstChild, true);		//* vs min.
			
				DBG("AFTER NEW REORG "); DBG(newTuple->atoms_); DBG(" "); DBG(newTuple->reductAtoms_); DBG(" ");  DBG(newTuple->solutions_); DBG(" @ "); DBG(newTuple->weight_); DBG(std::endl);
				DBG("AFTER NEW REORG "); DBG(me.atoms_); DBG(" "); DBG(me.reductAtoms_); DBG(" ");  DBG(me.solutions_); DBG(" @ "); DBG(me.weight_); DBG(std::endl);

				origin = me.origins_.erase(origin);

			#ifdef VEC_CERTS_TYPE
				tupleTypes.push_back(newTuple);
			#else
				auto ins = tupleTypes.emplace(newTuple->supp_, newTuple);
				assert(ins.second);
			#endif
				DBG(&me); DBG(" ~> "); DBG(newTuple); DBG("\n");
				return newTuple;			//new tuple (me) should be added!
			}
		}
		else
		{
#ifdef DEBUG_INFO
			++first;
#endif
			DBG("SIMPLE/FIRST OF TUPLE"); DBG("\n");
			me.supp_ = tupleType;
			#ifdef USE_N_PASSES
				me.certsdone_ = further + 1;
			#endif
		#ifdef VEC_CERTS_TYPE
			tupleTypes.push_back(&me);
		#else
			auto ins = tupleTypes.emplace(me.supp_, &me);
			assert(ins.second);
		#endif
			origin++;
		}
		return nullptr; 	//no new tuple has to be added
	}

//#define SIMPLE

	void DynAspSupportedAlgorithm::evaluateNode(
			vertex_t node,
			const ITreeDecomposition &decomposition,
			INodeTupleSetMap &
#if defined(CLEANUP_SOLUTIONS) || defined(CLEANUP_COUNTER_SOLUTIONS)
				tuples
#endif
			,
			const IInstance &instance,
			ITupleSet &outputTuples) const
	{
		DBG("---------------------------------"); DBG(std::endl);
		//std::cout << "TD: " << &decomposition << "," << node << std::endl;

		TreeNodeInfo &info = static_cast<IGroundAspInstance &>(const_cast<IInstance&>(instance)).getNodeData(node);
		size_t childCount = decomposition.childCount(node);

		ITupleSet &meTuples = outputTuples;//tuples[node];
		unsigned int sz = meTuples.size();

	/*#ifdef SUPPORTED_CHECK
		//if (node != decomposition.root())
		{
			unsigned int anzSave = 0;
			for (size_t pos = 0; pos < childCount; ++pos)
			{
				TreeNodeInfo & pInfo = info.instance->getNodeData(decomposition.childAtPosition(node, pos));
				anzSave += POP_CNT(pInfo.getAtoms()) - POP_CNT(pInfo.transform(node));
			}
			if (anzSave >= 2)
				std::cout << POP_CNT(info.getAtoms()) << "," << anzSave << std::endl;
		}
	#endif*/

		if (childCount == 0)
		{
			#ifdef DEBUG_INFO
				leaf++;
			#endif

			for (size_t pos = 0; pos < sz; ++ pos)
			{
				DBG(pos);
				ITuple &tuple = *meTuples[pos];
				CertificateDynAspTuple& me = static_cast<CertificateDynAspTuple&>(tuple);
				me.supp_ = me.checkSupportedRules(info);
				++first;
				DBG("              ============"); DBG(std::endl);
				DBG("LEAF TUPLE ("); DBG(&tuple); DBG(","); DBG_COLL(static_cast<CertificateDynAspTuple&>(tuple).atoms_); DBG(","); DBG_COLL(static_cast<CertificateDynAspTuple&>(tuple).reductAtoms_); DBG(")"); DBG(me.supp_); DBG("\n");
			}
		}
		else
		{
	#ifdef DEBUG_INFO
			if (childCount == 1)
				++exchange;
			else
				++join;
	#endif

			htd::vertex_t firstChild = decomposition.childAtPosition(node, 0);
			for (size_t pos = 0; pos < sz; ++ pos)
			{
				DBG(pos);
				ITuple &tuple = *meTuples[pos];
				DBG("              ============"); DBG(std::endl);
				DBG("EXCHANGE TUPLE ("); DBG(&tuple); DBG(","); DBG_COLL(static_cast<CertificateDynAspTuple&>(tuple).atoms_); DBG(","); DBG_COLL(static_cast<CertificateDynAspTuple&>(tuple).reductAtoms_); DBG(")"); DBG("\n");
				CertificateDynAspTuple& me = static_cast<CertificateDynAspTuple&>(tuple);
				DBG("isClone ("); DBG(me.isClone()); DBG("), "); DBG(me.certificate_pointer_set_); DBG("<sols: "); DBG(me.solutions_); DBG(" @"); DBG(me.weight_); DBG(">");

				DBG("\n");
				me.supp_ = (~((atom_vector)0)) >> 1;
				//assert (me.getClone() == &me);
				if (me.isPseudo() || me.solutionCount() == 0 /*|| me.isClone()*/ || 
				#ifdef USE_N_PASSES
					me.certsdone_ > further) 
				#else
					me.supp_ != (~((atom_vector)0) >> 1)) //certificate_pointer_set_ != nullptr)
				#endif
					continue;
				DBG("post");
				CertificateDynAspTuple::ExtensionPointers::iterator ptrOrigins;

		
			#ifdef SIMPLE
				//TODO: FIXME: WTFISTHAT: I should burn in hell for this :P
				if (!me.isPseudo())
				{
					BigNumber cnt = 0;
					for (ptrOrigins = me.origins_.begin(); ptrOrigins != me.origins_.end(); ++ptrOrigins) // && pos < sz; ++pos)
					{
						auto& down = *ptrOrigins;
						BigNumber interm = 1;
						for (const auto* downptr : down)	//
							interm *= downptr->solutionCount();
						cnt += interm;
					}
					me.solutions_ = cnt;
				}
			#endif

				atom_vector mesupp = me.checkSupportedRules(info); 
				DBG("PRESUPP: ==> "); DBG(mesupp); DBG(std::endl);
				TupleTypes tupleTypes; //CertificateDynAspTuple::DynAspCertificatePointer> certificate_signature;
				for (ptrOrigins = me.origins_.begin(); ptrOrigins != me.origins_.end(); ) // && pos < sz; ++pos)
				{
					auto& down = *ptrOrigins;
					assert(down.size() >= 1);
					atom_vector supp = mesupp;
					std::size_t cpos = 0;
					for (const auto* downptr : down)	//
					{
						//supp |= downptr->supp_;
						DBG("postdown"); DBG("\n");
					#ifdef INT_ATOMS_TYPE
						htd::vertex_t child = decomposition.childAtPosition(node, cpos);
						atom_vector suppch = info.transform(downptr->supp_, child);
						DBG("CHILD_SUPP "); DBG(child); DBG("old: "); DBG(downptr->supp_); DBG(" ==> "); DBG(suppch); DBG(std::endl);
						DBG( ~suppch & info.int_rememberedAtoms); DBG(" vs "); DBG( info.instance->getNodeData(child).getAtoms() & ~(downptr->supp_)); DBG(std::endl);
						atom_vector nsuppch =  info.instance->getNodeData(child).getAtoms() & ~(downptr->supp_);
						//TODO: watch out!!
						//if (POP_CNT(~suppch & info.int_rememberedAtoms) < POP_CNT(info.instance->getNodeData(child).getAtoms() & ~(downptr->supp_)))
						if (downptr->supp_ == ~((atom_vector)0) || POP_CNT(info.transform(nsuppch, child)) < POP_CNT(nsuppch))
						{
							cpos = childCount + 1;
							break;
						}
						supp |= suppch;
					#else
						assert(false);
					#endif
						++cpos;
					}

					if (node != decomposition.root())
					{
						TreeNodeInfo &cInfo = info.instance->getNodeData(decomposition.parent(node));
						if (cpos < childCount + 1 && POP_CNT(~supp & info.getAtoms()) > POP_CNT(cInfo.transform(~supp & info.getAtoms(), node)))
							cpos = childCount + 1;
					}
				#ifdef DEBUG
					if (cpos < childCount + 1)
					{
						DBG("SUPP! ==> "); DBG(supp); DBG(std::endl);
					}
				#endif
					/*cpos = 0;
					for (const auto* downptr : down)
					{
						htd::vertex_t child = decomposition.childAtPosition(node, cpos);
						if ((POP_CNT(~supp & info.getAtoms()) < POP_CNT(info.instance->getNodeData(child).getAtoms() & ~(downptr->supp_))))
						{
							cpos = childCount + 1;
							break;
						}
						++cpos;
					}*/
					
					
					assert(supp != (~((atom_vector)0)) >> 1);
					assert(supp != ~((atom_vector)0));
					if (cpos == childCount + 1)
					{
						DBG("KILLING unsupported");
						DBG(std::endl);
						supp = ~((atom_vector)0);
					#ifdef SIMPLE
						if (!me.isPseudo())
							me.mergeData(&me, ptrOrigins, info, firstChild, true);
						//newTuple->mergeData(&me, origin, info, firstChild, true);		// vs min.
						ptrOrigins = me.origins_.erase(ptrOrigins);
						//propagateUnsupported(*ptrOrigins, true);
						//TODO: evolution_!
					#endif
					}
				#ifdef SIMPLE
					else
				#endif
					//if (cpos < childCount + 1)
					{
						CertificateDynAspTuple *t = insertCompressed(tupleTypes, supp, me, ptrOrigins, info, firstChild);
						if (t)
							outputTuples.insert(t);
					}
					/*else	//erase
					{
						DBG("KILLING unsupported");
						DBG(std::endl);
						if (!me.isPseudo())
						me.mergeData(&me, ptrOrigins, info, firstChild, true);
						//newTuple->mergeData(&me, origin, info, firstChild, true);		// vs min.
						ptrOrigins = me.origins_.erase(ptrOrigins);
						//propagateUnsupported(*ptrOrigins, true);
						//TODO: evolution_!
					}*/
				}

#ifdef CLEANUP_SOLUTIONS
				/*for (size_t childIndex = 0; childIndex < childCount; ++childIndex)
				{
					htd::vertex_t child = decomposition.childAtPosition(node, childIndex);
					ITupleSet &childTuples = tuples[child];

					for (size_t pos = 0; pos < childTuples.size(); ++pos)
					{
						if (static_cast<IDynAspTuple *>(childTuples[pos])->cleanUp(child, decomposition, tuples, false ))
							--pos;
					}
				}*/
				
				//DANGEROUS!
				//crashes solutions!
				/*if (node == decomposition.root())		//cleanup after every pass
					for (size_t pos = 0; pos < outputTuples.size(); ++pos)
						if (static_cast<IDynAspTuple *>(outputTuples[pos])->cleanUpNonSolRoot(node, info, decomposition, tuples, false))
						{
							//assert(false);
							outputTuples.erase(pos);
							--pos;
						}*/
#endif
			}
		}

		DBG(std::endl);
		DBG(outputTuples.size()); DBG(std::endl);
	}

	bool DynAspSupportedAlgorithm::needAllTupleSets() const
	{
		return !further; //false;
	}

	/*
	void DynAspSupportedAlgorithm::propagateUnsupported(const CertificateDynAspTuple* t, bool del) const
	{
		for (auto* up : t->evolution_)
		{
			for (auto it = up->origins_.begin(); it != up->origins_.end(); )
			{
				bool found = false;
				for (auto* p : *it)
					if (p == t)
					{
						found = true;
						break;
					}

				if (found)
				{
					up->mergeData(up, it, info, firstChild, true);
					if (del)
						it = up->origins_.erase(it);
					else
						++it;
				}
				else
					++it;
			}
			propagateUnsupported(up, up->origins_.size() == 0);
		}
		if (del)
			t->evolution_.clear();
	}*/

} // namespace dynasp

#endif
