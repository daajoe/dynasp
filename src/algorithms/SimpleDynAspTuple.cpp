#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif
#include "../util/debug.hpp"

#include "SimpleDynAspTuple.hpp"
#include "../../include/dynasp/IDynAspTuple.hpp"
#include <dynasp/IGroundAspInstance.hpp>
#include <dynasp/create.hpp>

#include <stack>

namespace dynasp
{
	using htd::vertex_t;
	using htd::ConstCollection;
	using htd::vertex_container;

	//using sharp::Hash;

	using std::size_t;
	using std::unordered_set;
	using std::stack;

	/*size_t SimpleDynAspTuple::DynAspCertificate::hash() const
	{
		Hash hash;

		for(atom_t atom : this->atoms)
			hash.addUnordered(atom);
		hash.incorporateUnordered();
		hash.add(this->atoms.size());

		hash.add(this->pseudo ? (size_t)1 : (size_t)0);

		return hash.get();
	}

	bool SimpleDynAspTuple::DynAspCertificate::operator==(
			const DynAspCertificate &other) const
	{
		return this->atoms == other.atoms
			&& this->pseudo == other.pseudo;
	}*/

	SimpleDynAspTuple::SimpleDynAspTuple(/*bool leaf*/)
	{
	/*#ifdef SUPPORTED_CHECK
		supp_ = ~((atom_vector)0) >> 1;
	#endif*/
		/*if (leaf)
		{
			DynAspCertificate c;
			c.pseudo = true;
			certificates_.insert(std::move(c));
		}*/
		//TODO: @GCC-BUG
		//certificates_.insert({ { }, true });
	}

	SimpleDynAspTuple::~SimpleDynAspTuple() { }

	/*bool SimpleDynAspTuple::merge(const IDynAspTuple &tuple)
	{
		const SimpleDynAspTuple &mergee = 
			static_cast<const SimpleDynAspTuple &>(tuple);

		DBG(std::endl); DBG("  merge\t");
		DBG_COLL(atoms_); DBG("=");  DBG_COLL(mergee.atoms_);
		DBG_COLL(reductAtoms_); DBG("=");  DBG_COLL(mergee.reductAtoms_);
		DBG("\t");
		//DBG_SCERT(certificates_); DBG("=");
		//DBG_SCERT(mergee.certificates_);

		if(atoms_ != mergee.atoms_) return false;
		if(reductAtoms_ != mergee.reductAtoms_) return false;
		//TODO: uncomment pseudo?
	#ifndef COMBINE_PSEUDO_PSEUDO_SOLUTIONS
		if(pseudo != mergee.pseudo) return false;
	#endif

		//TODO: if certificates are in subset relation, we can sometimes merge
		//if(certificates_ != mergee.certificates_) return false;

		if(weight_ > mergee.weight_)
		{
			solutions_ = mergee.solutions_;
			weight_ = mergee.weight_;
		}
		else if(weight_ == mergee.weight_)
		{
			solutions_ += mergee.solutions_;
		}

		DBG("\t=>\t"); DBG_COLL(atoms_); DBG(" "); //DBG_SCERT(certificates_);

		DBG("\t=>\t"); DBG_COLL(reductAtoms_); DBG(" "); //DBG_SCERT(certificates_);

		return true;
	}*/

	/*int64_t ipow(int32_t base, uint8_t exp) {
		static const uint8_t highest_bit_set[] = {
				0, 1, 2, 2, 3, 3, 3, 3,
				4, 4, 4, 4, 4, 4, 4, 4,
				5, 5, 5, 5, 5, 5, 5, 5,
				5, 5, 5, 5, 5, 5, 5, 5,
				6, 6, 6, 6, 6, 6, 6, 6,
				6, 6, 6, 6, 6, 6, 6, 6,
				6, 6, 6, 6, 6, 6, 6, 6,
				6, 6, 6, 6, 6, 6, 6, 255, // anything past 63 is a guaranteed overflow with base > 1
				255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255,
		};

		uint64_t result = 1;

		switch (highest_bit_set[exp]) {
			case 255: // we use 255 as an overflow marker and return 0 on overflow/underflow
				if (base == 1) {
					return 1;
				}

				if (base == -1) {
					return 1 - 2 * (exp & 1);
				}

				return 0;
			case 6:
				if (exp & 1) result *= base;
				exp >>= 1;
				base *= base;
			case 5:
				if (exp & 1) result *= base;
				exp >>= 1;
				base *= base;
			case 4:
				if (exp & 1) result *= base;
				exp >>= 1;
				base *= base;
			case 3:
				if (exp & 1) result *= base;
				exp >>= 1;
				base *= base;
			case 2:
				if (exp & 1) result *= base;
				exp >>= 1;
				base *= base;
			case 1:
				if (exp & 1) result *= base;
			default:
				return result;
		}
	}*/

