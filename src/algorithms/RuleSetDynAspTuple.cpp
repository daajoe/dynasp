#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif
#include "../util/debug.hpp"

#include "RuleSetDynAspTuple.hpp"
#include "../../include/dynasp/DynAspCertificateAlgorithm.hpp"

#include <dynasp/IGroundAspInstance.hpp>
#include <stack>
#include <dynasp/create.hpp>

//FIXME: use config.h info here, build check into autoconf
#include <sys/times.h>

#ifdef HAVE_UNISTD_H //FIXME: what if not def'd?
	#include <unistd.h>
#endif

namespace dynasp
{
	using htd::vertex_t;
	using htd::ConstCollection;
	using htd::vertex_container;

	using sharp::Hash;

	using std::size_t;
	using std::unordered_set;
	using std::stack;

	/*size_t RuleSetDynAspTuple::DynAspCertificate::hash() const
	{
		Hash hash;

		for(atom_t atom : this->atoms)
			hash.addUnordered(atom);
		hash.incorporateUnordered();
		hash.add(this->atoms.size());

		for(rule_t rule : this->rules)
			hash.addUnordered(rule);
		hash.incorporateUnordered();
		hash.add(this->rules.size());

		hash.add(this->same ? (size_t)1 : (size_t)0);

		return hash.get();
	}

	bool RuleSetDynAspTuple::DynAspCertificate::operator==(
			const DynAspCertificate &other) const
	{
		return this->atoms == other.atoms
			&& this->rules == other.rules
			&& this->same == other.same;
	}*/

	RuleSetDynAspTuple::RuleSetDynAspTuple(bool /*leaf*/)
		//: weight_(0), solutions_(1)
	{
		#ifdef SUPPORTED_CHECK
			supp_ = (~0);
		#endif
		/*if (leaf)
		{
			DynAspCertificate a;
			a.same = true;
			certificates_.insert(std::move(a));
		}*/
		//TODO: @GCC-BUG
		//if(leaf) certificates_.insert({ { }, { }, true });
	}

	RuleSetDynAspTuple::~RuleSetDynAspTuple() { }

	bool RuleSetDynAspTuple::operator==(const ITuple &other) const
	{
		//TODO: really !=?!
		/*if(typeid(other) != typeid(*this))
			return false;*/

		if (!CertificateDynAspTuple::operator==(other))
			return false;

	#ifndef INT_ATOMS_TYPE
		const RuleSetDynAspTuple &tmpother =
				static_cast<const RuleSetDynAspTuple &>(other);

		return rules_ == tmpother.rules_;
	#else
		return true;
	#endif
	}


	void RuleSetDynAspTuple::mergeHash(Hash& 
	#ifndef INT_ATOMS_TYPE
	h
	#endif
	
	) const
	{
	#ifndef INT_ATOMS_TYPE
		for(rule_t rule : rules_)
			h.addUnordered(rule);
		h.incorporateUnordered();
		h.add(rules_.size());

		//return h.get();
	#endif
	}

	bool RuleSetDynAspTuple::isIntermediateSolution(const TreeNodeInfo& info) const
	{
#ifdef INT_ATOMS_TYPE
		return (info.getRules() & atoms_) == 0;
#else
		return rules_.size() == 0;
#endif
	}

	bool RuleSetDynAspTuple::isSolution(const TreeNodeInfo& info) const
	{
		const RuleSetDynAspTuple &me = *static_cast<const RuleSetDynAspTuple*>(getClone());
		DBG("isSolution "); DBG(this); DBG("(~>"); DBG(duplicate_); DBG(")"); DBG(" "); DBG(isPseudo()); DBG(","); //  certificate_pointer_set_->size()  << std::endl;

		DBG("rules: "); 
	#ifndef INT_ATOMS_TYPE	
		DBG(me.rules_.size()); 
	#endif	
		DBG(" ");
		if (certificate_pointer_set_ == nullptr)
			DBG("ASSERT: certificate_pointer_set_ is NULL");
		else
		{
			DBG(certificate_pointer_set_->size());
			DBG(" solution: "); DBG((certificate_pointer_set_->size() == 0 ||
									 (certificate_pointer_set_->size() == 1 && !certificate_pointer_set_->begin()->strict))); DBG(" / ");

		}
		DBG(this->solutions_);  DBG("@"); DBG(this->weight_);
		DBG("\n");
#ifndef COMBINE_PSEUDO_PSEUDO_SOLUTIONS
#ifndef SINGLE_LAYER
//#ifndef CLEANUP_SOLUTIONS

	if (!create::isSatOnly())
		assert(!(isPseudo() ^ (certificate_pointer_set_ == nullptr)));
//#endif
#endif
#endif

		if (isPseudo())
			return false;
		#ifdef INT_ATOMS_TYPE
		if ((me.atoms_ & info.getRules()) != 0)
		#else
		if (!me.rules_.empty())
		#endif
			return false;
		if (certificate_pointer_set_ == nullptr)
			return true;

		size_t reductModelCount = certificate_pointer_set_->size();
		for (const auto &cert : *certificate_pointer_set_)
			if (!cert.strict ||
				#ifdef INT_ATOMS_TYPE
					(static_cast<const CertificateDynAspTuple*>(cert.cert)->atoms_ & info.getRules()) != 0)
				#else
					(!static_cast<const RuleSetDynAspTuple*>(cert.cert)->rules_.empty()))
				#endif
				
				--reductModelCount;

		DBG(" model count: "); DBG(reductModelCount); DBG("\n");

