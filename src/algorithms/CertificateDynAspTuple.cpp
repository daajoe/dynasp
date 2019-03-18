//
// Created by hecher on 5/4/16.
//

#include <dynasp/CertificateDynAspTuple.hpp>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../util/debug.hpp"


#include "SimpleDynAspTuple.hpp"
#include <dynasp/IDynAspTuple.hpp>

#include <stack>
#include <dynasp/IGroundAspInstance.hpp>
#include <dynasp/create.hpp>

using namespace dynasp;

using htd::vertex_t;
using htd::ConstCollection;
using htd::vertex_container;

using sharp::Hash;

using std::size_t;
using std::unordered_set;
using std::stack;

//CertificateDynAspTuple::ExtensionPointersCache CertificateDynAspTuple::extcache;

//void CertificateDynAspTuple::createPointerCache()
//{
//	extcache.clear();
//}

//const CertificateDynAspTuple::ExtensionPointer* CertificateDynAspTuple::cachePointer(const CertificateDynAspTuple::ExtensionPointer&& ptrs)
//{
//	return &(*extcache.emplace(ptrs).first);
//	/*std::pair<CertificateDynAspTuple::ExtensionPointersCache::iterator, bool> res = extcache.emplace(ptrs);
//	return res.second ? &ptrs : &(*res.first);*/
//}

//CertificateDynAspTuple::~CertificateDynAspTuple() { /*std::cout << "delete " << this << std::endl;*/ delete certificate_pointer_set_; }

//TODO: make it fast!
//TODO: better integrate this subfunctions in the corresponding main functions,
//but for now it seems ok, hence easier to handle & test;
//keep in mind that later on, at some stage, they should be called by each
//of the different sub-algorithms individually, i.e.\ for now we are fine :).
//PRECONDITION: project() successfully has been called before
void CertificateDynAspTuple::projectPtrs(IDynAspTuple& newTuple)
{
#ifndef SINGLE_LAYER_
	//return;
	//origins_.emplace_back(&static_cast<CertificateDynAspTuple&>(newTuple));
	//origins_.reserve(1);
#ifdef EXTENSION_POINTERS_SET_TYPE
	//origins_.insert({&static_cast<CertificateDynAspTuple&>(newTuple)});
	//origins_.insert(CertificateDynAspTuple::cachePointer({&static_cast<CertificateDynAspTuple&>(newTuple)}));
	origins_.insert({&static_cast<CertificateDynAspTuple&>(newTuple)});
#else
	origins_.push_back({&static_cast<CertificateDynAspTuple&>(newTuple)});
#endif


/*#ifdef SECOND_PASS_COMPRESSED
	//evolution_.insert(evolution_.end(), static_cast<CertificateDynAspTuple&>(newTuple).evolution_.begin(),
	//static_cast<CertificateDynAspTuple&>(newTuple).evolution_.end());
#endif*/
	//this->evolution_.push_back(newTuple);
#endif
}

bool CertificateDynAspTuple::isNewProjectionOk(unsigned char /*further*/) const
{
	return evolution_.size() > 0;
	//return (reductAtoms_ != 0) == (further != 0);
}

