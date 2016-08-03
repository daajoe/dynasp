#ifndef DYNASP_DYNASP_DYNASPCERTIFICATEALGORITHM_H_
#define DYNASP_DYNASP_DYNASPCERTIFICATEALGORITHM_H_

#include <dynasp/global>

#include <sharp/main>

#include <htd/main.hpp>
#include <dynasp/CertificateDynAspTuple.hpp>
#include <unordered_map>


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

namespace dynasp
{

	class DYNASP_API DynAspCertificateAlgorithm : public sharp::ITreeTupleAlgorithm
	{
	public:
		DynAspCertificateAlgorithm();
		virtual ~DynAspCertificateAlgorithm();

		virtual std::vector<const htd::ILabelingFunction *>
				preprocessOperations() const;

		virtual void evaluateNode(
				htd::vertex_t node,
				const htd::ITreeDecomposition &decomposition,
				sharp::INodeTupleSetMap &tuples,
				const sharp::IInstance &instance,
				sharp::ITupleSet &outputTuples) const;

		virtual bool needAllTupleSets() const;

		typedef CertificateDynAspTuple::Certificate_pointer_set TupleType;


		//typedef std::unordered_map<TupleType, CertificateDynAspTuple*> TupleTypes_;
#ifdef VEC_CERTS_TYPE
		typedef std::vector<CertificateDynAspTuple*> TupleTypes;
#else


		struct TupleTypesEqual
		{
			bool operator()( const TupleType* lhs, const TupleType* rhs ) const
			{
				//TODO: bug if both are vectors
				return *lhs == *rhs;
			}
		};

		struct TupleTypesHash
		{
			std::size_t operator()(const TupleType *data) const
			{
				//assert(false);
				//return 0;
				sharp::Hash h;

				//return data->size();
				//return 1; //data;
				//std::size_t ret = 31;
				//TODO

				//std::cout << "HASH " << data << " @ " << data->size() << std::endl;
				for (const auto &t : *data)
					h.addUnordered(t.hash());
					//::dynasp::CertificateDynAspTuple::hash_<T> hashFunction;
					//h.addUnordered(hashFunction(t));
				h.incorporateUnordered();
				h.add(data->size());
				return h.get();
				/*stdX::dynasp::certificate_hash_combine(ret, t);
				//std::cout << data << ": " << ret << std::endl;
				return ret;*/
			}
		};

		/*template <class T>
		struct type_hash
		{
		public:
			size_t operator()(const T* t) const
			{
				std::hash<T> h;
				return h(*t);
			}
		};*/


				/*void hash_combine(std::size_t &seed, const CertificateDynAspTuple::Certificate_pointer_set &v)
				{
					::dynasp::CertificateDynAspTuple::hash_<CertificateDynAspTuple::Certificate_pointer_set> hashFunction;

					seed ^= hashFunction(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
				}

			struct Xhash
			{
			public:
				std::size_t operator()(const TupleType* data) const
				{
					std::size_t ret = 31;
					//TODO
					for (const auto &t : *data)
						hash_combine(ret, t);
					return ret;
				}
			};*/


		typedef std::unordered_map<TupleType*, CertificateDynAspTuple*, TupleTypesHash, TupleTypesEqual > TupleTypes;
#endif
	private:
	#ifdef USE_OPTIMIZED_EXTENSION_POINTERS
		static CertificateDynAspTuple* insertCompressed(TupleTypes& tupleTypes, TupleType& tupleType, CertificateDynAspTuple& me, CertificateDynAspTuple::ExtensionPointers& origin, const TreeNodeInfo& info);
	#endif
		static CertificateDynAspTuple* insertCompressed(TupleTypes& tupleTypes, TupleType& tupleType, CertificateDynAspTuple& me, CertificateDynAspTuple::ExtensionPointers::iterator& origin, const TreeNodeInfo& info, htd::vertex_t firstChild = 0);
		/*struct Impl;
		Impl * const impl;*/

	}; // class DynAspCertificateAlgorithm

} // namespace dynasp


#endif // DYNASP_DYNASP_DYNASPCERTIFICATEALGORITHM_H_