		return !reductModelCount; //rules_.empty() && !isPseudo() &&  CertificateDynAspTuple::isSolution();
			//&& !reductModelCount;
	}

	bool RuleSetDynAspTuple::merge(const IDynAspTuple &tuple)
	{
	#ifndef INT_ATOMS_TYPE
		const RuleSetDynAspTuple &mergee =
				static_cast<const RuleSetDynAspTuple &>(tuple);
		if(rules_ != mergee.rules_) return false;
	#endif
		return CertificateDynAspTuple::merge(tuple);

		/*DBG(std::endl); DBG("  merge\t");
		DBG_COLL(atoms_); DBG("=");  DBG_COLL(mergee.atoms_);
		DBG("\t");
		DBG_COLL(rules_); DBG("="); DBG_COLL(mergee.rules_);
		DBG("\t");
		DBG_RSCERT(certificates_); DBG("=");
		DBG_RSCERT(mergee.certificates_);

		if(atoms_ != mergee.atoms_) return false;
		if(rules_ != mergee.rules_) return false;

		//TODO: if certificates are in subset relation, we can sometimes merge
		if(certificates_ != mergee.certificates_) return false;

		if(weight_ > mergee.weight_)
		{
			solutions_ = mergee.solutions_;
			weight_ = mergee.weight_;
		}
		else if(weight_ == mergee.weight_)
		{
			solutions_ += mergee.solutions_;
		}

		return true;*/
	}

	/*mpz_class RuleSetDynAspTuple::solutionCount() const
	{
		return solutions_;
	}

	size_t RuleSetDynAspTuple::solutionWeight() const
	{
		return weight_;
	}*/

	/*size_t RuleSetDynAspTuple::joinHash(const atom_vector &atoms) const
	{
		Hash h;
		size_t count = 0;
		for(atom_t atom : atoms)
			if(atoms_.find(atom) != atoms_.end())
			{
				h.addUnordered(atom);
				++count;
			}
		h.incorporateUnordered();
		h.add(count);
		return h.get();
	}*/

	IDynAspTuple *RuleSetDynAspTuple::project(const TreeNodeInfo &info, size_t child) const
	{
		const RuleSetDynAspTuple &me = *static_cast<const RuleSetDynAspTuple*>(getClone());
	#ifdef INT_ATOMS_TYPE	
		atom_vector ats = info.rule_transform(me.atoms_, child);
		DBG("PROJECT_PROP_RULE "); DBG_COLL(me.atoms_); DBG( " -> "); DBG_COLL(ats); DBG(std::endl);
		//if (POP_CNT(info.int_rememberedRules & me.atoms_) < POP_CNT(info.instance->getNodeData(child).getRules() & me.atoms_))
		if (POP_CNT(ats) < POP_CNT(info.instance->getNodeData(child).getRules() & me.atoms_))
	#else
		size_t ruleOkCount = 0;
		for(rule_t rule : info.rememberedRules)
			if(me.rules_.find(rule) != me.rules_.end())
				++ruleOkCount;
		if(ruleOkCount < me.rules_.size())
	#endif
	{
		DBG("kicking out "); DBG(&me); DBG(std::endl);
			return nullptr;
			}

		RuleSetDynAspTuple *newTuple = static_cast<RuleSetDynAspTuple*>(CertificateDynAspTuple::project(info, child)); // new RuleSetDynAspTuple(false);

		if (newTuple)
	#ifndef INT_ATOMS_TYPE
			/*for (rule_t r : info.rememberedRules)
				newTuple->rules_.insert(r);*/
			newTuple->rules_ = me.rules_;
	#else
			newTuple->atoms_ |= ats;
	#endif
		// only keep remembered atoms
		/*for(atom_t atom : info.rememberedAtoms)
		{
			if(me.atoms_.find(atom) != me.atoms_.end())
				newTuple->atoms_.insert(atom);
			if(me.reductAtoms_.find(atom) != me.reductAtoms_.end())
				newTuple->reductAtoms_.insert(atom);
		}

		// keep all rules
		// (check above already takes care of unsat forgotten rules)
		newTuple->rules_ = rules_;

		// keep weight and solution count
		newTuple->weight_ = weight_;
		newTuple->solutions_ = solutions_;*/

		// do the same for the certificates
		/*for(const DynAspCertificate &cert : certificates_)
		{
			ruleOkCount = 0;
			for(rule_t rule : info.rememberedRules)
				if(cert.rules.find(rule) != cert.rules.end())
					++ruleOkCount;
			if(ruleOkCount < cert.rules.size())
				continue;

			DynAspCertificate newCert;

			// only keep remembered atoms
			for(atom_t atom : info.rememberedAtoms)
				if(cert.atoms.find(atom) != cert.atoms.end())
					newCert.atoms.insert(atom);

			// copy rules from old certificate
			// (check above already takes care of unsat forgotten rules)
			newCert.rules = cert.rules;

			// copy whether cert represents the tuple
			newCert.same = cert.same;

			// add the new certificate
			newTuple->certificates_.insert(std::move(newCert));
		}*/

		return newTuple;
	}



	void RuleSetDynAspTuple::introduceFurther(
			const TreeNodeInfo &info,
			sharp::ITupleSet &outputTuples) const
	{

		size_t numIntro = info.introducedAtoms.size();
		//TODO: note indirection via getClone();

		assert(getClone() == this);
		const RuleSetDynAspTuple &me = *this; /*static_cast<const SimpleDynAspTuple*>(
	#ifdef SEVERAL_PASSES
				this
	#else
				getClone()
	#endif
				);*/

		atom_vector trueAtoms, reductFalseAtoms, falseAtoms;
	#ifndef INT_ATOMS_TYPE
		trueAtoms.reserve(numIntro + me.atoms_.size());
		trueAtoms.insert(trueAtoms.end(), me.atoms_.begin(), me.atoms_.end());
		reductFalseAtoms.reserve(numIntro + me.atoms_.size());
		reductFalseAtoms.insert(reductFalseAtoms.end(), me.reductAtoms_.begin(), me.reductAtoms_.end());

		//TODO: find more efficient
		for(const atom_t atom : info.rememberedAtoms)
			if(me.atoms_.find(atom) == me.atoms_.end() && me.reductAtoms_.find(atom) == me.reductAtoms_.end())
				falseAtoms.push_back(atom);
	#else
		trueAtoms = me.atoms_ & info.int_rememberedAtoms;
		reductFalseAtoms = me.reductAtoms_;
		falseAtoms = ~(me.atoms_ | me.reductAtoms_) & info.int_rememberedAtoms;
	#endif

		//FIXME: take into account assignment knowledge of info.instance.
		//FIXME: is code below better with stack-based subset enumeration?
		//FIXME: code below only works if max number of subsets fits into size_t


		//TODO: make this loop correct

		//for (const auto& t : outputTuples)
		//{
		for(size_t subset = 0; subset < /*ipow(3, numIntro) */(1u << numIntro); ++subset)
		{
		#ifndef INT_ATOMS_TYPE
			//const SimpleDynAspTuple& st = static_cast<const SimpleDynAspTuple&>(t);
			atom_vector /*newTrueAtoms,*/  reductFalseIndices;
			//newTrueAtoms.reserve(numIntro);
			reductFalseIndices.reserve(numIntro);
			for (size_t bit = 0; bit < numIntro; ++bit)
				if ((subset >> bit) & 1)
				//if (st.atoms_.find(info.introducedAtoms[bit]) != st.atoms_.end())
				{
					trueAtoms.push_back(info.introducedAtoms[bit]);
					//newTrueAtoms.push_back(info.introducedAtoms[bit]);
				}
				else
				{
					//falseAtoms.push_back(info.introducedAtoms[bit]);
					//newFalseAtoms.push_back(info.introducedAtoms[bit]);
					reductFalseIndices.push_back(bit);
				}
			unsigned int sz = reductFalseIndices.size();
		#else
			trueAtoms |= subset;
			unsigned int sz = numIntro - POP_CNT(atom_vector(subset));
		#endif
			for(size_t reduct = 0; reduct < (1u << sz); ++reduct)
			{
				if (!me.isPseudo() && reduct == 0)
					continue;
				//atom_vector newFalseAtoms;
				//newFalseAtoms.reserve(numIntro);
			#ifndef INT_ATOMS_TYPE
				for (size_t pos = 0; pos < reductFalseIndices.size(); ++pos)
					if ((reduct >> pos) & 1)
						reductFalseAtoms.push_back(info.introducedAtoms[reductFalseIndices[pos]]);
					else
					{
						falseAtoms.push_back(info.introducedAtoms[reductFalseIndices[pos]]);
						//newFalseAtoms.push_back(info.introducedAtoms[reductFalseIndices[pos]]);
					}
				rule_set tmpRules;
			#else
				size_t p = 0;
				for (size_t pos = 0; pos < sz; ++pos)
				{
					for (; p < numIntro; ++p)
						if (~(subset >> p) & 1)	//check for 0 bit
							break;

					if ((reduct >> pos) & 1)
						reductFalseAtoms |=  1 << p;
					else
						falseAtoms |= 1 << p;
					++p;
				}
				atom_vector tmpRules = 0;
			#endif


					bool validTuple = checkRules(
						trueAtoms,
						falseAtoms,
						reductFalseAtoms,
#ifndef INT_ATOMS_TYPE
						me.rules_,
					#else
						(me.atoms_ & info.getRules()) | info.int_introducedRules,
					#endif
						info, &tmpRules);

				#ifndef INT_ATOMS_TYPE
					validTuple &= checkRules(
						trueAtoms,
						falseAtoms,
						reductFalseAtoms,
						info.introducedRules, info, &tmpRules);
				#endif

				//TODO: use whole rules


				//DBG("\t"); DBG(validTuple ? "yes" : "no"); DBG(" ");

				if(validTuple)
				{

					DBG("\ttuple "); DBG("t"); DBG_COLL(trueAtoms); DBG(" f"); DBG_COLL(falseAtoms);  DBG(" r"); DBG_COLL(reductFalseAtoms);
					//TODO: pevent COPY, use move semantics!
					RuleSetDynAspTuple *newTuple = new RuleSetDynAspTuple(false);
					newTuple->insertPtrs(*this);
				#ifndef INT_ATOMS_TYPE
					newTuple->reductAtoms_.insert(reductFalseAtoms.begin(), reductFalseAtoms.end());

					newTuple->rules_ = std::move(tmpRules);
					newTuple->atoms_.insert(
							trueAtoms.begin(),
							trueAtoms.end());
				#else
					newTuple->reductAtoms_ = reductFalseAtoms;
					newTuple->atoms_ = trueAtoms | tmpRules;
				#endif
					/*newTuple->introWeight_ = info.instance->weight(newTrueAtoms, newFalseAtoms, info);

					newTuple->weight_ = weight_ + newTuple->introWeight_;


					newTuple->pseudo = pseudo || reductFalseAtoms.size() > 0;
					assert(newTuple->pseudo);*/
				#ifdef USE_PSEUDO
					#ifndef YES_BUT_NO_PSEUDO
						newTuple->pseudo = true;
					#endif
				#else
					newTuple->reductAtoms_ |= (1 << (INT_ATOMS_TYPE - 1));
				#endif
					#ifdef COMBINE_PSEUDO_PSEUDO_SOLUTIONS
						newTuple->solutions_ = 0;
					#endif
					//pseudo solutions have 0 solutions, shall be removed later...
				/*#ifdef COMBINE_PSEUDO_PSEUDO_SOLUTIONS
					newTuple->solutions_ = reductFalseAtoms.size() > 0 ? 0 : solutions_;
				#else
					newTuple->solutions_ = solutions_;
				#endif*/
					/*DBG(newTuple->isPseudo() ? "yes" : "no");
					DBG("\t");
					DBG(newTuple);
					DBG("\t");
					DBG(newTuple->weight_);
					DBG(" ");
					DBG(newTuple->solutions_);*/


					DBG(std::endl);
					//DBG("\t\t");
					//DBG_SCERT(newTuple->certificates_);

					outputTuples.insert(newTuple);
				}
			#ifndef INT_ATOMS_TYPE
				for (size_t pos = 0; pos < reductFalseIndices.size(); ++pos)
					if ((reduct >> pos) & 1)
						reductFalseAtoms.pop_back();
					else
					//{
						falseAtoms.pop_back();
			#else
				reductFalseAtoms = reductAtoms_;
				falseAtoms = ~(me.atoms_ | me.reductAtoms_) & info.int_rememberedAtoms;
			#endif
			}

			DBG(std::endl);

			// reset to state before big loop
			//FIXME: can this be optimized (i.e. no loop again at end)?
			//newFalseAtoms.clear();
			//newTrueAtoms.clear();
		#ifndef INT_ATOMS_TYPE
			for(size_t bit = 0; bit < numIntro; ++bit)
				if((subset >> bit) & 1) trueAtoms.pop_back();
		#else
			trueAtoms = me.atoms_ & info.int_rememberedAtoms;
		#endif
				//else falseAtoms.pop_back();
		}
	}



	void RuleSetDynAspTuple::introduceFurtherCompressed(
			const TreeNodeInfo &info, IDynAspTuple::ComputedCertificateTuples& certTuplesComputed,
			sharp::ITupleSet &outputTuples, unsigned int maxsize) const
	{


		/*introduceFurther(info, outputTuples);
		return;*/

		DBG("INSERTING "); DBG(isPseudo()); DBG(","); DBG_COLL(atoms_); DBG(","); DBG_COLL(reductAtoms_); DBG(","); 
	#ifndef INT_ATOMS_TYPE	
		DBG_COLL(rules_); 
	#endif	
		DBG(std::endl);
		//assert(!pseudo);

		size_t numIntro = info.introducedAtoms.size();
		RuleSetDynAspTuple *newTuple = nullptr;
		assert(getClone() == this);

			//TODO: note indirection via getClone();
			const RuleSetDynAspTuple &me = *this;
			atom_vector trueAtoms, reductFalseAtoms, falseAtoms;
		#ifndef INT_ATOMS_TYPE
			trueAtoms.reserve(numIntro + me.atoms_.size());
			trueAtoms.insert(trueAtoms.end(), me.atoms_.begin(), me.atoms_.end());
			reductFalseAtoms.reserve(numIntro + me.atoms_.size());
			reductFalseAtoms.insert(reductFalseAtoms.end(), me.reductAtoms_.begin(), me.reductAtoms_.end());

			//TODO: find more efficient
			for(const atom_t atom : info.rememberedAtoms)
				if(me.atoms_.find(atom) == me.atoms_.end() && me.reductAtoms_.find(atom) == me.reductAtoms_.end())
					falseAtoms.push_back(atom);
		#else
			trueAtoms = me.atoms_ & info.int_rememberedAtoms;
			reductFalseAtoms = me.reductAtoms_;
			falseAtoms = info.int_rememberedAtoms & ~(me.atoms_ | me.reductAtoms_);
		#endif



		//FIXME: take into account assignment knowledge of info.instance.
		//FIXME: is code below better with stack-based subset enumeration?
		//FIXME: code below only works if max number of subsets fits into size_t
		//TODO: make this loop correct

		//assert(evolution_.size());// || isPseudo());

		//TODO: make this part better

		//assert(evolution_.size());
		for (unsigned int out = 0; out < maxsize/*outputTuples.size()*/; ++out)
		{
			auto *ptr = static_cast<CertificateDynAspTuple *>(outputTuples[out]);
			if (ptr->isPseudo())
				break;
			if (ptr->solutionCount() == 0)
				continue;
		#ifdef INT_ATOMS_TYPE
			assert(ptr->reductAtoms_ == 0);
			if (const_cast<RuleSetDynAspTuple*>(this)->checkRelationExt(*this, ((ptr->atoms_) & info.int_rememberedAtoms), ((me.atoms_ | me.reductAtoms_) & info.int_rememberedAtoms), false) == CertificateDynAspTuple::ESR_NONE)
				continue;

			if (((me.atoms_ | me.reductAtoms_) & info.int_rememberedAtoms & info.int_negatedAtoms) != (ptr->atoms_ & info.int_rememberedAtoms & info.int_negatedAtoms))
				continue;
		#else
			assert(false);
		#endif

			//break;
			/*for (auto* ptr : evolution_)
			{*/
			/*assert(!ptr->isPseudo());
			if (!isPseudo())
			{
				IDynAspTuple::EvaluatedTuples::iterator doneIterator;
				if ((doneIterator = tuplesDone.find(ptr)) != tuplesDone.end())    //already computed
				{
					for (auto *tuple : doneIterator->second)
					{
						//if (isPseudo())	//extend pseudo solutions, evolution_ is wrong for now, fix it!
						insertMerged(origins_, static_cast<CertificateDynAspTuple *>(tuple)->origins_);
						insertMerged(ptr->evolution_, static_cast<CertificateDynAspTuple *>(tuple)->evolution_);
					}

					//DBG("ALREADY VISITED "); DBG(ptr->atoms_); DBG(","); DBG(ptr->reductAtoms_); DBG(std::endl);
					continue;
				}
			}
			IDynAspTuple::EvaluatedTupleResult result;*/

			DBG("VISITING "); DBG_COLL(ptr->atoms_); DBG(","); DBG_COLL(ptr->reductAtoms_); DBG(std::endl);
			//for (const auto& t : outputTuples)
			//{
			//for(size_t subset = 0; subset < (1u << numIntro); ++subset)
			//{
			//const SimpleDynAspTuple& st = static_cast<const SimpleDynAspTuple&>(t);
		#ifndef INT_ATOMS_TYPE
			atom_vector /*newFalseAtoms,*/  reductFalseIndices;
			//newFalseAtoms.reserve(numIntro);
			reductFalseIndices.reserve(numIntro);
			for (size_t bit = 0; bit < numIntro; ++bit)
				//if ((subset >> bit) & 1)
				if (ptr->atoms_.find(info.introducedAtoms[bit]) != ptr->atoms_.end())
				{
				/*#ifdef USE_REDUCT_SPEEDUP
					if (info.instance->isNegatedAtom(info.introducedAtoms[bit]))
				#endif*/
						reductFalseIndices.push_back(bit);
				/*#ifdef USE_REDUCT_SPEEDUP
					else
						trueAtoms.push_back(info.introducedAtoms[bit]);
				#endif*/
				}
				else
					falseAtoms.push_back(info.introducedAtoms[bit]);
					//newFalseAtoms.push_back(info.introducedAtoms[bit]);
			size_t sz = reductFalseIndices.size();
		#else
			atom_vector subset = (ptr->atoms_ & info.int_introducedAtoms);
			size_t sz = POP_CNT(subset);
			falseAtoms |= info.int_introducedAtoms & ~ptr->atoms_;
		#endif
			for(size_t reduct = 0; reduct < (1u << sz); ++reduct)
			{
				if (!isPseudo() && reduct == 0)
					continue;

			#ifndef INT_ATOMS_TYPE

			#ifdef USE_REDUCT_SPEEDUP
				bool addedReduct = false;
				for (size_t pos = 0; pos < sz; ++pos)
					if ((reduct >> pos) & 1)
						if (info.instance->isNegatedAtom(info.introducedAtoms[reductFalseIndices[pos]]))
						{
							addedReduct = true;
							break;
						}
				#endif
			#else
				#ifdef USE_REDUCT_SPEEDUP
					bool addedReduct = false;
					atom_vector ftmp = falseAtoms, ttmp = trueAtoms, rftmp = reductFalseAtoms;
				#endif
				size_t p = 0;
				for (size_t pos = 0; pos < sz; ++pos)
				{
					for (; p < numIntro; ++p)
						if ((TO_INT(subset) >> p) & 1)	//check for next 1 bit
							break;

					if ((reduct >> pos) & 1)
					
						#ifdef USE_REDUCT_SPEEDUP
						if (TO_INT(info.int_negatedAtoms) & (1 << p))	//check whether we have a negated atom here
						#endif
						//if (info.instance->isNegatedAtom(info.introducedAtoms[pos]))
						{
						#ifdef USE_REDUCT_SPEEDUP
							addedReduct = true;
						#endif
							reductFalseAtoms |=  1 << p;
						}

						#ifdef USE_REDUCT_SPEEDUP
						else
							falseAtoms |= 1 << p;
						#endif
					else
						trueAtoms |= 1 << p;
					++p;
				}
			#endif
#ifdef USE_REDUCT_SPEEDUP
				if (!isPseudo() && !addedReduct)// !reductFalseAtoms.size())
				{
				#ifdef INT_ATOMS_TYPE
					falseAtoms = ftmp;
					trueAtoms = ttmp;
					reductFalseAtoms = rftmp;
				#endif
					continue;
				}
#endif
				/*atom_vector newTrueAtoms;
				newTrueAtoms.reserve(numIntro);*/
			#ifndef INT_ATOMS_TYPE
				for (size_t pos = 0; pos < sz; ++pos)
					if ((reduct >> pos) & 1)
					{

						//assert (info.instance->isNegatedAtom(info.introducedAtoms[reductFalseIndices[pos]]));
#ifdef USE_REDUCT_SPEEDUP
						if (info.instance->isNegatedAtom(info.introducedAtoms[reductFalseIndices[pos]]))
#endif
							reductFalseAtoms.push_back(info.introducedAtoms[reductFalseIndices[pos]]);
#ifdef USE_REDUCT_SPEEDUP
						else
							falseAtoms.push_back(info.introducedAtoms[reductFalseIndices[pos]]);
#endif
					}
					else
						trueAtoms.push_back(info.introducedAtoms[reductFalseIndices[pos]]);
						//newTrueAtoms.push_back(info.introducedAtoms[reductFalseIndices[pos]]);
			#endif
				//TODO: prevent copy somehow?
				if (!newTuple)
					newTuple = new RuleSetDynAspTuple(false);

			#ifdef INT_ATOMS_TYPE
				newTuple->reductAtoms_ = reductFalseAtoms;
				newTuple->atoms_ = trueAtoms;
				atom_vector tmpRules = 0;
			#else
				newTuple->mergeHashValue = 0;
				newTuple->reductAtoms_.clear();
				newTuple->atoms_.clear();
				newTuple->reductAtoms_.insert(reductFalseAtoms.begin(), reductFalseAtoms.end());
				newTuple->atoms_.insert(
							trueAtoms.begin(),
							trueAtoms.end());
				rule_set tmpRules;
			#endif
				#ifdef USE_PSEUDO
					#ifndef YES_BUT_NO_PSEUDO
						newTuple->pseudo = true;
					#endif
				#else
					newTuple->reductAtoms_ |= (1 << (INT_ATOMS_TYPE - 1));
				#endif
				#ifdef COMBINE_PSEUDO_PSEUDO_SOLUTIONS
					newTuple->solutions_ = 0;
				#endif

					bool validTuple = checkRules(
						trueAtoms,
						falseAtoms,
						reductFalseAtoms,
					#ifndef INT_ATOMS_TYPE	
						me.rules_, 
					#else
						(me.atoms_ & info.getRules()) | info.int_introducedRules,
					#endif
						info, &tmpRules);

				#ifndef INT_ATOMS_TYPE
					if (validTuple)
						validTuple &= checkRules(
						trueAtoms,
						falseAtoms,
						reductFalseAtoms,
						info.introducedRules, info, &tmpRules);
				#endif

				#ifndef INT_ATOMS_TYPE
					newTuple->rules_ = std::move(tmpRules);
				#else
					newTuple->atoms_ |= tmpRules;
				#endif


				if (validTuple) {
				std::pair<IDynAspTuple::ComputedCertificateTuples::iterator, bool> found = certTuplesComputed.insert(newTuple);
				if (found.second)
				{

					//TODO: use whole rules


					/*DBG("\t");
					DBG(validTuple ? "yes" : "no");
					DBG(" ");*/

					if (validTuple && ptr->checkRelationExt(*newTuple, info) //(newTuple->atoms_.size() + newTuple->reductAtoms_.size() == ptr->atoms_.size() &&
							/*ptr->checkRelation(*newTuple, false)*/ >= CertificateDynAspTuple::ESR_EQUAL)//)
					{

						DBG("\ttuple "); DBG(newTuple->isPseudo()); DBG(" t"); DBG_COLL(newTuple->atoms_); DBG(" r"); DBG_COLL(newTuple->reductAtoms_); 
						#ifndef INT_ATOMS_TYPE
						DBG(" rr "); DBG_COLL(newTuple->rules_); 
					#endif	
						DBG(std::endl);
						//TODO: pevent COPY, use move semantics!
						//SimpleDynAspTuple *newTuple = new SimpleDynAspTuple();
						newTuple->insertPtrs(*this);

										//newTuple->reductAtoms_.insert(reductFalseAtoms.begin(), reductFalseAtoms.end());

						/*newTuple->atoms_.insert(
								trueAtoms.begin(),
								trueAtoms.end());*/

						//do not forget fake evolution here..
						//newTuple->evolution_ = ptr->evolution_; //.push_back(ptr);

						/*newTuple->introWeight_ = info.instance->weight(newTrueAtoms, newFalseAtoms, info);

						newTuple->weight_ = weight_ + newTuple->introWeight_;


						newTuple->pseudo = pseudo || reductFalseAtoms.size() > 0;*/

						assert(newTuple->isPseudo());
						//pseudo solutions have 0 solutions, shall be removed later...
						/*
#ifdef COMBINE_PSEUDO_PSEUDO_SOLUTIONS
						newTuple->solutions_ = reductFalseAtoms.size() > 0 ? 0 : solutions_;
#else
						newTuple->solutions_ = solutions_;
#endif*/
						DBG(newTuple->isPseudo() ? "yes" : "no");
						DBG("\t");
						DBG(newTuple);
						DBG("\t");
						/*DBG(newTuple->weight_);
						DBG(" ");
						DBG(newTuple->solutions_);*/


						DBG(std::endl);
						//DBG("\t\t");
						//DBG_SCERT(newTuple->certificates_);

						outputTuples.insert(newTuple);
						//certTuplesComputed.insert(newTuple);
						/*if (!isPseudo())
							result.push_back(newTuple);*/

						newTuple = nullptr;
					}
					else
					{
						DBG("\twrong "); DBG("t"); DBG_COLL(trueAtoms); DBG(" f"); DBG_COLL(falseAtoms);  DBG(" r"); DBG_COLL(reductFalseAtoms); DBG(std::endl);
						certTuplesComputed.erase(found.first);
					}
				}
				else if (//static_cast<CertificateDynAspTuple*>(*found.first)->atoms_.size() + static_cast<CertificateDynAspTuple*>(*found.first)->reductAtoms_.size() == ptr->atoms_.size() &&
						ptr->checkRelationExt(*static_cast<CertificateDynAspTuple*>(*found.first), info/*false*/) >= CertificateDynAspTuple::ESR_EQUAL)
				{
					DBG("\talready there "); DBG("size: "); DBG(certTuplesComputed.size()); DBG(" t"); DBG_COLL(trueAtoms); DBG(" f"); DBG_COLL(falseAtoms);  DBG(" r"); DBG_COLL(reductFalseAtoms); DBG(std::endl);
				#ifdef EXTENSION_POINTERS_SET_TYPE

					insertSet(origins_, static_cast<CertificateDynAspTuple*>(*found.first)->origins_);
				#else
					static_cast<CertificateDynAspTuple*>(*found.first)->origins_.insert(static_cast<CertificateDynAspTuple*>(*found.first)->origins_.end(), origins_.begin(), origins_.end());
					//insertMerged(origins_, static_cast<CertificateDynAspTuple*>(*found.first)->origins_);
				#endif
					//insertMerged(ptr->evolution_, static_cast<CertificateDynAspTuple*>(*found.first)->evolution_);
					//static_cast<CertificateDynAspTuple*>(*found.first)->insertPtrs(*this);
					/*if (!isPseudo())
						result.push_back(static_cast<CertificateDynAspTuple*>(*found.first));*/

				} }
			#ifndef INT_ATOMS_TYPE
				for (size_t pos = 0; pos < reductFalseIndices.size(); ++pos)
					if ((reduct >> pos) & 1)
					{
					#ifdef USE_REDUCT_SPEEDUP
						if (info.instance->isNegatedAtom(info.introducedAtoms[reductFalseIndices[pos]]))
					#endif
							reductFalseAtoms.pop_back();

					#ifdef USE_REDUCT_SPEEDUP
						else
							falseAtoms.pop_back();
					#endif
					}
					else
					//{
						trueAtoms.pop_back();
			#else
				trueAtoms = me.atoms_ & info.int_rememberedAtoms;
				reductFalseAtoms = me.reductAtoms_;
				falseAtoms = (info.int_rememberedAtoms & ~(me.atoms_ | me.reductAtoms_)) | (info.int_introducedAtoms & ~ptr->atoms_);
			#endif
			}

			DBG(std::endl);

			// reset to state before big loop
			//FIXME: can this be optimized (i.e. no loop again at end)?
			//newFalseAtoms.clear();
			//newTrueAtoms.clear();
		#ifndef INT_ATOMS_TYPE
			for (size_t bit = 0; bit < numIntro; ++bit)
				if (ptr->atoms_.find(info.introducedAtoms[bit]) == ptr->atoms_.end())
					falseAtoms.pop_back();
		#else
			falseAtoms = (info.int_rememberedAtoms & ~(me.atoms_ | me.reductAtoms_));
		#endif
				//else falseAtoms.pop_back();

			/*if (!isPseudo())
				tuplesDone.emplace(ptr, std::move(result));*/
		}
		delete newTuple;
	}


	void RuleSetDynAspTuple::introduce(
			const TreeNodeInfo &info, IDynAspTuple::ComputedCertificateTuples& certTuplesComputed,

			sharp::ITupleSet &outputTuples) const
	{
		size_t numIntro = info.introducedAtoms.size();
		//TODO: note indirection via getClone();
		const RuleSetDynAspTuple &me = *static_cast<const RuleSetDynAspTuple*>(getClone());
	#ifndef INT_ATOMS_TYPE
		atom_vector trueAtoms, reductFalseAtoms, falseAtoms;
		trueAtoms.reserve(numIntro + me.atoms_.size());
		trueAtoms.insert(trueAtoms.end(), me.atoms_.begin(), me.atoms_.end());
		reductFalseAtoms.reserve(numIntro + me.atoms_.size());
		reductFalseAtoms.insert(reductFalseAtoms.end(), me.reductAtoms_.begin(), me.reductAtoms_.end());
	#else
		atom_vector trueAtoms = 0, reductFalseAtoms = 0, falseAtoms = 0;
		reductFalseAtoms = me.reductAtoms_;
		trueAtoms = me.atoms_ & info.int_rememberedAtoms;
	#endif

		//TODO: find more efficient
	#ifdef INT_ATOMS_TYPE
		falseAtoms = ~(me.atoms_ | me.reductAtoms_) & info.int_rememberedAtoms;
	#else
		for(const atom_t atom : info.rememberedAtoms)
			if(me.atoms_.find(atom) == me.atoms_.end() && me.reductAtoms_.find(atom) == me.reductAtoms_.end())
				falseAtoms.push_back(atom);
	#endif
		//FIXME: take into account assignment knowledge of info.instance.
		//FIXME: is code below better with stack-based subset enumeration?
		//FIXME: code below only works if max number of subsets fits into size_t


		//TODO: make this loop correct
	
		RuleSetDynAspTuple* newTuple = nullptr;

		for(size_t subset = 0; subset < /*ipow(3, numIntro) */(1u << numIntro); ++subset)
		{
			atom_vector newTrueAtoms;
		#ifndef INT_ATOMS_TYPE	
			/*newFalseAtoms,*/ 
			atom_vector reductFalseIndices;
			newTrueAtoms.reserve(numIntro);
			reductFalseIndices.reserve(numIntro);
			for(size_t bit = 0; bit < numIntro; ++bit)
				if((subset >> bit) & 1)
				{
					trueAtoms.push_back(info.introducedAtoms[bit]);
					newTrueAtoms.push_back(info.introducedAtoms[bit]);
				}
				else
				{
					//falseAtoms.push_back(info.introducedAtoms[bit]);
					//newFalseAtoms.push_back(info.introducedAtoms[bit]);
					reductFalseIndices.push_back(bit);
				}
			unsigned int sz = reductFalseIndices.size();
		#else
			newTrueAtoms = subset;
			trueAtoms |= subset;
			unsigned int sz = numIntro - POP_CNT(newTrueAtoms);
		#endif

			for(size_t reduct = 0; reduct < (1u << sz); ++reduct)
			{
	//#ifdef THREE_PASSES
	//#ifdef NO_COMPUTE
				if ((dynasp::create::passes() == 1 || dynasp::create::passes() >= 3) && reduct > 0)
					break;
	//#endif
				atom_vector newFalseAtoms
				#ifdef INT_ATOMS_TYPE
					= 0
				#endif
				;
			#ifndef INT_ATOMS_TYPE
				newFalseAtoms.reserve(numIntro);
				for (size_t pos = 0; pos < reductFalseIndices.size(); ++pos)
					if ((reduct >> pos) & 1)
						reductFalseAtoms.push_back(info.introducedAtoms[reductFalseIndices[pos]]);
					else
					{
						falseAtoms.push_back(info.introducedAtoms[reductFalseIndices[pos]]);
						newFalseAtoms.push_back(info.introducedAtoms[reductFalseIndices[pos]]);
					}
			#else
				size_t p = 0;
				for (size_t pos = 0; pos < sz; ++pos)
				{
					for (; p < numIntro; ++p)
						if (~(subset >> p) & 1)	//check for 0 bit
							break;
					
					if ((reduct >> pos) & 1)
						reductFalseAtoms |=  1 << p;
					else
					{
						falseAtoms |= 1 << p;
						newFalseAtoms |= 1 << p;
					}
					++p;
				}
			#endif
			#ifdef INT_ATOMS_TYPE
				atom_vector tmpRules = 0;
			#else
				rule_set tmpRules;
			#endif

				bool validTuple = checkRules(
					trueAtoms,
					falseAtoms,
					reductFalseAtoms,
				#ifdef INT_ATOMS_TYPE
					info.int_introducedRules | (me.atoms_ & info.getRules()),
				#else
					me.rules_,
				#endif
					info, &tmpRules);
			#ifndef INT_ATOMS_TYPE
				if (validTuple)
					validTuple &= checkRules(
					trueAtoms,
					falseAtoms,
					reductFalseAtoms,
					info.introducedRules, info, &tmpRules);

			#endif
				if (validTuple) 
				{
					if (!newTuple)
						newTuple = new RuleSetDynAspTuple(false);

					#ifdef INT_ATOMS_TYPE
						newTuple->reductAtoms_ = reductFalseAtoms;
						newTuple->atoms_ = trueAtoms | tmpRules;
					#else
						newTuple->mergeHashValue = 0;	
						newTuple->reductAtoms_.clear();
						newTuple->atoms_.clear();
						newTuple->reductAtoms_.insert(reductFalseAtoms.begin(), reductFalseAtoms.end());
						newTuple->atoms_.insert(trueAtoms.begin(),
									trueAtoms.end());
						newTuple->rules_ = std::move(tmpRules);
					#endif
					
					#ifndef USE_PSEUDO
						newTuple->reductAtoms_ |= ((TO_INT(reductAtoms_) & (1 << (INT_ATOMS_TYPE - 1))) | ((reductFalseAtoms != 0 ? 1 : 0) << (INT_ATOMS_TYPE - 1)));
					#else
						#ifndef YES_BUT_NO_PSEUDO
							newTuple->pseudo = pseudo ||
							#ifdef INT_ATOMS_TYPE
								reductFalseAtoms != 0
							#else
								reductFalseAtoms.size() > 0
							#endif
							;
						#endif
					#endif
					//#ifdef THREE_PASSES
					if (dynasp::create::passes() >= 3)
						assert(!newTuple->isPseudo());
					//#endif
					std::pair<IDynAspTuple::ComputedCertificateTuples::iterator, bool> found = certTuplesComputed.insert(newTuple);
					
					newTuple->weight_ = weight_ + info.instance->weight(newTrueAtoms, newFalseAtoms, info);
					#ifdef COMBINE_PSEUDO_PSEUDO_SOLUTIONS
						newTuple->solutions_ =
						#ifdef INT_ATOMS_TYPE
								reductFalseAtoms != 0
							#else
								reductFalseAtoms.size() > 0
							#endif
						? 0 : solutions_  * info.instance->cost(newTrueAtoms, newFalseAtoms, info);
					#else
						newTuple->solutions_ = solutions_  * info.instance->cost(newTrueAtoms, newFalseAtoms, info);
					#endif
					if (found.second)
					{
						DBG("\ttuple "); DBG(" t "); DBG_COLL(trueAtoms); DBG(" f "); DBG_COLL(falseAtoms);  DBG(" r "); DBG_COLL(reductFalseAtoms); 
						#ifndef INT_ATOMS_TYPE
							DBG(" rr "); 
							DBG_COLL(rules_);
						#endif
						//TODO: pevent COPY, use move semantics!
						//SimpleDynAspTuple *newTuple = new SimpleDynAspTuple();
						newTuple->insertPtrs(*this);

						DBG(newTuple->isPseudo() ? "yes" : "no");
						DBG("\t");
						DBG(newTuple);
						DBG("\t");
						DBG(newTuple->weight_);
						DBG(" ");
						DBG(newTuple->solutions_);
						DBG(std::endl);
						
						outputTuples.insert(newTuple);
						newTuple = nullptr;
					}
					else
					{
						DBG("\talready there "); DBG("size: "); DBG(certTuplesComputed.size()); DBG(" t"); DBG_COLL(trueAtoms); DBG(" f"); DBG_COLL(falseAtoms);  DBG(" r"); DBG_COLL(reductFalseAtoms); DBG(std::endl);
					#ifdef EXTENSION_POINTERS_SET_TYPE
						insertSet(origins_, static_cast<CertificateDynAspTuple*>(*found.first)->origins_);
					#else
						static_cast<CertificateDynAspTuple*>(*found.first)->origins_.insert(static_cast<CertificateDynAspTuple*>(*found.first)->origins_.end(), origins_.begin(), origins_.end());
					#endif
						static_cast<CertificateDynAspTuple*>(*found.first)->merge(*newTuple, nullptr);
					}
				}

				#ifndef INT_ATOMS_TYPE
				for (size_t pos = 0; pos < reductFalseIndices.size(); ++pos)
					if ((reduct >> pos) & 1)
						reductFalseAtoms.pop_back();
					else
					//{
						falseAtoms.pop_back();
					/*	newFalseAtoms.pop_back();
					}*/
				#else
					reductFalseAtoms = me.reductAtoms_;
					falseAtoms = ~(me.atoms_ | me.reductAtoms_) & info.int_rememberedAtoms;
				#endif
			}

			DBG(std::endl);

			// reset to state before big loop
			//FIXME: can this be optimized (i.e. no loop again at end)?
			//newFalseAtoms.clear();
			//newTrueAtoms.clear();
			#ifndef INT_ATOMS_TYPE
			for(size_t bit = 0; bit < numIntro; ++bit)
				if((subset >> bit) & 1) trueAtoms.pop_back();
			#else
				trueAtoms = me.atoms_ & info.int_rememberedAtoms;
			#endif
				//else falseAtoms.pop_back();
		}
		delete newTuple;
	}


	CertificateDynAspTuple::EJoinResult RuleSetDynAspTuple::join(const TreeNodeInfo& info, const std::vector<unsigned int>& its, const std::vector<std::vector<IDynAspTuple *>*>& beg, const htd::ITreeDecomposition& td, htd::vertex_t node, ExtensionPointer& p)
	{

		RuleSetDynAspTuple* first = static_cast<RuleSetDynAspTuple*>((*beg[0])[its[0]]);

	#ifdef INT_ATOMS_TYPE
		htd::vertex_t child = td.childAtPosition(node, 0);
		atoms_ = info.transform(first->atoms_, child);
		reductAtoms_ = info.transform(first->reductAtoms_, child)
	#ifndef USE_PSEUDO
		| (first->reductAtoms_ & atom_vector(1 << (INT_ATOMS_TYPE - 1)))
	#endif
		;
		
	#else
		mergeHashValue = 0;
		atoms_ = first->atoms_;
		reductAtoms_ = first->reductAtoms_;
		rules_.clear();
		std::unordered_map<rule_t, unsigned int> visits;
		for (const auto& r : first->rules_)
			visits.emplace(r, 1);
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
		float sol = info.instance->cost(t,f,info);
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
	#ifdef INT_ATOMS_TYPE
		atom_vector atoms = atoms_, leftTrueAtoms = 0, leftFalseAtoms = 0, leftReductFalseAtoms = 0;
		atoms_ = info.rule_transform(first->atoms_, child);
	
		if (dynasp::create::isNon())	//non-norm, size 2 join
		{
			//assert(false);
			leftTrueAtoms        = atoms                          & ~info.getJoinAtoms(),
			leftFalseAtoms       = ~atoms       & info.transform(child) /*&  info.getAtoms()*/ & ~info.getJoinAtoms() & ~reductAtoms_,
			leftReductFalseAtoms = reductAtoms_                   & ~info.getJoinAtoms();

			if (POP_CNT(atoms_) < POP_CNT(info.instance->getNodeData(child).getRules() & first->atoms_))
				return CertificateDynAspTuple::EJR_NO;
		}
	#endif
	#ifdef NON_NORM_JOIN

		atom_vector rules = 0, rules_r = 0, rightTrueAtoms = 0, rightFalseAtoms = 0, rightReductFalseAtoms = 0, rtrans = 0;
		if (dynasp::create::isNon())
		{
		#ifdef INT_ATOMS_TYPE
			rules = /*atoms |=*/ atoms_ & ~info.getJoinRules();
			atoms_ &= info.getJoinRules();
		#else
			assert(false);
		#endif
			assert(its.size() == 2);
			assert(info.int_introducedRules == 0);
			assert(info.int_introducedAtoms == 0);
		}
	#endif
		for (unsigned int index = 1; index < its.size(); ++index)
		{
			RuleSetDynAspTuple* t = static_cast<RuleSetDynAspTuple*>((*beg[index])[its[index]]);
	#ifndef USE_PSEUDO
			//atoms_ |= info.transform(t->atoms_, child);
			reductAtoms_ |= /*info.transform(t->reductAtoms_, child) |*/ (t->reductAtoms_ & atom_vector(1 << (INT_ATOMS_TYPE - 1)));
	#else
			pseudo |= t->pseudo;
	#endif
	
			#ifdef INT_ATOMS_TYPE
				child = td.childAtPosition(node, index);
				atom_vector trans = info.rule_transform(t->atoms_, child);
				#ifdef NON_NORM_JOIN
					if (dynasp::create::isNon())
					{
						if (POP_CNT(trans) < POP_CNT(info.instance->getNodeData(child).getRules() & t->atoms_))
							return CertificateDynAspTuple::EJR_NO;
						
						//atoms_ &= trans /*& info.getJoinRules()*/;
						rules_r = /*atoms |=*/ trans & ~info.getJoinRules();
					}
				#endif
				atoms_ &= trans; // & info.getRules()) | ~info.getRules());
			#else
				for (const auto& r : t->rules_)
				{
					const auto it = visits.find(r);
					if (it != visits.end())
					{
						assert(it->second <= index);
						if (it->second == index)
							it->second++;
						else
							visits.erase(it);
					}
				}
				#ifdef NON_NORM_JOIN
					assert(false); //TODO
				#endif
			#endif
		
		#ifdef NON_NORM_JOIN
			//child = td.childAtPosition(node, index);

			if (dynasp::create::isNon())
			{
			#ifdef INT_ATOMS_TYPE
				atoms |= (trans = info.transform(t->atoms_, child));
				reductAtoms_ |= (rtrans = info.transform(t->reductAtoms_, child));
				
				rightTrueAtoms        = trans                    & ~info.getJoinAtoms();
				rightFalseAtoms       = ~trans & info.transform(child) /*  & info.getAtoms()*/ & ~info.getJoinAtoms() & ~rtrans;
				rightReductFalseAtoms = rtrans                   & ~info.getJoinAtoms();

			#else
				//child = td.childAtPosition(node, index);
				assert(false);	//TODO
			#endif
			}
		#endif

			//TODO: nicht naeher bestimmt up to now
			if (!isPseudo())
			{
				solutions_ *= t->solutions_ / sol;
				assert(t->weight_ >= weight);
				weight_ += t->weight_ - weight;
			}
			p[index] = t;
		}

		#ifdef NON_NORM_JOIN

		if (dynasp::create::isNon())// && rightTrueAtoms | rightFalseAtoms | rightReductFalseAtoms | leftTrueAtoms | leftFalseAtoms | leftReductFalseAtoms)
		{
		#ifdef INT_ATOMS_TYPE

		/*atom_vector leftTrueAtoms =  atoms_ & info.getAtoms() & ~info.getJoinAtoms(),
			~atoms_       & info.getAtoms() & ~info.getJoinAtoms() & ~reductAtoms_,
			reductAtoms_       & info.getAtoms() & ~info.getJoinAtoms(),

			other.atoms_ & info.getAtoms() & ~info.getJoinAtoms(),
			~other.atoms_ & info.getAtoms() & ~info.getJoinAtoms() & ~other.reductAtoms_,
			other.reductAtoms_ & info.getAtoms() & ~info.getJoinAtoms(),*/

		unsigned int rule_cnt = POP_CNT(rules) + POP_CNT(rules_r);
		for (unsigned int pos = POP_CNT(info.getAtoms()) + info.introducedRules.size(); rule_cnt && pos < INT_ATOMS_TYPE; ++pos)
		{
			atom_vector r = 1 << pos;
			if ((r & rules) == 0 && (r & rules_r) == 0)
				continue;

			IGroundAspRule::SatisfiabilityInfo si;
			const IGroundAspRule &ruleObject = info.instance->rule(info.ruleIdOfPosition(pos));//its->second);

			if ((r & rules) != 0)
			{

				DBG("checking left rule "); DBG(r); DBG(" with "); DBG(rightTrueAtoms); DBG(","); DBG(rightFalseAtoms); DBG(","); DBG(rightReductFalseAtoms); DBG(std::endl);
				si = ruleObject.check(
						rightTrueAtoms,
						rightFalseAtoms,
						rightReductFalseAtoms, 
						#ifdef SUPPORTED_CHECK
							nullptr,
						#endif
						info);
			}
			else // rightIter != rightRules.end()
			{

				DBG("checking right rule "); DBG(r); DBG(" with "); DBG(leftTrueAtoms); DBG(","); DBG(leftFalseAtoms); DBG(","); DBG(leftReductFalseAtoms); DBG(std::endl);
				si = ruleObject.check(
						leftTrueAtoms,
						leftFalseAtoms,
						leftReductFalseAtoms, 
						#ifdef SUPPORTED_CHECK
							nullptr,
						#endif
						info);
			}
			--rule_cnt;

			DBG("check result: "); DBG(si.satisfied); DBG(" / "); DBG(si.unsatisfied); DBG("joinatoms: "); DBG(info.getJoinAtoms()); DBG(std::endl);

			if (si.unsatisfied) return CertificateDynAspTuple::EJR_NO;
			if (si.satisfied) continue;

			atoms_ |= r;
		}
		#else
			assert(false); //TODO
		#endif
		}
		#endif

	#ifdef INT_ATOMS_TYPE
		atoms_ |= atoms;
	#else
		rules_.reserve(visits.size());
		for (const auto& v : visits)
			if (v.second == its.size())
				rules_.insert(v.first);
	#endif
	#ifndef USE_PSEUDO
		return (reductAtoms_ >> (INT_ATOMS_TYPE - 1)) != 0 ? CertificateDynAspTuple::EJR_PSEUDO : CertificateDynAspTuple::EJR_NON_PSEUDO;
	#else
		return pseudo ? CertificateDynAspTuple::EJR_PSEUDO : CertificateDynAspTuple::EJR_NON_PSEUDO;
	#endif
	}