//PRECONDITION: merge() successfully has been called before
void CertificateDynAspTuple::mergePtrs(IDynAspTuple &merg, bool 
#ifndef NOT_MERGE_PSEUDO
	project
#endif
)
{

#ifndef NOT_MERGE_PSEUDO		//problem is here that the base tuple from which we projected, might have already had parents/evolution_
	//assert(false);
	if (project) // && evolution_.size())
	{
		CertificateDynAspTuple& mm = static_cast<CertificateDynAspTuple&>(merg);
		assert(mm.origins_.size() == 1);	//only one ext ptr here!
		//assert(evolution_.size());	
		CertificateDynAspTuple* newPseudo = static_cast<CertificateDynAspTuple*>((*mm.origins_.begin())[0]);
	
		DBG("mergePtrs"); DBG(" "); DBG(newPseudo->reductAtoms_); DBG(","); DBG(reductAtoms_); DBG(std::endl);

		//assert(false);
		if (/*newPseudo->evolution_.size() == 0 &&*/ newPseudo->reductAtoms_ && !reductAtoms_)	//new Pseudo thingy
		{
			assert( newPseudo->evolution_.size() == 0 );
			//assert(false);
			std::size_t pos = (std::size_t)(-1);
			for (auto& org : origins_)
			{
				assert(org.size() == 1);
				auto* tu = org[0];

				for (auto* evo : tu->evolution_)		//go up
				{
					/*if (evo->evolution_.size() == 0)	//skip deleted ones!
						continue;*/
					DBG("EVO NOT 0!"); DBG(std::endl);
					for (auto& orig : evo->origins_)	//go down
					{
						if (pos == (std::size_t)(-1))
						{
							pos = 0;
							for (auto* ptr : orig)
							{
								if (ptr == tu)
									break;
								++pos;
							}
							if (pos == orig.size())
								pos = (std::size_t)(-1);
						}
						if (pos != (std::size_t)(-1) && orig[pos] == tu)
						{
							auto cpy = orig;
							cpy[pos] = newPseudo;
							//std::cout << cpy << std::endl;
							DBG(cpy); DBG(std::endl);
						#ifdef EXTENSION_POINTERS_SET_TYPE
							evo->origins_.insert(std::move(cpy));
						#else
							
							////insertMerged(evo->origins_, cpy); //.insert(std::move(cpy));

							//DBG("insert orig: "); DBG(evo); DBG(","); DBG(newPseudo); DBG(std::endl);
							insertMergedOnce(std::move(cpy), evo->origins_);
							////evo->origins_.push_back(std::move(cpy));
						#endif
						#ifdef REVERSE_EXTENSION_POINTER_SET_TYPE
							newPseudo->evolution_.emplace(evo);
						#else
							DBG("insert evo: "); DBG(evo); DBG(","); DBG(newPseudo); DBG(std::endl);
							////newPseudo->evolution_.emplace_back(evo);
							insertMergedOnce(evo, newPseudo->evolution_);
						#endif
						}
					}
				}
			}
		}
	}
#endif




	//return;
	//return;
	//DO ALSO SOME WORK OF MERGE, because of these STUPID COSTS;
	//INCLUSION/EXCLUSION.. and so on..
	//prevent information loss in case of later unmerges ~> insertCompressed of DynAspCertificateAlgorithm
#ifndef SINGLE_LAYER_
	//return;
	//if (certificate_pointer_set_ != mergee.certificate_pointer_set_)
	//	return false;


	//ORIGINAL RESPONSIBILITY OF MERGE
	/*if(weight_ > mergee.weight_)
	{
		solutions_ = mergee.solutions_;
		weight_ = mergee.weight_;
	}
	else if(weight_ == mergee.weight_)
	{
		solutions_ += mergee.solutions_;
	}*/
	//TODO: ugly as hell
	//CertificateDynAspTuple& mergee = const_cast<CertificateDynAspTuple&>(static_cast<const CertificateDynAspTuple&>(merg));

	/*assert(mergee.origins_.size() > 0);
	assert(origins_.size() > 0);

	//set weights first..
	if (mergee.origins_.size() == 1)
		mergee.origins_.front().setWeight(mergee.weight_);

	if (origins_.size() == 1)
		origins_.front().setWeight(weight_);*/

	//ORIGINAL RESPONSIBILITY OF MERGE
	/*if(weight_ > mergee.weight_)
	{
		solutions_ = mergee.solutions_;
		weight_ = mergee.weight_;
	}
	else if(weight_ == mergee.weight_)
	{
		solutions_ += mergee.solutions_;
	}*/

	//insertMerged(static_cast<const CertificateDynAspTuple&>(mergee).origins_, origins_);
#ifdef EXTENSION_POINTERS_SET_TYPE
	insertSet(static_cast<CertificateDynAspTuple&>(merg).origins_, origins_);
#else
	origins_.insert(origins_.end(), make_move_iterator(static_cast<CertificateDynAspTuple&>(merg).origins_.begin()), make_move_iterator(static_cast<CertificateDynAspTuple&>(merg).origins_.end()));
	//insertMerged(static_cast<const CertificateDynAspTuple&>(merg).origins_, origins_);
#endif
/*#ifdef SECOND_PASS_COMPRESSED
	//insertMerged(static_cast<const CertificateDynAspTuple&>(merg).evolution_, evolution_);
#endif*/
	//return true;
#endif
}


	CertificateDynAspTuple::EJoinResult CertificateDynAspTuple::join(const TreeNodeInfo& info, const std::vector<unsigned int>& its, const std::vector<std::vector<IDynAspTuple *>*>& beg, const htd::ITreeDecomposition& td, htd::vertex_t node, ExtensionPointer& p)
	{
	/*#ifdef SUPPORTED_CHECK
		assert(false);
		char* pxs = nullptr;
		*pxs = '0';
	#endif*/
		CertificateDynAspTuple* first = static_cast<CertificateDynAspTuple*>((*beg[0])[its[0]]);

		//std::cout << "first: " << first->reductAtoms_ << "," <<  first->joinHash(td.childAtPosition(node,0), info.getAtoms(), info) << std::endl;

	#ifdef INT_ATOMS_TYPE
		htd::vertex_t child = td.childAtPosition(node, 0);
		atoms_ = info.transform(first->atoms_, child);
		reductAtoms_ = info.transform(first->reductAtoms_, child)
	#ifndef USE_PSEUDO
		| (first->reductAtoms_ & atom_vector(1 << (INT_ATOMS_TYPE - 1)))
	#endif
		;
		#ifdef SUPPORTED_CHECK
			supp_ = info.transform(first->supp_, child);
			atom_vector nsupp =  info.instance->getNodeData(child).getAtoms() & ~(first->supp_);
			if (!dynasp::create::isSatOnly() && dynasp::create::isNon() &&	//non-norm, size 2 join
			    (POP_CNT(info.transform(nsupp, child)) < POP_CNT(nsupp)))
			{
				DBG("kicking out join first");
				DBG(std::endl);
				return CertificateDynAspTuple::EJR_NO;
			}
		#endif
	#else
		mergeHashValue = 0;
		atoms_ = first->atoms_;
		reductAtoms_ = first->reductAtoms_;

	#endif
	#ifdef USE_PSEUDO
		pseudo = first->pseudo;
	#endif
		solutions_ = first->solutions_;
		weight_ = first->weight_;
		p[0] = first;
	#ifndef INT_ATOMS_TYPE
		SortedAtomVector f;
		for (const auto& a : info.introducedAtoms)
			if (atoms_.find(a) == atoms_.end())
				f.insert(a);

		for (const auto& a : info.rememberedAtoms)
			if (atoms_.find(a) == atoms_.end())
				f.insert(a);
	#endif

	#if !defined(INT_ATOMS_TYPE) && !defined(VEC_ATOMS_TYPE)
		atom_vector t;
		t.insert(t.end(), atoms_.begin(), atoms_.end());
		unsigned int weight = info.instance->weight(t, f, info);
		float sol = inf.instance->cost(t,f,info);
		#ifdef NON_NORM_JOIN
			assert(false);	//TODO
		#endif
	#else
		#ifdef NON_NORM_JOIN
			#ifdef INT_ATOMS_TYPE
				unsigned int weight = info.instance->weight(atoms_ & info.getJoinAtoms(), ~atoms_ & info.getJoinAtoms(), info);
				float sol = info.instance->cost(atoms_ & info.getJoinAtoms(), ~atoms_ & info.getJoinAtoms(), info);
			#else
				assert(false);
			#endif
		#else	

		unsigned int weight = info.instance->weight(atoms_, 
			#ifdef INT_ATOMS_TYPE
				~atoms_ & info.getAtoms()
			#else
				f
			#endif
			, info);
		float sol = info.instance->cost(atoms_, 
			#ifdef INT_ATOMS_TYPE
				~atoms_ & info.getAtoms()
			#else
				f
			#endif
			, info);
		#endif
	#endif
		for (unsigned int index = 1; index < its.size(); ++index)
		{
			CertificateDynAspTuple* t = static_cast<CertificateDynAspTuple*>((*beg[index])[its[index]]);

			//std::cout << std::endl << t->joinHash(td.childAtPosition(node,index), info.getAtoms(), info) << std::endl;
			//child = td.childAtPosition(node, index);
	#ifndef USE_PSEUDO
			//atoms_ |= info.transform(t->atoms_, child);
			reductAtoms_ |= /*info.transform(t->reductAtoms_, child) |*/ (t->reductAtoms_ & atom_vector(1 << (INT_ATOMS_TYPE - 1)));
			//std::cout << t->reductAtoms_ << std::endl;
		/*#ifdef SUPPORTED_CHECK
			supp_ |= t->supp_;
		#endif*/
	#else
			pseudo |= t->pseudo;
	#endif
		#ifdef NON_NORM_JOIN

			child = td.childAtPosition(node, index);
			#ifdef INT_ATOMS_TYPE
				atoms_ |= info.transform(t->atoms_, child);
				reductAtoms_ |= info.transform(t->reductAtoms_, child);
				#ifdef SUPPORTED_CHECK
				{
					atom_vector supps = 
						info.transform(t->supp_, child);
					atom_vector nsupps =  info.instance->getNodeData(child).getAtoms() & ~(t->supp_);
					if ( !dynasp::create::isSatOnly() && dynasp::create::isNon()	//non-norm, size 2 join
					  &&  (POP_CNT(info.transform(nsupps, child)) < POP_CNT(nsupps)))
					{
						DBG("kicking out join t"); 
						DBG(std::endl);
						return CertificateDynAspTuple::EJR_NO;
					}
					supp_ |= supps;
				}
				#endif

			#else
				assert(false);	//TODO
			#endif
		#endif
			//TODO: not naeher bestimmt up to now
			if (!isPseudo())
			{
				solutions_ *= t->solutions_ / sol;
				assert(t->weight_ >= weight);
				weight_ += t->weight_ - weight;
			}
			p[index] = t;
		}
		//supp_ |= checkSupportedRules(info);
	#ifndef USE_PSEUDO
		//std::cout << sizeof(std::size_t) << "vs:" << sizeof(unsigned int) << "," << sizeof(size_t) << "," << reductAtoms_ << "," << first->reductAtoms_ << std::endl;
		assert((reductAtoms_ != 0) >= (first->reductAtoms_ != 0));
		return (reductAtoms_ >> (INT_ATOMS_TYPE - 1)) != 0
		#ifndef NOT_MERGE_PSEUDO
			|| reductAtoms_
		#endif
			? CertificateDynAspTuple::EJR_PSEUDO : CertificateDynAspTuple::EJR_NON_PSEUDO;
	#else
		return	pseudo ? CertificateDynAspTuple::EJR_PSEUDO : CertificateDynAspTuple::EJR_NON_PSEUDO;
	#endif
	}

	//PRECONDITION: merge() successfully has been called before
	void CertificateDynAspTuple::merge(CertificateDynAspTuple& mergee, ExtensionPointer *p)
	{
		DBG(std::endl); DBG("  merge\t");
		DBG_COLL(atoms_); DBG("=");  DBG_COLL(mergee.atoms_);
		DBG(" ");
		DBG_COLL(reductAtoms_); DBG("=");  DBG_COLL(mergee.reductAtoms_);
		DBG("\t");
		DBG(&mergee); DBG(":"); DBG(mergee.solutionCount()); DBG("@"); DBG(mergee.solutionWeight()); DBG(","); DBG(this); DBG(":"); DBG(solutions_); DBG("@"); DBG(weight_); DBG(std::endl);

		if (p) 
		{
			//insertMerged(static_cast<const CertificateDynAspTuple&>(mergee).origins_, origins_);
		#ifdef EXTENSION_POINTERS_SET_TYPE
			origins_.insert(std::move(*p));
			//insertSet(*p, origins_);
		#else
			//origins_.insert(origins_.end(), *p);
			origins_.emplace_back(std::move(*p));
			//insertMerged(static_cast<const CertificateDynAspTuple&>(merg).origins_, origins_);
		#endif
		}
		if(weight_ > mergee.weight_)
		{
			solutions_ = mergee.solutions_;
			weight_ = mergee.weight_;
		}
		else if(weight_ == mergee.weight_)
		{
			solutions_ += mergee.solutions_;
		}
		#ifdef SUPPORTED_CHECK
		supp_ |= mergee.supp_;
		#endif
	}

	//static unsigned int cntr = 0;

	//PRECONDITION: join is feasible, i.e. join() successfully has been called before
	void CertificateDynAspTuple::joinPtrs(IDynAspTuple* tupleleft, IDynAspTuple* tupleright, bool reuseMemory)
	{
		//return;
		//reuseMemory = false;
		/*++cntr;
		std::cout << "CNT  " << cntr << std::endl;*/
		//return;
		//return;
		//assert(false);
	//#define SINGLE_LAYER
	//#ifdef A

	#ifndef SINGLE_LAYER
		//TODO: use it
		//make cross join, i.e.\ n+m tuple out of n and m tuples
		//ExtensionPointers tuples;
		//if (reuseMemory)
		assert(!reuseMemory);
		if (reuseMemory)
			origins_ = std::move(static_cast<CertificateDynAspTuple*>(tupleleft)->origins_);
	#ifdef USE_OPTIMIZED_EXTENSION_POINTERS
		origins_.reserve(origins_.size() +  (static_cast<CertificateDynAspTuple*>(tupleleft)->origins_.size() + static_cast<CertificateDynAspTuple*>(tupleright)->origins_.size()) / 3);

		auto & orig = reuseMemory ? origins_ : static_cast<CertificateDynAspTuple*>(tupleleft)->origins_;
		origins_.insert(origins_.end(), orig.begin(), orig.end());
		origins_.insert(origins_.end(), static_cast<CertificateDynAspTuple*>(tupleright)->origins_.begin(), static_cast<CertificateDynAspTuple*>(tupleright)->origins_.end());

	#else
	#ifndef EXTENSION_POINTERS_LIST_TYPE
		origins_.reserve(origins_.size() +  (static_cast<CertificateDynAspTuple*>(tupleleft)->origins_.size() * static_cast<CertificateDynAspTuple*>(tupleright)->origins_.size()) / 3);
	#endif
		auto & orig = reuseMemory ? origins_ : static_cast<CertificateDynAspTuple*>(tupleleft)->origins_;
		//unsigned int s = orig.size();
		for //(unsigned int p = 0; p < s; ++p)//
			 (auto &left : orig) //static_cast<CertificateDynAspTuple*>(tupleleft)->origins_)
		{
			//auto &left = orig[p];
			//ExtensionPointer tuple;
			bool first = true;
			auto leftsize = left.size();
			for (auto &right : static_cast<CertificateDynAspTuple*>(tupleright)->origins_)
			{
				//ExtensionPointer p;
				//p.reserve(left.size() + right.size());
				//p.insert(p.end(), left.begin(), left.end());
				if (first && reuseMemory)

	#ifndef EXTENSION_POINTERS_SET_TYPE
					left.insert(left.end(), right.begin(), right.end());
	#else
					{}
	#endif
				else
				{
					ExtensionPointer p;
					p.reserve(leftsize + right.size());
					p.insert(p.end(), left.begin(), left.begin() + leftsize);
					p.insert(p.end(), right.begin(), right.end());

					/*for (unsigned int i = 0; i < p.size(); ++i)
						for (unsigned int j = i + 1; j < p.size(); ++j)
								assert(p[i] != p[j]);*/

					//TODO: use move semantics
	#ifdef EXTENSION_POINTERS_SET_TYPE
					//std::cout << p << std::endl;

					//origins_.insert(CertificateDynAspTuple::cachePointer(std::move(p)));
					origins_.emplace(std::move(p));
					//insertSetOnce(p, origins_);
	#else
					origins_.emplace_back(std::move(p));
					//insertMergedOnce(std::move(p), origins_);
	#endif

					//insertMerged(tuples, origins_);
					//origins_.push_back(tuple);
				}
				first = false;
			}
		}
	#endif
		//std::cout << origins_.size() << std::endl;

		//std::cout << "CNT  " << cntr << std::endl;

		//TODO: move semantics
		//TODO: evolution needed?
	/*#ifdef SECOND_PASS_COMPRESSED
		//insertMerged(static_cast<CertificateDynAspTuple*>(tupleleft)->evolution_, evolution_);
		//insertMerged(static_cast<CertificateDynAspTuple*>(tupleright)->evolution_, evolution_);
	#endif*/
	#endif

		//std::cout << static_cast<CertificateDynAspTuple*>(tupleleft)->origins_.size()  << "," <<  static_cast<CertificateDynAspTuple*>(tupleright)->origins_.size() << std::endl;
	//#endif
	}

	void CertificateDynAspTuple::postEvaluate(unsigned char further)
	{
		//std::cout << "uppointer" << std::endl;
		//return;
	#ifndef SINGLE_LAYER_
		//TODO: prevent this stupid work
		//CertificateDynAspTuple::ReverseExtensionPointers ptrs = {this};//&dtuple};
		if (dynasp::create::passes() >= 4 && this->isPseudo() != further)
			return;
	#ifndef NOT_MERGE_PSEUDO
		if ((reductAtoms_ != 0) != (further != 0))
		{
			//std::cout << "PROBL: " << reductAtoms_ << "," << (bool)further << std::endl;
			//std::cout << this << "," << atoms_ << std::endl;
			return;
		}
	#endif
		for (const CertificateDynAspTuple::ExtensionPointer& down : origins_)
		{
			CertificateDynAspTuple* last = nullptr;
			for (CertificateDynAspTuple* ptr : down)
			{
				if (dynasp::create::passes() >= 4 && ptr->getClone() != ptr) //(unsigned char)ptr->isPseudo() != further)
					continue;
				assert(this == getClone());
				//assert(false);
				if (last && down.size() == 2)
				{
				#ifdef REVERSE_EXTENSION_POINTER_SEARCH_IDX
					std::unordered_set<const CertificateDynAspTuple*> x;
					last->evolutionWith_.emplace(this, std::move(x)).first->second.emplace(ptr);
				#endif
				}
				//else
				{
				#ifdef REVERSE_EXTENSION_POINTER_SET_TYPE
					ptr->evolution_.emplace(this);
					//insertSet(ptrs, ptr->evolution_);
				#else
					
					#ifndef NOT_MERGE_PSEUDO
					std::size_t sz = ptr->evolution_.size();
					#endif
					insertMergedOnce(this, ptr->evolution_);
				#ifndef NOT_MERGE_PSEUDO
					//assert(!further || reductAtoms_  || ptr->evolution_.size() == sz);
					//std::cout << (int)further << "," << reductAtoms_ << "DOWNTHING: " << ptr << "," << ptr->atoms_ << "," << ptr->reductAtoms_ << std::endl;
					assert(!further || reductAtoms_  || ptr->evolution_.size() == sz);
				#endif
				#endif
				}
			#ifndef COMBINE_PSEUDO_PSEUDO_SOLUTIONS
				assert(!ptr->isPseudo() || isPseudo());
			#endif
				last = ptr;
			}
		}
	#endif
		//std::cout << "uppointer" << std::endl;
	}

	//TODO: prevent exponential runtime w.r.t. bag size
	//TODO: remove this shice!
	//PRECONDITION: insert is feasible, i.e. insert() successfully has been called before
	void CertificateDynAspTuple::insertPtrs(const CertificateDynAspTuple& tuple) //sharp::ITupleSet &outputTuples)
	{

	#ifndef SINGLE_LAYER_
	//TODO: move?!
		//origins_ = std::move(tuple.origins_);
		origins_ = /*std::move*/(tuple.origins_);
		/*for (auto & t : outputTuples)
		{
			CertificateDynAspTuple& ts = reinterpret_cast<CertificateDynAspTuple&>(t);
			//ts.evolution_ = tuple.evolution_;
			ts.origins_ = this->origins_;
		}*/
	#endif
	}

	CertificateDynAspTuple* CertificateDynAspTuple::clone(Certificate_pointer_set* certificates, const ExtensionPointer& ptr)
	{
		CertificateDynAspTuple* newTuple = this->clone();
		//increase();
		newTuple->solutions_ = 0;//this->solutions_;
	#ifndef USE_PSEUDO
		//newTuple->atoms_ = this->atoms_;
		newTuple->reductAtoms_ = this->reductAtoms_;	//for the pseudo flag
	#else
		#ifndef YES_BUT_NO_PSEUDO
			newTuple->pseudo = this->pseudo;
		#endif
	#endif
		newTuple->weight_ = this->weight_;
		//TODO: remove const cast!?
		assert(this->getClone() == this);
		newTuple->duplicate_ = this;//->getClone(); //const_cast<SimpleDynAspTuple*>(this);

	#ifdef USE_N_PASSES
		if (duplicate_ == nullptr)	//CLAIM that this thing (me) is not allowed to be removed/deleted!
			duplicate_ = this;
	#endif

		//TODO: move semantics
	#ifdef EXTENSION_POINTERS_SET_TYPE
		newTuple->origins_.insert(ptr);
	#else
		newTuple->origins_.push_back(ptr);
	#endif
		//TODO: make evolution_ easy to copy and then do not copy :D
		//newClone->evolution_ = this->evolution_;
	#ifdef USE_N_PASSES
		newTuple->certsdone_ = this->certsdone_;
	#endif
		newTuple->certificate_pointer_set_ = certificates;
		//newClone->duplicate_ = true;
	#ifdef SUPPORTED_CHECK
		newTuple->supp_ = this->supp_;
	#endif
		return newTuple;
	}

	CertificateDynAspTuple::Certificate_pointer_set* CertificateDynAspTuple::cloneCertificates(unsigned char 
	#ifdef USE_N_PASSES
	pass
	#endif
	
	) const
	{
		Certificate_pointer_set *tupleType = certificate_pointer_set_;
		#ifdef USE_N_PASSES
			assert(dynasp::create::passes() >= 4 || tupleType == nullptr || certsdone_ > pass);
		#endif
		if (tupleType == nullptr ||
			#ifdef USE_N_PASSES
				certsdone_ > pass
			#else
				tupleType != nullptr	//tautology
			#endif
			)
		{
			tupleType = new Certificate_pointer_set(); //TupleType();
		#ifdef USE_N_PASSES
			if (dynasp::create::passes() >= 4 && pass >= 1 && certificate_pointer_set_ != nullptr)		//binary search for first pseudo cert
			{
				tupleType->reserve(certificate_pointer_set_->size());
				for (const auto& p : *certificate_pointer_set_)
					if (!p.cert->isPseudo())		
					#ifdef VEC_CERT_TYPE
						tupleType->push_back(p); //CertificateDynAspTuple::DynAspCertificatePointer(up, true));
					#else
						tupleType->insert(p); //CertificateDynAspTuple::DynAspCertificatePointer(up, true));
					#endif

					/*if (me.certificate_pointer_set_->back()->cert->isPseudo())
							{
								TupleType::iterator itx = std::lower_bound(me.certificate_pointer_set_->begin(), me.certificate_pointer_set_->end(), me.certificate_pointer_set_->back(), *this);
								assert(itx != me.certificate_pointer_set_->end());
								tupleType = new TupleType(me.certificate_pointer_set_->begin(), itx);
							}
							else
								tupleType = new TupleType(me.certificate_pointer_set_->begin(), me.certificate_pointer_set_->end());*/
			}
						/*else	//TODO: improve!
							tupleType = new TupleType();*/
		#endif
		}
		return tupleType;
	}

	#ifndef INT_ATOMS_TYPE
	//always prepare for the clones!
	void CertificateDynAspTuple::prepareForBag(const atom_vector& atoms)
	{
		//atoms_bar_ = ~atoms_ & atoms;
		//asssert(false);			//TODO:!
		assert(getClone() == this);
		for (const auto& a : atoms)
			if (atoms_.find(a) == atoms_.end())
				atoms_bar_.push_back(a);
	}
	#endif

