#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif

#include <dynasp/Factory.hpp>

#include "instances/GroundAspRule.hpp"
#include "instances/GroundAspInstance.hpp"
#include "algorithms/ClassicDynAspTuple.hpp"

namespace
{
	//FIXME: have default factory, so as to avoid if(!factory_) check
	dynasp::IFactory *factory_;
}

namespace dynasp
{
	void Factory::set(IFactory *factory)
	{
		if(factory_) delete factory_;
		factory_ = factory;
	}

	IGroundAspRule *Factory::createRule()
	{
		if(!factory_) return new GroundAspRule();
		return factory_->createRule();
	}

	IGroundAspInstance *Factory::createInstance()
	{
		if(!factory_) return new GroundAspInstance();
		return factory_->createInstance();
	}

	IDynAspTuple *Factory::createTuple()
	{
		if(!factory_) return new ClassicDynAspTuple();
		return factory_->createTuple();
	}

} // namespace dynasp

