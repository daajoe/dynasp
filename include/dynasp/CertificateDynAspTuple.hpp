//
// Created by hecher on 5/4/16.
//

#ifndef DYNASP_2ND_LEVEL_EXTENSIONS_CERTIFICATEDYNASPTUPLE_HPP
#define DYNASP_2ND_LEVEL_EXTENSIONS_CERTIFICATEDYNASPTUPLE_HPP

#include <dynasp/global>

#include <dynasp/IGroundAspRule.hpp>

#include <sharp/Hash.hpp>

#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <list>
#include <forward_list>
#include <dynasp/IDynAspTuple.hpp>
#include <dynasp/ReferenceCounter.hpp>

#include <dynasp/SortedAtomVector.hpp>

namespace dynasp
{

	using namespace sharp;

	class DYNASP_API CertificateDynAspTuple : public dynasp::IDynAspTuple//, ReferenceCounter
	{
	public:
		enum ESubsetRelation { ESR_NONE, ESR_EQUAL, ESR_INCLUDED, ESR_INCLUDED_STRICT };

		virtual bool cleanUpRoot(const TreeNodeInfo& info);
		virtual bool cleanUpRoot(htd::vertex_t node, const TreeNodeInfo& info, const htd::ITreeDecomposition &decomposition, INodeTupleSetMap &tuples, bool doDelete);
		virtual bool cleanUp();
		virtual bool cleanUp(htd::vertex_t node, const htd::ITreeDecomposition &decomposition, INodeTupleSetMap &tuples, bool doDelete);
		virtual void cleanUpSecondLevel() { evolution_.clear(); delete certificate_pointer_set_; certificate_pointer_set_ = nullptr; }

		//static void createPointerCache();

		struct DYNASP_LOCAL DynAspCertificatePointer
		{
				const CertificateDynAspTuple *cert;
				bool strict;

				DynAspCertificatePointer() {}
				DynAspCertificatePointer(const CertificateDynAspTuple* certificate_, bool strict_) : cert(certificate_), strict(strict_) {}

				/*virtual std::size_t hash() const {
					sharp::Hash hash;
					hash.add(reinterpret_cast<size_t>(cert));
					hash.add(this->strict ? (size_t)1 : (size_t)0);
					return hash.get();
				}*/

				void updateStrict(bool update) const
				{
					//TODO: laugh about this style :D (and change it?!)
					const_cast<DynAspCertificatePointer*>(this)->strict |= update;
				}

				size_t hash() const
				{
					return (reinterpret_cast<size_t>(cert)) | (((size_t)strict) << (8 * sizeof(size_t) - 1));
					/*sharp::Hash hash;
					hash.add(reinterpret_cast<size_t>(cert));
					hash.add(static_cast<unsigned char>(strict));
					return hash.get();*/
				}

				bool operator==(const DynAspCertificatePointer &other) const {
					//std::cout << " == " << std::endl;
					return cert == other.cert && strict == other.strict;
				}
		};

#ifdef CHECK_CONSENSE
		struct IConsenseData 
		{
		};
		virtual void updateConsense(const DynAspCertificatePointer&, IConsenseData *&, const TreeNodeInfo&, unsigned int, htd::vertex_t) const { }
		virtual void setStrict(bool, IConsenseData&) const { }
		virtual void clearConsense(IConsenseData &) const { }
		virtual ESubsetRelation isConsense(IConsenseData &, const TreeNodeInfo&, unsigned int, const htd::ITreeDecomposition&, htd::vertex_t) const { return ESR_INCLUDED_STRICT; }
#endif
		//template <class T>
		struct DynAspCertificatePointerEqual
		{
			constexpr bool operator()( const DynAspCertificatePointer& lhs, const DynAspCertificatePointer& rhs ) const
			{
				return lhs.cert == rhs.cert;
			}
		};

		/*template <>
		struct hash<dynasp::CertificateDynAspTuple::DynAspCertificatePointer>
		*/
		//template <class T>
		struct DynAspCertificatePointerHash
		{
		public:
			size_t operator()(const DynAspCertificatePointer &t) const
			{
				return (reinterpret_cast<size_t>(t.cert));
				/*
				sharp::Hash hash;
				hash.add(reinterpret_cast<size_t>(t.cert));
				//hash.add(reinterpret_cast<unsigned char>(t.strict));
				return hash.get();*/
			}
		};

