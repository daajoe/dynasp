#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif
#include "../util/debug.hpp"

#include "GroundAspRule.hpp"
#include <dynasp/TreeNodeInfo.hpp>
#include <sstream>

namespace dynasp
{
	using std::vector;
	using std::size_t;
	using std::unordered_map;

	GroundAspRule::GroundAspRule()
		: minimumSet_(false), choiceRule_(false),
		  minimumBodyWeight_(0), maximumBodyWeight_(0)
	{ }

	GroundAspRule::~GroundAspRule() { }

	void GroundAspRule::addHeadAtom(atom_t atom)
	{
		//std::cout << atom << Atom::FALSE << std::endl;
		if(atom != Atom::FALSE) head_.insert(atom);
	}

	void GroundAspRule::updateDependencyGraph(htd::DirectedGraph& graph) const
	{
		for (const auto& h : head_)
		{
			for (const auto& b: positiveBody_)
				graph.addEdge(b.first, h);
			for (const auto& b: negativeBody_)
				graph.addEdge(b.first, h);
		}
	}
	

	bool GroundAspRule::isFact() const
	{
		return positiveBody_.size() == 0 && negativeBody_.size() == 0 && head_.size() == 1;
	}

	bool GroundAspRule::isConstrainedNFact() const
	{
		return positiveBody_.size() == 1 && negativeBody_.size() == 0 && head_.size() == 0;
	}

	bool GroundAspRule::isConstrainedFact() const
	{
		return positiveBody_.size() == 0 && negativeBody_.size() == 1 && head_.size() == 0;
	}

	void GroundAspRule::makeChoiceHead()
	{
		choiceRule_ = true;
	}

	bool GroundAspRule::isRule() const
	{
		return choiceRule_ || head_.size() > 1 || positiveBody_.size() > 0 || negativeBody_.size() > 0;
	}

	std::string GroundAspRule::toQBF(bool reduct, unsigned int id) const
	{
	//#define DBGV(X) X
	#define DBGV(X) ""
		//return std::move("");
		std::stringstream ss;
		bool fnd = false;
		for (auto v : positiveBody_)
		{
			if (fnd)
				ss << ",";	
			ss << "-" << (reduct ? DBGV("aa") : DBGV("a")) << (reduct ? id + v.first + 0 : v.first + 0);
			fnd = true;
		}
		for (auto v : head_)
		{
			if (fnd)
				ss << ",";	
			ss << (reduct ? DBGV("aa") : DBGV("a")) << (reduct ? id + v + 0 : v + 0);
			fnd = true;
		}
		for (auto v : negativeBody_)
		{
			if (fnd)
				ss << ",";	
			ss << DBGV("a") << (v.first + 0);
			fnd = true;
		}
		return std::move(ss.str());
	}

	const std::string GroundAspRule::toString() const
	{
		std::stringstream res;
		bool first = true;
		if (choiceRule_)
			res << " { ";
		for (const auto& h : head_)
		{
			if (!first)
				res << " ; ";
			res << "a" << h;
			first = false;
		}
		if (choiceRule_)
			res << " } ";

		if (isRule())
			res << " :- ";
		first = true;

		if (hasWeightedBody())
			res << minimumBodyWeight_ << " #sum { ";
		for (const auto& t : positiveBody_)
		{
			if (!first)
				res << (hasWeightedBody() ? " ; " : " , ");
			if (hasWeightedBody())
				res << t.second << ", a" << t.first <<  " : a" << t.first;// << " = " <<  t.second;
			else
				res << "a" << t.first;
			first = false;
		}
		for (const auto& t : negativeBody_)
		{
			if (!first)
				res << (hasWeightedBody() ? " ; " : " , ");
			if (hasWeightedBody())
				res << t.second << ", a" << t.first << " : not " << "a" << t.first;// << " = " <<  t.second;
			else
				res << " not " << "a" << t.first;
			first = false;
		}

		if (hasWeightedBody())
			res << " } ";
		if (!first)
			res << " , ";
		return res.str();
	}

	void GroundAspRule::addPositiveBodyAtom(atom_t atom)
	{
		this->addPositiveBodyAtom(atom, 1);
	}