//TODO: implement mergeData
void CertificateDynAspTuple::mergeData(CertificateDynAspTuple* oldTuple, CertificateDynAspTuple::ExtensionPointers::iterator& origin, const TreeNodeInfo& info, htd::vertex_t firstChild, bool newTuple)
{
	if (dynasp::create::passes() < 4)
		assert(oldTuple->getClone() == oldTuple);
	#ifdef CORRECT_WEIGHT_REORGANIZATION
	assert(oldTuple->solutions_);
	#endif
	//assert(getClone() == oldTuple);
	//pseudo does not get split up
	assert(!isPseudo());
	assert(!oldTuple->isPseudo());
	/*if (isPseudo())
	{
		assert(oldTuple->isPseudo());
		return;
	}*/

	DBG("REORG "); DBG(oldTuple); DBG(" origins: "); DBG(oldTuple->origins_.size()); DBG(" ->new "); DBG(this); DBG(std::endl);
#ifdef CORRECT_WEIGHT_REORGANIZATION
	auto it = origin->begin();
	assert(it != origin->end());

	BigNumber splitCount = (*it)->solutions_;
	DBG("PAR "); DBG((*it));  DBG(" "); DBG((*it)->solutions_); DBG(std::endl);
	unsigned int splitWeight = (*it)->weight_;

	if (dynasp::create::passes() < 4)
		assert(oldTuple->getClone() == oldTuple);
	unsigned int common_weight = 0;
	float common_count = 1;

	if (++it != origin->end())	//MORE THAN TWO CHILDREN, ASSUMING EQUI JOIN
	{
	#ifndef INT_ATOMS_TYPE
		SortedAtomVector f;
		for (const auto& a : info.introducedAtoms)
			if (oldTuple->atoms_.find(a) == oldTuple->atoms_.end())
				f.insert(a);

		for (const auto& a : info.rememberedAtoms)
			if (oldTuple->atoms_.find(a) == oldTuple->atoms_.end())
				f.insert(a);
	#endif

	#if !defined(INT_ATOMS_TYPE) && !defined(VEC_ATOMS_TYPE)
		atom_vector t;
		t.insert(t.end(), oldTuple->atoms_.begin(), oldTuple->atoms_.end());
		common_weight = info.instance->weight(t, f, info);
		common_count = info.instance->cost(t, f, info);
		#ifdef NON_NORM_JOIN
			assert(false);	//TODO
		#endif
	#else
		#ifdef NON_NORM_JOIN
			#ifdef INT_ATOMS_TYPE
				common_weight = info.instance->weight(oldTuple->atoms_ & info.getJoinAtoms(), ~oldTuple->atoms_ & info.getJoinAtoms(), info);
				common_count = info.instance->cost(oldTuple->atoms_ & info.getJoinAtoms(), ~oldTuple->atoms_ & info.getJoinAtoms(), info);
			#else
				assert(false);
			#endif
		#else	
		common_weight = info.instance->weight(oldTuple->atoms_, 
		#ifdef INT_ATOMS_TYPE
			~oldTuple->atoms_ & info.getAtoms()
		#else
			f
		#endif
		, info);
		
		common_count = info.instance->cost(oldTuple->atoms_, 
		#ifdef INT_ATOMS_TYPE
			~oldTuple->atoms_ & info.getAtoms()
		#else
			f
		#endif
		, info);

		#endif
	#endif
	}
	else if (splitCount)		//EXCHANGE NODE! determine common elements
	{
		#ifdef INT_ATOMS_TYPE
			atom_vector t = info.getAtoms() & ~info.transform(info.instance->getNodeData(firstChild).getAtoms(), firstChild);
			common_weight = info.instance->weight(t & oldTuple->atoms_, t & ~oldTuple->atoms_, info);
			splitWeight += common_weight;

			common_count = info.instance->cost(t & oldTuple->atoms_, t & ~oldTuple->atoms_, info);
			splitCount *= common_count;

		#else
			assert(false);	//TODO
		#endif
	}
	else
		return;

	for (; it != origin->end(); ++it)
	{
		DBG("it sol: "); DBG((*it)->solutions_); DBG(std::endl);
		//assert((*it)->solutions_);
		splitCount *= (*it)->solutions_ / common_count;
		assert((*it)->weight_ >= common_weight);
		splitWeight += (*it)->weight_ - common_weight;
	}

	if (splitCount == 0)
		return;


	DBG("OLD ONE REORG "); DBG(splitCount); DBG(" @ "); DBG(splitWeight); DBG(" c:("); DBG(common_weight); DBG(")"); DBG(std::endl);
	assert(oldTuple->weight_ <= splitWeight);
	//	splitWeight = 0;
	/*if (splitCount == 0)
	{
		solutions_ = 0;
		return;
	}*/


	//unsigned int minWeight = splitWeight;
	if (newTuple)
	{
		if (oldTuple != this)
		{
			solutions_ = splitCount;
			weight_ = splitWeight;
		}
	}
	else if (weight_ >= splitWeight)
	{
		if (weight_ == splitWeight)
			solutions_ += splitCount;
		else
			solutions_ = splitCount;
		weight_ = splitWeight;
		//also reorg the new tuple!
		/*for (auto it = origins_.begin(); it != origins_.end(); ++it)
		{
			if (it == origin)
				continue;
			unsigned int weight = 0;
			BigNumber count = 1;

			for (auto *t : *it)
			{
				weight += t->weight_;
				count *= t->solutions_;
			}
			assert (weight >= splitWeight);	//nothing to do here
			if (weight == splitWeight)	//we found something else with the same weight, can never be less than the minimum!
			{
				solutions_ += splitCount;
				minWeight = (unsigned int)-1;
				break;
			}
			else if (weight < minWeight)
			{
				minWeight = weight;
				minCount = count;
				assert (minWeight >= splitWeight);	//nothing to do here
			}
			else if (weight == minWeight)
			{
				minCount += count;
			}
		}
		if (minWeight != (unsigned int)-1)
		{
			weight_ = minWeight;
			solutions_ = minCount;
		}*/
	}

	if (oldTuple->weight_ < splitWeight)	//we were not the minimum anyway
		return;

	//from here on we are the minimum now!

	//for (auto& e : oldTuple->origins_)

	BigNumber minCount = 0;
	unsigned int minWeight = (unsigned int)(-1);
	for (auto it = oldTuple->origins_.begin(); it != oldTuple->origins_.end(); ++it)
	{
		if (it == origin)	//self origin
			continue;

		auto itx = it->begin();
		assert(itx != it->end());
		unsigned int weight = (*itx)->weight_;
		BigNumber count = (*itx)->solutions_;
		if (++itx == it->end())
		{
			weight += common_weight;
			count *= common_count;
		}
		for (; itx != it->end(); ++itx)
		{
			assert((*itx)->weight_ >= common_weight);
			weight += (*itx)->weight_ - common_weight;
			count *= (*itx)->solutions_ / common_count;
		}
		if (count == 0)
			continue;
		
		assert (weight >= splitWeight);	//nothing to do here
		if (weight == splitWeight)	//we found something else with the same weight, can never be less than the minimum!
		{
			oldTuple->solutions_ -= splitCount;
			assert(oldTuple->solutions_ >= 0);
			//oldTuple->weight_ = splitWeight;
			assert(oldTuple->weight_ == splitWeight);
			return;
		}
		else if (weight < minWeight)
		{
			minWeight = weight;
			minCount = count;
			assert (minWeight >= splitWeight);	//nothing to do here
		}
		else if (weight == minWeight)
		{
			minCount += count;
		}
	}

	oldTuple->weight_ = minWeight;
	oldTuple->solutions_ = minCount;
#else
	//this->duplicate_ = oldTuple;
	BigNumber counts;
	// 2 Cases:
	//if (oldExtendedTuple)	//new tuple created (~> not found)
	{

	}
	//else					//old certificates reused (~> found)
	{
		//TODO: implement weight: NOT NECESSARY as with PSEUDO solution?
		//TODO: implement pseudo flag?? or let it be?

/*#ifndef COMBINE_PSEUDO_PSEUDO_SOLUTIONS
		assert(oldTuple->solutions_ > 1);
#endif*/
		counts = 1;
		for (auto *tpl : *origin)
		{
			//special case for weights, only minimum weight solution count gets removed
			//if (tpl->weight_ == weight_)
				counts *= tpl->solutions_;
		}
/*#ifndef COMBINE_PSEUDO_PSEUDO_SOLUTIONS
		assert(counts > 0);
#endif*/
		oldTuple->solutions_ -= counts;
		this->solutions_ += counts;
#ifndef COMBINE_PSEUDO_PSEUDO_SOLUTIONS
		/*assert(oldTuple->solutions_ > 0);
		assert(solutions_ > 0);*/
#endif
	}
#endif
}