//#undef TAKE_PSEUDO_RISKS
//#define TAKE_PSEUDO_RISKS

//TODO: only INT_ATOMS_TYPE at the moment!
#ifdef CHECK_CONSENSE
#ifndef INT_ATOMS_TYPE
	assert(false);
#endif

#ifdef DEBUG_INFO
	clock_t jointime = 0;
#endif

	CertificateDynAspTuple::ESubsetRelation RuleSetDynAspTuple::isConsense(CertificateDynAspTuple::IConsenseData & d, 
#ifdef CACHE_CERTS
		CertificateDynAspTuple::IConsenseData &d1,
#endif
	const TreeNodeInfo& info, 
#ifndef CACHE_CERTS	
		unsigned int cnt, 
#endif	
	const htd::ITreeDecomposition& td, htd::vertex_t node, bool strictOcc) const
	{
		DBG(" STATUS "); DBG(info.getRules()); DBG(","); DBG(isPseudo()); DBG(std::endl);
		if (info.int_rememberedRules == 0)
			return CertificateDynAspTuple::ESR_INCLUDED_STRICT;
	
		//std::cout << "non_strct " << std::endl;

		//TODO: IMPROVEME: Not sure if this is correct in all the cases!
		/*if (!isPseudo())	//simple check, no rules, no pseudo
			return (atoms_ & static_cast<RuleConsenseData&>(d).rules) >= static_cast<RuleConsenseData&>(d).rules ? CertificateDynAspTuple::ESR_INCLUDED_STRICT : CertificateDynAspTuple::ESR_NONE;*/

		/*{
			DBG("CHECK CONS "); DBG(atoms_); DBG(","); DBG(static_cast<RuleConsenseData&>(d).rules); DBG(std::endl);
			return (atoms_ & (info.int_rememberedRules)) == static_cast<RuleConsenseData&>(d).rules ? CertificateDynAspTuple::ESR_INCLUDED_STRICT : CertificateDynAspTuple::ESR_NONE;
		}*/
		
#ifndef CACHE_CERTS
		if ((atoms_ & static_cast<RuleConsenseData&>(d).rules) < static_cast<RuleConsenseData&>(d).rules)
			return CertificateDynAspTuple::ESR_NONE;

	#ifdef TAKE_PSEUDO_RISKS	
		if (!isPseudo())	//simple check, no rules, no pseudo
			return CertificateDynAspTuple::ESR_INCLUDED_STRICT;
	#endif
		RuleConsenseData& dd = static_cast<RuleConsenseData&>(d);
	#ifdef REVERSE_EXTENSION_POINTER_SEARCH_IDX
		auto itu = dd.ups.begin(), its = dd.tupleSet.begin();
		bool found = false;
		while (itu != dd.ups.end() && its != dd.tupleSet.end())
		{
			if (itu->first < its->first)
				++itu;
			else
			{
				if (itu->first == its->first)
				{
					if (dd.strict || itu->second || its->second)
						return CertificateDynAspTuple::ESR_INCLUDED_STRICT;
					if (!strictOcc)
						return CertificateDynAspTuple::ESR_EQUAL;
					found = true;
					++itu;
				}
				++its;
			}
		}
		return found ? CertificateDynAspTuple::ESR_EQUAL : CertificateDynAspTuple::ESR_NONE;
	#endif
		
	#ifdef DEBUG_INFO
		struct tms cpu;
		clock_t wall = times(&cpu);
	#endif
		bool foundOne = false;
		std::vector<unsigned int> poses(cnt);
		//RuleConsenseData& dd = static_cast<RuleConsenseData&>(d);
		for (unsigned int pos = cnt - 1; poses[0] < dd.tuples[0].size(); poses[pos]++)
		{
			if (poses[pos] < dd.tuples[pos].size())
			{
				pos = cnt - 1;
				bool pseudo = false, strict = false;
				atom_vector rules = ~0;

				for (unsigned int p = 0; p < cnt; ++p)
				{	
					pseudo |= dd.tuples[p][poses[p]]->cert->isPseudo();
					strict |= dd.tuples[p][poses[p]]->strict;
					rules &= info.rule_transform(dd.tuples[p][poses[p]]->cert->atoms_, td.childAtPosition(node, p));
				/*#ifndef ENUM_SOLUTIONS
					auto rt = dd.tuples[p][poses[p]]->cert->reductAtoms_ & info.getRules();
				#else
					auto rt = info.rule_transform(dd.tuples[p][poses[p]]->cert->atoms_, td.childAtPosition(node, p));
				#endif
					rules &= rt | (info.getRules() & ~info.transform(td.childAtPosition(node,p))); // & ~rt);
				*/


					if (pseudo && !isPseudo())
						break;
				}
				if (isPseudo() == pseudo && (atoms_ & rules) >= rules)	//both conditions at once!
				//if (isPseudo() == pseudo && (atoms_ & info.int_rememberedRules) == rules)	//both conditions at once!
				{
					if (dd.strict || strict)
					{
						DBG("strict.. strict "); DBG(dd.strict); DBG(","); DBG(strict); DBG(std::endl);
					#ifdef DEBUG_INFO
						jointime += times(&cpu) - wall;
					#endif
						return CertificateDynAspTuple::ESR_INCLUDED_STRICT;
					}
					if (!dd.strict && !strictOcc)
					{
					#ifdef DEBUG_INFO
						jointime += times(&cpu) - wall;
					#endif
						return CertificateDynAspTuple::ESR_EQUAL;
					}
					foundOne = true;
				}
			}
			else
			{
				poses[pos] = 0;
				--pos;
			}
		}
#else
		RuleConsenseData& dd = static_cast<RuleConsenseData&>(d);
		RuleConsenseData& dd1 = static_cast<RuleConsenseData&>(d1);
	
		//printf("%x %x %x\n", atoms_ & info.getRules(), dd.rules, dd1.rules);
		
		if ((atoms_ & (dd.rules & dd1.rules)) < (dd.rules & dd1.rules))
			return CertificateDynAspTuple::ESR_NONE;
		
	#ifdef TAKE_PSEUDO_RISKS	
		if (!isPseudo())	//simple check, no rules, no pseudo
			return CertificateDynAspTuple::ESR_INCLUDED_STRICT;
	#endif

		//std::cout << "OK" << std::endl;
	#ifdef DEBUG_INFO
		struct tms cpu;
		clock_t wall = times(&cpu);
	#endif
		bool foundOne = false;
		for (unsigned int pos = 0; pos < dd.tuples.size(); pos++)
		{
			if (dd.tuples[pos]->cert->isPseudo() && !isPseudo())
				continue;
			
			for (unsigned int ppos = 0; ppos < dd1.tuples.size(); ppos++)
			{
				assert(dd.tuples[pos] != dd1.tuples[ppos]);
				bool pseudo = dd.tuples[pos]->cert->isPseudo() | dd1.tuples[ppos]->cert->isPseudo(), 
				     strict = dd.tuples[pos]->strict | dd1.tuples[ppos]->strict;
				atom_vector rules = info.rule_transform(dd.tuples[pos]->cert->atoms_, td.childAtPosition(node, 0)) & 
						    info.rule_transform(dd1.tuples[ppos]->cert->atoms_, td.childAtPosition(node, 1));
				/*#ifndef ENUM_SOLUTIONS
					auto rt = dd.tuples[p][poses[p]]->cert->reductAtoms_ & info.getRules();
				#else
					auto rt = info.rule_transform(dd.tuples[p][poses[p]]->cert->atoms_, td.childAtPosition(node, p));
				#endif
					rules &= rt | (info.getRules() & ~info.transform(td.childAtPosition(node,p))); // & ~rt);
				*/
				
				if (isPseudo() == pseudo && (atoms_ & rules) >= rules)	//both conditions at once!
				//if (isPseudo() == pseudo && (atoms_ & info.int_rememberedRules) == rules)	//both conditions at once!
				{
					//std::cout << "RULES: " << rules << "(" << dd.tuples[pos]->cert << "," << dd1.tuples[ppos]->cert << ")" << info.rule_transform(dd.tuples[pos]->cert->atoms_, td.childAtPosition(node, 0)) << "," <<  info.rule_transform(dd1.tuples[ppos]->cert->atoms_, td.childAtPosition(node, 1)) << " ID: " << td.childAtPosition(node, 1)  << std::endl;
					//std::cout << "DD-Rules: " << &dd << "," << dd.rules << std::endl;
					//std::cout << "DD1-Rules: " << &dd1 << "," << dd1.rules << std::endl;
					//assert ((atoms_ & (dd.rules & dd1.rules)) >= (dd.rules & dd1.rules));
					if (dd.strict | dd1.strict | strict)
					{
						DBG("strict.. strict "); DBG(dd.strict | dd1.strict); DBG(","); DBG(strict); DBG(std::endl);
					#ifdef DEBUG_INFO
						jointime += times(&cpu) - wall;
					#endif
						return CertificateDynAspTuple::ESR_INCLUDED_STRICT;
					}
					DBG("str: "); DBG(~strictOcc); DBG(" vs "); DBG(!strictOcc); DBG(std::endl);
					if (!strictOcc)
					{
						DBG("never strict.."); DBG(dd.strict | dd1.strict | strictOcc); DBG(","); DBG(strict); DBG(std::endl);
					#ifdef DEBUG_INFO
						jointime += times(&cpu) - wall;
					#endif
						return CertificateDynAspTuple::ESR_EQUAL;
					}
					foundOne = true;
				}
			}
		}

#endif
	#ifdef DEBUG_INFO
		jointime += times(&cpu) - wall;
	#endif

		if (foundOne)
		{
			//assert ((atoms_ & (dd.rules & dd1.rules)) >= (dd.rules & dd1.rules));
			return CertificateDynAspTuple::ESR_EQUAL;
		}

		return CertificateDynAspTuple::ESR_NONE;
	}


	void RuleSetDynAspTuple::updateConsense(const DynAspCertificatePointer& cert, CertificateDynAspTuple::IConsenseData *& d, const TreeNodeInfo& info, unsigned int 
	
#ifndef CACHE_CERTS
	pos
#endif	
	, htd::vertex_t id) const
	{
		if (info.int_rememberedRules)
		{
			RuleConsenseData* dd = d ? static_cast<RuleConsenseData*>(d) :  new RuleConsenseData() ;
			d = dd;

			{
				DBG("UPDT CONS: "); DBG(dd); DBG(";"); DBG(info.rule_transform(cert.cert->atoms_, id)); DBG(std::endl);
				//std::cout << "UPDT CONS: " << (dd) << (";") << "(ID: " << id  << ") ," << (info.rule_transform(cert.cert->atoms_, id)) << (std::endl);
				dd->rules &= info.rule_transform(cert.cert->atoms_, id);
				//std::cout << "RESULT " << dd->rules << std::endl;
				/*auto rt = info.rule_transform(cert.cert->atoms_, id);
				dd->rules &= rt | (info.getRules() & ~info.transform(id));*/
			}
			//TODO: IMPROVEME: Not sure if this is correct in all the cases!
			//if (isPseudo())
		#ifdef REVERSE_EXTENSION_POINTER_SEARCH_IDX
			if (pos == 0)
			{
				auto itx = cert.cert->evolutionWith_.find(this);
				assert(itx != cert.cert->evolutionWith_.end());
				auto it = itx->second.begin();
				//cert.cert->evolutionWith_.reserve(cert.cert->evolutionWith_.size() + (end - itx));
				for (; it != itx->second.end(); ++it)
					dd->ups.emplace(*it, cert.strict).first->second |= cert.strict;
			}
			else
			{
				/*if (cert.strict)
					dd->tupleSetS.emplace(cert.cert);
				else*/
				dd->tupleSet.emplace(cert.cert, cert.strict).first->second |= cert.strict;
			}
			return;
		#endif

	#ifdef TAKE_PSEUDO_RISKS
		if (isPseudo())
	#endif
		#ifdef CACHE_CERTS
			insertMergedOnce(&cert, dd->tuples);
		#else
			{
				assert(pos <= dd->tuples.size());
				if (pos == dd->tuples.size())
					dd->tuples.emplace_back();

			/*#ifndef ENUM_SOLUTIONS
				const_cast<CertificateDynAspTuple*>(cert.cert)->reductAtoms_ = info.rule_transform(cert.cert->atoms_, id) | (cert.cert->reductAtoms_ & (1 << (INT_ATOMS_TYPE - 1))); //cert.cert->atoms_ & info.
				for (auto it = dd->tuples[pos].begin(); it != dd->tuples[pos].end(); ++it)
					if ((*it)->cert->reductAtoms_ == cert.cert->reductAtoms_ && (*it)->cert->isPseudo() == cert.cert->isPseudo() && (*it)->strict == cert.strict)
						return;

				dd->tuples[pos].push_back(&cert);
			#else*/
				insertMergedOnce(&cert, dd->tuples[pos]);
			//#endif
				
				/*if (dd->tuples[pos].size() >= 3)
					std::cout << dd->tuples[pos].size() << std::endl;*/
				
			}
		#endif
		}	
	}

