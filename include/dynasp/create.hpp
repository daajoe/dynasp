#ifndef DYNASP_DYNASP_CREATE_H_
#define DYNASP_DYNASP_CREATE_H_

#include <dynasp/global>

#include <dynasp/IGroundAspRule.hpp>
#include <dynasp/IGroundAspRuleFactory.hpp>
#include <dynasp/IGroundAspInstance.hpp>
#include <dynasp/IGroundAspInstanceFactory.hpp>
#include <dynasp/IHypergraphConverter.hpp>
#include <dynasp/IHypergraphConverterFactory.hpp>
#include <dynasp/IDynAspTuple.hpp>
#include <dynasp/IDynAspTupleFactory.hpp>
#include <dynasp/IDynAspCountingSolutionExtractor.hpp>
//TODO: algorithm factory

namespace dynasp
{
	class DYNASP_API create
	{
	public:
		enum ConfigurationType
		{
			PRIMAL_FULLTUPLE = 0,
			PRIMAL_SIMPLETUPLE,
			PRIMAL_INVERSESIMPLETUPLE,
			INCIDENCE_FULLTUPLE,
			INCIDENCEPRIMAL_FULLTUPLE,
			INCIDENCEPRIMAL_RULESETTUPLE
		}; // enum ConfigurationType

		static void set(ConfigurationType type);
		static void set(IGroundAspRuleFactory *factory);
		static void set(IGroundAspInstanceFactory *factory);
		static void set(IHypergraphConverterFactory *factory);
		static void set(IDynAspTupleFactory *factory);
		static ConfigurationType get();

		static IGroundAspRule *rule();
		static IGroundAspRule *rule(ConfigurationType type);

		static IGroundAspInstance *instance();
		static IGroundAspInstance *instance(ConfigurationType type);

		static IHypergraphConverter *hypergraphConverter();
		static IHypergraphConverter *hypergraphConverter(
				ConfigurationType type);

		static IDynAspTuple *tuple(bool leaf);
		static IDynAspTuple *tuple(bool leaf, ConfigurationType type);

		static IDynAspCountingSolutionExtractor *countingSolutionExtractor();

	private:
		create();

	}; // class create

} // namespace dynasp

#endif // DYNASP_DYNASP_CREATE_H_
