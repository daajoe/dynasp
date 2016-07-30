#ifndef DYNASP_ALGORITHMS_DYNASPCOUNTINGSOLUTION_H_
#define DYNASP_ALGORITHMS_DYNASPCOUNTINGSOLUTION_H_

#include <dynasp/global>

#include <dynasp/IDynAspCountingSolution.hpp>

#include <gmpxx.h>

namespace dynasp
{
	class DYNASP_LOCAL DynAspCountingSolution : public IDynAspCountingSolution
	{
	public:
		DynAspCountingSolution(mpz_class count, std::size_t optimalWeight);
		virtual ~DynAspCountingSolution();

		virtual mpz_class count() const;
		virtual std::size_t optimalWeight() const;

	private:
		mpz_class count_;
		std::size_t optimalWeight_;

	}; // class DynAspCountingSolution

} // namespace dynasp

#endif // DYNASP_ALGORITHMS_DYNASPCOUNTINGSOLUTION_H_
