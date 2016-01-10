#ifndef DYNASP_INSTANCES_INCIDENCEHYPERGRAPHCONVERTER_H_
#define DYNASP_INSTANCES_INCIDENCEHYPERGRAPHCONVERTER_H_

#include <dynasp/global>

#include "GroundAspInstance.hpp"

#include <dynasp/IHypergraphConverter.hpp>

namespace dynasp
{
	class DYNASP_LOCAL IncidenceHypergraphConverter
		: public IHypergraphConverter
	{
	public:
		IncidenceHypergraphConverter();
		virtual ~IncidenceHypergraphConverter();

		virtual htd::IHypergraph *convert(
				const IGroundAspInstance &instance) const;

	private:
		htd::IHypergraph *convert(const GroundAspInstance &instance) const;
		
	}; // class IncidenceHypergraphConverter

} // namespace dynasp

#endif // DYNASP_INSTANCES_INCIDENCEHYPERGRAPHCONVERTER_H_
