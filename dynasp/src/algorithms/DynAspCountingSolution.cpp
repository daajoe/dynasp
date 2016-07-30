#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif

#include "DynAspCountingSolution.hpp"

namespace dynasp
{
	using std::size_t;

	DynAspCountingSolution::DynAspCountingSolution(
			mpz_class count,
			size_t optimalWeight)
		: count_(count), optimalWeight_(optimalWeight)
	{ }

	DynAspCountingSolution::~DynAspCountingSolution() { }

	mpz_class DynAspCountingSolution::count() const
	{
		return count_;
	}

	size_t DynAspCountingSolution::optimalWeight() const
	{
		return optimalWeight_;
	}

} // namespace dynasp