	void GroundAspRule::addPositiveBodyAtom(atom_t atom, size_t weight)
	{
		positiveBody_[atom] += weight;
		maximumBodyWeight_ += weight;
		if(!minimumSet_) minimumBodyWeight_ += weight;
	}

	void GroundAspRule::addNegativeBodyAtom(atom_t atom)
	{
		this->addNegativeBodyAtom(atom, 1);
	}

	void GroundAspRule::addNegativeBodyAtom(atom_t atom, size_t weight)
	{
		negativeBody_[atom] += weight;
		maximumBodyWeight_ += weight;
		if(!minimumSet_) minimumBodyWeight_ += weight;
	}

	void GroundAspRule::setMinimumBodyWeight(size_t weight)
	{
		minimumSet_ = true;
		minimumBodyWeight_ = weight;
	}

	bool GroundAspRule::hasWeightedBody() const
	{
		return minimumBodyWeight_ != maximumBodyWeight_;
	}

	bool GroundAspRule::isPosWeightedAtom(atom_t atom) const
	{
		return hasWeightedBody() && positiveBody_.find(atom) != positiveBody_.end();
	}

	bool GroundAspRule::isNegWeightedAtom(atom_t atom) const
	{
		return hasWeightedBody() && negativeBody_.find(atom) != negativeBody_.end();
	}

	bool GroundAspRule::hasChoiceHead() const
	{
		return choiceRule_;
	}

	bool GroundAspRule::isHeadAtom(atom_t atom) const
	{
		return head_.find(atom) != head_.end();
	}

	bool GroundAspRule::isPositiveBodyAtom(atom_t atom) const
	{
		return positiveBody_.find(atom) != positiveBody_.end();
	}

	bool GroundAspRule::isNegativeBodyAtom(atom_t atom) const
	{
		return negativeBody_.find(atom) != negativeBody_.end();
	}

	GroundAspRule::SatisfiabilityInfo GroundAspRule::check(
			const atom_vector &trueAtoms,
			const atom_vector &falseAtoms,
			const atom_vector &reductFalseAtoms,
		
		#ifdef SUPPORTED_CHECK
			atom_vector * supp,
		#endif

			const TreeNodeInfo& info) const
	{
		return this->check(trueAtoms, falseAtoms, reductFalseAtoms,
			#ifdef SUPPORTED_CHECK
				supp,
			#endif
				SatisfiabilityInfo
				{
					false,
					false,
					0,
					maximumBodyWeight_,
					0
				}, info);
	}

	bool GroundAspRule::isSimpleRule() const
	{
		return positiveBody_.size() == 0 && negativeBody_.size() == 0;
	}

