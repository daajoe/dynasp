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
		DynAspCountingSolution(BigNumber count, std::size_t optimalWeight);
		virtual ~DynAspCountingSolution();

		virtual BigNumber count() const;
		virtual std::size_t optimalWeight() const;

	private:
		BigNumber count_;
		std::size_t optimalWeight_;

	}; // class DynAspCountingSolution

} // namespace dynasp

#endif // DYNASP_ALGORITHMS_DYNASPCOUNTINGSOLUTION_H_
