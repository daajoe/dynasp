#ifndef DYNASP_INSTANCES_DISJPRIMALHYPERGRAPHCONVERTER_H_
#define DYNASP_INSTANCES_DISJPRIMALHYPERGRAPHCONVERTER_H_

#include <dynasp/global>

#include "GroundAspInstance.hpp"

#include <dynasp/IHypergraphConverter.hpp>

namespace dynasp
{
	class DYNASP_LOCAL DisjunctivePrimalHypergraphConverter : public IHypergraphConverter
	{
	public:
		DisjunctivePrimalHypergraphConverter();
		virtual ~DisjunctivePrimalHypergraphConverter();

		virtual htd::IHypergraph *convert(
				const IGroundAspInstance &instance) const;

	private:
		htd::IHypergraph *convert(const GroundAspInstance &instance) const;
		
	}; // class DisjunctivePrimalHypergraphConverter

} // namespace dynasp

#endif // DYNASP_INSTANCES_PRIMALHYPERGRAPHCONVERTER_H_