	GroundAspRule::SatisfiabilityInfo GroundAspRule::check(
			const atom_vector &newTrueAtoms,
			const atom_vector &newFalseAtoms,
			const atom_vector &newReductFalseAtoms,
		
		#ifdef SUPPORTED_CHECK
			atom_vector *supp,

		#endif

			GroundAspRule::SatisfiabilityInfo ei,
			const TreeNodeInfo& info) const
	{
		unordered_map<atom_t, size_t>::const_iterator it;

		DBG("\n\trule{"); DBG(ei.minBodyWeight); DBG(".");
		DBG(ei.maxBodyWeight); DBG("."); DBG(ei.seenHeadAtoms); DBG("} ");

		DBG_COLL(newTrueAtoms); DBG_COLL(newFalseAtoms);
		DBG_COLL(newReductFalseAtoms);
#ifndef INT_ATOMS_TYPE


		for(const atom_t atom : newReductFalseAtoms)
			if(!choiceRule_ && head_.find(atom) != head_.end())
				++ei.seenHeadAtoms;
			else if(choiceRule_ && head_.find(atom) != head_.end())
				ei.seenHeadAtoms = head_.size(); // h false
			else if((it = positiveBody_.find(atom)) != positiveBody_.end()
					&& (ei.maxBodyWeight -= it->second) < minimumBodyWeight_)
				return SatisfiabilityInfo { true, false, 0, 0, 0 }; // b false
			else if((it = negativeBody_.find(atom)) != negativeBody_.end()
					&& (ei.maxBodyWeight -= it->second) < minimumBodyWeight_)
				return SatisfiabilityInfo { true, false, 0, 0, 0 }; // reduct
			
		for(const atom_t atom : newTrueAtoms)
			if(!choiceRule_ && head_.find(atom) != head_.end())
				return SatisfiabilityInfo { true, false, 0, 0, 0 }; // h true
			else if(choiceRule_ && ei.seenHeadAtoms != head_.size()
					&& head_.find(atom) != head_.end()
					&& ++ei.seenHeadAtoms == head_.size())
				return SatisfiabilityInfo { true, false, 0, 0, 0 }; // h true
			else if((it = negativeBody_.find(atom)) != negativeBody_.end()
					&& (ei.maxBodyWeight -= it->second) < minimumBodyWeight_)
				return SatisfiabilityInfo { true, false, 0, 0, 0 }; // b false
			else if((it = positiveBody_.find(atom)) != positiveBody_.end())
				ei.minBodyWeight += it->second;

		for(const atom_t atom : newFalseAtoms)
			if(!choiceRule_ && head_.find(atom) != head_.end())
				++ei.seenHeadAtoms;
			else if(choiceRule_ && ei.seenHeadAtoms != head_.size()
					&& head_.find(atom) != head_.end()
					&& ++ei.seenHeadAtoms == head_.size())
				return SatisfiabilityInfo { true, false, 0, 0, 0 }; // h true
			else if((it = positiveBody_.find(atom)) != positiveBody_.end()
					&& (ei.maxBodyWeight -= it->second) < minimumBodyWeight_)
				return SatisfiabilityInfo { true, false, 0, 0, 0 }; // b false
			else if((it = negativeBody_.find(atom)) != negativeBody_.end())
				ei.minBodyWeight += it->second;
#else

		#ifdef SUPPORTED_CHECK
			if (supp && (~(*supp) & info.getAtoms()))	//still support required
			{
				bool supported = true;
				std::size_t maxBodyW = ei.maxBodyWeight, suppPos = 0;
				//std::cout << info.introducedAtoms << "," << info.rememberedAtoms << "," << std::endl;
				//std::cout << toString() << std::endl;
				assert(info.introducedAtoms.size() + info.rememberedAtoms.size() >= negativeBody_.size() + positiveBody_.size() + head_.size());
				for (size_t p = 0; p < info.introducedAtoms.size() + info.rememberedAtoms.size(); ++p)
				{
					const atom_t atom = p >= info.introducedAtoms.size() ? info.rememberedAtoms[p - info.introducedAtoms.size()] : info.introducedAtoms[p];
					DBG("CHECKING "); DBG(atom); DBG(std::endl);
					if (TO_INT(((~(*supp)) >> p) & 1) && head_.find(atom) != head_.end())	//found it!
					{
						suppPos |= (1 << p);		//remember position 
						DBG("FOUND suppos "); DBG(suppPos); DBG(std::endl);
					}
					if (!suppPos && !TO_INT((info.getAtoms() & ~(*supp)) >> (p + 1)))	//anything left?
					{
						DBG("NOTHING LEFT"); DBG(std::endl);
						break;
					}
					if ((TO_INT(newReductFalseAtoms | newTrueAtoms) >> p) & 1) 
					{
						DBG("TRUE atom "); DBG( negativeBody_.find(atom) != negativeBody_.end()); DBG(" "); DBG(maxBodyW); DBG(",min "); DBG(minimumBodyWeight_); DBG(std::endl);
						if ((it = negativeBody_.find(atom)) != negativeBody_.end()
						&& (maxBodyW -= it->second) < minimumBodyWeight_)
						{
							supported = false;
							break;
						}
						if (((TO_INT(newReductFalseAtoms) >> p) & 1) &&
						(it = positiveBody_.find(atom)) != positiveBody_.end()
						 && (maxBodyW -= it->second) < minimumBodyWeight_)
						{
						 	supported = false;
							break;
						}
					}
					else if ((TO_INT(newFalseAtoms) >> p) & 1)
					{
						DBG("FALSE atom "); DBG(std::endl);
						if ((it = positiveBody_.find(atom)) != positiveBody_.end()
						&& (maxBodyW -= it->second) < minimumBodyWeight_)
						{
							supported = false;
							break;
						}
					}
				}
				if (supported) // && suppPos != (std::size_t)(-1))
				{
					(*supp) |= suppPos;
					DBG("RESULT: ==> SUPP!"); DBG(std::endl);
				}
				else
				{
					DBG("RESULT: ==> NOT SUPP!"); DBG(std::endl);
				}
				/*else
					std::cout << "UNS" << std::endl;*/
			}
		#endif

				//std::cout << newTrueAtoms << "," << newReductFalseAtoms << "," << newFalseAtoms << std::endl;

			for (size_t p = 0; p < info.introducedAtoms.size() + info.rememberedAtoms.size(); ++p)
		{
			const atom_t atom = p >= info.introducedAtoms.size() ? info.rememberedAtoms[p - info.introducedAtoms.size()] : info.introducedAtoms[p]; //eibagaccess[p, newTrueAtoms];

			/*else*/ if ((TO_INT(newReductFalseAtoms) >> p) & 1)        //true Atom
			{
				if (!choiceRule_ && head_.find(atom) != head_.end())
					++ei.seenHeadAtoms;
				else if (choiceRule_ && head_.find(atom) != head_.end())
					ei.seenHeadAtoms = head_.size(); // h false
				else if ((it = positiveBody_.find(atom)) != positiveBody_.end()
						 && (ei.maxBodyWeight -= it->second) < minimumBodyWeight_)
					return SatisfiabilityInfo {true, false, 0, 0, 0}; // b false
				else if ((it = negativeBody_.find(atom)) != negativeBody_.end()
						 && (ei.maxBodyWeight -= it->second) < minimumBodyWeight_)
					return SatisfiabilityInfo {true, false, 0, 0, 0}; // reduct
			}
			/*}

			for (size_t p = 0; p < info.introducedAtoms.size() + info.rememberedAtoms.size(); ++p)
		{
			const atom_t atom = p >= info.introducedAtoms.size() ? info.rememberedAtoms[p - info.introducedAtoms.size()] : info.introducedAtoms[p]; //eibagaccess[p, newTrueAtoms];
			*/else if ((TO_INT(newTrueAtoms) >> p) & 1)		//true Atom
			{
				if(!choiceRule_ && head_.find(atom) != head_.end())
					return SatisfiabilityInfo { true, false, 0, 0, 0 }; // h true
				else if(choiceRule_ && ei.seenHeadAtoms != head_.size()
						&& head_.find(atom) != head_.end()
						&& ++ei.seenHeadAtoms == head_.size())
					return SatisfiabilityInfo { true, false, 0, 0, 0 }; // h true
				else if((it = negativeBody_.find(atom)) != negativeBody_.end()
						&& (ei.maxBodyWeight -= it->second) < minimumBodyWeight_)
					return SatisfiabilityInfo { true, false, 0, 0, 0 }; // b false
				else if((it = positiveBody_.find(atom)) != positiveBody_.end())
					ei.minBodyWeight += it->second;

			}
			/*}

			for (size_t p = 0; p < info.introducedAtoms.size() + info.rememberedAtoms.size(); ++p)
		{
			const atom_t atom = p >= info.introducedAtoms.size() ? info.rememberedAtoms[p- info.introducedAtoms.size() ] : info.introducedAtoms[p]; //eibagaccess[p, newTrueAtoms];

			*/else if ((TO_INT(newFalseAtoms) >> p) & 1)
			{
				if(!choiceRule_ && head_.find(atom) != head_.end())
					++ei.seenHeadAtoms;
				else if(choiceRule_ && ei.seenHeadAtoms != head_.size()
						&& head_.find(atom) != head_.end()
						&& ++ei.seenHeadAtoms == head_.size())
					return SatisfiabilityInfo { true, false, 0, 0, 0 }; // h true
				else if((it = positiveBody_.find(atom)) != positiveBody_.end()
						&& (ei.maxBodyWeight -= it->second) < minimumBodyWeight_)
					return SatisfiabilityInfo { true, false, 0, 0, 0 }; // b false
				else if((it = negativeBody_.find(atom)) != negativeBody_.end())
					ei.minBodyWeight += it->second;
			}
			/*else
				assert(false);*/
		}

#endif
		assert(ei.seenHeadAtoms <= head_.size());
		if(ei.seenHeadAtoms != head_.size()) return ei; // unknown
		if(ei.minBodyWeight < minimumBodyWeight_) return ei; // unknown
		return SatisfiabilityInfo { false, true, 0, 0, 0 }; // h false, b true
	}

