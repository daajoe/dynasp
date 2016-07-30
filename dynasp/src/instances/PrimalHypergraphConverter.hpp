#ifndef DYNASP_INSTANCES_PRIMALHYPERGRAPHCONVERTER_H_
#define DYNASP_INSTANCES_PRIMALHYPERGRAPHCONVERTER_H_

#include <dynasp/global>

#include "GroundAspInstance.hpp"

#include <dynasp/IHypergraphConverter.hpp>

namespace dynasp
{
	class DYNASP_LOCAL PrimalHypergraphConverter : public IHypergraphConverter
	{
	public:
		PrimalHypergraphConverter();
		virtual ~PrimalHypergraphConverter();

		virtual htd::IHypergraph *convert(
				const IGroundAspInstance &instance) const;

	private:
		htd::IHypergraph *convert(const GroundAspInstance &instance) const;
		
	}; // class PrimalHypergraphConverter

} // namespace dynasp

#endif // DYNASP_INSTANCES_PRIMALHYPERGRAPHCONVERTER_H_