std::size_t CertificateDynAspTuple::joinHash(htd::vertex_t child, const atom_vector &atoms, const TreeNodeInfo& info) const
{
#ifdef INT_ATOMS_TYPE
	/*#ifndef USE_PSEUDO
		//return TO_INT((atoms_ | reductAtoms_) & (atoms | (atom_vector(1 << (INT_ATOMS_TYPE - 1))));

		return TO_INT(atoms_ & atoms & info.getAtoms()) | (TO_INT(reductAtoms_ & ((atoms & info.getAtoms()) | (atom_vector(1 << (INT_ATOMS_TYPE - 1))))) << (INT_ATOMS_TYPE - 1));
	#else*/
	//NOTE: info.getAtoms() due to RuleSetDynAspTuple, in other words: ONLY JOIN ATOMS!
//#ifdef USE_LAME_JOIN
		//std::cout << atoms_ << "," << reductAtoms_ << "," << atoms << "->" <<  (TO_INT(info.transform(atoms_,child) & atoms)) << "," << ((TO_INT(info.transform(reductAtoms_,child) & atoms) << (INT_ATOMS_TYPE - 0))) << "," << ((TO_INT(info.transform(atoms_,child) & atoms)) | ((TO_INT(info.transform(reductAtoms_,child) & atoms) << (INT_ATOMS_TYPE - 0))))  << std::endl;

		return TO_INT(info.transform(atoms_,child) & atoms) | (TO_INT(info.transform(reductAtoms_,child) & atoms) << (INT_ATOMS_TYPE - 0));
		//return TO_INT(atoms_ & atoms) | (TO_INT(reductAtoms_ & atoms) << (INT_ATOMS_TYPE - 0));
/*#else
		return TO_INT(atoms_) | (TO_INT(reductAtoms_) << (INT_ATOMS_TYPE - 0));
#endif*/
	//#endif
#else
	Hash h;
	size_t count = 0;
	for(atom_t atom : atoms)
		if(atoms_.find(atom) != atoms_.end())
		{
#ifdef VEC_ATOMS_TYPE
			h.add(atom);
#else
			h.addUnordered(atom);
#endif
			++count;
		}
#ifndef VEC_ATOMS_TYPE
	h.incorporateUnordered();
#endif
	h.add(count);
	count = 0;
	for(atom_t atom : atoms)
		if(reductAtoms_.find(atom) != reductAtoms_.end())
		{
#ifdef VEC_ATOMS_TYPE
			h.add(atom);
#else
			h.addUnordered(atom);
#endif
			++count;
		}
#ifndef VEC_ATOMS_TYPE
	h.incorporateUnordered();
#endif
	h.add(count);
	return h.get();
#endif
}