		/*template <class T>
		struct cert_hash
		{
			size_t operator()(const T &t) const
			  {
				  sharp::Hash hash;
				hash.add(reinterpret_cast<size_t>(t.cert));
				  return hash.get();
			  }
		};*/

#ifdef EXTENSION_POINTER_SET_TYPE
		typedef std::unordered_set<CertificateDynAspTuple*, std::hash<CertificateDynAspTuple*> > ExtensionPointer;
#else
		/*class ExtensionPointer :: public std::vector<CertificateDynAspTuple*>
		{
			protected:
				
		};*/
		typedef std::vector<CertificateDynAspTuple *> ExtensionPointer;
#endif
		/*class ExtensionPointer : public std::vector<CertificateDynAspTuple *> {
		private:
			std::size_t weight_;
		public:
			ExtensionPointer() : weight_(0) { }
			ExtensionPointer(CertificateDynAspTuple* t) { push_back(t); }
			std::size_t getWeight() { return weight_; }
			void setWeight(std::size_t weight) { weight_ = weight; }
			void operator=(const ExtensionPointer& rhs) { std::vector<CertificateDynAspTuple *> ::operator=(rhs); weight_ = rhs.weight_; }
			ExtensionPointer(const ExtensionPointer& rhs) : std::vector<CertificateDynAspTuple *>(rhs) { weight_ = rhs.weight_; }
		};*/


	/*template<typename T>
		static void certificate_hash_combine(std::size_t &seed, const T &v)
		{
			std::hash<T> hashFunction;
			seed ^= hashFunction(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		}*/

		struct ExtensionPointerHash
		{
			std::size_t operator()(const ExtensionPointer &data) const
			{
				std::size_t seed = 31;
				std::hash<CertificateDynAspTuple*> hashFunction;
				for (auto *t : data)
					seed ^= hashFunction(t) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
					//certificate_hash_combine(ret, (size_t)t);
				return seed;

					sharp::Hash h;
					for (const auto* t : data)

		#ifndef EXTENSION_POINTER_SET_TYPE
						h.add((size_t)t);
		#else
						h.addUnordered((size_t)t);
					h.incorporateUnordered();
		#endif
					h.add(data.size());
					return h.get();


				}
			};
	/*namespace dynasp
		{
			template<typename T>
			void certificate_hash_combine(std::size_t &seed, const T &v)
			{
				::dynasp::CertificateDynAspTuple::hash_<T> hashFunction;

				std::cout << "v " << v.cert << " ("  << seed << "): " << hashFunction(v) << std::endl;

				seed ^= hashFunction(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
				std::cout << seed << std::endl;
			}
		}*/


		//typedef std::unordered_set<const ExtensionPointer*> ExtensionPointers;
		typedef std::unordered_set<ExtensionPointer, ExtensionPointerHash> ExtensionPointersCache;

#ifdef EXTENSION_POINTERS_SET_TYPE

		typedef std::unordered_set<ExtensionPointer, ExtensionPointerHash > ExtensionPointers;
#else
#ifdef EXTENSION_POINTERS_LIST_TYPE
		//typedef std::deque<ExtensionPointer > ExtensionPointers;
		typedef std::list<ExtensionPointer > ExtensionPointers;
		/*template <class T>
		class ForwardExtList<T> : public std::forward_list<T>
		{
			public:
				
		};
		typedef ForwardExtList<ExtensionPointer > ExtensionPointers;*/
#else
		typedef std::vector<ExtensionPointer > ExtensionPointers;
#endif
#endif
	#ifdef USE_OPTIMIZED_EXTENSION_POINTERS
		typedef ExtensionPointersCache ExtensionPointerTabuList;
	#endif
#ifdef REVERSE_EXTENSION_POINTER_SET_TYPE
		typedef std::unordered_set<CertificateDynAspTuple *, std::hash<CertificateDynAspTuple*>> ReverseExtensionPointers;
#else
		typedef std::vector<CertificateDynAspTuple *> ReverseExtensionPointers;
#endif
	#ifndef VEC_CERT_TYPE
		typedef std::unordered_set<DynAspCertificatePointer, DynAspCertificatePointerHash, DynAspCertificatePointerEqual > Certificate_pointer_set;
	#else
		typedef std::vector<DynAspCertificatePointer> Certificate_pointer_set;
	#endif

