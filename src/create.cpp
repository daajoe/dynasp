#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif

#include "instances/GroundAspRule.hpp"
#include "instances/GroundAspInstance.hpp"
#include "instances/PrimalHypergraphConverter.hpp"
#include "instances/IncidenceHypergraphConverter.hpp"
#include "instances/IncidencePrimalHypergraphConverter.hpp"
#include "algorithms/FullDynAspTuple.hpp"
#include "algorithms/SimpleDynAspTuple.hpp"
#include "algorithms/RuleSetDynAspTuple.hpp"
#include "algorithms/InverseSimpleDynAspTuple.hpp"
#include "algorithms/DynAspCountingSolutionExtractor.hpp"

#include <dynasp/create.hpp>

#include <stdexcept>

namespace dynasp
{
	namespace
	{
		create::ConfigurationType type_ = create::INCIDENCE_FULLTUPLE;
		IGroundAspRuleFactory *ruleFactory_ = nullptr;
		IGroundAspInstanceFactory *instanceFactory_ = nullptr;
		IHypergraphConverterFactory *hypergraphConverterFactory_ = nullptr;
		IDynAspTupleFactory *tupleFactory_ = nullptr;
	}

	void create::set(ConfigurationType type)
	{
		switch(type)
		{
		case create::PRIMAL_FULLTUPLE:
		case create::PRIMAL_SIMPLETUPLE:
		case create::PRIMAL_INVERSESIMPLETUPLE:
		case create::INCIDENCE_FULLTUPLE:
		case create::INCIDENCEPRIMAL_FULLTUPLE:
		case create::INCIDENCEPRIMAL_RULESETTUPLE:
			type_ = type;
			break;

		default:
			throw std::domain_error("Invalid type.");
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
		if(ruleFactory_) return ruleFactory_->create();
		return create::rule(type_);
	}

	IGroundAspRule *create::rule(ConfigurationType type)
	{
		switch(type)
		{
		default:
			return new GroundAspRule();
		}
	}

	IGroundAspInstance *create::instance()
	{
		if(instanceFactory_) return instanceFactory_->create();
		return create::instance(type_);
	}

	IGroundAspInstance *create::instance(ConfigurationType type)
	{
		switch(type)
		{
		default:
			return new GroundAspInstance();
		}
	}

	IHypergraphConverter *create::hypergraphConverter()
	{
		if(hypergraphConverterFactory_)
			return hypergraphConverterFactory_->create();
		return create::hypergraphConverter(type_);
	}

	IHypergraphConverter *create::hypergraphConverter(ConfigurationType type)
	{
		switch(type)
		{
		case create::PRIMAL_FULLTUPLE:
		case create::PRIMAL_SIMPLETUPLE:
		case create::PRIMAL_INVERSESIMPLETUPLE:
			return new PrimalHypergraphConverter();

		case create::INCIDENCE_FULLTUPLE:
			return new IncidenceHypergraphConverter();

		case create::INCIDENCEPRIMAL_FULLTUPLE:
		case create::INCIDENCEPRIMAL_RULESETTUPLE:
			return new IncidencePrimalHypergraphConverter();

		default:
			throw std::domain_error("Invalid type.");
		}
	}

	IDynAspTuple *create::tuple(bool leaf)
	{
		if(tupleFactory_) return tupleFactory_->create(leaf);
		return create::tuple(leaf, type_); 
	}

	IDynAspTuple *create::tuple(bool leaf, ConfigurationType type)
	{
		switch(type)
		{
		case create::PRIMAL_FULLTUPLE:
		case create::INCIDENCE_FULLTUPLE:
		case create::INCIDENCEPRIMAL_FULLTUPLE:
			return new FullDynAspTuple(leaf);

		case create::PRIMAL_SIMPLETUPLE:
			return new SimpleDynAspTuple(leaf);

		case create::PRIMAL_INVERSESIMPLETUPLE:
			return new InverseSimpleDynAspTuple(leaf);

		case create::INCIDENCEPRIMAL_RULESETTUPLE:
			return new RuleSetDynAspTuple(leaf);

		default:
			throw std::domain_error("Invalid type.");
		}
	}

	IDynAspCountingSolutionExtractor *create::countingSolutionExtractor()
	{
		//TODO: implement and use factory for this (in sharp)
		return new DynAspCountingSolutionExtractor();
	}

} // namespace dynasp

