#ifndef DYNASP_DYNASP_IGROUNDASPRULEFACTORY_H_
#define DYNASP_DYNASP_IGROUNDASPRULEFACTORY_H_

#include <dynasp/global>

#include <dynasp/IGroundAspRule.hpp>

namespace dynasp
{
	class DYNASP_API IGroundAspRuleFactory
	{
	protected:
		IGroundAspRuleFactory &operator=(IGroundAspRuleFactory &)
		{ return *this; }

	public:
		virtual ~IGroundAspRuleFactory() = 0;

		virtual IGroundAspRule *create() const = 0;

	}; // class IGroundAspRuleFactory

	inline IGroundAspRuleFactory::~IGroundAspRuleFactory() { }

} // namespace dynasp

#endif // DYNASP_DYNASP_IGROUNDASPRULEFACTORY_H_