		inline CertificateDynAspTuple() : weight_(0), /*introWeight_(0),*/ solutions_(1),
									#ifdef INT_ATOMS_TYPE
										  atoms_(0), reductAtoms_(0),
									#endif
									#ifdef USE_PSEUDO
										#ifndef YES_BUT_NO_PSEUDO
										  	pseudo(false),
										#endif
									#endif
									#ifndef INT_ATOMS_TYPE
										   mergeHashValue(0),
									#endif
										  certificate_pointer_set_(nullptr), duplicate_(nullptr) { }

		virtual CertificateDynAspTuple* clone() const = 0;
		CertificateDynAspTuple* clone(Certificate_pointer_set& certificates, const ExtensionPointer& ptr);
		inline const CertificateDynAspTuple* isClone() const { return duplicate_; }
		inline const CertificateDynAspTuple* getClone() const { return isClone() != nullptr ? isClone() : this; }



		virtual void merge(CertificateDynAspTuple& mergee, ExtensionPointer *tuple);
		virtual bool join(const TreeNodeInfo& info, const std::vector<unsigned int>& its, const std::vector<std::vector<IDynAspTuple *>*>&, const htd::ITreeDecomposition& td, htd::vertex_t node, ExtensionPointer& p);
		virtual std::size_t joinHash(htd::vertex_t child, const atom_vector &atoms, const TreeNodeInfo& info) const;
		virtual size_t joinHash(const atom_vector &atoms, const TreeNodeInfo& info) const;
//virtual std::size_t joinHash(const vertex_container &atoms) const = 0;
		size_t hash() const;
		size_t mergeHash() const;
		virtual void mergeHash(Hash& /*h*/) const { }
		virtual bool merge(const IDynAspTuple &tuple);
		virtual IDynAspTuple *project(const TreeNodeInfo &info, size_t child) const;
		//virtual void /*CertificateDynAspTuple**/ postJoin(const TreeNodeInfo & info, atom_vector& trueAtoms, atom_vector& falseAtoms);
		const mpz_class& solutionCount() const;
		size_t solutionWeight() const;

		//TODO: make it better!
		//can be removed: just for some assertion check
		friend class DynAspAlgorithm;
		friend class SimpleDynAspTuple;
		friend class RuleSetDynAspTuple;
		friend class DynAspCertificateAlgorithm;

		//TODO: remove ugly stuff
		virtual inline ~CertificateDynAspTuple() { delete certificate_pointer_set_; /*if (duplicate_) const_cast<CertificateDynAspTuple*>(duplicate_)->decrease();*/ };

		virtual ESubsetRelation checkRelation(const CertificateDynAspTuple& other, const TreeNodeInfo& info, bool write = true);

		virtual inline bool isPseudo() const { return
									#ifdef USE_PSEUDO
										#ifndef YES_BUT_NO_PSEUDO
												pseudo
										#endif
									#else
										reductAtoms_ >> (INT_ATOMS_TYPE - 1) != 0
									#endif
										#ifdef COMBINE_PSEUDO_PSEUDO_SOLUTIONS
											#ifndef YES_BUT_NO_PSEUDO
													&&
											#endif
									   		solutions_ == 0
										#endif
												; }

		#ifdef COMBINE_PSEUDO_PSEUDO_SOLUTIONS
		virtual inline bool isPseudoFlag() const { return
									#ifdef USE_PSEUDO
										#ifndef YES_BUT_NO_PSEUDO
												pseudo
										#endif
									#else
										reductAtoms_ >> (INT_ATOMS_TYPE - 1) != 0
									#endif
												; }


		#endif

		//virtual bool isPseudoSolution() { return pseudo; } //return pseudoatoms.size() > 0; }

		virtual bool isSolution(const TreeNodeInfo& info) const;
		virtual bool operator==(const ITuple &other) const;
		virtual void debug() const { }
	protected:

		//static ExtensionPointersCache extcache;
		//static const ExtensionPointer* cachePointer(const ExtensionPointer&& ptrs);

