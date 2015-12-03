#ifndef DYNASP_DYNASP_IFACTORY_H_
#define DYNASP_DYNASP_IFACTORY_H_

#include <dynasp/global>

#include <dynasp/IGroundAspRule.hpp>
#include <dynasp/IGroundAspInstance.hpp>
#include <dynasp/IDynAspTuple.hpp>

namespace dynasp
{
	class DYNASP_API IFactory
	{
	protected:
		IFactory &operator=(IFactory &) { return *this; }

	public:
		virtual ~IFactory() = 0;

		virtual IGroundAspRule *createRule() const = 0;
		virtual IGroundAspInstance *createInstance() const = 0;
		virtual IDynAspTuple *createTuple() const = 0;

	}; // class IFactory

	inline IFactory::~IFactory() { }

} // namespace dynasp

#endif // DYNASP_DYNASP_IFACTORY_H_
