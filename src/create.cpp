#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif

#include "instances/GroundAspRule.hpp"
#include "instances/GroundAspInstance.hpp"
#include "instances/PrimalHypergraphConverter.hpp"
#include "instances/IncidenceHypergraphConverter.hpp"
#include "algorithms/ClassicDynAspTuple.hpp"
#include "algorithms/DynAspCountingSolutionExtractor.hpp"

#include <dynasp/create.hpp>

namespace dynasp
{
	namespace
	{
		create::ConfigurationType type_ = create::DEFAULT;
		IGroundAspRuleFactory *ruleFactory_ = nullptr;
		IGroundAspInstanceFactory *instanceFactory_ = nullptr;
		IHypergraphConverterFactory *hypergraphConverterFactory_ = nullptr;
		IDynAspTupleFactory *tupleFactory_ = nullptr;
	}

	void create::set(ConfigurationType type)
	{
		switch(type)
		{
		case create::PRIMAL_CLASSICCERTIFICATES:
		case create::PRIMAL_INVERSECERTIFICATES:
		case create::INCIDENCE_CLASSICCERTIFICATES:
			type_ = type;
			break;

		case create::DEFAULT:
		default:
			type_ = create::DEFAULT;
			break;
		}
	}

	void create::set(IGroundAspRuleFactory *factory)
	{
		if(ruleFactory_)
			delete ruleFactory_;
		ruleFactory_ = factory;
	}

	void create::set(IGroundAspInstanceFactory *factory)
	{
		if(instanceFactory_)
			delete instanceFactory_;
		instanceFactory_ = factory;
	}

	void create::set(IHypergraphConverterFactory *factory)
	{
		if(hypergraphConverterFactory_)
			delete hypergraphConverterFactory_;
		hypergraphConverterFactory_ = factory;
	}

	void create::set(IDynAspTupleFactory *factory)
	{
		if(tupleFactory_)
			delete tupleFactory_;
		tupleFactory_ = factory;
	}
	
	IGroundAspRule *create::rule()
	{
		return create::rule(type_);
	}

	IGroundAspRule *create::rule(ConfigurationType type)
	{
		switch(type)
		{
		case create::DEFAULT:
		default:
			if(ruleFactory_)
				return ruleFactory_->create();
			return new GroundAspRule();
		}
	}

	IGroundAspInstance *create::instance()
	{
		return create::instance(type_);
	}

	IGroundAspInstance *create::instance(ConfigurationType type)
	{
		switch(type)
		{
		case create::DEFAULT:
		default:
			if(instanceFactory_)
				return instanceFactory_->create();
			return new GroundAspInstance();
		}
	}

	IHypergraphConverter *create::hypergraphConverter()
	{
		return create::hypergraphConverter(type_);
	}

	IHypergraphConverter *create::hypergraphConverter(ConfigurationType type)
	{
		switch(type)
		{
		case create::PRIMAL_CLASSICCERTIFICATES:
		case create::PRIMAL_INVERSECERTIFICATES:
			std::cout << "foo1" << std::endl;
			return new PrimalHypergraphConverter();

		case create::INCIDENCE_CLASSICCERTIFICATES:
			std::cout << "foo2" << std::endl;
			return new IncidenceHypergraphConverter();

		case create::DEFAULT:
		default:
			if(hypergraphConverterFactory_)
				return hypergraphConverterFactory_->create();
			return new PrimalHypergraphConverter();
		}
	}

	IDynAspTuple *create::tuple(bool leaf)
	{
		return create::tuple(leaf, type_); 
	}

	IDynAspTuple *create::tuple(bool leaf, ConfigurationType type)
	{
		switch(type)
		{
		case create::DEFAULT:
		default:
			if(tupleFactory_)
				return tupleFactory_->create(leaf);
			return new ClassicDynAspTuple(leaf);
		}
	}

	IDynAspCountingSolutionExtractor *create::countingSolutionExtractor()
	{
		//TODO: implement and use factory for this (in sharp)
		return new DynAspCountingSolutionExtractor();
	}

} // namespace dynasp