size_t CertificateDynAspTuple::joinHash(const atom_vector &atoms, const TreeNodeInfo& info) const
{
#ifdef INT_ATOMS_TYPE
	/*#ifndef USE_PSEUDO
		//return TO_INT((atoms_ | reductAtoms_) & (atoms | (atom_vector(1 << (INT_ATOMS_TYPE - 1))));

		return TO_INT(atoms_ & atoms & info.getAtoms()) | (TO_INT(reductAtoms_ & ((atoms & info.getAtoms()) | (atom_vector(1 << (INT_ATOMS_TYPE - 1))))) << (INT_ATOMS_TYPE - 1));
	#else*/
	//NOTE: info.getAtoms() due to RuleSetDynAspTuple, in other words: ONLY JOIN ATOMS!
//#ifdef USE_LAME_JOIN
		return TO_INT(atoms_ & atoms & info.getAtoms()) | (TO_INT(reductAtoms_ & atoms & info.getAtoms()) << (INT_ATOMS_TYPE - 0));
		//return TO_INT(atoms_ & atoms) | (TO_INT(reductAtoms_ & atoms) << (INT_ATOMS_TYPE - 0));
/*#else
		return TO_INT(atoms_) | (TO_INT(reductAtoms_) << (INT_ATOMS_TYPE - 0));
#endif*/
	//#endif
#else
	Hash h;
	size_t count = 0;
	for(atom_t atom : atoms)
		if(atoms_.find(atom) != atoms_.end())
		{
#ifdef VEC_ATOMS_TYPE
			h.add(atom);
#else
			h.addUnordered(atom);
#endif
			++count;
		}
#ifndef VEC_ATOMS_TYPE
	h.incorporateUnordered();
#endif
	h.add(count);
	count = 0;
	for(atom_t atom : atoms)
		if(reductAtoms_.find(atom) != reductAtoms_.end())
		{
#ifdef VEC_ATOMS_TYPE
			h.add(atom);
#else
			h.addUnordered(atom);
#endif
			++count;
		}
#ifndef VEC_ATOMS_TYPE
	h.incorporateUnordered();
#endif
	h.add(count);
	return h.get();
#endif
}

//TODO: update IDynAspTuple interface
size_t CertificateDynAspTuple::mergeHash() const
{
#ifdef INT_ATOMS_TYPE
	/*assert(atoms_ >> (INT_ATOMS_TYPE - 1) == 0);
	assert(reductAtoms_ >> (INT_ATOMS_TYPE - 1) == 0);*/
	//assert(INT_ATOMS_TYPE <= 32);

/*std::cout << "Hash(" <<   ((size_t(reductAtoms_)
	
		#ifdef COMBINE_PSEUDO_PSEUDO_SOLUTIONS
		& (TO_INT((unsigned int)(-1)) & (0 << (INT_ATOMS_TYPE - 1)))
		#endif 
		) << (INT_ATOMS_TYPE)) << std::endl;*/


	return TO_INT(atoms_) | ((TO_INT(reductAtoms_)
	//return TO_INT(atoms_) | (reductAtoms_)
	
		#ifdef COMBINE_PSEUDO_PSEUDO_SOLUTIONS
		& ~(1 << (INT_ATOMS_TYPE - 1))
		#endif 
		)
	
	
	<< (INT_ATOMS_TYPE - 0))
	#ifdef USE_PSEUDO
		#ifndef COMBINE_PSEUDO_PSEUDO_SOLUTIONS
			| ((this->pseudo ? 1 : 0) << (2 * INT_ATOMS_TYPE - 1))
		#endif
	#endif
	;
#else
	if (mergeHashValue != 0)
		return mergeHashValue;
	//TODO: update for certificates (?) depending on merge condition
	Hash h;

	for(atom_t atom : atoms_)
#ifdef VEC_ATOMS_TYPE
		h.add(atom);
#else
		h.addUnordered(atom);
#endif
#ifndef VEC_ATOMS_TYPE
	h.incorporateUnordered();
#endif
	h.add(atoms_.size());

	for(atom_t atom : reductAtoms_)
#ifdef VEC_ATOMS_TYPE
		h.add(atom);
#else
		h.addUnordered(atom);
#endif
#ifndef VEC_ATOMS_TYPE
	h.incorporateUnordered();
#endif
	h.add(reductAtoms_.size());

	mergeHash(h);

	//TODO: uncomment pseudo?
#ifndef COMBINE_PSEUDO_PSEUDO_SOLUTIONS
	h.add(this->pseudo ? (size_t)1 : (size_t)0);
#endif
	const_cast<CertificateDynAspTuple*>(this)->mergeHashValue = h.get();
	return mergeHashValue;
#endif
}


size_t CertificateDynAspTuple::hash() const
{
	//is this method needed by anyone?!
	assert(false);
	//TODO: update for certificates, if not already in mergeHash (see above)
	Hash h;

	h.add(this->mergeHash());
	h.add(weight_);
	//FIXME: add support for BigNumber
	//h.add(solutions_);

	return h.get();
}

