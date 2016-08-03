#ifndef DYNASP_DYNASP_IDYNASPCOUNTINGSOLUTION_H_
#define DYNASP_DYNASP_IDYNASPCOUNTINGSOLUTION_H_

#include <dynasp/global>

#include <sharp/main>

#include <gmpxx.h>

#include <cstddef>

namespace dynasp
{
	class DYNASP_API IDynAspCountingSolution : public sharp::ISolution
	{
	protected:
		IDynAspCountingSolution &operator=(IDynAspCountingSolution &)
		{ return *this; }

	public:
		virtual ~IDynAspCountingSolution() = 0;

		virtual mpz_class count() const = 0;
		virtual std::size_t optimalWeight() const = 0;

	}; // class IDynAspCountingSolution

	inline IDynAspCountingSolution::~IDynAspCountingSolution() { }

} // namespace dynasp

#endif // DYNASP_DYNASP_IDYNASPCOUNTINGSOLUTION_H_
