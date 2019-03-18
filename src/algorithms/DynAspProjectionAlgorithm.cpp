#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif
#include "../util/debug.hpp" 

#include <dynasp/DynAspProjectionAlgorithm.hpp>
#include <dynasp/CertificateDynAspTuple.hpp>

#include "../instances/GroundAspInstance.hpp"
#include "../../include/dynasp/IDynAspTuple.hpp"
#include "../../include/dynasp/DynAspProjectionAlgorithm.hpp"

#include <dynasp/TreeNodeInfo.hpp>
#include <dynasp/create.hpp>

#include <unordered_set>
#include <algorithm>
#include <climits>
#include <vector>
#include <fstream>

//FIXME: use config.h info here, build check into autoconf
#include <sys/times.h>

#ifdef HAVE_UNISTD_H //FIXME: what if not def'd?
	#include <unistd.h>
#endif

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

	TreeNodeInfo** DynAspProjectionAlgorithm::ExtensionPointerHash::info = nullptr;
	TreeNodeInfo* DynAspProjectionAlgorithm::TupleHash::info = nullptr;

	DynAspProjectionAlgorithm::DynAspProjectionAlgorithm() { } //: impl(nullptr) { }

	DynAspProjectionAlgorithm::~DynAspProjectionAlgorithm() {}
	
	void DynAspProjectionAlgorithm::onExit() const
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

