#ifndef DYNASP_DYNASP_IGROUNDASPINSTANCEFACTORY_H_
#define DYNASP_DYNASP_IGROUNDASPINSTANCEFACTORY_H_

#include <dynasp/global>

#include <dynasp/IGroundAspInstance.hpp>

namespace dynasp
{
	class DYNASP_API IGroundAspInstanceFactory
	{
	protected:
		IGroundAspInstanceFactory &operator=(IGroundAspInstanceFactory &)
		{ return *this; }

	public:
		virtual ~IGroundAspInstanceFactory() = 0;

		virtual IGroundAspInstance *create() const = 0;

	}; // class IGroundAspInstanceFactory

	inline IGroundAspInstanceFactory::~IGroundAspInstanceFactory() { }

} // namespace dynasp

#endif // DYNASP_DYNASP_IGROUNDASPINSTANCEFACTORY_H_
