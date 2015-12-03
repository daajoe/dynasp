#ifndef DYNASP_DYNASP_FACTORY_H_
#define DYNASP_DYNASP_FACTORY_H_

#include <dynasp/global>

#include <dynasp/IGroundAspRule.hpp>
#include <dynasp/IGroundAspInstance.hpp>
#include <dynasp/IDynAspTuple.hpp>
#include <dynasp/IFactory.hpp>

namespace dynasp
{
	class DYNASP_API Factory
	{
	public:
		//FIXME: have different factories for each interface
		static void set(IFactory *factory);

		static IGroundAspRule *createRule();
		static IGroundAspInstance *createInstance();
		static IDynAspTuple *createTuple();
		//static IDynAspAlgorithm *createAlgorithm();

	private:
		Factory();
		Factory(const Factory &);
		Factory &operator=(const Factory &);

	}; // class Factory

} // namespace dynasp

#endif // DYNASP_DYNASP_FACTORY_H_