		virtual void cleanUpRecursively();
		virtual void cleanUpRecursively(htd::vertex_t node, const htd::ITreeDecomposition &decomposition, INodeTupleSetMap &tuples, IDynAspTuple::DeleteMode mode);
		/*struct DYNASP_LOCAL DynAspCertificate
		{
			std::unordered_set<atom_t> atoms;
			//std::unordered_set<atom_t> pseudoatoms;
			//bool same; // certificate is the same as the model it belongs to

			std::size_t hash() const;
			bool operator==(const DynAspCertificate &other) const;

		};*/

		//typedef std::unordered_set<atom_t> atom_set;
		//typedef std::set<atom_t> atom_set_o;
		/*typedef std::unordered_set<
				DynAspCertificate,
				sharp::Hasher<DynAspCertificate> >
			certificate_set;*/

		//TODO: not required for Second Level {
		std::size_t weight_; //, introWeight_;
		mpz_class solutions_;
		//TODO: }

		//atom_set atoms_, reductAtoms_;
#ifdef VEC_ATOMS_TYPE
		typedef SortedAtomVector atom_set;
		/*SortedAtomVector atoms_, reductAtoms_;
		SortedAtomVector atoms_bar_;*/
#else
#ifdef INT_ATOMS_TYPE
		typedef atom_vector atom_set;
#else
		typedef std::unordered_set<atom_t> atom_set;
#endif
#endif
		atom_set atoms_,
				reductAtoms_;		//TODO: not required for "First" Level
#ifdef USE_PSEUDO
#ifndef YES_BUT_NO_PSEUDO
		bool pseudo;				//TODO: not required for "First" Level (dangerous?)
#endif
#endif
#ifndef INT_ATOMS_TYPE
		atom_vector atoms_bar_;		//TODO: not required for Second Level
		size_t mergeHashValue;
#endif

		ExtensionPointers origins_;		//TODO: not required for Second Level
		ReverseExtensionPointers evolution_;

		Certificate_pointer_set* certificate_pointer_set_; //TODO: not required for Second Level
		const CertificateDynAspTuple* duplicate_;	//TODO: not required for Second Level

#ifdef USE_OPTIMIZED_EXTENSION_POINTERS
		ExtensionPointerTabuList* non_evolution_;
#endif

		virtual void mergeData(CertificateDynAspTuple* oldExtendedTuple, CertificateDynAspTuple::ExtensionPointers::iterator& origin, const TreeNodeInfo& info, htd::vertex_t firstChild, bool newTuple = false);

		//TODO: make it fast!
		//TODO: better integrate this subfunctions in the corresponding main functions,
		//but for now it seems ok, hence easier to handle & test;
		//keep in mind that later on, at some stage, they should be called by each
		//of the different sub-algorithms individually, i.e.\ for now we are fine :).

		//PRECONDITION: project() successfully has been called before
		virtual void projectPtrs(IDynAspTuple& newTuple);

		//PRECONDITION: merge() successfully has been called before
		virtual void mergePtrs(IDynAspTuple &tuple);

		//PRECONDITION: join is feasible, i.e. join() successfully has been called before
		virtual void joinPtrs(IDynAspTuple* tupleleft, IDynAspTuple* tupleright, bool reuseMemory);

		virtual void postEvaluate();
		//TODO: prevent exponential runtime w.r.t. bag size
		//TODO: remove this shice!
		//PRECONDITION: insert is feasible, i.e. insert() successfully has been called before
		virtual void insertPtrs(const CertificateDynAspTuple& tuple); //sharp::ITupleSet &outputTuples)

#ifndef INT_ATOMS_TYPE
		virtual inline void clean() {
			atoms_bar_.clear();
		}
		virtual void prepareForBag(const atom_vector& /*atoms*/);
#endif
		virtual CertificateDynAspTuple::ESubsetRelation checkRules(const rule_vector & rules, const CertificateDynAspTuple & other, const TreeNodeInfo& info, bool write = true) = 0;
	/*#ifdef INT_ATOMS_TYPE
		virtual CertificateDynAspTuple::ESubsetRelation checkRules(const atom_vector & rules, const CertificateDynAspTuple & other, const TreeNodeInfo& info, bool write = true) = 0;
	#endif*/


