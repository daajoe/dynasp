#ifndef DYNASP_ALGORITHMS_DYNASPCOUNTINGSOLUTION_H_
#define DYNASP_ALGORITHMS_DYNASPCOUNTINGSOLUTION_H_

#include <dynasp/global>

#include <dynasp/IDynAspCountingSolution.hpp>

namespace dynasp
{
	class DYNASP_LOCAL DynAspCountingSolution : public IDynAspCountingSolution
	{
	public:
		DynAspCountingSolution(std::size_t count, std::size_t optimalWeight);
		virtual ~DynAspCountingSolution();

		virtual std::size_t count() const;
		virtual std::size_t optimalWeight() const;

	private:
		std::size_t count_;
		std::size_t optimalWeight_;

	}; // class DynAspCountingSolution

} // namespace dynasp

#endif // DYNASP_ALGORITHMS_DYNASPCOUNTINGSOLUTION_H_
