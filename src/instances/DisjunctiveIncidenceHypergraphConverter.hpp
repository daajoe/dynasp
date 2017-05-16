#ifndef DYNASP_INSTANCES_DISJINCHYPERGRAPHCONVERTER_H_
#define DYNASP_INSTANCES_DISJINCHYPERGRAPHCONVERTER_H_

#include <dynasp/global>

#include "GroundAspInstance.hpp"

#include <dynasp/IHypergraphConverter.hpp>

namespace dynasp
{
	class DYNASP_LOCAL DisjunctiveIncidenceHypergraphConverter : public IHypergraphConverter
	{
	public:
		DisjunctiveIncidenceHypergraphConverter();
		virtual ~DisjunctiveIncidenceHypergraphConverter();

		virtual htd::IHypergraph *convert(
				const IGroundAspInstance &instance) const;

	private:
		htd::IHypergraph *convert(const GroundAspInstance &instance) const;
		
	}; // class DisjunctiveIncidenceHypergraphConverter

} // namespace dynasp

#endif // DYNASP_INSTANCES_PRIMALHYPERGRAPHCONVERTER_H_
