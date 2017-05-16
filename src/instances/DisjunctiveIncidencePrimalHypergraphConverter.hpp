#ifndef DYNASP_INSTANCES_DISJINCPRIMHYPERGRAPHCONVERTER_H_
#define DYNASP_INSTANCES_DISJINCPRIMHYPERGRAPHCONVERTER_H_

#include <dynasp/global>

#include "GroundAspInstance.hpp"

#include <dynasp/IHypergraphConverter.hpp>

namespace dynasp
{
	class DYNASP_LOCAL DisjunctiveIncidencePrimalHypergraphConverter : public IHypergraphConverter
	{
	public:
		DisjunctiveIncidencePrimalHypergraphConverter();
		virtual ~DisjunctiveIncidencePrimalHypergraphConverter();

		virtual htd::IHypergraph *convert(
				const IGroundAspInstance &instance) const;

	private:
		htd::IHypergraph *convert(const GroundAspInstance &instance) const;
		
	}; // class DisjunctiveIncidencePrimalHypergraphConverter

} // namespace dynasp

#endif // DYNASP_INSTANCES_PRIMALHYPERGRAPHCONVERTER_H_