	GroundAspRule::SatisfiabilityInfo GroundAspRule::check(
			const atom_vector &sharedTrueAtoms,
			const atom_vector &sharedFalseAtoms,
			const atom_vector &sharedReductFalseAtoms,
			SatisfiabilityInfo ei1,
			SatisfiabilityInfo ei2, const TreeNodeInfo &info) const
	{
		SatisfiabilityInfo ei
		{
			false,
			false,
			ei1.minBodyWeight + ei2.minBodyWeight,
			ei1.maxBodyWeight + ei2.maxBodyWeight - maximumBodyWeight_,
			ei1.seenHeadAtoms + ei2.seenHeadAtoms
		};

		bool headFalse = choiceRule_ && (ei1.seenHeadAtoms == head_.size() 
					  				 ||  ei2.seenHeadAtoms == head_.size());

		if(headFalse) ei.seenHeadAtoms = head_.size();

		unordered_map<atom_t, size_t>::const_iterator it;
#ifdef INT_ATOMS_TYPE
		assert(false);
		for (size_t p = 0; p < info.introducedAtoms.size() + info.rememberedAtoms.size(); ++p)
		{
			const atom_t atom = p >= info.introducedAtoms.size() ? info.rememberedAtoms[p - info.introducedAtoms.size()] : info.introducedAtoms[p];
			if ((TO_INT(sharedReductFalseAtoms) >> p) & 1)
			{
				if((it = negativeBody_.find(atom)) != negativeBody_.end())
					ei.maxBodyWeight += it->second;
				else if((it = positiveBody_.find(atom)) != positiveBody_.end())
					ei.maxBodyWeight += it->second;
				else if((!choiceRule_ || !headFalse)
						&& head_.find(atom) != head_.end())
					--ei.seenHeadAtoms;
			}
			else if ((TO_INT(sharedTrueAtoms) >> p) & 1)
			{
				if((it = negativeBody_.find(atom)) != negativeBody_.end())
					ei.maxBodyWeight += it->second;
				else if((it = positiveBody_.find(atom)) != positiveBody_.end())
					ei.minBodyWeight -= it->second;
				else if((!choiceRule_ || !headFalse)
						&& head_.find(atom) != head_.end())
					--ei.seenHeadAtoms;
			}
			else
			{
				assert((TO_INT(sharedFalseAtoms) >> p) & 1);
				if((it = negativeBody_.find(atom)) != negativeBody_.end())
					ei.minBodyWeight -= it->second;
				else if((it = positiveBody_.find(atom)) != positiveBody_.end())
					ei.maxBodyWeight += it->second;
				else if((!choiceRule_ || !headFalse)
						&& head_.find(atom) != head_.end())
					--ei.seenHeadAtoms;
			}
		}
#else
		for(const atom_t atom : sharedReductFalseAtoms)
			if((it = negativeBody_.find(atom)) != negativeBody_.end())
				ei.maxBodyWeight += it->second;
			else if((it = positiveBody_.find(atom)) != positiveBody_.end())
				ei.maxBodyWeight += it->second;
			else if((!choiceRule_ || !headFalse)
					&& head_.find(atom) != head_.end())
				--ei.seenHeadAtoms;

		for(const atom_t atom : sharedTrueAtoms)
			if((it = negativeBody_.find(atom)) != negativeBody_.end())
				ei.maxBodyWeight += it->second;
			else if((it = positiveBody_.find(atom)) != positiveBody_.end())
				ei.minBodyWeight -= it->second;
			else if((!choiceRule_ || !headFalse)
					&& head_.find(atom) != head_.end())
				--ei.seenHeadAtoms;

		for(const atom_t atom : sharedFalseAtoms)
			if((it = negativeBody_.find(atom)) != negativeBody_.end())
				ei.minBodyWeight -= it->second;
			else if((it = positiveBody_.find(atom)) != positiveBody_.end())
				ei.maxBodyWeight += it->second;
			else if((!choiceRule_ || !headFalse)
					&& head_.find(atom) != head_.end())
				--ei.seenHeadAtoms;
#endif
		if(choiceRule_ && !headFalse && ei.seenHeadAtoms == head_.size())
			return SatisfiabilityInfo { true, false, 0, 0, 0 }; // h true
		if(ei.maxBodyWeight < minimumBodyWeight_)
			return SatisfiabilityInfo { true, false, 0, 0, 0 }; // b false
		if(ei.seenHeadAtoms == head_.size()
				&& ei.minBodyWeight >= minimumBodyWeight_)
			return SatisfiabilityInfo { false, true, 0, 0, 0 }; // known false
		return ei; // still unknown
	}

