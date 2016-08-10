#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif
#include "../util/debug.hpp" 

#include <dynasp/DynAspCertificateAlgorithm.hpp>
#include <dynasp/CertificateDynAspTuple.hpp>

#include "../instances/GroundAspInstance.hpp"
#include "../../include/dynasp/IDynAspTuple.hpp"
#include "../../include/dynasp/DynAspCertificateAlgorithm.hpp"

#include <dynasp/TreeNodeInfo.hpp>
#include <dynasp/create.hpp>

#include <unordered_set>
#include <algorithm>
#include <climits>
#include <vector>
#include <cassert>

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


	DynAspCertificateAlgorithm::DynAspCertificateAlgorithm() { } //: impl(nullptr) { }

	DynAspCertificateAlgorithm::~DynAspCertificateAlgorithm()
	{
	#ifdef DEBUG_INFO
        //TODO: JSON output
        sharp::Benchmark::output()->debug("LEAF", leaf);
		sharp::Benchmark::output()->debug("EXCHANGE", exchange);
		sharp::Benchmark::output()->debug("JOIN", join);
		sharp::Benchmark::output()->debug("FIRSTS", first);
		sharp::Benchmark::output()->debug("REUSES", reuse);
		sharp::Benchmark::output()->debug("SPLITUPS", splitup);
	#endif
		//if(impl) delete impl;
	}

	vector<const ILabelingFunction *> DynAspCertificateAlgorithm::preprocessOperations()
		const
	{
		return vector<const ILabelingFunction *>();
	}

	//TODO: change first layer in order to produce ALL models (including pseudo solutions), modify first layer algorithm, remove true second layer algorithm
	//TODO: further increase compression due to recovering certificates among ALL tuples, not only those where we originated from
	//TODO: data structure refactoring, search stuff performance increase


#ifdef USE_OPTIMIZED_EXTENSION_POINTERS
	CertificateDynAspTuple* DynAspCertificateAlgorithm::insertCompressed(TupleTypes& tupleTypes, TupleType& tupleType, CertificateDynAspTuple& me, CertificateDynAspTuple::ExtensionPointers& origin, const TreeNodeInfo& info)
	{
		//std::cout << "INSERT_COMPRESS " << &tupleType << " @ " << tupleType.size() << std::endl;
		DBG("INSERT_COMPRESS"); DBG("\n");
		DBG(&tupleType); 
		for (const auto& v : tupleType)
		{
			//for (const auto& v : t)
			{
				DBG(" "); DBG(v.cert); DBG_COLL(v.cert->atoms_); DBG(" "); DBG_COLL(v.cert->reductAtoms_); DBG(" "); DBG(v.strict); DBG(" "); DBG(v.cert->solutions_); DBG(std::endl);
			}
			DBG(std::endl);
		}
		if (me.certificate_pointer_set_ != nullptr) //tupleTypes.size() > 0)	//me is still empty
		{
			CertificateDynAspTuple* found = nullptr;
			DBG("SEARCHING "); DBG(tupleTypes.size()); DBG("\n");
			//tupleTypes.find(tupleType);

		#ifdef VEC_CERTS_TYPE
			for (CertificateDynAspTuple* t : tupleTypes)
			{
				assert(t->certificate_pointer_set_ != nullptr);
				//TODO: @BUG here (operator ==) [if both are vectors]
				//TODO: BUG here (operator==) [if both are vectors]
				if (*t->certificate_pointer_set_ == tupleType)
				{
					found = t;
					break;
				}
			}
		#else
			//std::cout << "SEARCHING FOR " << &tupleType << " @ " << tupleType.size() << std::endl;
			auto it = tupleTypes.find(&tupleType);
			if (it != tupleTypes.end())
				found = it->second;
			/*if (!found)
			{
				std::cout << "CHECKING for " << &tupleType << std::endl;
				//assert(false);
				for (const auto & t : tupleTypes)
				{
					std::cout << t.first << "," << t.second << std::endl;
					if (t.first == &tupleType)
					{
						std::cout << "found" << std::endl;
						assert(false);
					}
				}
			}*/
		#endif
			if (found)
			{
#ifdef DEBUG_INFO
				++reuse;
#endif
				DBG("FOUND");
				if (found != &me)
				{
					//TODO: merge with WEIGHTS/COUNTS -> see DynASP2
					//mpz_class count, weight;
					//removeExtensionPointer(count, weight);
					//keep * in mind for join nodes
					//TODO: move semantics!
				#ifdef EXTENSION_POINTERS_SET_TYPE
					found->origins_.insert(origin);
				#else
					found->origins_.push_back(origin);
				#endif
					//DONE: remove origin from me!
					found->mergeData(&me, origin, info);		//* vs min.

					/*std::cout << "\tPRECANDIDATE (";
					for (auto candO = origin->begin(); candO != origin->end(); ++candO)
						std::cout << (*candO) << ",";
					std::cout << ")" << std::endl;*/

					if (me.non_evolution == nullptr)
						me.non_evolution = new CertificateDynAspTuple::ExtensionPointerTabuList();
					me.non_evolution->insert(origin);
					//origin = me.origins_.erase(origin);

					/*std::cout << "\tPOSTCANDIDATE (";
					for (auto candO = origin->begin(); candO != origin->end(); ++candO)
						std::cout << (*candO) << ",";
					std::cout << ")" << std::endl;*/

					//origin = me.origins_.erase(origin);
					//found->increaseCounter(count);
					//found->increaseWeight(weight);
				}
				//free resources
				delete &tupleType;
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
				//up->origins_.insert(up->origins_.end(), {newTuple});
				//up->origins_.insert(up->origins_.end(), CertificateDynAspTuple::ExtensionPointer({newTuple}));
				CertificateDynAspTuple* newTuple = me.clone(tupleType, *origin);
				for (auto* up : me.evolution_)
				{
					//CertificateDynAspTuple::ExtensionPointers p({{newTuple}});
					//p.push_back(newTuple);

					unsigned int mePos = (unsigned int)-1;
				#ifdef EXTENSION_POINTERS_SET_TYPE
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

							auto& target = 
					#ifdef EXTENSION_POINTERS_SET_TYPE
						tmp
					#else
						up->origins_
					#endif
							;
							target.push_back(e);
							target[target.size() - 1][mePos] = newTuple;
						}
					}
				
				#ifdef EXTENSION_POINTERS_SET_TYPE
					for (const auto& t : tmp)
						up->origins_.emplace(std::move(t));	
				#endif
					//up->origins_.emplace_back({{newTuple}});
				}
				//Done: manage counter, weights
				newTuple->mergeData(&me, origin, info, true);		//* vs min.

				/*std::cout << "\tPRECANDIDATE (";
				for (auto candO = origin->begin(); candO != origin->end(); ++candO)
					std::cout << (*candO) << ",";
				std::cout << ")" << std::endl;*/
				
				if (me.non_evolution == nullptr)
					me.non_evolution = new CertificateDynAspTuple::ExtensionPointerTabuList();
				me.non_evolution->insert(origin);

				/*std::cout << "\tPOSTCANDIDATE (";
				for (auto candO = origin->begin(); candO != origin->end(); ++candO)
					std::cout << (*candO) << ",";
				std::cout << ")" << std::endl;*/

				assert(newTuple->certificate_pointer_set_ != nullptr);
			#ifdef VEC_CERTS_TYPE
				tupleTypes.push_back(newTuple);
			#else
				auto ins = tupleTypes.emplace(newTuple->certificate_pointer_set_, newTuple);
				assert(ins.second);
				//std::cout << "          INSERT        " << newTuple << std::endl;
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
			me.certificate_pointer_set_ = &tupleType;
			assert(me.certificate_pointer_set_ != nullptr);
		#ifdef VEC_CERTS_TYPE
			tupleTypes.push_back(&me);
		#else
			auto ins = tupleTypes.emplace(me.certificate_pointer_set_, &me);
			assert(ins.second);
			//std::cout << "          INSERT        " << &me << std::endl;
		#endif
		}
		return nullptr; 	//no new tuple has to be added
	}