		CertificateDynAspTuple::ESubsetRelation checkRelationExt(const CertificateDynAspTuple & other, const TreeNodeInfo& info);
		//CertificateDynAspTuple::ESubsetRelation checkRelationExt(const CertificateDynAspTuple & other, const TreeNodeInfo & info);
		template <class T>
		CertificateDynAspTuple::ESubsetRelation checkRelationExt(const CertificateDynAspTuple& other, const T& atoms1, const T& atoms2, bool write = true);

		/*template <class T>
		CertificateDynAspTuple::ESubsetRelation checkRelation(const CertificateDynAspTuple& other, T& atoms1, T& atoms2, bool subset = true, bool write = true)
		{
			CertificateDynAspTuple::ESubsetRelation  result = CertificateDynAspTuple::ESR_NONE;

			if (subset && std::includes(atoms1.begin(), atoms1.end(), atoms2.begin(), atoms2.end()))
				result = atoms1.size() > atoms2.size() ? CertificateDynAspTuple::ESR_INCLUDED_STRICT : CertificateDynAspTuple::ESR_INCLUDED;
			else if (!subset && std::includes(atoms2.begin(), atoms2.end(), atoms1.begin(), atoms1.end()))
				result = atoms2.size() > atoms1.size() ? CertificateDynAspTuple::ESR_INCLUDED_STRICT : CertificateDynAspTuple::ESR_INCLUDED;

			if (result != CertificateDynAspTuple::ESR_NONE && atoms1.size() == atoms2.size())
				result = CertificateDynAspTuple::ESR_EQUAL;

			if (write && result >= CertificateDynAspTuple::ESR_EQUAL)
				//this->certificate_pointer_set_.insert(CertificateDynAspTuple::DynAspCertificatePointer(&other, result == CertificateDynAspTuple::ESR_INCLUDED_STRICT));
				this->certificate_pointer_set_->emplace_back(CertificateDynAspTuple::DynAspCertificatePointer(&other, result == CertificateDynAspTuple::ESR_INCLUDED_STRICT));

			return result;
		}*/

}; // class CertificateDynAspTuple

/*size_t CertificateDynAspTuple::DynAspCertificatePointer::hash() const
{
	sharp::Hash hash;
	hash.add(reinterpret_cast<size_t>(cert));
	hash.add(this->strict ? (size_t)1 : (size_t)0);
	return hash.get();
}

bool CertificateDynAspTuple::DynAspCertificatePointer::operator==(
		const DynAspCertificatePointer &other) const
{
	return this->cert == other.cert
		&& this->strict == other.strict;
}*/

} // namespace dynasp


using namespace dynasp;

template <class T>
CertificateDynAspTuple::ESubsetRelation CertificateDynAspTuple::checkRelationExt(const CertificateDynAspTuple& other, const T& atoms1, const T& atoms2, bool write)
{
#ifdef INT_ATOMS_TYPE

	CertificateDynAspTuple::ESubsetRelation result = ((atoms1 & atoms2) == atoms2) ?
														(atoms1 == atoms2 ? CertificateDynAspTuple::ESR_EQUAL : CertificateDynAspTuple::ESR_INCLUDED_STRICT)
													: CertificateDynAspTuple::ESR_NONE;
#else
	bool subset = true;
	if ((subset && atoms2.size() > atoms1.size()) ||
		(!subset && atoms1.size() > atoms2.size()))
		return CertificateDynAspTuple::ESR_NONE;

	const T& a1 = subset ? atoms1 : atoms2, &a2 = subset ? atoms2 : atoms1;

	for (const auto & t: a2)
		if (a1.find(t) == a1.end())
			return CertificateDynAspTuple::ESR_NONE;

	CertificateDynAspTuple::ESubsetRelation result = a1.size() == a2.size() ? CertificateDynAspTuple::ESR_EQUAL : CertificateDynAspTuple::ESR_INCLUDED_STRICT;
#endif
	if (write)
#ifdef VEC_CERT_TYPE
		this->certificate_pointer_set_->emplace_back(CertificateDynAspTuple::DynAspCertificatePointer(&other, result == CertificateDynAspTuple::ESR_INCLUDED_STRICT));
#else
		this->certificate_pointer_set_->insert(CertificateDynAspTuple::DynAspCertificatePointer(&other, result == CertificateDynAspTuple::ESR_INCLUDED_STRICT));
#endif

	return result;
}

#endif //DYNASP_2ND_LEVEL_EXTENSIONS_CERTIFICATEDYNASPTUPLE_HPP