#endif

	//TODO: speedmode, look into atom_set/find => improve speed
	IDynAspTuple *RuleSetDynAspTuple::join(
			const TreeNodeInfo &info,
			const atom_vector &baseVertices,
			const atom_vector &joinVertices,
			const IDynAspTuple &tuple,
			const atom_vector& tupleVertices) const
	{
		//assert(false);
		const RuleSetDynAspTuple &other =
			static_cast<const RuleSetDynAspTuple &>(tuple);

		DBG(std::endl); DBG("  join ");
		DBG_COLL(joinVertices); DBG("\t");
		DBG_COLL(atoms_); DBG("x"); DBG_COLL(other.atoms_);
		DBG("\t");
	#ifndef INT_ATOMS_TYPE
		DBG_COLL(rules_); DBG("x"); DBG_COLL(other.rules_);
	#endif
		DBG_COLL(reductAtoms_); DBG("x"); DBG_COLL(other.reductAtoms_);
		DBG("\t");
		//DBG_RSCERT(certificates_); DBG("x");
		//DBG_RSCERT(other.certificates_);
	#ifndef INT_ATOMS_TYPE
		atom_set joinSet;
		//atom_set joinSet(joinVertices.begin(), joinVertices.end());
		joinSet.insert(joinVertices.begin(), joinVertices.end());
		atom_vector trueAtoms, falseAtoms, reductFalseAtoms;
		falseAtoms.reserve(joinVertices.size());
		//TODO: make special case
	//#ifndef VEC_ATOMS_TYPE
		trueAtoms.reserve(joinVertices.size());
	/*#else
		bool speedMode = false;
	#endif*/
		atom_vector leftTrueAtoms, leftFalseAtoms;
		leftTrueAtoms.reserve(baseVertices.size());
		leftFalseAtoms.reserve(baseVertices.size());
		reductFalseAtoms.reserve(baseVertices.size());

		atom_vector rightTrueAtoms, rightFalseAtoms, reductRightFalseAtoms;
		rightTrueAtoms.reserve(tupleVertices.size());
		rightFalseAtoms.reserve(tupleVertices.size());
		reductRightFalseAtoms.reserve(tupleVertices.size());

		//TODO: make special case
		//special case? does it help?
		/*if (atoms_ == other.atoms_ && reductAtoms_ == other.reductAtoms_)
		{
#ifdef VEC_ATOMS_TYPE
			speedMode = true;
#endif
			for (atom_t atom : joinVertices)
			{
				if (!info.instance->isAtom(atom)) continue;
				if (atoms_.find(atom) == atoms_.end())
					falseAtoms.push_back(atom);
#ifndef VEC_ATOMS_TYPE
				else
					trueAtoms.push_back(atom);
				if (reductAtoms_.find(atom) == reductAtoms_.end())
					reductFalseAtoms.push_back(atom);
#endif
			}
			RuleSetDynAspTuple *newTuple = new RuleSetDynAspTuple(false);

			for (atom_t atom : baseVertices)
				if (!info.instance->isAtom(atom)) continue;
				else if (joinSet.find(atom) != joinSet.end()) continue;
				else if (atoms_.find(atom) != atoms_.end())
					leftTrueAtoms.push_back(atom);
				else
					leftFalseAtoms.push_back(atom);
			// check and join rules
			if (!checkJoinRules(
					info,
					joinVertices,
					leftTrueAtoms,
					leftFalseAtoms,
					atom_vector(0),
					rules_,
					leftTrueAtoms,
					leftFalseAtoms,
					atom_vector(0),
					other.rules_,
					newTuple->rules_))
			{
				delete newTuple;
				return nullptr;
			}
			// check and join rules
			if(!checkRules(*info.instance,

#ifdef VEC_ATOMS_TYPE
					atoms_,
#else
						   trueAtoms,
#endif
						   falseAtoms,
#ifdef VEC_ATOMS_TYPE
					reductAtoms_,
#else
						   reductFalseAtoms,
#endif
						   info.rememberedRules))
			{
				//delete newTuple;
				return nullptr;
			}
		}
		else*/
		//{
			trueAtoms.reserve(joinVertices.size());
			reductFalseAtoms.reserve(joinVertices.size());
			// check that atom assignment matches on the join atoms
			for (atom_t atom : joinVertices)
			{
				if (!info.instance->isAtom(atom)) continue;
				else if (atoms_.find(atom) == atoms_.end())
				{
					if (other.atoms_.find(atom) != other.atoms_.end())
						return nullptr;
					falseAtoms.push_back(atom);
				}
				else
				{
					if (other.atoms_.find(atom) == other.atoms_.end())
						return nullptr;
					trueAtoms.push_back(atom);
				}
				if (reductAtoms_.find(atom) == reductAtoms_.end())
				{
					if (other.reductAtoms_.find(atom) != other.reductAtoms_.end())
						return nullptr;
				}
				else
				{
					if (other.reductAtoms_.find(atom) == other.reductAtoms_.end())
						return nullptr;
					reductFalseAtoms.push_back(atom);
				}
			}

			for (atom_t atom : baseVertices)
				if (!info.instance->isAtom(atom)) continue;
				else if (joinSet.find(atom) != joinSet.end()) continue;
				else if (atoms_.find(atom) != atoms_.end())
					leftTrueAtoms.push_back(atom);
				else if (reductAtoms_.find(atom) != reductAtoms_.end())
					reductFalseAtoms.push_back(atom);
				else
					leftFalseAtoms.push_back(atom);

			for (atom_t atom : tupleVertices)
				if (!info.instance->isAtom(atom)) continue;
				else if (joinSet.find(atom) != joinSet.end()) continue;
				else if (other.atoms_.find(atom) != other.atoms_.end())
					rightTrueAtoms.push_back(atom);
				else if (other.reductAtoms_.find(atom) != other.reductAtoms_.end())
					reductRightFalseAtoms.push_back(atom);
				else
					rightFalseAtoms.push_back(atom);
			rule_set rules_out;

			// check and join rules
			if (!checkJoinRules(
					info,
					joinVertices,
					leftTrueAtoms,
					leftFalseAtoms,
					reductFalseAtoms,
					rules_,
					rightTrueAtoms,
					rightFalseAtoms,
					reductRightFalseAtoms,
					other.rules_,
					rules_out))
					//newTuple->rules_))
			{

				DBG("joincheckfalse");
				//delete newTuple;
				return nullptr;
			}
		//}
		#else
			atom_vector rules_out = 0;
			if ((atoms_ & joinVertices & info.getAtoms()) == (other.atoms_ & joinVertices & info.getAtoms()) && (reductAtoms_ & joinVertices & info.getAtoms()) == (other.reductAtoms_ & joinVertices & info.getAtoms()))
			{
				DBG("joinok"); DBG(baseVertices); DBG(" "); DBG(tupleVertices); DBG(std::endl);
				if (!checkJoinRules(
					info,
					joinVertices,
					 atoms_       & info.getAtoms() & ~joinVertices & baseVertices,
					~atoms_       & info.getAtoms() & ~joinVertices & baseVertices & ~reductAtoms_,
					reductAtoms_       & info.getAtoms() & ~joinVertices & baseVertices,
					atoms_       & info.getRules(),
					 other.atoms_ & info.getAtoms() & ~joinVertices & tupleVertices,
					~other.atoms_ & info.getAtoms() & ~joinVertices & tupleVertices & ~other.reductAtoms_,
					other.reductAtoms_ & info.getAtoms() & ~joinVertices & tupleVertices,
					other.atoms_ & info.getRules(),
					rules_out))
					//newTuple->rules_))
				{
					DBG("joincheckfalse");
					//delete newTuple;
					return nullptr;
				}
			}
			else
				return nullptr;
		#endif

		RuleSetDynAspTuple *newTuple = new RuleSetDynAspTuple(false);
		#ifdef INT_ATOMS_TYPE
			newTuple->atoms_ = (atoms_ & info.getAtoms()) | (other.atoms_ & info.getAtoms()) | rules_out;
			newTuple->reductAtoms_ = reductAtoms_ | other.reductAtoms_;
		#else
			newTuple->rules_ = std::move(rules_out);
			// join atom assignment
			newTuple->atoms_ = atoms_;
				//TODO: SORTED INSERT
			#ifdef VEC_ATOMS_TYPE
				newTuple->atoms_.insertDup(other.atoms_); //.begin(), other.atoms_.end());
			#else
				newTuple->atoms_.insert(other.atoms_.begin(), other.atoms_.end());
			#endif

				newTuple->reductAtoms_ = reductAtoms_;
				//TODO: sorted insert
			#ifdef VEC_ATOMS_TYPE
				newTuple->reductAtoms_.insertDup(other.reductAtoms_); //.begin(), other.reductAtoms_.end());
			#else
				newTuple->reductAtoms_.insert(other.reductAtoms_.begin(), other.reductAtoms_.end());
			#endif
		#endif
		// join weights/counts
		#ifdef USE_PSEUDO
			#ifndef YES_BUT_NO_PSEUDO
				newTuple->pseudo = pseudo || other.pseudo;
			#endif
		#endif
		newTuple->solutions_ = solutions_ * other.solutions_ / info.instance->cost(
			#ifdef INT_ATOMS_TYPE 
				atoms_ & joinVertices & info.getAtoms(), ~atoms_ & joinVertices & info.getAtoms()
			#else
				trueAtoms, falseAtoms
			#endif
			, info);

#ifdef VEC_ATOMS_TYPE
		//TODO: special case!
		/*if (speedMode)
			newTuple->weight_ = weight_ + other.weight_ - info.instance.weight(atoms_, falseAtoms, info);
		else*/
		newTuple->weight_ = weight_ + other.weight_ - info.instance->weight(trueAtoms, falseAtoms, info);
#else
	#ifndef INT_ATOMS_TYPE
		newTuple->weight_ = weight_ + other.weight_ - info.instance->weight(trueAtoms, falseAtoms, info);
	#else
		newTuple->weight_ = weight_ + other.weight_ - info.instance->weight(atoms_ & joinVertices & info.getAtoms(), ~atoms_ & joinVertices & info.getAtoms(), info);
	#endif
#endif
		/*newTuple->postJoin(info,

						   //TODO: special case
#ifdef VEC_ATOMS_TYPE
						   speedMode ? atoms_ : trueAtoms,
#else
						   trueAtoms,
#endif


						   falseAtoms);*/

		/*// join atom assignment
		newTuple->atoms_ = atoms_;
		for(atom_t atom : other.atoms_)
			newTuple->atoms_.insert(atom);

		// join weights/counts
		newTuple->solutions_ = solutions_ * other.solutions_;
		newTuple->weight_ = weight_ + other.weight_ - info.instance->weight(
				trueAtoms,
				falseAtoms, info);*/

		// join certificates
		/*atom_vector certTrueAtoms, reductFalseAtoms;
		atom_vector leftCertTrueAtoms, leftReductFalseAtoms;
		atom_vector rightCertTrueAtoms, rightReductFalseAtoms;*/

		//TODO: use something more intelligent than a nested loop join
		/*for(const DynAspCertificate &cert1 : certificates_)
		for(const DynAspCertificate &cert2 : other.certificates_)
		{
			// cleanup
			certTrueAtoms.clear();
			reductFalseAtoms.clear();
			leftCertTrueAtoms.clear();
			leftReductFalseAtoms.clear();
			rightCertTrueAtoms.clear();
			rightReductFalseAtoms.clear();

			DBG(std::endl); DBG("    cert ");
			DBG_COLL(joinVertices); DBG("\t");
			DBG_COLL(cert1.atoms); DBG("x"); DBG_COLL(cert2.atoms);
			DBG("\t");
			DBG_COLL(cert1.rules); DBG("x"); DBG_COLL(cert2.rules);

			// check if join condition is fulfilled
			bool skip = false;
			for(atom_t atom : trueAtoms)
				if(cert1.atoms.find(atom) == cert1.atoms.end())
				{
					if(cert2.atoms.find(atom) != cert2.atoms.end())
					{
						skip = true;
						break;
					}
					reductFalseAtoms.push_back(atom);
				}
				else
				{
					if(cert2.atoms.find(atom) == cert2.atoms.end())
					{
						skip = true;
						break;
					}
					certTrueAtoms.push_back(atom);
				}
			if(skip) continue;

			for(atom_t atom : leftTrueAtoms)
				if(cert1.atoms.find(atom) != cert1.atoms.end())
					leftCertTrueAtoms.push_back(atom);
				else
					leftReductFalseAtoms.push_back(atom);

			for(atom_t atom : rightTrueAtoms)
				if(cert2.atoms.find(atom) != cert2.atoms.end())
					rightCertTrueAtoms.push_back(atom);
				else
					rightReductFalseAtoms.push_back(atom);

			DynAspCertificate newCert;

			// join and check rules
			if(!checkJoinRules(
						info,
						joinVertices,
						leftCertTrueAtoms,
						leftFalseAtoms,
						leftReductFalseAtoms,
						cert1.rules,
						rightCertTrueAtoms,
						rightFalseAtoms,
						rightReductFalseAtoms,
						cert2.rules,
						newCert.rules))
			{
				continue;
			}

			newCert.atoms.insert(cert1.atoms.begin(), cert1.atoms.end());
			newCert.atoms.insert(cert2.atoms.begin(), cert2.atoms.end());
			newCert.same = cert1.same && cert2.same;

			DBG("\t>>\t"); DBG_COLL(newCert.atoms); DBG("\t");
			DBG_COLL(newCert.rules);

			newTuple->certificates_.insert(std::move(newCert));
		}*/
		
		// cleanup
		//certTrueAtoms.clear();
		//reductFalseAtoms.clear();
		//leftCertTrueAtoms.clear();
		//leftReductFalseAtoms.clear();
		//rightCertTrueAtoms.clear();
		//rightReductFalseAtoms.clear();

		DBG("\t=>\t"); DBG_COLL(newTuple->atoms_); DBG("\t");
	#ifndef INT_ATOMS_TYPE
		DBG_COLL(newTuple->rules_); DBG("\t");
	#endif
		DBG("\t=>\t"); DBG_COLL(newTuple->reductAtoms_); DBG(" ");
		//DBG_RSCERT(newTuple->certificates_);

		return newTuple;
	}