#endif


CertificateDynAspTuple* DynAspCertificateAlgorithm::insertCompressed(TupleTypes& tupleTypes, TupleType& tupleType, CertificateDynAspTuple& me, CertificateDynAspTuple::ExtensionPointers::iterator& origin, const TreeNodeInfo& info, htd::vertex_t firstChild)
	{
		//std::cout << "INSERT_COMPRESS " << &tupleType << " @ " << tupleType.size() << std::endl;
		DBG("INSERT_COMPRESS"); DBG("\n");
		DBG(&tupleType); 
	#ifdef DEBUG
		for (const auto& v : tupleType)
		{
			//for (const auto& v : t)
			{
				DBG(" "); DBG(v.cert); DBG_COLL(v.cert->atoms_); DBG(" "); DBG_COLL(v.cert->reductAtoms_); DBG(" "); DBG(v.cert->isPseudo()); DBG(" "); DBG(v.strict); DBG(" "); DBG(v.cert->solutions_); DBG(std::endl);
			}
			DBG(std::endl);
		}
	#endif
		if (me.certificate_pointer_set_ != nullptr) //tupleTypes.size() > 0)	//me is still empty
		{
			CertificateDynAspTuple* found = nullptr;
			DBG("SEARCHING "); DBG(tupleTypes.size()); DBG("\n");
			//tupleTypes.find(tupleType);

		#ifdef VEC_CERTS_TYPE
			for (CertificateDynAspTuple* t : tupleTypes)
			{
				assert(t->certificate_pointer_set_ != nullptr);
				//TODO: @BUG here (operator ==) [if both are vectors]
				//TODO: BUG here (operator==) [if both are vectors]
				if (*t->certificate_pointer_set_ == tupleType)
				{
					found = t;
					break;
				}
			}
		#else
			//std::cout << "SEARCHING FOR " << &tupleType << " @ " << tupleType.size() << std::endl;
			auto it = tupleTypes.find(&tupleType);
			if (it != tupleTypes.end())
				found = it->second;
			/*if (!found)
			{
				std::cout << "CHECKING for " << &tupleType << std::endl;
				//assert(false);
				for (const auto & t : tupleTypes)
				{
					std::cout << t.first << "," << t.second << std::endl;
					if (t.first == &tupleType)
					{
						std::cout << "found" << std::endl;
						assert(false);
					}
				}
			}*/
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
					DBG("BEFORE REORG "); DBG(found->solutions_); DBG(" @ "); DBG(found->weight_); DBG(std::endl);
					DBG("BEFORE REORG "); DBG(me.solutions_); DBG(" @ "); DBG(me.weight_); DBG(std::endl);

					found->mergeData(&me, origin, info, firstChild);		//* vs min.

					DBG("AFTER REORG "); DBG(found->solutions_); DBG(" @ "); DBG(found->weight_); DBG(std::endl);
					DBG("AFTER REORG "); DBG(me.solutions_); DBG(" @ "); DBG(me.weight_); DBG(std::endl);

				#ifdef EXTENSION_POINTERS_SET_TYPE
					found->origins_.insert(*origin);
				#else
					found->origins_.emplace_back(*origin);
				#endif
					//DONE: remove origin from me!

					/*std::cout << "\tPRECANDIDATE (";
					for (auto candO = origin->begin(); candO != origin->end(); ++candO)
						std::cout << (*candO) << ",";
					std::cout << ")" << std::endl;*/

					origin = me.origins_.erase(origin);

					/*std::cout << "\tPOSTCANDIDATE (";
					for (auto candO = origin->begin(); candO != origin->end(); ++candO)
						std::cout << (*candO) << ",";
					std::cout << ")" << std::endl;*/

					//origin = me.origins_.erase(origin);
					//found->increaseCounter(count);
					//found->increaseWeight(weight);
				}
				else
					++origin;
				//free resources
				delete &tupleType;
			}
			else	//split up, make new tuple since the certificates differ
			{
#ifdef USE_CLEVER_COMPRESSION
				for (const auto& t : tupleTypes)
				{
					if (t.second->certificate_pointer_set_)

				}
#endif
#ifdef DEBUG_INFO
				++splitup;
#endif
				DBG("NEW/SPLITTING UP"); DBG("\n");
				//DONE: insert, split up, remanage pointers, store compressed stuff for further references
				//CertificateDynAspTuple* newTuple = me.clone(tupleType, std::move(me.origins_[meOrigin]));
				//TODO: FIXME, why not working?
				//up->origins_.insert(up->origins_.end(), {newTuple});
				//up->origins_.insert(up->origins_.end(), CertificateDynAspTuple::ExtensionPointer({newTuple}));
				CertificateDynAspTuple* newTuple = me.clone(tupleType, *origin);
				for (auto* up : me.evolution_)
				{
					//CertificateDynAspTuple::ExtensionPointers p({{newTuple}});
					//p.push_back(newTuple);

					unsigned int mePos = (unsigned int)-1;
				#if defined(EXTENSION_POINTERS_SET_TYPE) || defined(EXTENSION_POINTERS_LIST_TYPE)
					std::vector<CertificateDynAspTuple::ExtensionPointer> tmp;
					//tmp.reserve(up->origins_.size());
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
							//assert(mePos != (unsigned int)-1);
						}
						if (mePos != (unsigned int)-1 && e[mePos] == &me)
						{
							CertificateDynAspTuple::ExtensionPointer cp = e;
							cp[mePos] = newTuple;

						/*#ifdef EXTENSION_POINTERS_SET_TYPE
							up->origins_.insert(std::move(cp));
						#else
							up->origins_.emplace_back(std::move(cp));
						#endif*/
					#if defined(EXTENSION_POINTERS_SET_TYPE) || defined(EXTENSION_POINTERS_LIST_TYPE)
						tmp
					#else
						up->origins_
					#endif
							.emplace_back(std::move(cp));
							//target[target.size() - 1][mePos] = newTuple;
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
					//up->origins_.emplace_back({{newTuple}});
				}
				DBG("BEFORE NEW REORG "); DBG(newTuple->solutions_); DBG(" @ "); DBG(newTuple->weight_); DBG(std::endl);
				DBG("BEFORE NEW REORG "); DBG(me.solutions_); DBG(" @ "); DBG(me.weight_); DBG(std::endl);

				//Done: manage counter, weights
				newTuple->mergeData(&me, origin, info, firstChild, true);		//* vs min.
			
				DBG("AFTER NEW REORG "); DBG(newTuple->solutions_); DBG(" @ "); DBG(newTuple->weight_); DBG(std::endl);
				DBG("AFTER NEW REORG "); DBG(me.solutions_); DBG(" @ "); DBG(me.weight_); DBG(std::endl);


				/*std::cout << "\tPRECANDIDATE (";
				for (auto candO = origin->begin(); candO != origin->end(); ++candO)
					std::cout << (*candO) << ",";
				std::cout << ")" << std::endl;*/

				origin = me.origins_.erase(origin);

				/*std::cout << "\tPOSTCANDIDATE (";
				for (auto candO = origin->begin(); candO != origin->end(); ++candO)
					std::cout << (*candO) << ",";
				std::cout << ")" << std::endl;*/

				assert(newTuple->certificate_pointer_set_ != nullptr);
			#ifdef VEC_CERTS_TYPE
				tupleTypes.push_back(newTuple);
			#else
				auto ins = tupleTypes.emplace(newTuple->certificate_pointer_set_, newTuple);
				assert(ins.second);
				//std::cout << "          INSERT        " << newTuple << std::endl;
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
			me.certificate_pointer_set_ = &tupleType;
			assert(me.certificate_pointer_set_ != nullptr);
		#ifdef VEC_CERTS_TYPE
			tupleTypes.push_back(&me);
		#else
			auto ins = tupleTypes.emplace(me.certificate_pointer_set_, &me);
			assert(ins.second);
			//std::cout << "          INSERT        " << &me << std::endl;
		#endif
			origin++;
		}
		return nullptr; 	//no new tuple has to be added
	}

	//TODO: add to outputTuples
	void DynAspCertificateAlgorithm::evaluateNode(
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
		//std::cout << ("evaluteNode 2nd") << std::endl;
		//return;
		//const bool SUBSET = true;

		/*TODO: new Info for non-subset checks {*/
		//atom_vector atoms;


		/*const ConstCollection<vertex_t> introduced =
			decomposition.introducedVertices(node);*/

		TreeNodeInfo &info = static_cast<IGroundAspInstance &>(const_cast<IInstance&>(instance)).getNodeData(node);
		/*TreeNodeInfo info
		{
			//TODO: nasty
			static_cast<IGroundAspInstance &>(const_cast<IInstance&>(instance)),
			vertex_container(),
			vertex_container(),
			vertex_container(),
			vertex_container()
		#ifdef INT_ATOMS_TYPE
			,0,0
		#endif
		};
	#ifdef INT_ATOMS_TYPE
		unsigned int pos = 0;
		const ConstCollection<vertex_t> remembered =
			decomposition.rememberedVertices(node);
	
		//FIXME: do this via labelling functions
		for(const auto & vertex : introduced)
			if (!static_cast<const IGroundAspInstance &>(instance).isRule(vertex))
			{
				info.introducedAtoms.push_back(vertex);
				info.int_introducedAtoms |= 1 << pos++;
			}
		for(const auto & vertex : remembered)
			if (!static_cast<const IGroundAspInstance &>(instance).isRule(vertex))
			{
				info.rememberedAtoms.push_back(vertex);
				info.int_introducedAtoms |= 1 << pos++;
			}
	#endif*/

/*#ifdef INT_ATOMS_TYPE
		atoms = info.getAtoms();
#endif*/
		//rule_vector rules;

#ifndef INT_ATOMS_TYPE

		atom_vector atoms;
		atoms.reserve(info.introducedAtoms.size() + info.rememberedAtoms.size());
		atoms.insert(atoms.end(), info.introducedAtoms.begin(), info.introducedAtoms.end());
		atoms.insert(atoms.end(), info.rememberedAtoms.begin(), info.rememberedAtoms.end());

	#endif

		/*if (create::get() == create::PRIMAL_SIMPLETUPLE)
		{
			//contains all the rules
			rules.reserve(info.introducedRules.size()); // + info.rememberedRules.size());
			rules.insert(rules.end(), info.introducedRules.begin(), info.introducedRules.end());
			//rules.insert(atoms.end(), info.rememberedRules.begin(), info.rememberedRules.end());
		}*/
/*#ifdef INT_ATOMS_TYPE
		else
		{
		}
#endif

		for(const auto & vertex : decomposition.bagContent(node))
		{
			//FIXME: do this via labelling functions
			if (!static_cast<const IGroundAspInstance &>(instance).isRule(vertex))
#ifndef INT_ATOMS_TYPE
				atoms.push_back(vertex);
#else
			{}
#endif
			else if (create::get() == create::PRIMAL_SIMPLETUPLE)	//ONLY for simpletupletype
			{
				//assert(false);
				rules.push_back(vertex);
			}
		}
		//ONLY for NON simpletupletype
		if (create::get() != create::PRIMAL_SIMPLETUPLE)
		{
			for (const auto &i : info.introducedAtoms)
				if (static_cast<const IGroundAspInstance &>(instance).isRule(i))
					rules.push_back(i);
		}
		else
		{
			//std::cout << create::PRIMAL_SIMPLETUPLE << " vs " << create::get() << std::endl;
			//assert(false);
		}*/



		//for(const auto &i : introduced)
		{
			/*	vertex_t vertex = *i;

				//FIXME: do this via labelling functions
				if(info.instance.isRule(vertex))
					info.rememberedRules.push_back(vertex);
			//for(auto i = introduced.begin(); i != introduced.end(); ++i)
			{*/
			/*	vertex_t vertex = *i;

				//FIXME: do this via labelling functions
				if(info.instance.isRule(vertex))
					info.rememberedRules.push_back(vertex);
				else info.rememberedAtoms.push_back(vertex);*/
			/*if(info.instance.isRule(vertex))
				info.introducedRules.push_back(vertex);
			else info.introducedAtoms.push_back(vertex);*/
		}
		//TODO }*/

		/*//FIXME: remove bitvectors to calculate subsets, then this is not needed
		if(sizeof(size_t) * CHAR_BIT < (remembered.size() + introduced.size()))
			throw std::runtime_error("Treewidth too high.");

		DBG(node); DBG(": ra"); DBG_COLL(info.rememberedAtoms); DBG(" rr");
		DBG_COLL(info.rememberedRules); DBG(" ia");
		DBG_COLL(info.introducedAtoms); DBG(" ir");
		DBG_COLL(info.introducedRules); DBG(" children: ");*/

		size_t childCount = decomposition.childCount(node);

		//vertex_t child;
		/*// contains vertices already joined together
		unordered_set<vertex_t> joinBase;
		vertex_container baseVertices, nextBaseVertices;

		if(childCount != 0)
		{
			child = decomposition.child(node, 0);

			const std::vector<vertex_t> childBag =
				decomposition.bagContent(child);

			for(auto i = childBag.begin(); i != childBag.end(); ++i)
				joinBase.insert(*i);
		}

		vertex_container v1, v2, *joinVertices = &v1, *nextJoinVertices = &v2;
		unordered_set<IDynAspTuple *, IDynAspTuple::join_hash>
			prev(1, IDynAspTuple::join_hash(*joinVertices)),
			curr(1, IDynAspTuple::join_hash(*nextJoinVertices));*/

		//bool firstChild = true;


		ITupleSet &meTuples = outputTuples;//tuples[node];
		unsigned int sz = meTuples.size();

#ifdef CLEANUP_SOLUTIONS
//#ifdef SEVERAL_PASSES
		if (node != decomposition.root())
			for (size_t pos = 0; pos < outputTuples.size(); ++pos)
				if (static_cast<CertificateDynAspTuple *>(outputTuples[pos])->evolution_.size() == 0)
				{
					delete outputTuples[pos];
					outputTuples.erase(pos);
					--pos;
				}
//#endif
#endif

		// for each child, project and join tuples
		if(childCount == 0)			//leaf
		{
#ifdef DEBUG_INFO
			++leaf;
#endif

			//std::cout << "LEAF" << std::endl;
			DBG("LEAF"); DBG("\n");
			// project out unneeded vertices
			for (ITuple &tuple : meTuples)		//make quadratic stuff
			{
				DBG("LEAF TUPLE ("); DBG(&tuple); DBG(")"); DBG("\n");
				CertificateDynAspTuple &childTuple = static_cast<CertificateDynAspTuple &>(tuple);
				if (childTuple.isPseudo() || childTuple.solutionCount() == 0)
					continue;
				childTuple.certificate_pointer_set_ = new CertificateDynAspTuple::Certificate_pointer_set();
#ifndef INT_ATOMS_TYPE
				childTuple.prepareForBag(atoms);
#endif
				for (const ITuple &tuple2 : meTuples)
				{
					const CertificateDynAspTuple &childTuple2 = static_cast<const CertificateDynAspTuple &>(tuple2);
					//std::cout << childTuple.checkRelation(childTuple2) << std::endl;
					CertificateDynAspTuple::ESubsetRelation  result;
					if ((result = childTuple.checkRules(info.introducedRules, childTuple2, info)) >= CertificateDynAspTuple::ESR_EQUAL)
					{
						//tupleType->emplace_back(CertificateDynAspTuple::DynAspCertificatePointer(up, (result == CertificateDynAspTuple::ESR_INCLUDED_STRICT)));
						//if ((result = childTuple.checkRelation(childTuple2)) >= CertificateDynAspTuple::ESR_EQUAL)
						DBG("SUBSET "); DBG(&tuple); DBG(" "); DBG(result); DBG(" ("); DBG(&tuple2); DBG(")"); DBG("\n");
					}
				}
#ifndef INT_ATOMS_TYPE
				childTuple.clean();
#endif
				/*CertificateDynAspTuple* t = DynAspCertificateAlgorithm::insertCompressed(tupleTypes, *tupleType, childTuple, ptrOrigins);
				if (t)
					outputTuples.insert(t);*/
				DBG("SUBSET SIZE "); DBG(childTuple.certificate_pointer_set_->size()); DBG("\n");
			}
		}
		else if (childCount == 1)// || childCount > 1)	//exchange
		{
#ifdef DEBUG_INFO
			++exchange;
#endif
			DBG("EXCHANGE");
			htd::vertex_t firstChild = decomposition.childAtPosition(node, 0);

			//std::cout << "EXCHANGE" << std::endl;

			//std::cout << "            EXCHANGE       " << std::endl;

			//ITupleSet &childTuples = tuples[0];

			//TupleTypes tupleTypes; //CertificateDynAspTuple::DynAspCertificatePointer> certificate_signature;
			//TupleTypes_ tupleTypes_; //CertificateDynAspTuple::DynAspCertificatePointer> certificate_signature;
			// project out unneeded vertices
			//TODO: FIXME: Problem if reorganization happens, so just directly use index...
			//but what if it is a set?
			//for (ITuple &tuple : meTuples)
			for (size_t pos = 0; pos < sz/*meTuples.size()*/; ++ pos)
			{
				DBG(pos);
				ITuple &tuple = *meTuples[pos];
				DBG("EXCHANGE TUPLE ("); DBG(&tuple); DBG(","); DBG_COLL(static_cast<CertificateDynAspTuple&>(tuple).atoms_); DBG(","); DBG_COLL(static_cast<CertificateDynAspTuple&>(tuple).reductAtoms_); DBG(")"); DBG("\n");
				CertificateDynAspTuple& me = static_cast<CertificateDynAspTuple&>(tuple);
				DBG("isClone ("); DBG(me.isClone()); DBG("), "); DBG(me.certificate_pointer_set_); DBG("<sols: "); DBG(me.solutions_); DBG(" @"); DBG(me.weight_); DBG(">");

				/*DBG(me.isClone());
				DBG(me.isPseudo());*/
				DBG("\n");
				//DBG(me.solutionCount());
				//DBG("\n");
				//DBG(me.isPseudo());

				if (me.isPseudo() || me.solutionCount() == 0 || me.isClone() || me.certificate_pointer_set_ != nullptr)
					continue;
				DBG("post");
#ifndef INT_ATOMS_TYPE
				me.prepareForBag(atoms);
#endif
				CertificateDynAspTuple::ExtensionPointers::iterator ptrOrigins;

				TupleTypes tupleTypes; //CertificateDynAspTuple::DynAspCertificatePointer> certificate_signature;
				bool non_solution = false;
				//size_t pos = 0, sz = me.origins_.size();
				for (ptrOrigins = me.origins_.begin(); ptrOrigins != me.origins_.end(); ) // && pos < sz; ++pos)
				{
					//std::cout << "ptrOrigins" << std::endl;
					auto& down = *ptrOrigins;
					TupleType *tupleType = new TupleType(); //CertificateDynAspTuple::DynAspCertificatePointer> certificate_signature;
					//std::cout << tupleType << std::endl;
					assert(down.size() == 1);		//since we have an exchange node
					for (const auto* downptr : down)	//
					{
						DBG("down "); DBG(downptr); DBG(","); DBG(downptr->certificate_pointer_set_->size()); DBG("\n");
					#ifdef COMBINE_PSEUDO_PSEUDO_SOLUTIONS
						if (downptr->certificate_pointer_set_ == nullptr)
							continue;
					#else
						assert(downptr->certificate_pointer_set_ != nullptr);
					#endif
						for (const CertificateDynAspTuple::DynAspCertificatePointer &cert : *downptr->certificate_pointer_set_)
						{
							assert(!downptr->isPseudo());
							DBG("cert "); DBG(cert.cert); DBG(","); DBG(cert.strict); DBG("\n");
							for (const CertificateDynAspTuple *up : cert.cert->evolution_)
							{
#ifdef USE_SELF_STRICT_DETECTION
								if ((non_solution = (up == &me && cert.strict)))
								{
									auto& ptrComponent = (*ptrOrigins->begin())->evolution_;
									#ifdef REVERSE_EXTENSION_POINTER_SET_TYPE
										ptrComponent->evolution_.erase(&me);
									#else
										for (size_t pos = 0; pos < ptrComponent.size(); ++pos)
											if (ptrComponent[pos] == &me)
											{
												ptrComponent.erase(ptrComponent.begin() + pos);
												break;
											}
									#endif
									ptrOrigins = me.origins_.erase(ptrOrigins);
									break;
								}
#endif
								assert(up == up->getClone());
								DBG("evo "); DBG(up); DBG("\n");
								CertificateDynAspTuple::ESubsetRelation result = me.checkRules(info.introducedRules, *up, info, false);

								//TODO: more efficient search to eliminate duplicates
								bool found = false;

							#ifdef VEC_CERT_TYPE
								for (auto &t : *tupleType)
									if (t.cert == up)
									{
										//potentially update stuff?
										//TODO: update correct?! missing subset stuff!
										t.strict |= (cert.strict | (result == CertificateDynAspTuple::ESR_INCLUDED_STRICT));
										DBG("DOUBLE INSERT PREVENTION"); DBG("\n");
										found = true;
										break;
									}
							#endif
								//if (!found && (result = me.checkRelation(*up, false)) >= CertificateDynAspTuple::ESR_EQUAL)
								if (!found && result >= CertificateDynAspTuple::ESR_EQUAL)
								{
								#ifdef VEC_CERT_TYPE
									tupleType->emplace_back(CertificateDynAspTuple::DynAspCertificatePointer(up, cert.strict |
											(result ==
																									   CertificateDynAspTuple::ESR_INCLUDED_STRICT)));
								#else
									bool strict = cert.strict | (result == CertificateDynAspTuple::ESR_INCLUDED_STRICT);
									tupleType->insert(CertificateDynAspTuple::DynAspCertificatePointer(up, strict)).first->updateStrict(strict);
								#endif
									DBG("SUBSET "); DBG(&tuple); DBG(","); DBG_COLL(static_cast<CertificateDynAspTuple&>(tuple).atoms_); DBG(","); DBG_COLL(static_cast<CertificateDynAspTuple&>(tuple).reductAtoms_); DBG(" "); DBG(result); DBG(" "); DBG(cert.strict); DBG(" ("); DBG(up); DBG(")");
								}
							}
							if (non_solution)
								break;
						}
						if (non_solution)
						{
							delete tupleType;
							break;
						}
						DBG("postdown"); DBG("\n");
					}
					if (!non_solution)
					{
						CertificateDynAspTuple *t = DynAspCertificateAlgorithm::insertCompressed(tupleTypes, *tupleType,
																								 me, ptrOrigins, info, firstChild);
						tupleType = nullptr;
						if (t)
							outputTuples.insert(t);
					}
				}
#ifndef INT_ATOMS_TYPE
				me.clean();
#endif
			}
		}
		else
#ifdef USE_LAME_JOIN
		{
#ifdef DEBUG_INFO
			++join;
#endif
			DBG("JOIN"); DBG("\n");
			//std::cout << "JOIN " << childCount << std::endl;

			//std::cout << "            JOIN       " << std::endl;
			/*in D-FLAT^2 funktioniert es ja soweit ich verstanden habe in Joinknoten so: Wenn ich die CCs eines SCs x haben will,
			 * schau ich ja für jedes EPT von x alle Paare von CCs an. Für jedes solche CC Paar schaue ich mir alle Paare von back pointers an.
			 * Wenn beide auf das Gleiche zeigen, hab ich einen CC für x gefunden. Soweit stimmt das?


			Wenn ja, fällt mir eine mögliche Optimierung ein:
			 Für jedes EPT (a,b) von x: Definiere S_a = { n | n ist CC von a } und analog S_b.
			 (Also ich markiere mir alle Knoten, die CC von a oder b sind.)
			 Dann gehe alle SCs im Joinknoten durch und wenn einer ein EPT (a',b') hat
			 sodass a' in S_a und b' in S_b ist, dann ist jener SC ein CC von x.

			Zum Vergleich der Laufzeit:

			#EPT ... Maximale Anzahl an EPTs eines jeden SC
			#SC ... Maximale Anzahl an SCs in jedem IT
			#CC ... Maximale Anzahl an CCs eines jeden SC
			#BP ... Maximale Anzahl an back pointers eines jeden SC/CC

			Man kann vermutlich annehmen, dass #FPT, #CC und #BP gleich #SC sind. So kommt man auf folgende Laufzeiten, wenn man annimmt, dass Mengenoperationen in konstanter Zeit laufen:

			Alt: #EPT * #CC^2 * #BP^2 = #SC^5
			Neu: #EPT * (2 #CC + #SC * #EPT) = O(#SC^3)*/

			//assert(false);
			//TODO: make join

			//TupleTypes tupleTypes; //CertificateDynAspTuple::DynAspCertificatePointer> certificate_signature;
			// project out unneeded vertices
			//EPT (a,b,c,...)
			//TODO: FIXME: Problem if reorganization happens, so just directly use index...
			//but what if it is a set?
			//for (ITuple &tuple : meTuples)
			for (size_t pos = 0; pos < sz/*meTuples.size()*/; ++ pos)
			{
				ITuple &tuple = *meTuples[pos];
			//for (ITuple &tuple : meTuples)		//make quadratic stuff
			//{
				DBG("JOIN TUPLE ("); DBG(&tuple); DBG(", pseudo ");
				CertificateDynAspTuple &me = static_cast<CertificateDynAspTuple &>(tuple);
				DBG(me.isPseudo()); DBG(")"); DBG(std::endl);
				DBG("isClone ("); DBG(me.isClone()); DBG("), "); DBG(me.certificate_pointer_set_); DBG("\n");
				//TODO: REMOVE me.isClone(), since it is subsumed by me.certificate_pointer_set != nullptr!
				if (me.isPseudo() || me.solutionCount() == 0 || me.isClone() || me.certificate_pointer_set_ != nullptr)
					continue;
#ifndef INT_ATOMS_TYPE
				me.prepareForBag(atoms);
#endif
#ifndef USE_OPTIMIZED_EXTENSION_POINTERS
				CertificateDynAspTuple::ExtensionPointers::iterator ptrOrigins;
#endif

				/*for (ptrOrigins = me.origins_.begin(); ptrOrigins != me.origins_.end(); ++ptrOrigins)
				{
					std::cout << "\tCANDIDATE (";
					for (auto candO = ptrOrigins->begin(); candO != ptrOrigins->end(); ++candO)
						std::cout << (*candO) << ",";
					std::cout << ")" << std::endl;
				}*/

				TupleTypes tupleTypes; //CertificateDynAspTuple::DynAspCertificatePointer> certificate_signature;

#ifdef USE_OPTIMIZED_EXTENSION_POINTERS
				CertificateDynAspTuple::ExtensionPointer::const_iterator *its = new CertificateDynAspTuple::ExtensionPointer::const_iterator[me.origins_.size()];
				CertificateDynAspTuple::ExtensionPointer::const_iterator *begins = new CertificateDynAspTuple::ExtensionPointer::const_iterator[me.origins_.size()];
				CertificateDynAspTuple::ExtensionPointer::const_iterator *ends = new CertificateDynAspTuple::ExtensionPointer::const_iterator[me.origins_.size()];
				CertificateDynAspTuple::ExtensionPointer currentTuple;
				CertificateDynAspTuple::ExtensionPointer* ptrOrigins = &currentTuple;
				int idx = 0;
				//init
				for (auto ptrOrigins = me.origins_.cbegin(); ptrOrigins != me.origins_.cend(); ++ptrOrigins)// && pos < sz; ++pos)
				{
					its[idx] = ptrOrigins->cbegin();
					begins[idx] = ptrOrigins->cbegin();
					ends[idx] = ptrOrigins->cend();
					currentTuple[idx] = *ptrOrigins->cbegin();
					++idx;
				}
				idx--;

				while (idx >= 0)
				{
					if (me.non_evolution_ != nullptr && me.non_evolution_->find(*ptrOrigins) != me.non_evolution_->end())
						{
#else

				//size_t pos = 0, sz = me.origins_.size();
				for (ptrOrigins = me.origins_.begin(); ptrOrigins != me.origins_.end();)// && pos < sz; ++pos)
				{
#endif
					DBG("\tCANDIDATE (");
					for (auto candO = ptrOrigins->begin(); candO != ptrOrigins->end(); ++candO)
					{
						DBG((*candO)); DBG(",");
					}
					DBG(")"); DBG("\n");
					//std::cout << (*ptrOrigins)[0] << "," << (*ptrOrigins)[1] << std::endl;
					//std::cout << ptrOrigins->size() << " vs " << childCount << std::endl;
					assert(ptrOrigins->size() == childCount);
					TupleType *tupleType = new TupleType();
					//EPT (a',b',c',...)
					for (const ITuple &tupleCand : meTuples)		//make quadratic stuff
					{
						const CertificateDynAspTuple &meCand = static_cast<const CertificateDynAspTuple &>(tupleCand);
						const CertificateDynAspTuple &meCandClone = *meCand.getClone();
						//const CertificateDynAspTuple &meCandOrig = static_cast<const CertificateDynAspTuple &>(tupleCand).isClone() != nullptr ?
						//							 *static_cast<const CertificateDynAspTuple &>(tupleCand).isClone() : static_cast<const CertificateDynAspTuple &>(tupleCand);
						//TODO: meCand.isClone() subsumed by meCand.certificate_pointer_set_ != nullptr
						/*if (meCand.isClone() || meCand.certificate_pointer_set_ != nullptr)
							continue;*/

						const auto itCand = tupleType->find(CertificateDynAspTuple::DynAspCertificatePointer(&meCandClone, true));

						if (itCand != tupleType->end() && itCand->strict)
							continue;

						bool inserted = itCand != tupleType->end();

						/*DBG("\tCANDIDATE TUPLE ("); DBG(&meCandClone); DBG(")"); DBG("\n");
						DBG("\tisClone ("); DBG(meCand.isClone()); DBG("), "); DBG(meCand.certificate_pointer_set_); DBG("\n");*/
						CertificateDynAspTuple::ESubsetRelation result = itCand != tupleType->end() ? CertificateDynAspTuple::ESR_EQUAL : CertificateDynAspTuple::ESR_NONE;
						//if ((result = me.checkRelation(*meCand.getClone(), false)) >= CertificateDynAspTuple::ESR_EQUAL)
						if (itCand != tupleType->end() ||
								(result = me.checkRules(info.introducedRules, meCandClone, info, false)) >= CertificateDynAspTuple::ESR_EQUAL)
						{
							//DBG("WOULD be OK "); DBG(result); DBG(": "); DBG(&me); DBG(" ("); DBG(&meCandClone); DBG(")");  DBG("\n");
							CertificateDynAspTuple::ExtensionPointers::const_iterator candOrigins;
							for (candOrigins = meCand.origins_.begin(); candOrigins != meCand.origins_.end(); ++candOrigins)
							{
								//std::cout << "\ţ CANDIDATE down " << downptr << "," << downptr->certificate_pointer_set_->size() << std::endl;
								//NUMBER of child nodes
								bool found = false, strictTotal = false;
								//bool pseu = false;
								//std::cout << "checking all set inclusions " << childCount << std::endl;
								for (size_t childIndex = 0; childIndex < childCount; ++childIndex)
								{
									//Search whether (a') \in S_a
									found = false;
									//std::cout << "child " << childIndex << " "  << (*ptrOrigins)[childIndex] << std::endl;

									//TODO: uncomment pseudo
								#ifdef COMBINE_PSEUDO_PSEUDO_SOLUTIONS
									if ((*ptrOrigins)[childIndex]->certificate_pointer_set_ == nullptr)
										break;
								#else
									assert((*ptrOrigins)[childIndex]->certificate_pointer_set_ != nullptr);
								#endif

								#ifdef VEC_CERT_TYPE

									//std::cout << "checking subset stuff " << (*ptrOrigins)[childIndex]->certificate_pointer_set_->size() << std::endl;
									for (const auto &elem: *((*ptrOrigins)[childIndex]->certificate_pointer_set_))
										if (elem.cert == (*candOrigins)[childIndex]->getClone())
										{
											strictTotal |= elem.strict;
											found = true;
											break;
										}

								#else

									const CertificateDynAspTuple::DynAspCertificatePointer t((*candOrigins)[childIndex]->getClone(), false);
									const auto& fpos = ((*ptrOrigins)[childIndex]->certificate_pointer_set_)->find(t);
									if (fpos != ((*ptrOrigins)[childIndex]->certificate_pointer_set_)->end())
									{
										strictTotal |= fpos->strict;
										//pseu |= fpos->cert->isPseudo();
										found = true;
									}
									//TODO
								#endif
									//std::cout << found << std::endl;
									if (!found)
										break;

								}
								//DBG("FOUND "); DBG(found); DBG(std::endl);
								if (found && (!inserted || strictTotal))
								{
									//DBG("\tCANDIDATE Ok (");
									/*for (auto candO = candOrigins->begin(); candO != candOrigins->end(); ++candO)
									{
										DBG((*candO)); DBG(","); // << cand->certificate_pointer_set_->size() << std::endl;
									}*/
									/*DBG(")"); DBG("\n");
									DBG("WILL SUBSET "); DBG(&tuple); DBG(" "); DBG(result); DBG(" "); DBG(strictTotal);
									DBG(" ("); DBG(&meCandClone); DBG(")"); DBG("\n");*/
									//std::cout << "WILL INSERT: " << found << std::endl;
									//TODO: make search more efficient
									found = false;
									strictTotal |=  (result ==
													 CertificateDynAspTuple::ESR_INCLUDED_STRICT);
								#ifdef VEC_CERT_TYPE
									for (auto &t : *tupleType)
										if (t.cert == &meCandClone)
										{	//potentially update stuff?
											//TODO: update correct?! missing subset stuff!
											t.strict |= strictTotal;
											//DBG("DOUBLE INSERT PREVENTION"); DBG("\n");
											found = true;
											break;
										}

								#endif
									//DBG(" "); DBG(found); DBG(std::endl);
									if (!found)
									{
										//DBG("WTF"); DBG(meCandClone.isPseudo()); DBG(" vs "); DBG(pseu); DBG(std::endl);
										DBG("SUBSET "); DBG(&tuple); DBG(" "); /*DBG(result);*/ DBG(" "); DBG(strictTotal);
										//DBG(" ("); DBG(&meCandClone); DBG(")"); DBG("\n");

									#ifdef VEC_CERT_TYPE
										tupleType->emplace_back(CertificateDynAspTuple::DynAspCertificatePointer(&meCandClone,
																									   strictTotal |
																									   (result ==
																										CertificateDynAspTuple::ESR_INCLUDED_STRICT)));

									#else

										tupleType->insert(CertificateDynAspTuple::DynAspCertificatePointer(&meCandClone,
																									   strictTotal)).first->updateStrict(strictTotal);
										inserted = true;
										if (strictTotal)
											break;
									#endif
									}
								}
							}
						}
					}
					DBG("postdown"); DBG("\n");
					CertificateDynAspTuple* t = DynAspCertificateAlgorithm::insertCompressed(tupleTypes, *tupleType, me, ptrOrigins, info);
					tupleType = nullptr;
					if (t)
						outputTuples.insert(t);

#ifdef USE_OPTIMIZED_EXTENSION_POINTERS
						}
					for (; idx >= 0 && ++its[idx] == ends[idx]; --idx)
						its[idx] = begins[idx];	//reset
					if (idx >= 0)
						idx = me.origins_.size() - 1;
#endif
				}

#ifdef USE_OPTIMIZED_EXTENSION_POINTERS
				delete[] &its;
				delete[] &begins;
				delete[] &ends;
#endif

#ifndef INT_ATOMS_TYPE
				me.clean();
#endif
			}

			//for (size_t childIndex = 0; childIndex < childCount; ++childIndex)
			{
				//child = decomposition.child(node, childIndex);
				//vertex_t nextChild;

				//if (childIndex + 1 < childCount)
				{
				//	nextChild = decomposition.child(node, childIndex + 1);
				//	const std::vector<vertex_t> nextBag =
				//			decomposition.bagContent(nextChild);

					/*//FIXME: tree decomposition should supply this
					nextJoinVertices->clear();
					nextBaseVertices.clear();
					nextBaseVertices.insert(
							nextBaseVertices.end(),
							joinBase.begin(),
							joinBase.end());

					for(auto i = nextBag.begin(); i != nextBag.end(); ++i)
						if(!joinBase.insert(*i).second)
							nextJoinVertices->push_back(*i);*/
				}

				//const ITupleSet &childTuples = tuples[child];

				/*DBG(child); DBG_COLL(*joinVertices);
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

				DBG(merged.size()); DBG(",");*/

				// join the tuples to the ones of the previous child
				//if (firstChild)
				{
					/*// if we are the first child, just swap our tuples into
					// the previous set and continue to the next child
					prev.reserve(merged.size());
					curr.reserve(merged.size());
					curr.insert(merged.begin(), merged.end());
					prev.swap(curr);
					swap(joinVertices, nextJoinVertices);
					swap(baseVertices, nextBaseVertices);

					DBG(prev.size()); DBG(") ");

					firstChild = false;
					continue;*/
				}



				/*// elsewise, join our tuples to the current set
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
				}*/

				/*// delete old tuples in prev
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
					return;*/
			}

		}
		//DONE: add all tuples
		//for (ITuple &tuple : meTuples)		//make output
		//	outputTuples.insert(&tuple);
#else
	{
#ifdef DEBUG_INFO
			++join;
#endif
			DBG("JOIN"); DBG("\n");
			for (size_t pos = 0; pos < sz/*meTuples.size()*/; ++ pos)
			{
				ITuple &tuple = *meTuples[pos];

				DBG("JOIN TUPLE ("); DBG(&tuple); DBG(", pseudo ");
				CertificateDynAspTuple &me = static_cast<CertificateDynAspTuple &>(tuple);
				DBG(me.isPseudo()); DBG(")"); DBG(std::endl);
				DBG("isClone ("); DBG(me.isClone()); DBG("), "); DBG(me.certificate_pointer_set_); DBG("\n");
				//TODO: REMOVE me.isClone(), since it is subsumed by me.certificate_pointer_set != nullptr!
				if (me.isPseudo() || me.solutionCount() == 0 || me.isClone() || me.certificate_pointer_set_ != nullptr)
					continue;
#ifndef INT_ATOMS_TYPE
				me.prepareForBag(atoms);
#endif
				TupleTypes tupleTypes; //CertificateDynAspTuple::DynAspCertificatePointer> certificate_signature;
				//me.postEvaluate();
				//size_t pos = 0, sz = me.origins_.size();
				for (auto ptrOrigins = me.origins_.begin(); ptrOrigins != me.origins_.end();)// && pos < sz; ++pos)
				{
					DBG("\tCANDIDATE (");
					for (auto candO = ptrOrigins->begin(); candO != ptrOrigins->end(); ++candO)
					{
						DBG((*candO)); DBG(",");
					}
					DBG(")"); DBG("\n");
					//std::cout << (*ptrOrigins)[0] << "," << (*ptrOrigins)[1] << std::endl;
					//std::cout << ptrOrigins->size() << " vs " << childCount << std::endl;
					assert(ptrOrigins->size() == childCount);
					struct ConsenseType { bool strict; unsigned int lastTouchedBy; bool p; 
									#ifdef CHECK_CONSENSE
										CertificateDynAspTuple::IConsenseData* data;
									#endif
											ConsenseType() : strict(false), lastTouchedBy(1), p(false)
										#ifdef CHECK_CONSENSE	
											, data(nullptr)
										#endif	
											{ }
											void update(bool s, bool ps) { strict |= s; p |= ps; }
										};
					//typedef std::unordered_map<CertificateDynAspTuple *, ConsenseType, IDynAspTuple::merge_hash, IDynAspTuple::ComputedCertificateTuplesEqual> CollectedCertificates;
					typedef std::unordered_map<CertificateDynAspTuple *, ConsenseType, std::hash<CertificateDynAspTuple*> > CollectedCertificates;
					CollectedCertificates certs;
					//certs.reserve(outputTuples.size());
					unsigned int ppos = 0;
					for (auto ptr = ptrOrigins->begin(); ptr != ptrOrigins->end(); ++ptr)
					{
						/*std::cout << "before" << std::endl;
						std::unordered_set<const IDynAspTuple*> ss, ss2;
						for (const auto &p : *(*ptr)->certificate_pointer_set_)
						{
							std::cout << p.cert << "," << p.cert->atoms_ << "," << p.cert->reductAtoms_ << "," <<
							p.cert->getClone() << "," << p.strict << std::endl;
							for (const auto* pp : p.cert->evolution_)
							{
								ss.insert(pp);
								std::cout << pp << std::endl;
							}
						}
						std::cout << std::endl;
						//DBG_COLL(*(*ptr)->certificate_pointer_set_);
						if ((ptr + 1) != ptrOrigins->end())
						{
							for (const auto &p : *(*(ptr + 1))->certificate_pointer_set_)
							{
								std::cout << p.cert << "," << p.cert->atoms_ << "," << p.cert->reductAtoms_ << "," <<
								p.cert->getClone() << "," << p.strict << std::endl;
								for (const auto *pp : p.cert->evolution_)
								{
									ss2.insert(pp);
									std::cout << pp << std::endl;
								}
							}
							std::cout << std::endl;
							assert(ss == ss2);
						}
						std::cout << "end" << std::endl;*/
						//assert((ptr + 1) == ptrOrigins->end() || *(*ptr)->certificate_pointer_set_ == *(*(ptr + 1))->certificate_pointer_set_);
						//std::cout << "LISTEN" << std::endl;
						assert(!(*ptr)->isPseudo());
						DBG("CERT SET "); DBG((*ptr)->certificate_pointer_set_); DBG(std::endl);
						for (auto &cc : *(*ptr)->certificate_pointer_set_)
						{
							DBG("CERT "); DBG(cc.cert); DBG(" / "); DBG(cc.strict); DBG(","); DBG(cc.cert->isPseudo()); DBG("("); DBG(cc.cert->atoms_); DBG(","); DBG(cc.cert->reductAtoms_); DBG(")"); DBG(std::endl);
							for (auto *tupleCand : cc.cert->evolution_)
							{
								DBG("EVO "); DBG(tupleCand); DBG(std::endl);
								/*DBG("\tEVO_PTRS (");
								for (auto candO = tupleCand->origins_.begin(); candO != tupleCand->origins_.end(); ++candO)
								{
									DBG("EVO_PTR ");
									for (auto cand1 = candO->begin(); cand1 != candO->end(); ++cand1)
									{
										DBG((*cand1)); DBG("["); DBG((*cand1)->getClone()); DBG("]"); DBG(",");
									}
									DBG(std::endl);
								}
								DBG(")"); DBG("\n");*/

								assert(cc.cert == cc.cert->getClone());
								//std::cout << cc.strict << "," << cc.cert << "," << cc.cert->atoms_ << "," << cc.cert->reductAtoms_ << "," << cc.cert->solutions_ << "," << cc.cert->weight_ << "@" << tupleCand << "," << tupleCand->atoms_ << "," << tupleCand->reductAtoms_ << std::endl;
								//auto* tupleCand = cc.cert;
								assert(tupleCand == tupleCand->getClone());
								CollectedCertificates::iterator it;
								if (ptr == ptrOrigins->begin())
								{
									CertificateDynAspTuple::ESubsetRelation result = me.checkRules(info.introducedRules, *tupleCand, info, false);
									DBG("RES: "); DBG(result); DBG(std::endl);
									assert(result >= CertificateDynAspTuple::ESR_EQUAL);

									const auto& it = certs.emplace(tupleCand, ConsenseType()).first;
									it->second.update(cc.strict | (result == CertificateDynAspTuple::ESR_INCLUDED_STRICT), cc.cert->isPseudo());	//licked by the first
								#ifdef CHECK_CONSENSE
									tupleCand->updateConsense(cc, it->second.data, info, ppos, decomposition.childAtPosition(node, ppos));
									if (result == CertificateDynAspTuple::ESR_INCLUDED_STRICT)
										tupleCand->setStrict(true, *it->second.data);

									//(*ptr)->initConsense(it->second.data, info, decomposition.childAtPosition(node, ppos));
									//cc.cert->updateConsense(*it->second.data);
								#endif
								}
								else if ((it = certs.find(tupleCand)) != certs.end())
								{
								#ifdef STRICT_MODE
									assert(it->second.lastTouchedBy <= ptr - ptrOrigins->begin());
								#endif
									assert(it->second.lastTouchedBy <= ptr - ptrOrigins->begin() + 1);
									if (it->second.lastTouchedBy >= ptr - ptrOrigins->begin())
									{
										it->second.strict |= cc.strict;
										it->second.p |= cc.cert->isPseudo();
									#ifdef CHECK_CONSENSE
										//(*ptr)->updateConsense(*it->second.data, info, 
										tupleCand->updateConsense(cc, it->second.data, info, ppos, decomposition.childAtPosition(node, ppos));
									#endif
										if (it->second.lastTouchedBy == ptr - ptrOrigins->begin())
											it->second.lastTouchedBy++;        //licked by janitor!
									}
									else
									{
									#ifdef CHECK_CONSENSE
										cc.cert->clearConsense(*it->second.data);
										//(*ptr)->clearConsense(*it->second.data);
									#endif
										certs.erase(it);
									}
								}
							}
						}
						++ppos;
					}

					TupleType *tupleType = new TupleType();
					tupleType->reserve(certs.size());
					for (auto& p : certs)
					{
						assert (p.second.lastTouchedBy <= ptrOrigins->size());	//licked by at most everyone!
						CertificateDynAspTuple::ESubsetRelation result = CertificateDynAspTuple::ESR_INCLUDED_STRICT;
						if (p.second.lastTouchedBy == ptrOrigins->size() && (!p.first->isPseudo() || p.second.p) 
						#ifdef CHECK_CONSENSE	
							&& (result = p.first->isConsense(*p.second.data, info, ppos, decomposition, node)) >= CertificateDynAspTuple::ESR_EQUAL
						#endif	
							)	//licked by everyone!
						{
							//assert(!p.first->isPseudo() || p.second.p);
							//assert(p.first->isConsense(*p.second.data, info, ppos));
							DBG("WTF"); DBG(p.first->isPseudo()); DBG( " vs "); DBG(p.second.p); DBG(std::endl);

							//DBG("SUBSET "); DBG(&tuple); DBG(" "); /*DBG(result);*/ DBG(" "); DBG(p.second.strict);
							DBG("SUBSET REAL"); DBG(p.first); DBG(" "); /*DBG(result);*/ DBG(" "); DBG(p.second.strict);
							//assert(p.second.p == p.first->isPseudo());	//licked by everyone!
							/*if (!p.second.strict)
								p.second.strict |=
										(me.checkRules(info.introducedRules, *p.first, info, false) ==
											CertificateDynAspTuple::ESR_INCLUDED_STRICT);*/
#ifdef VEC_CERT_TYPE
								tupleType->emplace_back(CertificateDynAspTuple::DynAspCertificatePointer(p.first, p.second.strict && result == CertificateDynAspTuple::ESR_INCLUDED_STRICT));

#else
								tupleType->insert(CertificateDynAspTuple::DynAspCertificatePointer(p.first,
																								   p.second.strict && result == CertificateDynAspTuple::ESR_INCLUDED_STRICT));
#endif
						}
					#ifdef CHECK_CONSENSE
						p.first->clearConsense(*p.second.data);
					#endif 
					}
					DBG("postdown"); DBG("\n");
				//#ifdef STRICT_MODE
					assert(tupleType->size());
				//#endif
					if (tupleType->size())
					{
						CertificateDynAspTuple *t = DynAspCertificateAlgorithm::insertCompressed(tupleTypes, *tupleType,
																									 me, ptrOrigins, info);
						if (t)
							outputTuples.insert(t);
					}
				}
#ifndef INT_ATOMS_TYPE
				me.clean();
#endif
			}
		}
#endif

#ifdef CLEANUP_COUNTER_SOLUTIONS
		if (needAllTupleSets())	//otherwise will be cleaned for sure
			for(size_t childIndex = 0; childIndex < childCount; ++childIndex)
			{
				ITupleSet &childTuples = tuples[decomposition.childAtPosition(node, childIndex)];
				for (size_t pos = 0; pos < childTuples.size(); ++pos)
					static_cast<IDynAspTuple *>(childTuples[pos])->cleanUpSecondLevel();
			}
#endif

		DBG(std::endl);

		// for each remaining tuple, 
		/*for(IDynAspTuple *tuple : prev)
		{
			tuple->introduce(info, outputTuples);
			delete tuple;
		}

		DBG("\ttuples: "); DBG(prev.size()); DBG(" ");*/
		DBG(outputTuples.size()); DBG(std::endl);
	}

	bool DynAspCertificateAlgorithm::needAllTupleSets() const
	{
		return false;
	}

} // namespace dynasp