	IGroundAspRule::const_iterator GroundAspRule::begin() const
	{
		DBG(this); DBG(" "); DBG(choiceRule_ ? "c" : "h"); DBG_COLL_ONLY(head_);
		DBG(" p"); DBG_MAP(positiveBody_); DBG(" n"); DBG_MAP(negativeBody_);
		DBG(" >= "); DBG(minimumBodyWeight_); DBG(std::endl);

		return IGroundAspRule::const_iterator(new ConstEnumerator(
					head_.begin(), head_.end(),
					positiveBody_.begin(), positiveBody_.end(),
					negativeBody_.begin(), negativeBody_.end()));
	}

	IGroundAspRule::const_iterator GroundAspRule::end() const
	{
		return IGroundAspRule::const_iterator(new ConstEnumerator(
					head_.end(),
					positiveBody_.end(),
					negativeBody_.end()));
	}

	GroundAspRule::ConstEnumerator::ConstEnumerator() { ended_ = true; }

	GroundAspRule::ConstEnumerator::ConstEnumerator(
			SetIter hbegin, SetIter hend,
			MapIter pbegin, MapIter pend,
			MapIter nbegin, MapIter nend)
		: hbegin_(hbegin), hend_(hend),
		  pbegin_(pbegin), pend_(pend),
		  nbegin_(nbegin), nend_(nend),
		  ended_(hbegin == hend && pbegin == pend && nbegin == nend)
	{ }