IDynAspTuple *CertificateDynAspTuple::project(const TreeNodeInfo &info, size_t child) const
{
	CertificateDynAspTuple *newTuple = clone(); //new SimpleDynAspTuple(/*false*/);
	const CertificateDynAspTuple &me = dynasp::create::passes() >= 2 ? *this : *getClone()

/*#ifdef SEVERAL_PASSES
			*this
#else
			*getClone()
#endif*/
			;


	#ifdef SUPPORTED_CHECK
	#ifdef INT_ATOMS_TYPE	
		atom_vector ats = info.transform((me.supp_), child);
		atom_vector nats =  info.instance->getNodeData(child).getAtoms() & (~me.supp_);
		DBG("PROJECT_PROP_RULE "); DBG_COLL(me.supp_); DBG( " -> "); DBG_COLL(ats); DBG(std::endl);
		//if (POP_CNT(info.int_rememberedRules & me.atoms_) < POP_CNT(info.instance->getNodeData(child).getRules() & me.atoms_))
		if (!create::isSatOnly() &&  POP_CNT(info.transform(nats, child)) < POP_CNT(nats))
		{
			#ifdef USE_LAME_BASE_JOIN
				assert(false);	//we might here kick out stuff, we do not want to kick out yet!
			#endif
			//std::cout << "kicking out: " << nats << "," << info.transform(nats, child) << std::endl;
			//DBG(~ats); DBG(" vs "); DBG(~me.supp_); DBG(std::endl);
			DBG("kicking out "); DBG(&me); DBG(std::endl);
			return nullptr;
		}
	#endif
	#endif

#ifdef INT_ATOMS_TYPE
#ifndef USE_PSEUDO
	#ifdef NOT_MERGE_PSEUDO
	assert(me.reductAtoms_ == 0 || me.reductAtoms_ >> (INT_ATOMS_TYPE - 1) != 0);
	#endif
#endif
	newTuple->atoms_ = info.transform(me.atoms_, child);
	newTuple->reductAtoms_ = info.transform(me.reductAtoms_, child) 
		#ifndef USE_PSEUDO
			| (this->reductAtoms_ & atom_vector(1 << (INT_ATOMS_TYPE - 1)))
		#endif
		;
	#ifdef SUPPORTED_CHECK
		newTuple->supp_ = ats; //info.transform(me.supp_, child);
	#endif

	DBG("PROJECT "); DBG_COLL(me.atoms_); DBG(" -> "); DBG_COLL(newTuple->atoms_); DBG(std::endl); 
	DBG("PROJECT2 "); DBG_COLL(me.reductAtoms_); DBG(" -> "); DBG_COLL(newTuple->reductAtoms_); DBG(std::endl); 
#else
	// only keep remembered atoms
	for(atom_t atom : info.rememberedAtoms)
	{
		if(me.atoms_.find(atom) != me.atoms_.end())
			newTuple->atoms_.insert(atom);
		if(me.reductAtoms_.find(atom) != me.reductAtoms_.end())
			newTuple->reductAtoms_.insert(atom);
	}
#endif

	// keep weight and solution count
	newTuple->weight_ = weight_;
	newTuple->solutions_ = solutions_;
	DBG("SOLs "); DBG(newTuple->solutions_); DBG(std::endl);
#ifdef USE_PSEUDO
	#ifndef YES_BUT_NO_PSEUDO
		newTuple->pseudo = pseudo;
	#endif
#endif

	/*// do the same for the certificates
	for(const DynAspCertificate &cert : certificates_)
	{
		DynAspCertificate newCert;

		// only keep remembered atoms
		for(atom_t atom : info.rememberedAtoms)
			if(cert.atoms.find(atom) != cert.atoms.end())
				newCert.atoms.insert(atom);

		// copy whether cert represents the tuple
		newCert.same = cert.same;

		// add the new certificate
		newTuple->certificates_.insert(std::move(newCert));
	}*/

	return newTuple;
}

/*void CertificateDynAspTuple::postJoin(const TreeNodeInfo & info, atom_vector& trueAtoms, atom_vector& falseAtoms)
{
	// join atom assignment
		newTuple->atoms_ = atoms_;
			//TODO: SORTED INSERT
		#ifdef VEC_ATOMS_TYPE
			atoms_.insertDup(other.atoms_); //.begin(), other.atoms_.end());
		#else
			atoms_.insert(other.atoms_.begin(), other.atoms_.end());
		#endif
			//for(atom_t atom : other.atoms_)
			//	newTuple->atoms_.insert(atom);

			reductAtoms_ = reductAtoms_;
			//TODO: sorted insert
		#ifdef VEC_ATOMS_TYPE
			reductAtoms_.insertDup(other.reductAtoms_); //.begin(), other.reductAtoms_.end());
		#else
			reductAtoms_.insert(other.reductAtoms_.begin(), other.reductAtoms_.end());
		#endif
		//for(atom_t atom : other.reductAtoms_)
		//	newTuple->reductAtoms_.insert(atom);

		// join weights/counts
		pseudo = pseudo || other.pseudo;
		solutions_ = solutions_ * other.solutions_;

		weight_ = weight_ + other.weight_ - info.instance.weight(atoms_, falseAtoms);
		//return this;
}*/

bool CertificateDynAspTuple::merge(const IDynAspTuple &tuple)
{
	const CertificateDynAspTuple &mergee =
			static_cast<const CertificateDynAspTuple &>(tuple);

	DBG(std::endl); DBG("  merge\t");
	DBG_COLL(atoms_); DBG("=");  DBG_COLL(mergee.atoms_);
	DBG(" ");
	DBG_COLL(reductAtoms_); DBG("=");  DBG_COLL(mergee.reductAtoms_);
	DBG("\t");
	DBG(&tuple); DBG(":"); DBG(tuple.solutionCount()); DBG("@"); DBG(tuple.solutionWeight()); DBG(","); DBG(this); DBG(":"); DBG(solutions_); DBG("@"); DBG(weight_); DBG(std::endl);
	//DBG_SCERT(certificates_); DBG("=");
	//DBG_SCERT(mergee.certificates_);

	if(atoms_ != mergee.atoms_) return false;
	if((reductAtoms_ 

#ifdef COMBINE_PSEUDO_PSEUDO_SOLUTIONS
		& ~(1 << (INT_ATOMS_TYPE - 1))
		#endif 
	)!= (mergee.reductAtoms_
		#ifdef COMBINE_PSEUDO_PSEUDO_SOLUTIONS
		& ~(1 << (INT_ATOMS_TYPE - 1))
		#endif 

	)) return false;
	//TODO: uncomment pseudo?
#ifndef COMBINE_PSEUDO_PSEUDO_SOLUTIONS
#ifdef USE_PSEUDO
	if(pseudo != mergee.pseudo) return false;
#endif
#endif

	/*std::cout << "merging: " << weight_ << "," << mergee.weight_ << std::endl;
	std::cout << "resulting evolution: " << evolution_.size() << std::endl;*/

	//TODO: if certificates are in subset relation, we can sometimes merge
	//if(certificates_ != mergee.certificates_) return false;

//#ifdef SINGLE_LAYER
	if(weight_ > mergee.weight_)
	{
		solutions_ = mergee.solutions_;
		weight_ = mergee.weight_;
	}
	else if(weight_ == mergee.weight_)
	{
		solutions_ += mergee.solutions_;
	}
//#endif

	#ifdef SUPPORTED_CHECK
		supp_ |= mergee.supp_;
	#endif

	DBG("\t=>\t"); DBG_COLL(atoms_); DBG(" "); //DBG_SCERT(certificates_);

	DBG("\t=>\t"); DBG_COLL(reductAtoms_); DBG(" "); //DBG_SCERT(certificates_);

	return true;
}

//TODO:
void CertificateDynAspTuple::cleanUpRecursively()
{
#ifdef CLEANUP_SOLUTIONS
	std::unordered_set<CertificateDynAspTuple*> done;
	//done.reserve(origins_.size() * TYPICAL_CHILDS);
	//done.reserve(origins_.size() * origins_.front().size());
	for (const auto& ptr : origins_)
	{
		for (const auto& ptrComponent : ptr)
		//for (unsigned int p = 0; p < ptr.size(); ++p)
		{
			if (!done.insert(ptrComponent).second)
				continue;
			//std::cout << "rec access " << ptrComponent << std::endl;
			//const auto& ptrComponent = ptr[p];
		#ifdef REVERSE_EXTENSION_POINTER_SET_TYPE
			ptrComponent->evolution_.erase(this);
		#else
			for (size_t pos = 0; pos < ptrComponent->evolution_.size(); ++pos)
				//TODO
				if (ptrComponent->evolution_[pos] == this) //getClone())
				{
					//TODO: make faster
					//ptrComponent->evolution_.find(this);
					ptrComponent->evolution_.erase(ptrComponent->evolution_.begin() + pos); //find(this));//his);
					break;
				}
		#endif
			//propagate towards leaves
			if (ptrComponent->evolution_.size() == 0)
				ptrComponent->cleanUpRecursively();
		}
	}
	DBG("FREECLEANUP: "); DBG(this); DBG("\n");
	//std::cout << "delete " << this << std::endl;
	delete this;
#endif
}

bool CertificateDynAspTuple::cleanUpRoot(const TreeNodeInfo& info)
{
#ifdef CLEANUP_SOLUTIONS
	DBG("CLEANUP_ROOT"); DBG("\n");
	if (!isIntermediateSolution(info)) // isSolution())
	{
		cleanUpRecursively();
		return true;
	}
#endif
	return false;
}

bool CertificateDynAspTuple::cleanUpNonSolRoot(htd::vertex_t node, const TreeNodeInfo& info, const htd::ITreeDecomposition &decomposition, INodeTupleSetMap &tuples, bool realDelete)
{
#ifdef CLEANUP_SOLUTIONS
	DBG("CLEANUP_NON_SOLROOT"); DBG("\n");
	if (!isSolution(info) && (realDelete || solutions_ > 0)) // isSolution())
	{
		cleanUpRecursively(node, decomposition, tuples, realDelete ? IDynAspTuple::DM_DELETE : IDynAspTuple::DM_MARK);
		return true;
	}
#endif
	return false;
}

bool CertificateDynAspTuple::cleanUpRoot(htd::vertex_t node, const TreeNodeInfo& info, const htd::ITreeDecomposition &decomposition, INodeTupleSetMap &tuples, bool realDelete)
{
#ifdef CLEANUP_SOLUTIONS
	DBG("CLEANUP_ROOT"); DBG("\n");
	if (!isIntermediateSolution(info) && (realDelete || solutions_ > 0)) // isSolution())
	{
		cleanUpRecursively(node, decomposition, tuples, realDelete ? IDynAspTuple::DM_DELETE : IDynAspTuple::DM_MARK);
		return true;
	}
#endif
	return false;
}