/*	CertificateDynAspTuple::ESubsetRelation RuleSetDynAspTuple::checkRules(const atom_vector &rules, const CertificateDynAspTuple & other, const TreeNodeInfo& info, bool write)
	{
		//enforce here a partition of true atoms (1st layer) into trueAtoms (2nd layer) and falseReductAtoms (2nd layer, but true on 1st layer)
		//furthermore, falseAtoms keeps the same, i.e.\ \bar{atoms_}
		// check rules

		assert(getClone() == this);
		assert(other.getClone() == &other);
		//TODO: remove const cast
		//RuleSetDynAspTuple& me = *const_cast<RuleSetDynAspTuple*>(static_cast<const RuleSetDynAspTuple*>(getClone()));
		RuleSetDynAspTuple& me = *this;
		//other = *getClone();

		CertificateDynAspTuple::ESubsetRelation validCert = CertificateDynAspTuple::ESR_NONE;
		if ((validCert = checkRelationExt(other, info)) >= CertificateDynAspTuple::ESR_EQUAL)
		{
			bool valid = true;

			const atom_vector & trueAtoms =  other.atoms_;
			const atom_vector & reductFalseAtoms = other.reductAtoms_;

			//rule_set output;

			valid &= me.checkRules(
					trueAtoms,
					info.getAtoms() &~me.atoms_,
					reductFalseAtoms,
					me.atoms_ & info.getRules()
					, info, nullptr);//&output);

			valid &= me.checkRules(
					trueAtoms,
					info.getAtoms() &~me.atoms_,
					reductFalseAtoms,
					rules, info, nullptr); //&output);


			if (!valid)// || output != rules_)
				validCert = CertificateDynAspTuple::ESR_NONE;
			else if (write)
#ifdef VEC_CERT_TYPE
				this->certificate_pointer_set_->emplace_back(CertificateDynAspTuple::DynAspCertificatePointer(&other, validCert == CertificateDynAspTuple::ESR_INCLUDED_STRICT));
#else
				this->certificate_pointer_set_->insert(CertificateDynAspTuple::DynAspCertificatePointer(&other,
																										validCert == CertificateDynAspTuple::ESR_INCLUDED_STRICT)).
						first->updateStrict(validCert == CertificateDynAspTuple::ESR_INCLUDED_STRICT);
#endif
		}
		return validCert;
	}
*/

	CertificateDynAspTuple::ESubsetRelation RuleSetDynAspTuple::checkRules(const rule_vector &/*rules*/, const CertificateDynAspTuple & other, const TreeNodeInfo& info, bool write)
	{
		//enforce here a partition of true atoms (1st layer) into trueAtoms (2nd layer) and falseReductAtoms (2nd layer, but true on 1st layer)
		//furthermore, falseAtoms keeps the same, i.e.\ \bar{atoms_}
		// check rules

		if (dynasp::create::passes() < 4)
			assert(getClone() == this);
		assert(other.getClone() == &other);
		//TODO: remove const cast
		//RuleSetDynAspTuple& me = *const_cast<RuleSetDynAspTuple*>(static_cast<const RuleSetDynAspTuple*>(getClone()));
		RuleSetDynAspTuple& me = *this;
		//other = *getClone();

		CertificateDynAspTuple::ESubsetRelation validCert = CertificateDynAspTuple::ESR_NONE;
		if ((validCert = checkRelationExt(other, info)) /*other.atoms_.size() + other.reductAtoms_.size() == atoms_.size() &&

			(validCert = checkRelation(other, false))*/ >= CertificateDynAspTuple::ESR_EQUAL)
		/*if (other.atoms_.size() + other.reductAtoms_.size() == me.atoms_.size() &&
			(validCert = me.checkRelation(other, false)) >= CertificateDynAspTuple::ESR_EQUAL)*/
		{
			bool valid = true;

#if !defined(VEC_ATOMS_TYPE) && !defined(INT_ATOMS_TYPE)
			//TODO: remove copies
		atom_vector trueAtoms(other.atoms_.begin(), other.atoms_.end());
		atom_vector reductFalseAtoms(other.reductAtoms_.begin(), other.reductAtoms_.end());
#else
	#ifdef INT_ATOMS_TYPE
			const atom_vector & trueAtoms =  other.atoms_ & info.int_rememberedAtoms;
			const atom_vector & reductFalseAtoms = other.reductAtoms_;
	#endif
#endif

			//TODO: remove copies
			/*rule_vector rules_v;
			for (const auto& r : rules_)
				rules_v.push_back(r);*/
			/*for (const auto& t : info.introducedAtoms)
				if (atoms_.find(t) == atoms_.end())
					atoms_bar.push_back(t);*/

			//rule_set output;

			valid &= me.checkRules(
#ifdef VEC_ATOMS_TYPE
			other.atoms_
#else
					trueAtoms
#endif
					,
#ifdef INT_ATOMS_TYPE
					~me.atoms_ & info.getAtoms(),
#else
					atoms_bar_,
#endif
#ifdef VEC_ATOMS_TYPE
				other.reductAtoms_
#else
					reductFalseAtoms
#endif
					,
#ifdef INT_ATOMS_TYPE
					me.atoms_ & info.getRules(),
#else
					me.rules_,
#endif
					info, nullptr);//&output);

			/*valid &= me.checkRules(
					other.atoms_,
					atoms_bar_,
					other.reductAtoms_,
					rules, info, nullptr); //&output);
			*/

			if (!valid)// || output != rules_)
				validCert = CertificateDynAspTuple::ESR_NONE;
			else if (write)
#ifdef VEC_CERT_TYPE
				this->certificate_pointer_set_->emplace_back(CertificateDynAspTuple::DynAspCertificatePointer(&other, validCert == CertificateDynAspTuple::ESR_INCLUDED_STRICT));
#else
				this->certificate_pointer_set_->insert(CertificateDynAspTuple::DynAspCertificatePointer(&other,
																										validCert == CertificateDynAspTuple::ESR_INCLUDED_STRICT)).
						first->updateStrict(validCert == CertificateDynAspTuple::ESR_INCLUDED_STRICT);
#endif
			/*if (valid != CertificateDynAspTuple::ESR_NONE)
				assert(output == rules_);*/
		}
		return validCert;
	/*#else
		throw std::runtime_error("NOT IMPLEMENTED!");
	#endif*/
	}



	/***********************************\
	|* PRIVATE MEMBERS                 *|
	\***********************************/
	bool RuleSetDynAspTuple::checkRules(
			const atom_vector &trueAtoms,
			const atom_vector &falseAtoms,
			const atom_vector &reductFalseAtoms,
			const atom_vector &rules,
			const TreeNodeInfo& info,
		#ifdef INT_ATOMS_TYPE	
			atom_vector* outputRules
		#else
			rule_set *outputRules
		#endif
		)
	{
		#ifdef INT_ATOMS_TYPE
		const unsigned int cnt = (unsigned int)(POP_CNT(info.getAtoms()) + POP_CNT(info.getRules()));
		for (unsigned int pos = POP_CNT(info.getAtoms()); pos < cnt; ++pos)
		{
			atom_vector rr = 1 << pos;
			if ((rr & rules) == 0)
				continue;
			/*rule_t rule = 0;
			const auto its = info.ruleAtPosition.find(info.ruleIdOfPosition(pos));
			if (its == info.ruleAtPosition.end())
				continue;
			else
				rule = its->second;*/
			rule_t rule = info.ruleIdOfPosition(pos);
			assert((rr & info.getRules()) != 0);
			/*const auto its = info.ruleAtPosition.find(info.ruleIdOfPosition(pos));
			if (its == info.ruleAtPosition.end())
				continue;
			else
				rule = its->second;*/
		#else
		for(rule_t rule : rules)
		{
		#endif
			IGroundAspRule::SatisfiabilityInfo si =
				info.instance->rule(rule).check(
						trueAtoms,
						falseAtoms,
						reductFalseAtoms, 
						#ifdef SUPPORTED_CHECK
							nullptr,
						#endif
						info);

			if(si.unsatisfied) return false;
			if(si.satisfied) continue;

			if (outputRules)
			#ifdef INT_ATOMS_TYPE
				(*outputRules) |= rr;
			#else
				outputRules->insert(rule);
			#endif
		}

		return true;
	}

#ifndef INT_ATOMS_TYPE
	bool RuleSetDynAspTuple::checkRules(
			const atom_vector &trueAtoms,
			const atom_vector &falseAtoms,
			const atom_vector &reductFalseAtoms,
			const rule_set &rules,
			const TreeNodeInfo& info,
			rule_set *outputRules
			)
	{
		for(rule_t rule : rules)
		{
			IGroundAspRule::SatisfiabilityInfo si =
				info.instance->rule(rule).check(
						trueAtoms,
						falseAtoms,
						reductFalseAtoms, 
						#ifdef SUPPORTED_CHECK
							nullptr,
						#endif
						info);

			if(si.unsatisfied) return false;
			if(si.satisfied) continue;

			if (outputRules)
				outputRules->insert(rule);
		}

		return true;
	}
#endif

	void RuleSetDynAspTuple::debug() const
	{
		#ifndef INT_ATOMS_TYPE 
		 DBG(","); 
		 DBG_COLL(rules_); 
		 #endif
	}

	bool RuleSetDynAspTuple::checkJoinRules(
			const TreeNodeInfo &info,
		#ifdef INT_ATOMS_TYPE
			const atom_vector &joinVertices,
		#else
			const rule_vector &joinVertices,
		#endif
			const atom_vector &leftTrueAtoms,
			const atom_vector &leftFalseAtoms,
			const atom_vector &leftReductFalseAtoms,
		#ifdef INT_ATOMS_TYPE
			const atom_vector &leftRules,
		#else
			const rule_set &leftRules,
		#endif
			const atom_vector &rightTrueAtoms,
			const atom_vector &rightFalseAtoms,
			const atom_vector &rightReductFalseAtoms,
		#ifdef INT_ATOMS_TYPE
			const atom_vector &rightRules,
			atom_vector &outputRules
		#else
			const rule_set &rightRules,
			rule_set &outputRules
		#endif
		)
	{
	#ifdef INT_ATOMS_TYPE
		unsigned int szx = (unsigned int)(POP_CNT(info.getAtoms()) + POP_CNT(info.getRules())); //info.introducedRules.size() + info.rememberedRules.size());
		for (unsigned int pos = POP_CNT(info.getAtoms()) + info.introducedRules.size(); pos < szx /* INT_ATOMS_TYPE*/; ++pos)
		{
			atom_vector r = 1 << pos;
			if ((r & info.int_rememberedRules) == 0)
				continue;

			rule_t rule = info.ruleIdOfPosition(pos);

			/*const auto its = info.ruleAtPosition.find(info.ruleIdOfPosition(pos));
			assert (its != info.ruleAtPosition.end());*/

			if ((leftRules & r) == 0 && (rightRules & r) == 0)
				continue;
			else if ((leftRules & r) != 0 && (rightRules & r) != 0)
			{
				outputRules |= r;
				continue;
			}

			IGroundAspRule::SatisfiabilityInfo si;
			const IGroundAspRule &ruleObject = info.instance->rule(rule);//its->second);
			DBG("joinv("); DBG(joinVertices); DBG(")");

			if ((joinVertices & r) != 0)
				continue; // if we have a join rule which was already true
			else if ((leftRules & r) != 0)
			{
			
				DBG("checking left rule "); DBG(r); DBG("("); DBG(rule); DBG(")"); DBG(" with "); DBG(rightTrueAtoms); DBG(","); DBG(rightFalseAtoms); DBG(","); DBG(rightReductFalseAtoms); DBG(std::endl);
				si = ruleObject.check(
						rightTrueAtoms,
						rightFalseAtoms,
						rightReductFalseAtoms, 
						#ifdef SUPPORTED_CHECK
							nullptr,
						#endif
						info);
			}
			else // rightIter != rightRules.end()
			{

				DBG("checking right rule "); DBG(r);  DBG("("); DBG(rule); DBG(")"); DBG(" with "); DBG(leftTrueAtoms); DBG(","); DBG(leftFalseAtoms); DBG(","); DBG(leftReductFalseAtoms); DBG(std::endl);
				si = ruleObject.check(
						leftTrueAtoms,
						leftFalseAtoms,
						leftReductFalseAtoms, 
						#ifdef SUPPORTED_CHECK
							nullptr,
						#endif
						info);
			}


			DBG("check result: "); DBG(si.satisfied); DBG(" / "); DBG(si.unsatisfied); DBG("joinatoms: "); DBG(joinVertices); DBG(std::endl);
			if(si.unsatisfied) return false;
			if(si.satisfied) continue;

			outputRules |= r;
		}

	#else


		unordered_set<rule_t> rules;
		for(rule_t rule : joinVertices)
			if(info.instance->isRule(rule)) //FIXME: do filtering outside
				rules.insert(rule);

		for(rule_t rule : info.rememberedRules)
		{
			auto leftIter = leftRules.find(rule);
			auto rightIter = rightRules.find(rule);

			if(leftIter == leftRules.end() && rightIter == rightRules.end())
				continue;

			IGroundAspRule::SatisfiabilityInfo si;
			const IGroundAspRule &ruleObject = info.instance->rule(rule);

			if(leftIter != leftRules.end() && rightIter != rightRules.end())
			{
				outputRules.insert(rule);
				continue;
			}
			else if(rules.find(rule) != rules.end())
			{
				continue; // if we have a join rule which was already true
			}
			else if(leftIter != leftRules.end())
			{
				si = ruleObject.check(
						rightTrueAtoms,
						rightFalseAtoms,
						rightReductFalseAtoms, 
						#ifdef SUPPORTED_CHECK
							nullptr,
						#endif
						info);
			}
			else // rightIter != rightRules.end()
			{
				si = ruleObject.check(
						leftTrueAtoms,
						leftFalseAtoms,
						leftReductFalseAtoms, 
						#ifdef SUPPORTED_CHECK
							nullptr,
						#endif
						info);
			}

			if(si.unsatisfied) return false;
			if(si.satisfied) continue;

			outputRules.insert(rule);
		}
	#endif
		return true;
	}
} // namespace dynasp
