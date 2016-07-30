#ifndef DYNASP_DYNASP_IHYPERGRAPHCONVERTER_H_
#define DYNASP_DYNASP_IHYPERGRAPHCONVERTER_H_

#include <dynasp/global>

#include <dynasp/IGroundAspInstance.hpp>

#include <htd/main.hpp>

namespace dynasp
{
	class DYNASP_API IHypergraphConverter
	{
	protected:
		IHypergraphConverter &operator=(IHypergraphConverter &)
		{ return *this; }

	public:
		virtual ~IHypergraphConverter() = 0;

		virtual htd::IHypergraph *convert(
				const IGroundAspInstance &instance) const = 0;

	}; // class IHypergraphConverter

	inline IHypergraphConverter::~IHypergraphConverter() { }

} // namespace dynasp

#endif // DYNASP_DYNASP_IHYPERGRAPHCONVERTER_H_