	GroundAspRule::ConstEnumerator::ConstEnumerator(
			SetIter hend,
			MapIter pend,
			MapIter nend)
		: hbegin_(hend), hend_(hend),
		  pbegin_(pend), pend_(pend),
		  nbegin_(nend), nend_(nend),
		  ended_(true)
	{ }

	GroundAspRule::ConstEnumerator::~ConstEnumerator() { }

	void GroundAspRule::ConstEnumerator::next()
	{	
		if(hbegin_ != hend_) ++hbegin_;
		else if(pbegin_ != pend_) ++pbegin_;
		else ++nbegin_;

		ended_ |= (hbegin_ == hend_ && pbegin_ == pend_ && nbegin_ == nend_);
	}

	const atom_t &GroundAspRule::ConstEnumerator::get() const
	{	
		if(hbegin_ != hend_) return *hbegin_;
		else if(pbegin_ != pend_) return pbegin_->first;
		else return nbegin_->first;
	}

	bool GroundAspRule::ConstEnumerator::valid() const
	{
		return !ended_;
	}

	GroundAspRule::ConstEnumerator *GroundAspRule::ConstEnumerator::clone()
		const
	{
		ConstEnumerator *ret = new ConstEnumerator();
		ret->hbegin_ = hbegin_;
		ret->hend_ = hend_;
		ret->pbegin_ = pbegin_;
		ret->pend_ = pend_;
		ret->nbegin_ = nbegin_;
		ret->nend_ = nend_;
		ret->ended_ = ended_;
		return ret;
	}

	bool GroundAspRule::ConstEnumerator::operator==(
			const sharp::IConstEnumerator<atom_t> &other) const
	{
		if(typeid(other) != typeid(*this)) return false;

		const ConstEnumerator &tmpother =
			static_cast<const ConstEnumerator &>(other);

		return tmpother.hbegin_ == hbegin_
			&& tmpother.hend_ == hend_
			&& tmpother.pbegin_ == pbegin_
			&& tmpother.pend_ == pend_
			&& tmpother.nbegin_ == nbegin_
			&& tmpother.nend_ == nend_;
	}

} // namespace dynasp


std::ostream& operator<<(std::ostream& os, const dynasp::IGroundAspRule& obj)
{
      os << obj.toString();
      return os;
}