bool CertificateDynAspTuple::cleanUp()
{
#ifdef CLEANUP_SOLUTIONS
	DBG("CLEANUP"); DBG("\n");
	if (evolution_.size() == 0
/*#ifdef SUPPORTED_CHECK	
	&& (getClone() == this || getClone()->evolution_.size() == 0)
#endif*/
	)    //not of use any more
	{
		cleanUpRecursively();
		return true;
	}
	return false;
#endif
}

void CertificateDynAspTuple::cleanUpRecursively(htd::vertex_t node, const htd::ITreeDecomposition &decomposition, INodeTupleSetMap &tuples, IDynAspTuple::DeleteMode mode)
{
#ifdef CLEANUP_SOLUTIONS
	DBG("CLEANUP"); DBG("\n");
	//if (evolution_.size() == 0)	//not of use any more
	{
		//std::cout << "CLEANUP " << this << std::endl;

		std::unordered_set<CertificateDynAspTuple*> done;
		//done.reserve(origins_.size() * TYPICAL_CHILDS);
		for (auto iptr = origins_.begin(); iptr != origins_.end();)
		//for (const auto& ptr : origins_)
		{
			const auto& ptr = *iptr;
			unsigned int idx = 0;
			//bool del = false;
			for (const auto& ptrComponent : ptr)
			//for (unsigned int p = 0; p < ptr.size(); ++p)
			{
				DBG("downclean: "); DBG(ptrComponent); DBG(" by "); DBG(this); DBG(std::endl);
				if (!done.insert(ptrComponent).second)
				{
					++idx;
					continue;
				}
				//std::cout << "access " << ptrComponent << std::endl;
				//const auto& ptrComponent = ptr[p];
			#ifdef REVERSE_EXTENSION_POINTER_SET_TYPE
				ptrComponent->evolution_.erase(this);
			#else
				for (size_t pos = 0; pos < ptrComponent->evolution_.size(); ++pos)
					//TODO
					if (ptrComponent->evolution_[pos] == this) //getClone())
					{
						//TODO: make faster
						//ptrComponent->evolution_.find(this);
						ptrComponent->evolution_.erase(ptrComponent->evolution_.begin() + pos); //find(this));//his);
						break;
					}
			#endif
				//propagate towards leaves
				if (ptrComponent->evolution_.size() == 0 && (mode != IDynAspTuple::DM_MARK || ptrComponent->solutions_ > 0))
				{
					//del = true;
					DBG("to cleanup: "); DBG(ptrComponent); DBG(ptrComponent->reductAtoms_); DBG("\n");
					ptrComponent->cleanUpRecursively(decomposition.childAtPosition(node, idx), decomposition, tuples, mode == IDynAspTuple::DM_MARK ? IDynAspTuple::DM_MARK : IDynAspTuple::DM_ERASE);//  true);
				}
				//ptrComponent->cleanUp(node, idx, decomposition, tuples);
				++idx;
			}
		//TODO: DANGEROUS! NON_SOLUTIONS NEEDED FOR PSEUDO SOLUTIONS!
		//#ifdef NOT_MERGE_PSEUDO
			if (/*del &&*/ create::isSupported() && mode == IDynAspTuple::DM_MARK)	//if we only mark, we should!!! remove the reference, otherwise double free in bad cases
				iptr = origins_.erase(iptr);
			else
		//#endif
				++iptr;
		}
		/*for (const auto& ptr : origins_)
			for (const auto& ptrComponent : ptr)
			{
				std::cout << "access t " << ptrComponent << std::endl;
				ptrComponent->cleanUp();
			}*/

		DBG("CLEANUP: "); DBG(this); DBG("\n");
		//std::cout << "delete " << this << std::endl;
		//delete this;
		//TODO: fix this stuff
		//decrease();
		//delete this;

		//cleanUpRecursively(node, pos, decomposition, tuples);
		//std::cout << "TRUE_DELETE " << this << std::endl;
		//TODO: make erase better
		switch (mode)
		{
			case IDynAspTuple::DM_ERASE:
				tuples[node].erase(*this);
				DBG("FREEERASE: "); DBG(this); DBG(std::endl);
				//NO break here!!
			case IDynAspTuple::DM_DELETE:
				DBG("FREEDELETE: "); DBG(this); DBG(","); DBG(atoms_); DBG(reductAtoms_); DBG("\n");
				fflush(stdout);
				delete this;
				break;
			case IDynAspTuple::DM_MARK:
				this->solutions_ = 0;
				//this->weight_ = 0;
				break;
			default:
				assert(false);
				break;
		}
		/*if (doDelete)
			tuples[node].erase(*this);
		//tuples[node].erase(pos);
		delete this;*/
		//return true;
	}
#endif
	//return false;
}

bool CertificateDynAspTuple::cleanUp(htd::vertex_t node, const htd::ITreeDecomposition &decomposition, INodeTupleSetMap &tuples, bool realDelete)
{
#ifdef CLEANUP_SOLUTIONS
	DBG("CLEANUP"); DBG("\n");
	if (evolution_.size() == 0 && (realDelete || solutions_ > 0))	//not of use any more
	{
		cleanUpRecursively(node, decomposition, tuples, realDelete ? IDynAspTuple::DM_ERASE : IDynAspTuple::DM_MARK);
		return true;
	}
#endif
	return false;
}

bool CertificateDynAspTuple::isSolution(const TreeNodeInfo& 
#ifdef SUPPORTED_CHECK
	info
#endif
) const
{
	DBG("isSolution "); DBG(this); DBG("(~>"); DBG(duplicate_); DBG(")"); DBG(" "); DBG(isPseudo()); DBG(","); //  certificate_pointer_set_->size()  << std::endl;
	if (certificate_pointer_set_ == nullptr)
		DBG("ASSERT: certificate_pointer_set_ is NULL ");
	else
	{
		DBG(certificate_pointer_set_->size());
		DBG(" solution: "); DBG((certificate_pointer_set_->size() == 0 ||
								 (certificate_pointer_set_->size() == 1 && !certificate_pointer_set_->begin()->strict))); DBG(" / ");
	}
	DBG(this->solutions_); DBG("@"); DBG(this->weight_);
	DBG("\n");
	//TODO: uncomment pseudo
#ifndef COMBINE_PSEUDO_PSEUDO_SOLUTIONS
#ifndef SINGLE_LAYER
//#ifndef CLEANUP_SOLUTIONS
	#ifdef NOT_MERGE_PSEUDO
	if (!create::isSatOnly() && !create::isSupported() && !create::isProjection())
		assert(!(isPseudo() ^ (certificate_pointer_set_ == nullptr)));
	#endif
//#endif
#endif
#endif
	//return true;
	return 
	#ifdef SUPPORTED_CHECK
		(create::isSatOnly() || ((~supp_ & info.getAtoms()) == 0)) && (supp_ != ~((atom_vector)0)) &&
	#endif
	#ifndef NOT_MERGE_PSEUDO
		solutions_ > 0 &&
	#endif
	!isPseudo() && (certificate_pointer_set_ == nullptr || certificate_pointer_set_->size() == 0 ||
						   (certificate_pointer_set_->size() == 1 && !certificate_pointer_set_->begin()->strict));
}

/*#ifdef SUPPORTED_CHECK
atom_t CertificateDynAspTuple::checkSupportedRules(const TreeNodeInfo& info) const
{
	atom_t supp = (~atoms_ & info.getAtoms());
	return  checkRules(atoms_, ~atoms_ & info.getAtoms(), reductAtoms_, supp, info.rememberedRules, info) | 
		checkRules(atoms_, ~atoms_ & info.getAtoms(), reductAtoms_, supp, info.introducedRules, info);
}
#endif*/

const BigNumber& CertificateDynAspTuple::solutionCount() const
{
	return solutions_;
}

size_t CertificateDynAspTuple::solutionWeight() const
{
	return weight_;
}
#ifdef BNB_CRAFT_ALGO
	std::size_t CertificateDynAspTuple::addExtPtr(ExtensionPointer&& ptr, std::size_t answer)
	{
		std::size_t pos = 0;
		auto& extPtr = insertMergedOnce(ptr, origins_, &pos);
		//TODO: Design flaw
		const_cast<ExtensionPointer&>(extPtr).addAnswerSetContrib(answer);
		return pos;
		/*for (auto& tupl : extPtr)
			static_cast<CertificateDynAspTuple *>(tupl)->addAnswerSetContrib(answer);*/
		//origins_.emplace_back(ptr);	
	}
#endif

