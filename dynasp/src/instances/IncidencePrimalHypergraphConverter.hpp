#ifndef DYNASP_INSTANCES_INCIDENCEPRIMALHYPERGRAPHCONVERTER_H_
#define DYNASP_INSTANCES_INCIDENCEPRIMALHYPERGRAPHCONVERTER_H_

#include <dynasp/global>

#include "GroundAspInstance.hpp"

#include <dynasp/IHypergraphConverter.hpp>

namespace dynasp
{
	class DYNASP_LOCAL IncidencePrimalHypergraphConverter
		: public IHypergraphConverter
	{
	public:
		IncidencePrimalHypergraphConverter();
		virtual ~IncidencePrimalHypergraphConverter();

		virtual htd::IHypergraph *convert(
				const IGroundAspInstance &instance) const;

	private:
		htd::IHypergraph *convert(const GroundAspInstance &instance) const;
		
	}; // class IncidencePrimalHypergraphConverter

} // namespace dynasp

#endif // DYNASP_INSTANCES_INCIDENCEPRIMALHYPERGRAPHCONVERTER_H_