	void SimpleDynAspTuple::introduce(
			const TreeNodeInfo &info, /*IDynAspTuple::EvaluatedTuples& tuplesDone,*/ IDynAspTuple::ComputedCertificateTuples& /*certTuplesComputed*/,
			sharp::ITupleSet &outputTuples) const
	{

		size_t numIntro = info.introducedAtoms.size();
		//TODO: note indirection via getClone();
		const SimpleDynAspTuple &me = *static_cast<const SimpleDynAspTuple*>(dynasp::create::passes() >= 2 ? this : getClone()
	/*#ifdef SEVERAL_PASSES
				this
	#else
				getClone()
	#endif*/
				);
		atom_vector trueAtoms, reductFalseAtoms, falseAtoms;
	#ifdef SUPPORTED_CHECK
		atom_vector supp = me.supp_;
	#endif
	#ifndef INT_ATOMS_TYPE
		trueAtoms.reserve(numIntro + me.atoms_.size());
		trueAtoms.insert(trueAtoms.end(), me.atoms_.begin(), me.atoms_.end());
		reductFalseAtoms.reserve(numIntro + me.atoms_.size());
		reductFalseAtoms.insert(reductFalseAtoms.end(), me.reductAtoms_.begin(), me.reductAtoms_.end());
	#else
		reductFalseAtoms = me.reductAtoms_;
		trueAtoms = me.atoms_;
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

		for(size_t subset = 0; subset < /*ipow(3, numIntro) */(1u << numIntro); ++subset)
		{
			atom_vector newTrueAtoms;
	#ifndef INT_ATOMS_TYPE
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
			//for(size_t reduct = 0; reduct < (1u << reductFalseIndices.size()); ++reduct)
	#else
			//std::cout << info.int_constrainedFactAtoms << ";" <<  (subset) << std::endl;
			if (info.int_constrainedFactAtoms & (~(subset | trueAtoms)))
				continue;
			else if (info.int_constrainedNFactAtoms & (subset | trueAtoms))
				continue;
			newTrueAtoms = subset;
			trueAtoms |= subset;
			unsigned int sz = numIntro - POP_CNT(newTrueAtoms);
		/*#ifdef SUPPORTED_CHECK
			supp &= trueAtoms;
		#endif*/
	#endif
			for(size_t reduct = 0; reduct < (1u << sz); ++reduct)
			{
	//#ifdef THREE_PASSES
	//#ifdef NO_COMPUTE
				if ((dynasp::create::passes() == 1 || dynasp::create::passes() >= 3) && reduct > 0)
					break;
	//#endif
	//#endif
				atom_vector newFalseAtoms
			#ifdef INT_ATOMS_TYPE	
				= 0
			#endif	
				;
	#ifndef INT_ATOMS_TYPE
				newFalseAtoms.reserve(numIntro);
				for (size_t pos = 0; pos < sz; ++pos)
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

				DBG("\ttuple "); DBG("t"); DBG_COLL(trueAtoms); DBG(" f"); DBG_COLL(falseAtoms);  DBG(" r"); DBG_COLL(reductFalseAtoms);

				bool validTuple = checkRules(
						trueAtoms,
						falseAtoms,
						reductFalseAtoms,
					#ifdef SUPPORTED_CHECK
						&(supp |= (reductFalseAtoms | falseAtoms)),
					#endif
						info.introducedRules, info);

				//TODO: use whole rules

				/*validTuple &= checkRules(
						trueAtoms,
						falseAtoms,
						reductFalseAtoms,
						info.rememberedRules);*/

				DBG("\t"); DBG(validTuple ? "yes" : "no"); DBG(" ");

				if(validTuple)
				{
					//TODO: pevent COPY, use move semantics!
					SimpleDynAspTuple *newTuple = new SimpleDynAspTuple(/*false*/);
					newTuple->insertPtrs(*this);

				#ifdef INT_ATOMS_TYPE
					newTuple->reductAtoms_ = reductFalseAtoms;
					newTuple->atoms_ = trueAtoms;
					#ifdef SUPPORTED_CHECK
						newTuple->supp_ = supp;
					#endif
				#else
					newTuple->reductAtoms_.insert(reductFalseAtoms.begin(), reductFalseAtoms.end());

					newTuple->atoms_.insert(
							trueAtoms.begin(),
							trueAtoms.end());
				#endif

					//newTuple->introWeight_ = info.instance->weight(newTrueAtoms, newFalseAtoms, info);

					newTuple->weight_ = weight_ + /*newTuple->introWeight_ */info.instance->weight(
							newTrueAtoms,
							newFalseAtoms, info);


	#ifndef USE_PSEUDO
					//TODO: remove!
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
					//pseudo solutions have 0 solutions, shall be removed later...
				#ifdef COMBINE_PSEUDO_PSEUDO_SOLUTIONS
					newTuple->solutions_ =
					#ifdef INT_ATOMS_TYPE
							reductFalseAtoms != 0
						#else
							reductFalseAtoms.size() > 0
						#endif
					? 0 : solutions_ * info.instance->cost(newTrueAtoms, newFalseAtoms, info);
				#else
					newTuple->solutions_ = solutions_ * info.instance->cost(newTrueAtoms, newFalseAtoms, info);
					//std::cout << "c:" << newTuple->solutions_ << std::endl;
				#endif
					DBG(newTuple->isPseudo() ? "yes" : "no");
					DBG("\t");
					DBG(newTuple);
					DBG("\t");
					DBG(newTuple->weight_);
					DBG(" ");
					DBG(newTuple->solutions_);

					// introduce atoms into certificates
					//atom_vector certTrueAtoms;
					/*for(const DynAspCertificate &cert : certificates_)
					{
						for(atom_t atom : atoms_)
							if(cert.atoms.find(atom) == cert.atoms.end())
								reductFalseAtoms.push_back(atom);
							else
								certTrueAtoms.push_back(atom);

						size_t certIntro = newTrueAtoms.size();
						for(size_t certSubset = 0;
								certSubset < (1u << certIntro);
								++certSubset)
						{
							for(size_t bit = 0; bit < certIntro; ++bit)
								if((certSubset >> bit) & 1)
									certTrueAtoms.push_back(newTrueAtoms[bit]);
								else
									reductFalseAtoms.push_back(newTrueAtoms[bit]);

							DynAspCertificate newCert;

							// check rules
							bool validCert = checkRules(
									certTrueAtoms,
									falseAtoms,
									reductFalseAtoms,
									info.introducedRules);

							validCert &= checkRules(
									certTrueAtoms,
									falseAtoms,
									reductFalseAtoms,
									info.rememberedRules);

							if(validCert)
							{
								newCert.atoms.insert(
										certTrueAtoms.begin(),
										certTrueAtoms.end());

								newCert.same = cert.same
									&& certSubset == (1u << certIntro) - 1u;

								newTuple->certificates_.insert(std::move(newCert));
							}

							// cleanup
							for(size_t bit = 0; bit < certIntro; ++bit)
								if((certSubset >> bit) & 1)
									certTrueAtoms.pop_back();
								else
									reductFalseAtoms.pop_back();
						}

						// cleanup
						reductFalseAtoms.clear();
						certTrueAtoms.clear();
					}*/

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
					/*	newFalseAtoms.pop_back();
					}*/
				#else
					reductFalseAtoms = me.reductAtoms_;
					falseAtoms = ~(me.atoms_ | me.reductAtoms_) & info.int_rememberedAtoms;
					
					#ifdef SUPPORTED_CHECK
						supp = me.supp_;
					#endif
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
				trueAtoms = me.atoms_;
			#endif

		}
		//TODO: find more efficient
	}


	void SimpleDynAspTuple::introduceFurther(
			const TreeNodeInfo &info,
			sharp::ITupleSet &outputTuples) const
	{

		size_t numIntro = info.introducedAtoms.size();
		//TODO: note indirection via getClone();

		assert(getClone() == this);
		const SimpleDynAspTuple &me = *this; /*static_cast<const SimpleDynAspTuple*>(
	#ifdef SEVERAL_PASSES
				this
	#else
				getClone()
	#endif
				);*/

		atom_vector trueAtoms, reductFalseAtoms, falseAtoms;
		#ifdef SUPPORTED_CHECK
			atom_vector supp = me.supp_;
		#endif

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
		trueAtoms = me.atoms_;
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
			//const SimpleDynAspTuple& st = static_cast<const SimpleDynAspTuple&>(t);
			//newTrueAtoms.reserve(numIntro);
		#ifndef INT_ATOMS_TYPE
			atom_vector /*newTrueAtoms,*/  reductFalseIndices;
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
			//newTrueAtoms = subset;
			trueAtoms |= subset;
			unsigned int sz = numIntro - POP_CNT(atom_vector(subset));
		#endif

			for(size_t reduct = 0; reduct < (1u << sz); ++reduct)
			{
				if (!me.isPseudo() &&
				
				#ifndef NOT_MERGE_PSEUDO
					reductAtoms_ == 0 &&
					//&& evolution_.size() != 0 
				#endif	

				reduct == 0)
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
			#endif
				/*#ifdef SUPPORTED_CHECK
					supp |= (reductFalseAtoms | falseAtoms);
				#endif*/
				bool validTuple = checkRules(
						trueAtoms,
						falseAtoms,
						reductFalseAtoms,
					#ifdef SUPPORTED_CHECK
						&(supp |= (reductFalseAtoms | falseAtoms)),
					#endif
						info.introducedRules, info);

				//TODO: use whole rules


				//DBG("\t"); DBG(validTuple ? "yes" : "no"); DBG(" ");

				if(validTuple)
				{

					DBG("\ttuple "); DBG("t"); DBG_COLL(trueAtoms); DBG(" f"); DBG_COLL(falseAtoms);  DBG(" r"); DBG_COLL(reductFalseAtoms);
					//TODO: pevent COPY, use move semantics!
					SimpleDynAspTuple *newTuple = new SimpleDynAspTuple();
					newTuple->insertPtrs(*this);
				#ifndef INT_ATOMS_TYPE
					newTuple->reductAtoms_.insert(reductFalseAtoms.begin(), reductFalseAtoms.end());

					newTuple->atoms_.insert(
							trueAtoms.begin(),
							trueAtoms.end());
				#else
					newTuple->reductAtoms_ = reductFalseAtoms;
					newTuple->atoms_ = trueAtoms;
					#ifdef SUPPORTED_CHECK
						newTuple->supp_ = supp;
					#endif
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

					#ifdef NOT_MERGE_PSEUDO
						newTuple->reductAtoms_ |= (1 << (INT_ATOMS_TYPE - 1));
					#else
						newTuple->solutions_ = 0;
						newTuple->weight_ = (std::size_t)(-1);
					#endif

				#endif
					//pseudo solutions have 0 solutions, shall be removed later...
				#ifdef COMBINE_PSEUDO_PSEUDO_SOLUTIONS
					newTuple->solutions_ = 0;
				#endif
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
				#ifdef SUPPORTED_CHECK
					supp = me.supp_; //(reductFalseAtoms | falseAtoms)),
				#endif

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
			trueAtoms = me.atoms_;
		#endif
				//else falseAtoms.pop_back();
		}
	}



	/*void SimpleDynAspTuple::introduceFurtherCompressed2(
			const TreeNodeInfo &info, IDynAspTuple::EvaluatedTuples& tuplesDone, IDynAspTuple::ComputedCertificateTuples& certTuplesComputed,
			sharp::ITupleSet &outputTuples) const
	{


		DBG("INSERTING "); DBG(pseudo); DBG(","); DBG(atoms_); DBG(","); DBG(reductAtoms_); DBG(std::endl);
		//assert(!pseudo);

		size_t numIntro = info.introducedAtoms.size();
		SimpleDynAspTuple *newTuple = nullptr;
		assert(getClone() == this);

			//TODO: note indirection via getClone();
			const SimpleDynAspTuple &me = *this;
			atom_vector trueAtoms, reductFalseAtoms, falseAtoms;
			trueAtoms.reserve(numIntro + me.atoms_.size());
			trueAtoms.insert(trueAtoms.end(), me.atoms_.begin(), me.atoms_.end());
			reductFalseAtoms.reserve(numIntro + me.atoms_.size());
			reductFalseAtoms.insert(reductFalseAtoms.end(), me.reductAtoms_.begin(), me.reductAtoms_.end());

			//TODO: find more efficient
			for(const atom_t atom : info.rememberedAtoms)
				if(me.atoms_.find(atom) == me.atoms_.end() && me.reductAtoms_.find(atom) == me.reductAtoms_.end())
					falseAtoms.push_back(atom);




		//FIXME: take into account assignment knowledge of info.instance.
		//FIXME: is code below better with stack-based subset enumeration?
		//FIXME: code below only works if max number of subsets fits into size_t
		//TODO: make this loop correct

		//assert(evolution_.size());// || isPseudo());

		//TODO: make this part better



		assert(evolution_.size());
		for (auto* ptr : evolution_)
		{
			assert(!ptr->isPseudo());
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
			IDynAspTuple::EvaluatedTupleResult result;

			DBG("VISITING "); DBG(ptr->atoms_); DBG(","); DBG(ptr->reductAtoms_); DBG(std::endl);
		//for (const auto& t : outputTuples)
		//{
		//for(size_t subset = 0; subset < (1u << numIntro); ++subset)
		//{
			//const SimpleDynAspTuple& st = static_cast<const SimpleDynAspTuple&>(t);
			atom_vector   reductFalseIndices;
			//newFalseAtoms.reserve(numIntro);
			reductFalseIndices.reserve(numIntro);
			for (size_t bit = 0; bit < numIntro; ++bit)
				//if ((subset >> bit) & 1)
				if (ptr->atoms_.find(info.introducedAtoms[bit]) != ptr->atoms_.end())
					reductFalseIndices.push_back(bit);
				else
					falseAtoms.push_back(info.introducedAtoms[bit]);
					//newFalseAtoms.push_back(info.introducedAtoms[bit]);

			for(size_t reduct = 0; reduct < (1u << reductFalseIndices.size()); ++reduct)
			{
				if (!isPseudo() && reduct == 0)
					continue;
				for (size_t pos = 0; pos < reductFalseIndices.size(); ++pos)
					if ((reduct >> pos) & 1)
						reductFalseAtoms.push_back(info.introducedAtoms[reductFalseIndices[pos]]);
					else
						trueAtoms.push_back(info.introducedAtoms[reductFalseIndices[pos]]);
						//newTrueAtoms.push_back(info.introducedAtoms[reductFalseIndices[pos]]);


				//TODO: prevent copy somehow?
				if (!newTuple)
					newTuple = new SimpleDynAspTuple();

				newTuple->reductAtoms_.insert(reductFalseAtoms.begin(), reductFalseAtoms.end());
				newTuple->atoms_.insert(
							trueAtoms.begin(),
							trueAtoms.end());

				std::pair<IDynAspTuple::ComputedCertificateTuples::iterator, bool> found = certTuplesComputed.insert(newTuple);
				bool del = false;
				if (found.second)
				{
					bool validTuple = checkRules(
							trueAtoms,
							falseAtoms,
							reductFalseAtoms,
							info.introducedRules);

					//TODO: use whole rules



					if (validTuple && (newTuple->atoms_.size() + newTuple->reductAtoms_.size() == ptr->atoms_.size() &&
							ptr->checkRelation(*newTuple, false) >= CertificateDynAspTuple::ESR_EQUAL))
					{

						DBG("\ttuple "); DBG("t"); DBG_COLL(trueAtoms); DBG(" f"); DBG_COLL(falseAtoms);  DBG(" r"); DBG_COLL(reductFalseAtoms);
						//TODO: pevent COPY, use move semantics!
						//SimpleDynAspTuple *newTuple = new SimpleDynAspTuple();
						newTuple->insertPtrs(*this);

						//newTuple->reductAtoms_.insert(reductFalseAtoms.begin(), reductFalseAtoms.end());

						//do not forget fake evolution here..
						newTuple->evolution_ = ptr->evolution_; //.push_back(ptr);

						newTuple->pseudo = true;

						//pseudo solutions have 0 solutions, shall be removed later...
						DBG(newTuple->isPseudo() ? "yes" : "no");
						DBG("\t");
						DBG(newTuple);
						DBG("\t");


						DBG(std::endl);
						//DBG("\t\t");
						//DBG_SCERT(newTuple->certificates_);

						outputTuples.insert(newTuple);
						//certTuplesComputed.insert(newTuple);
						if (!isPseudo())
							result.push_back(newTuple);

						newTuple = nullptr;
					}
					else
					{
						DBG("\twrong "); DBG("t"); DBG_COLL(trueAtoms); DBG(" f"); DBG_COLL(falseAtoms);  DBG(" r"); DBG_COLL(reductFalseAtoms); DBG(std::endl);
						certTuplesComputed.erase(found.first);
						del = true;
					}
				}
				else if (static_cast<CertificateDynAspTuple*>(*found.first)->atoms_.size() + static_cast<CertificateDynAspTuple*>(*found.first)->reductAtoms_.size() == ptr->atoms_.size() &&
							ptr->checkRelation(*static_cast<CertificateDynAspTuple*>(*found.first), false) >= CertificateDynAspTuple::ESR_EQUAL)
				{
					DBG("\talready there "); DBG("t"); DBG_COLL(trueAtoms); DBG(" f"); DBG_COLL(falseAtoms);  DBG(" r"); DBG_COLL(reductFalseAtoms); DBG(std::endl);
					insertMerged(origins_, static_cast<CertificateDynAspTuple*>(*found.first)->origins_);
					insertMerged(ptr->evolution_, static_cast<CertificateDynAspTuple*>(*found.first)->evolution_);
					//static_cast<CertificateDynAspTuple*>(*found.first)->insertPtrs(*this);
					if (!isPseudo())
						result.push_back(static_cast<CertificateDynAspTuple*>(*found.first));
					del = true;
				}
				if (del)
				{
					newTuple->reductAtoms_.clear();
					newTuple->atoms_.clear();
				}

				for (size_t pos = 0; pos < reductFalseIndices.size(); ++pos)
					if ((reduct >> pos) & 1)
						reductFalseAtoms.pop_back();
					else
					//{
						trueAtoms.pop_back();
			}

			DBG(std::endl);

			// reset to state before big loop
			//FIXME: can this be optimized (i.e. no loop again at end)?
			//newFalseAtoms.clear();
			//newTrueAtoms.clear();
			for (size_t bit = 0; bit < numIntro; ++bit)
				if (ptr->atoms_.find(info.introducedAtoms[bit]) == ptr->atoms_.end())
					falseAtoms.pop_back();
				//else falseAtoms.pop_back();

			if (!isPseudo())
				tuplesDone.emplace(ptr, std::move(result));
		}
		//TODO: schirches const zeugs
		if (isPseudo())
			const_cast<SimpleDynAspTuple*>(this)->evolution_.clear();
	}*/


//#define NOT_MERGE_PSEUDO
	void SimpleDynAspTuple::introduceFurtherCompressed(
			const TreeNodeInfo &info, /*IDynAspTuple::EvaluatedTuples& tuplesDone,*/ IDynAspTuple::ComputedCertificateTuples& certTuplesComputed,
			sharp::ITupleSet &outputTuples, unsigned int maxsize) const
	{

		/*introduceFurther(info, outputTuples);
		return;*/

		DBG("INSERTING "); DBG(isPseudo()); DBG(","); DBG_COLL(atoms_); DBG(","); DBG_COLL(reductAtoms_); DBG(std::endl);
		//assert(!pseudo);

		size_t numIntro = info.introducedAtoms.size();
		SimpleDynAspTuple *newTuple = nullptr;
		assert(getClone() == this);

			//TODO: note indirection via getClone();
			const SimpleDynAspTuple &me = *this;
			atom_vector trueAtoms, reductFalseAtoms, falseAtoms;
		/*#ifdef SUPPORTED_CHECK
			atom_vector supp = me.supp_;
		#endif*/

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
			trueAtoms = me.atoms_;
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
		//unsigned int sz = outputTuples.size();
		for (unsigned int out = 0; out < maxsize /*outputTuples.size()*/; ++out)
		{
			auto *ptr = static_cast<CertificateDynAspTuple *>(outputTuples[out]);
		#ifndef COMBINE_PSEUDO_PSEUDO_SOLUTIONS
			if (ptr->isPseudo()
		#ifdef NOT_MERGE_PSEUDO
				|| ptr->reductAtoms_
		#endif
			)
				break;
		#endif
			if (ptr->solutionCount() == 0
		#ifdef COMBINE_PSEUDO_PSEUDO_SOLUTIONS
			#ifndef YES_BUT_NO_PSEUDO
				&& !ptr->isPseudoFlag()
				#endif
				#endif
			)
				continue;

		#ifdef INT_ATOMS_TYPE
			assert(ptr->reductAtoms_ == 0);
			if (const_cast<SimpleDynAspTuple*>(this)->checkRelationExt(*this, ((ptr->atoms_) & info.int_rememberedAtoms), ((me.atoms_ | me.reductAtoms_) & info.int_rememberedAtoms), false) == CertificateDynAspTuple::ESR_NONE)
				continue;
			if (((me.atoms_ | me.reductAtoms_) & info.int_negatedAtoms & info.int_rememberedAtoms) != (ptr->atoms_ & info.int_rememberedAtoms & info.int_negatedAtoms))
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
			//newFalseAtoms.reserve(numIntro);
		#ifndef INT_ATOMS_TYPE
			atom_vector /*newFalseAtoms,*/  reductFalseIndices;
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
			falseAtoms |= info.int_introducedAtoms & ~ptr->atoms_; //(numIntro  ~subset);
		#endif

			for(size_t reduct = 0; reduct < (1u << sz); ++reduct)
			{
			#ifdef COMBINE_PSEUDO_PSEUDO_SOLUTIONS
				if (!isPseudoFlag() && reduct == 0)
			#else
				//std::cout << atoms_ << "," << reductAtoms_ << std::endl;
				assert(evolution_.size() == 0);
				if ((!isPseudo() 
				#ifndef NOT_MERGE_PSEUDO
					&& reductAtoms_ == 0
					//&& evolution_.size() != 0 
				#endif	
				) && reduct == 0)
			#endif
					continue;
				/*atom_vector newTrueAtoms;
				newTrueAtoms.reserve(numIntro);*/

				//TOOD: make faster for integer stuff
#ifndef INT_ATOMS_TYPE

#ifdef USE_REDUCT_SPEEDUP
				bool addedReduct = false;
				for (size_t pos = 0; pos < sz; ++pos)
					if ((reduct >> pos) & 1)
					{
						if (info.instance->isNegatedAtom(info.introducedAtoms[reductFalseIndices[pos]]))
						{
							addedReduct = true;
							break;
						}
						/*		else

								falseAtoms.push_back(info.introducedAtoms[reductFalseIndices[pos]]);*/
					}
				//else
				//trueAtoms.push_back(info.introducedAtoms[reductFalseIndices[pos]]);
#endif
#else
#ifdef USE_REDUCT_SPEEDUP
				bool addedReduct = false;
				atom_vector ftmp = falseAtoms,
					ttmp = trueAtoms,
					rftmp = reductFalseAtoms;
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
			#ifdef COMBINE_PSEUDO_PSEUDO_SOLUTIONS
				if (!isPseudoFlag() && !addedReduct)
			#else
				if ((!isPseudo()  	
				#ifndef NOT_MERGE_PSEUDO
					&& reductAtoms_ == 0
					//&& evolution_.size() != 0 
				#endif	
				&& !addedReduct) || (subset & reduct & info.int_negatedChoiceAtoms))// || POP_CNT(reductFalseAtoms) > 1) //|| POP_CNT(falseAtoms & info.int_negatedAtoms & ptr->atoms_) != 0)// !reductFalseAtoms.size())
			#endif
				{
#ifdef INT_ATOMS_TYPE
					falseAtoms = ftmp;
					trueAtoms = ttmp;
					reductFalseAtoms = rftmp;
#endif
					/*bool val = checkRules(
								trueAtoms,
								falseAtoms,
								reductFalseAtoms,
								info.introducedRules);

					std::cout <<" VALID: " << val << std::endl;
					assert(reductFalseAtoms.size() == 0);
					bool found = false;
						for (unsigned int o = 0; o < outputTuples.size(); ++o)
						{
							auto *pt = static_cast<CertificateDynAspTuple *>(outputTuples[o]);
							if (pt->isPseudo())
								break;
							assert(pt->reductAtoms_.size() == 0);

							std::cout << trueAtoms << " vs " << pt->atoms_ << std::endl;
							if (trueAtoms == pt->atoms_)
							{
							found = true;
							break;
							}
						}

						for (size_t pos = 0; pos < reductFalseIndices.size(); ++pos)
						if ((reduct >> pos) & 1) {
						falseAtoms.pop_back();
								}
						else
							trueAtoms.pop_back();
						assert(found);*/
					continue;
				}
				/*else
				{
					for (size_t pos = 0; pos < reductFalseIndices.size(); ++pos)
					if ((reduct >> pos) & 1) {

					falseAtoms.pop_back();
							}
					else
						trueAtoms.pop_back();

				}*/
#endif
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
					newTuple = new SimpleDynAspTuple();

			#ifdef INT_ATOMS_TYPE
				newTuple->reductAtoms_ = reductFalseAtoms;
				newTuple->atoms_ = trueAtoms;
				
				#ifdef SUPPORTED_CHECK
					newTuple->supp_ = (me.supp_ | reductFalseAtoms | falseAtoms);
				#endif

			#else
				newTuple->mergeHashValue =0;
				newTuple->reductAtoms_.clear();
				newTuple->atoms_.clear();
				newTuple->reductAtoms_.insert(reductFalseAtoms.begin(), reductFalseAtoms.end());
				newTuple->atoms_.insert(
							trueAtoms.begin(),
							trueAtoms.end());
				//newTuple->pseudo = true;
			#endif
#ifdef USE_PSEUDO
			#ifndef YES_BUT_NO_PSEUDO
				newTuple->pseudo = true;
			#endif
#else
			
			#ifdef NOT_MERGE_PSEUDO
				newTuple->reductAtoms_ |= (1 << (INT_ATOMS_TYPE - 1));
			#else
				newTuple->solutions_ = 0;
				newTuple->weight_ = (std::size_t)(-1);
			#endif
#endif
				#ifdef COMBINE_PSEUDO_PSEUDO_SOLUTIONS
					newTuple->solutions_ = 0;
				#endif
				std::pair<IDynAspTuple::ComputedCertificateTuples::iterator, bool> found = certTuplesComputed.insert(newTuple);
				if (found.second)
				{
					bool validTuple = checkRules(
							trueAtoms,
							falseAtoms,
							reductFalseAtoms, 
						#ifdef SUPPORTED_CHECK
							&newTuple->supp_,
						#endif
							info.introducedRules, info);

					//TODO: use whole rules


					/*DBG("\t");
					DBG(validTuple ? "yes" : "no");
					DBG(" ");*/

					/*if (trueAtoms == 2 && falseAtoms == 25 && reductFalseAtoms == 4)
					{
						std::cout << "INSIDE " << std::endl;
						for (const auto *p : certTuplesComputed)
							std::cout << p << std::endl;
					}*/

					//DBG_COLL(certTuplesComputed);

					if (validTuple && ptr->checkRelationExt(*newTuple, info) //(newTuple->atoms_.size() + newTuple->reductAtoms_.size() == ptr->atoms_.size() &&
							/*ptr->checkRelation(*newTuple, false)*/ >= CertificateDynAspTuple::ESR_EQUAL)//)
					{
						DBG("isPseudo "); DBG(isPseudo());
						DBG("\ttuple "); DBG("t"); DBG_COLL(trueAtoms); DBG(" f"); DBG_COLL(falseAtoms);  DBG(" r"); DBG_COLL(reductFalseAtoms);
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
					/*#ifdef USE_PSEUDO
						newTuple->pseudo = true;
					#else
						newTuple->reductAtoms_ |= (1 << (INT_ATOMS_TYPE - 1));
					#endif*/
					#ifdef NOT_MERGE_PSEUDO
						assert(newTuple->isPseudo());
					#endif
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
						DBG("NEW TUPLE COMPR ");  DBG_COLL(newTuple->atoms_); DBG(","); DBG_COLL(newTuple->reductAtoms_); DBG(", "); DBG(newTuple->solutions_); DBG(" @ "); DBG(newTuple->weight_); DBG(std::endl);
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
					DBG("\talready there "); DBG("t"); DBG_COLL(trueAtoms); DBG(" f"); DBG_COLL(falseAtoms);  DBG(" r"); DBG_COLL(reductFalseAtoms); DBG(std::endl);
				#ifdef EXTENSION_POINTERS_SET_TYPE

					insertSet(origins_, static_cast<CertificateDynAspTuple*>(*found.first)->origins_);
				#else
				#ifdef COMBINE_PSEUDO_PSEUDO_SOLUTIONS
					if (isPseudo() || (isPseudoFlag() && (reductAtoms_ & info.getAtoms())))
				#endif
					static_cast<CertificateDynAspTuple*>(*found.first)->origins_.insert(static_cast<CertificateDynAspTuple*>(*found.first)->origins_.end(), origins_.begin(), origins_.end());
					//insertMerged(origins_, static_cast<CertificateDynAspTuple*>(*found.first)->origins_);
				#endif
					//insertMerged(ptr->evolution_, static_cast<CertificateDynAspTuple*>(*found.first)->evolution_);
					//static_cast<CertificateDynAspTuple*>(*found.first)->insertPtrs(*this);
					/*if (!isPseudo())
						result.push_back(static_cast<CertificateDynAspTuple*>(*found.first));*/

		#ifdef COMBINE_PSEUDO_PSEUDO_SOLUTIONS
	#ifdef USE_PSEUDO
				#ifndef YES_BUT_NO_PSEUDO
					static_cast<CertificateDynAspTuple*>(*found.first)->pseudo = true;
				#endif
	#else
				#ifdef NOT_MERGE_PSEUDO
					static_cast<CertificateDynAspTuple*>(*found.first)->reductAtoms_ |= (1 << (INT_ATOMS_TYPE - 1));
				#endif
	#endif
					//static_cast<CertificateDynAspTuple*>(*found.first)->solutions_ = 0;
		#endif

				}
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
				trueAtoms = me.atoms_;
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
		//TODO: schirches const zeugs
		/*if (isPseudo())
			const_cast<SimpleDynAspTuple*>(this)->evolution_.clear();*/
	}



	//TODO: mind the indirect via getClone()
	/*IDynAspTuple *SimpleDynAspTuple::project(const TreeNodeInfo &info) const
	{
		SimpleDynAspTuple *newTuple = new SimpleDynAspTuple();
		const SimpleDynAspTuple &me = *static_cast<const SimpleDynAspTuple*>(getClone());

		// only keep remembered atoms
		for(atom_t atom : info.rememberedAtoms)
		{
			if(me.atoms_.find(atom) != me.atoms_.end())
				newTuple->atoms_.insert(atom);
			if(me.reductAtoms_.find(atom) != me.reductAtoms_.end())
				newTuple->reductAtoms_.insert(atom);
		}
		
		// keep weight and solution count
		newTuple->weight_ = weight_;
		newTuple->solutions_ = solutions_;
		newTuple->pseudo = pseudo;*/
		
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

		//return newTuple;
	//}

	IDynAspTuple *SimpleDynAspTuple::join(
			const TreeNodeInfo &info,
			const atom_vector &,
			const atom_vector &joinVertices,
			const IDynAspTuple &tuple,
			const atom_vector&) const
	{
			const SimpleDynAspTuple &other =
			static_cast<const SimpleDynAspTuple &>(tuple);
	
DBG(std::endl); DBG("  join "); DBG(atoms_ != other.atoms_ || reductAtoms_ != other.reductAtoms_); DBG(" ");
		DBG_COLL(joinVertices); DBG("\t");
		DBG_COLL(atoms_); DBG("x"); DBG_COLL(other.atoms_); DBG(" ");
		DBG_COLL(reductAtoms_); DBG("x"); DBG_COLL(other.reductAtoms_);
		DBG("\t");

	#ifndef USE_LAME_BASE_JOIN
		assert(false);
	#endif

#ifdef INT_ATOMS_TYPE
	#ifdef SUPPORTED_CHECK
		atom_vector supp = supp_ | other.supp_;
	#endif
	//#ifdef USE_LAME_JOIN
			if ((atoms_ & joinVertices) == (other.atoms_ & joinVertices) &&
				(reductAtoms_ & joinVertices) == (other.reductAtoms_ & joinVertices))
			{
				if (!checkRules(//*info.instance,
							atoms_ & joinVertices,
							//TODO: maybe projection here not needed
							//(info.int_introducedAtoms | info.int_rememberedAtoms) &
							~atoms_ & joinVertices,
							reductAtoms_ & joinVertices,
						#ifdef SUPPORTED_CHECK
							&supp,
						#endif
							info.rememberedRules, info))
				{
					//delete newTuple;
					return nullptr;
				}
			}
			else
				return nullptr;
	/*#else
			if ((atoms_) == (other.atoms_) &&
				(reductAtoms_) == (other.reductAtoms_))
			{
				if (!checkRules(// *info.instance,
							atoms_,
							//TODO: maybe projection here not needed
							//(info.int_introducedAtoms | info.int_rememberedAtoms) &
							~atoms_ & info.getAtoms(),
							reductAtoms_,
							info.rememberedRules, info))
				{
					//delete newTuple;
					return nullptr;
				}
			}
			else
				return nullptr;
	#endif*/
#else

				//DBG_SCERT(certificates_); DBG("x");
		//DBG_SCERT(other.certificates_);

		atom_vector trueAtoms, falseAtoms, reductFalseAtoms;
		falseAtoms.reserve(joinVertices.size());
	#ifndef VEC_ATOMS_TYPE
		trueAtoms.reserve(joinVertices.size());
		reductFalseAtoms.reserve(joinVertices.size());
	#else
		bool speedMode = false;
	#endif
		//special case? does it help?
		if (atoms_ == other.atoms_ && reductAtoms_ == other.reductAtoms_)
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
				if (reductAtoms_.find(atom) != reductAtoms_.end())
					reductFalseAtoms.push_back(atom);
			#endif
			}
			// check and join rules
			if(!checkRules(

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
						info.rememberedRules, info))
			{
				//delete newTuple;
				return nullptr;
			}
		}
		else
		{
			trueAtoms.reserve(joinVertices.size());
			reductFalseAtoms.reserve(joinVertices.size());

			// check that atom assignment matches on the join atoms
			for (atom_t atom : joinVertices)
			{
				if (!info.instance->isAtom(atom)) continue;
				if (atoms_.find(atom) == atoms_.end())
				{
					if (other.atoms_.find(atom) != other.atoms_.end())
					{
						//delete newTuple;
						return nullptr;
					}
					falseAtoms.push_back(atom);
				}
				else
				{
					if (other.atoms_.find(atom) == other.atoms_.end())
					{
						//delete newTuple;
						return nullptr;
					}
					trueAtoms.push_back(atom);
				}
				if (reductAtoms_.find(atom) == reductAtoms_.end())
				{
					if (other.reductAtoms_.find(atom) != other.reductAtoms_.end())
					{
						//delete newTuple;
						return nullptr;
					}
				}
				else
				{
					if (other.reductAtoms_.find(atom) == other.reductAtoms_.end())
					{
						//delete newTuple;
						return nullptr;
					}
					reductFalseAtoms.push_back(atom);
				}
			}
			// check and join rules
			if(!checkRules(
						trueAtoms,
						falseAtoms,
						reductFalseAtoms,
						info.rememberedRules, info))
			{
				//delete newTuple;
				return nullptr;
			}
		}
		/*SimpleDynAspTuple* newTuple = new SimpleDynAspTuple();
		newTuple->postJoin(
				info,
				#ifdef VEC_ATOMS_TYPE
		speedMode ? atoms_ : trueAtoms,
				#else
					trueAtoms,
				#endif

				falseAtoms);*/

#endif

		SimpleDynAspTuple *newTuple = new SimpleDynAspTuple();
	#ifdef INT_ATOMS_TYPE
		newTuple->atoms_ = atoms_ | other.atoms_;
		newTuple->reductAtoms_ = reductAtoms_ | other.reductAtoms_;
	#ifdef SUPPORTED_CHECK
		newTuple->supp_ = supp;
	#endif
		/*#ifndef USE_PSEUDO
										 | (other.reductAtoms_ & (1 << (INT_ATOMS_TYPE - 1)))
		#endif*/

		;
	#else
		// join atom assignment
		newTuple->atoms_ = atoms_;
			//TODO: SORTED INSERT
			#ifdef VEC_ATOMS_TYPE
				newTuple->atoms_.insertDup(other.atoms_); //.begin(), other.atoms_.end());
			#else
				newTuple->atoms_.insert(other.atoms_.begin(), other.atoms_.end());
			#endif

				newTuple->reductAtoms_ = reductAtoms_
										 ;
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

#ifdef VEC_ATOMS_TYPE
		if (speedMode)
			newTuple->solutions_ = solutions_ * other.solutions_ / info.instance->cost(atoms_, falseAtoms, info);
		else
			newTuple->solutions_ = solutions_ * other.solutions_ / info.instance->cost(trueAtoms, falseAtoms, info);
#else
#ifndef INT_ATOMS_TYPE
		newTuple->solutions_ = solutions_ * other.solutions_ / info.instance->cost(trueAtoms, falseAtoms, info);
#else
		newTuple->solutions_ = solutions_ * other.solutions_ / info.instance->cost(atoms_ & joinVertices, ~atoms_ & joinVertices, info);
#endif
#endif

		/*newTuple->solutions_ = solutions_ * other.solutions_;*/

#ifdef VEC_ATOMS_TYPE
		if (speedMode)
			newTuple->weight_ = weight_ + other.weight_ - info.instance->weight(atoms_, falseAtoms, info);
		else
			newTuple->weight_ = weight_ + other.weight_ - info.instance->weight(trueAtoms, falseAtoms, info);
#else
#ifndef INT_ATOMS_TYPE
		newTuple->weight_ = weight_ + other.weight_ - info.instance->weight(trueAtoms, falseAtoms, info);
#else
		newTuple->weight_ = weight_ + other.weight_ - info.instance->weight(atoms_ & joinVertices, ~atoms_ & joinVertices, info);
#endif
#endif
		// join certificates
		//TODO: use something more intelligent than a nested loop join
		/*atom_vector certTrueAtoms, certFalseAtoms;
		for(const DynAspCertificate &cert1 : certificates_)
		for(const DynAspCertificate &cert2 : other.certificates_)
		{
			// check if join condition is fulfilled
			bool skip = false;
			for(atom_t atom : joinVertices)
				if(!info.instance->isAtom(atom)) continue;
				else if(cert1.atoms.find(atom) == cert1.atoms.end())
				{
					if(cert2.atoms.find(atom) != cert2.atoms.end())
					{
						skip = true;
						break;
					}
					certFalseAtoms.push_back(atom);
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

			DynAspCertificate newCert;

			// join and check rules
			if(!checkRules(
						certTrueAtoms,
						certFalseAtoms,
						atom_vector(0),
						info.rememberedRules))
			{
				continue;
			}

			newCert.atoms.insert(cert1.atoms.begin(), cert1.atoms.end());
			newCert.atoms.insert(cert2.atoms.begin(), cert2.atoms.end());
			newCert.same = cert1.same && cert2.same;

			newTuple->certificates_.insert(std::move(newCert));

			// cleanup
			certTrueAtoms.clear();
			certFalseAtoms.clear();
		}*/

		DBG("\t=>\t"); DBG_COLL(newTuple->atoms_); DBG(" ");
		DBG("\t=>\t"); DBG_COLL(newTuple->reductAtoms_); DBG(" ");
		//DBG_SCERT(newTuple->certificates_);

		return newTuple;
	}

//#ifdef INT_ATOMS_TYPE
//	CertificateDynAspTuple::ESubsetRelation SimpleDynAspTuple::checkRules(const atom_vector & /*rules*/, const CertificateDynAspTuple & /*other*/, const TreeNodeInfo& /*info*/, bool /*write*/)
//	{
//		throw std::runtime_error("NOT IMPLEMENTED!");
//	}
//#endif

	//compare with ::insert! (esp. 2nd level / certificates)
	CertificateDynAspTuple::ESubsetRelation SimpleDynAspTuple::checkRules(const rule_vector & rules, const CertificateDynAspTuple & other, const TreeNodeInfo& info, bool write)
	{

		if (dynasp::create::passes() < 4)
			assert(getClone() == this);
		assert(other.getClone() == &other);

		

		//enforce here a partition of true atoms (1st layer) into trueAtoms (2nd layer) and falseReductAtoms (2nd layer, but true on 1st layer)
		//furthermore, falseAtoms keeps the same, i.e.\ \bar{atoms_}
		// check rules

		CertificateDynAspTuple::ESubsetRelation validCert = CertificateDynAspTuple::ESR_NONE;
		//TODO: nasty
		if ((validCert = checkRelationExt(other, info)) /*other.atoms_.size() + other.reductAtoms_.size() == atoms_.size() &&

			(validCert = checkRelation(other, false))*/ >= CertificateDynAspTuple::ESR_EQUAL)
		{
			bool valid = true;

#if !defined(VEC_ATOMS_TYPE) && !defined(INT_ATOMS_TYPE)
			//TODO: remove copies
		atom_vector trueAtoms(other.atoms_.begin(), other.atoms_.end());
		atom_vector reductFalseAtoms(other.reductAtoms_.begin(), other.reductAtoms_.end());
#else
			const atom_vector & trueAtoms =  other.atoms_;
			const atom_vector & reductFalseAtoms = other.reductAtoms_;
#endif

			/*for (const auto& t : info.introducedAtoms)
				if (atoms_.find(t) == atoms_.end())
					atoms_bar.push_back(t);

			valid &= checkRules(
					trueAtoms,
					atoms_bar,
					reductFalseAtoms,
					info.introducedRules);*/

			valid &= checkRules(
					trueAtoms,
#ifdef INT_ATOMS_TYPE
					//info.getAtoms() & ~(trueAtoms | reductFalseAtoms),
					info.getAtoms() & ~atoms_,
#else
					atoms_bar_,
#endif
					reductFalseAtoms,
				#ifdef SUPPORTED_CHECK
					nullptr,
				#endif
					rules, info);

			if (!valid)
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
		/*if (validCert)
		{
			newCert.same = cert.same
						   && certSubset == (1u << certIntro) - 1u;
		}
		return validCert;*/
	}

	//TODO: implement clone
	CertificateDynAspTuple* SimpleDynAspTuple::clone() const
	{
		return new SimpleDynAspTuple(/*false*/);
	}

	/***********************************\
	|* PRIVATE MEMBERS                 *|
	\***********************************/

	//TODO: keep in mind those pseudo-true things
	bool SimpleDynAspTuple::checkRules(
			const atom_vector &trueAtoms,
			const atom_vector &falseAtoms,
			const atom_vector &reductFalseAtoms,
		#ifdef SUPPORTED_CHECK
			atom_vector *supp,
		#endif
			const rule_vector &rules,
			const TreeNodeInfo& info
			)
	{
		for(const auto rule : rules)
		{
			DBG("r: "); DBG(rule); DBG("; ");
			if(!info.instance->isRule(rule)) continue;

			IGroundAspRule::SatisfiabilityInfo si =
				info.instance->rule(rule).check(
						trueAtoms,
						falseAtoms,
						reductFalseAtoms, 
					#ifdef SUPPORTED_CHECK	
						supp,
					#endif
						info);

			if(si.unsatisfied) return false;
		}

		return true;
	}

#ifdef SUPPORTED_CHECK
	atom_vector SimpleDynAspTuple::checkSupportedRules(const TreeNodeInfo& info)
	{
		atom_vector supp = (~atoms_ & info.getAtoms()) | info.int_negatedChoiceAtoms;
		DBG("PREPRESUPP "); DBG(supp); DBG(std::endl);
		checkRules(atoms_, ~atoms_ & info.getAtoms() & ~reductAtoms_, reductAtoms_, &supp, info.rememberedRules, info);
		checkRules(atoms_, ~atoms_ & info.getAtoms() & ~reductAtoms_, reductAtoms_, &supp, info.introducedRules, info);
		return supp;
	}
#endif		



} // namespace dynasp