bool CertificateDynAspTuple::operator==(const ITuple &other) const
{
	//assert(false);

	//TODO: really !=?!
	/*if(typeid(other) != typeid(*this))
		return false;*/

	const CertificateDynAspTuple &tmpother =
		static_cast<const CertificateDynAspTuple &>(other);

	return /*weight_ == tmpother.weight_
		&& solutions_ == tmpother.solutions_
		&&*/ atoms_ == tmpother.atoms_
		&& (reductAtoms_ 
		
		#ifdef COMBINE_PSEUDO_PSEUDO_SOLUTIONS
		& ~(1 << (INT_ATOMS_TYPE - 1))
		#endif 

		/*#ifdef COMBINE_PSEUDO_PSEUDO_SOLUTIONS
		& (TO_INT((unsigned int)(-1)) & (0 << (INT_ATOMS_TYPE - 1)))
		#endif */
)
		== (tmpother.reductAtoms_
		#ifdef COMBINE_PSEUDO_PSEUDO_SOLUTIONS
		& ~(1 << (INT_ATOMS_TYPE - 1))
		#endif 

/*#ifdef COMBINE_PSEUDO_PSEUDO_SOLUTIONS
		& (TO_INT((unsigned int)(-1)) & (0 << (INT_ATOMS_TYPE - 1)))
		#endif */
)

		//TODO: uncomment pseudo?
	#ifndef COMBINE_PSEUDO_PSEUDO_SOLUTIONS
	#ifdef USE_PSEUDO
		&& pseudo == tmpother.pseudo
	#endif
	#endif

	#ifdef  BNB_CRAFT_ALGO
		&& weight_ == tmpother.weight_
	#endif
		
		;//&& certificates_ == tmpother.certificates_;



	/*const CertificateDynAspTuple &tmpother =
		static_cast<const CertificateDynAspTuple &>(other);*/

	/*return weight_ == tmpother.weight_
		&& solutions_ == tmpother.solutions_
		&& atoms_ == tmpother.atoms_
		&& reductAtoms_ == tmpother.reductAtoms_

		//TODO: uncomment pseudo?
	#ifndef COMBINE_PSEUDO_PSEUDO_SOLUTIONS
	#ifndef INT_ATOMS_TYPE
		&& pseudo == tmpother.pseudo
	#endif
	#endif
		;*///&& certificates_ == tmpother.certificates_;
}

//CertificateDynAspTuple::ESubsetRelation CertificateDynAspTuple::checkRelationExt(const CertificateDynAspTuple & other, const TreeNodeInfo & info)
//{
//	CertificateDynAspTuple::ESubsetRelation rel = CertificateDynAspTuple::ESR_NONE;
//	//unsigned int nonDefaultNegated = 0;
//#ifdef USE_REDUCT_SPEEDUP
//	//atom_set nondefaults;
//	if (other.atoms_.size() + other.reductAtoms_.size() > atoms_.size())
//		return rel;
//
//	//TODO: improve
//	//if there is an atom in the base model which is not true nor true, but in the reduct false and it is default negated => clash
//	for (const auto& a : atoms_)
//		if (info.instance.isNegatedAtom(a) & (other.reductAtoms_.find(a) == other.reductAtoms_.end()) & (other.atoms_.find(a) == other.atoms_.end()))
//			return rel;
//		/*if (!info.instance.isNegatedAtom(a) && other.atoms_.find(a) == other.atoms_.end())
//		{
//			nonDefaultNegated++;
//			nondefaults.insert(a);
//		}*/
//
//	/*for (const auto& a : info.rememberedAtoms)
//		if (!info.instance.isNegatedAtom(a) && other.atoms_.find(a) == other.atoms_.end())
//		{
//			nonDefaultNegated++;
//			nondefaults.insert(a);
//		}*/
//#endif
//	if (other.atoms_.size() + /*nonDefaultNegated +*/ other.reductAtoms_.size()
//
//#ifdef USE_REDUCT_SPEEDUP
//	<=
//#else
//	==
//#endif
//				atoms_.size())
//			rel = checkRelation(other, false);
//		/*&& (rel = checkRelation(other, false)) >= CertificateDynAspTuple::ESR_EQUAL)
//#ifdef USE_REDUCT_SPEEDUP
//		rel = checkRelationExt(other, atoms_, nondefaults, false);
//#else
//	{}
//#endif*/
//	return rel;
//}
//

CertificateDynAspTuple::ESubsetRelation CertificateDynAspTuple::checkRelationExt(const CertificateDynAspTuple & other, const TreeNodeInfo & info)
{
	CertificateDynAspTuple::ESubsetRelation rel = CertificateDynAspTuple::ESR_NONE;
#ifdef INT_ATOMS_TYPE
	//#if defined(USE_REDUCT_SPEEDUP) && defined(SECOND_PASS_COMPRESSED)
	if (dynasp::create::isCompr() && dynasp::create::reductSpeedup()) {
		//get everything which is in the atoms, but not within other.reductatoms or other.atom
		//info.getAtoms() due to RuleSetDynAspTuple
		assert((~info.int_negatedAtoms & other.reductAtoms_ & info.getAtoms()) == 0);
		if ((atoms_ & ~(other.reductAtoms_ | other.atoms_) & (info.int_negatedAtoms)) == 0 &&
		(atoms_ & ~(other.atoms_) & info.int_negatedChoiceAtoms) == 0 
		)
		{
			//std::cout << (atoms_ & ~(other.reductAtoms_ | other.atoms_)) << "," << info.int_negatedChoiceAtoms << std::endl;
			//extended criteria
			//NOTE: we make other.reductAtoms_ & info.getAtoms() to project away the MSB for pseudo solution!
			//if (POP_CNT(other.atoms_) + POP_CNT(other.reductAtoms_ & info.getAtoms()) <= POP_CNT(atoms_))
			if (POP_CNT(other.atoms_ & info.getAtoms()) + POP_CNT(other.reductAtoms_ & info.getAtoms()) + POP_CNT(atoms_ & info.getAtoms() & ~info.int_negatedAtoms & ~other.atoms_) == POP_CNT(atoms_ & info.getAtoms()))
				rel = checkRelation(other, info, false);
		}
	} else { //#else
		//NOTE: we make other.reductAtoms_ & info.getAtoms() to project away the MSB for pseudo solution!
		if (POP_CNT(other.atoms_ & info.getAtoms()) + POP_CNT(other.reductAtoms_ & info.getAtoms()) == POP_CNT(atoms_ & info.getAtoms()))	//partition!
			rel = checkRelation(other, info, false);
	} //#endif
#else
	//#if defined(USE_REDUCT_SPEEDUP) && defined(SECOND_PASS_COMPRESSED)
	if (dynasp::create::isCompr() && dynasp::create::reductSpeedup()) {
	/*atom_set nondefaults;
	unsigned int nonDefaultNegated = 0;
	if (other.atoms_.size() + other.reductAtoms_.size() > atoms_.size())
		return rel;

	for (const auto& a : atoms_bar_)
		if (!instance.isNegatedAtom(a) && other.atoms_.find(a) == other.atoms_.end())
		{
			nonDefaultNegated++;
			nondefaults.insert(a);
		}*/

	if (other.atoms_.size() + other.reductAtoms_.size() > atoms_.size())
		return rel;

	//TODO: improve
	//if there is an atom in the base model which is not true nor true, but in the reduct false and it is default negated => clash
	for (const auto& a : atoms_)
		if (info.instance->isNegatedAtom(a) && (other.reductAtoms_.find(a) == other.reductAtoms_.end()) && (other.atoms_.find(a) == other.atoms_.end()))
			return rel;

	if (other.atoms_.size() /*+ nonDefaultNegated*/ + other.reductAtoms_.size() <= atoms_.size())
		rel = checkRelation(other, info, false);
		/*&& checkRelation(other, false) >= CertificateDynAspTuple::ESR_EQUAL)
		rel = checkRelationExt(other, atoms_, nondefaults, false);*/
	} else { //#else
	//assert(false);
	//TODO: why wrong? why UNCOMMENT?
	if (other.atoms_.size() + other.reductAtoms_.size() == atoms_.size())
		rel = checkRelation(other, info, false);
	} //#endif
#endif
	return rel;
}

//TODO: make fast subset check
CertificateDynAspTuple::ESubsetRelation CertificateDynAspTuple::checkRelation(const CertificateDynAspTuple & other, const TreeNodeInfo& info, bool write)
{

	/*atom_set_o atoms1(atoms_.begin(), atoms_.end()), atoms2(static_cast<const SimpleDynAspTuple&>(other).atoms_.begin(), static_cast<const SimpleDynAspTuple&>(other).atoms_.end());
	atom_set_o atoms2_(static_cast<const SimpleDynAspTuple&>(other).reductAtoms_.begin(), static_cast<const SimpleDynAspTuple&>(other).reductAtoms_.end());*/

	//std::cout << "reductCheck FAIL " << this << ";" << &other << std::endl;
	//std::cout << CertificateDynAspTuple::checkRelation(other, atoms1, atoms2_, subset, write) << std::endl;
	//info.getAtoms() due to rules!!
	if (CertificateDynAspTuple::checkRelationExt(other, atoms_
														#ifdef INT_ATOMS_TYPE
														& info.getAtoms()
														#endif
														, other.reductAtoms_
														  #ifdef INT_ATOMS_TYPE
														  & info.getAtoms()
														  #endif
														  , false) >= CertificateDynAspTuple::ESR_EQUAL)
	{
		//std::cout << "reductCheck OK " << this << ";" << &other << std::endl;
		return CertificateDynAspTuple::checkRelationExt(other, atoms_
															   #ifdef INT_ATOMS_TYPE
															   & info.getAtoms()
															   #endif
															   , other.atoms_
																 #ifdef INT_ATOMS_TYPE
																 & info.getAtoms()
																 #endif
																 , write);
	}
	return CertificateDynAspTuple::ESR_NONE;
}