#ifdef EXPERIMENT2
	unsigned int minU = 0, maxU = 0;

	struct NodeData
	{
		static unsigned int max_door, max_sens;
		
		unsigned int posL, posM, anzL, anzM;

		NodeData() : posL(0), posM(0), anzL(0), anzM(0) { }

		void inc_sens() { //unsigned int &pos, unsigned int &anz) {
			inc(NodeData::max_sens, posL, anzL);
			inc(NodeData::max_sens, posM, anzM);
		}

		void inc_door() { //unsigned int &pos, unsigned int &anz) {
			inc(NodeData::max_door, posL, anzL);
			inc(NodeData::max_door, posM, anzM);
		}

		void inc(const unsigned int &max, unsigned int &pos, unsigned int &anz) {
		{
			if (anz + 1 > max)
			{
				anz = 1;
				pos++;
			}
			else
				++anz;
		}
	};

	unsigned int NodeData::max_door = 2, NodeData::max_sens = 2;

#endif
	void DynAspProjectionAlgorithm::evaluateNode(
			vertex_t node,
			const ITreeDecomposition &decomposition,
			INodeTupleSetMap &
			,
			const IInstance &instance,
			ITupleSet &outputTuples) const
	{
#define EXPERIMENT
#ifdef EXPERIMENT
		
		IGroundAspInstance& inst = static_cast<IGroundAspInstance &>(const_cast<IInstance&>(instance));
		std::ofstream fo;
		fo.open(sharp::Benchmark::folder(), std::ofstream::app);
		for (const auto& rem : decomposition.forgottenVertices(node))
		{
			if (inst.isRule(rem))
				continue;
			//for (const auto& at : decomposition.bagContent(node))
			for (const auto& at : decomposition.rememberedVertices(node)) //bagContent(node))
			{
				if (inst.isRule(at))
					continue;
				if (inst.getName(rem).c_str()[0] != inst.getName(at).c_str()[0])
					continue;
				std::cout << "partial " << rem << "," << at << std::endl;
				fo << "partial(" << inst.getName(rem) << "," << inst.getName(at) << ")." << std::endl;
			}
			/*if (info.isNegatedChoiceAtom(rem))
			{
				
			}*/
		}
		fo.close();

#else
#ifdef  EXPERIMENT2
		bool found = false;

		/*#define MAX_DOOR 2
		#define MAX_SENS 2

		struct NodeData
		{
			unsigned int lowU, maxU, anzL, anzM;
			static unsigned int max_door, max_sens;

			NodeData() : lowU(0), maxU(0), anzL(0), anzM(0) { }

			void increment(unsigned int &pos, unsigned int &anz) {
				if (pos + 1 > MAX_DOOR)	
			}
		};*/


		//a->S1->b->S2

		std::unordered_map<htd::vertex_t, NodeData> assignments;

		for (const auto& he : decomposition.inducedHyperedges())
		{
			for (const auto& v : he)
			{
				const auto& it = assignments.find(v);
				if (it != assignments.end())
				{
					it->second.inc_door(); //it->second.posL, it->second.anzL);
					found = true;
				}
			}
		}
		if (!found)
		{
			assignments.emplace(	
		}
#else
		size_t childCount = decomposition.childCount(node);
		DBG("---------------------------------"); DBG(std::endl);

DBG(std::endl);
		DBG("resulting tuples: "); DBG(node); DBG("("); DBG(childCount); DBG(") "); DBG(" with "); DBG(outputTuples.size()); DBG(" ");
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
				DBG(&o); DBG(":"); DBG(static_cast<const CertificateDynAspTuple&>(o).duplicate_); DBG(",");  DBG(static_cast<const CertificateDynAspTuple&>(o).solutions_); DBG("@:"); DBG(static_cast<const CertificateDynAspTuple&>(o).weight_); DBG(","); DBG_COLL(static_cast<const CertificateDynAspTuple&>(o).atoms_);  DBG(","); DBG_COLL(static_cast<const CertificateDynAspTuple&>(o).reductAtoms_); static_cast<const CertificateDynAspTuple&>(o).debug(); DBG(","); DBG(static_cast<const CertificateDynAspTuple&>(o).isPseudo()); DBG(", orig_size: "); DBG(static_cast<const CertificateDynAspTuple&>(o).origins_.size()); DBG(" supp: "); 
				DBG(std::endl);
				
			}
		#endif

		DBG("resulting tuples: "); DBG(node); DBG(" with "); DBG(outputTuples.size()); DBG(" ");

		//std::cout << "TD: " << &decomposition << "," << node << std::endl;

		
		TreeNodeInfo &info = static_cast<IGroundAspInstance &>(const_cast<IInstance&>(instance)).getNodeData(node);
		
		DynAspProjectionAlgorithm::TupleHash::info = &info;

		if (childCount)
		{
			DynAspProjectionAlgorithm::ExtensionPointerHash::info = new TreeNodeInfo*[childCount];
			for (unsigned int i = 0; i < childCount; ++i)
				DynAspProjectionAlgorithm::ExtensionPointerHash::info[i] = &static_cast<IGroundAspInstance &>(const_cast<IInstance&>(instance)).getNodeData(decomposition.childAtPosition(node, i));
		}

		ITupleSet &meTuples = outputTuples;//tuples[node];
		unsigned int sz = meTuples.size();

		assert(childCount <= 2);
		assert(sizeof(std::size_t) >= 8);

		//ChildBuckets childBuckets;
		Buckets buckets;

		std::unordered_map<unsigned int, ChildBuckets> chldBuckets;

		CertificateDynAspTuple* repr = nullptr;
		for (size_t pos = 0; pos < sz; ++ pos)
		{
			ITuple &tuple = *meTuples[pos];
			CertificateDynAspTuple& me = static_cast<CertificateDynAspTuple&>(tuple);
			ChildBuckets& childBuckets = chldBuckets.emplace(me.atoms_ & info.int_projectionAtoms, std::move(ChildBuckets())).first->second;
			
			DBG("TUPLE "); DBG(&me); DBG(" "); DBG(me.atoms_); DBG(","); DBG(me.solutions_); DBG(std::endl);
			
			if (me.solutions_ == 0)
			{
				DBG("nosol guy"); DBG(std::endl);
				continue;
			}

			BigNumber n = 0;
			for (auto ptrOrigins = me.origins_.begin(); ptrOrigins != me.origins_.end(); ++ptrOrigins) // && pos < sz; ++pos)
			{
				if (childBuckets.insert(*ptrOrigins).second)
				{
					BigNumber cn = 1;
					for (auto* pt : *ptrOrigins)
					{	
						assert(pt->getClone()->solutions_);
						//assert(pt->duplicate_ || buckets.count(&me) == 0);
						cn *= pt->getClone()->solutions_;	
					}
					n += cn;
				}
			}
			if ((repr = (*buckets.insert(&me).first)) == &me)
			{
				DBG("I RULE"); DBG(std::endl);
				me.solutions_ = childCount ? n : 1;
				me.weight_ = 0;	//TODO
				assert(me.duplicate_ == nullptr);
			}
			else
			{
				DBG("I FOLLOW"); DBG(std::endl);
				repr->solutions_ += n;
				//SAFETY!
				me.solutions_ = 0;	//TODO
				me.weight_ = 0;		//TODO
	
				//safe for later!
				me.duplicate_ = repr;
			}
		}

		std::cout << ("BUCKET SIZE: ") << (buckets.size()) << (" TUPLE SIZE: ") << (outputTuples.size()) << (" IRRELEVANT ATOMS: ")
			<< (POP_CNT(info.getAtoms() & ~info.int_projectionAtoms)) << (std::endl);

		delete[] DynAspProjectionAlgorithm::ExtensionPointerHash::info;
		DynAspProjectionAlgorithm::ExtensionPointerHash::info = nullptr;

		DBG(std::endl);
		DBG(outputTuples.size()); DBG(std::endl);

		DBG(std::endl);
		DBG("resulting tuples: "); DBG(node); DBG("("); DBG(childCount); DBG(") "); DBG(" with "); DBG(outputTuples.size()); DBG(" ");
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
				DBG(&o); DBG(":"); DBG(static_cast<const CertificateDynAspTuple&>(o).duplicate_); DBG(",");  DBG(static_cast<const CertificateDynAspTuple&>(o).solutions_); DBG("@:"); DBG(static_cast<const CertificateDynAspTuple&>(o).weight_); DBG(","); DBG_COLL(static_cast<const CertificateDynAspTuple&>(o).atoms_);  DBG(","); DBG_COLL(static_cast<const CertificateDynAspTuple&>(o).reductAtoms_); static_cast<const CertificateDynAspTuple&>(o).debug(); DBG(","); DBG(static_cast<const CertificateDynAspTuple&>(o).isPseudo()); DBG(", orig_size: "); DBG(static_cast<const CertificateDynAspTuple&>(o).origins_.size()); DBG(" supp: "); 
				DBG(std::endl);
				
			}
		#endif

		DBG("resulting tuples: "); DBG(node); DBG(" with "); DBG(outputTuples.size()); DBG(" ");
#endif
#endif
	}

	bool DynAspProjectionAlgorithm::needAllTupleSets() const
	{
		return !further; //false;
	}

} // namespace dynasp

