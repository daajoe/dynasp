#ifndef DYNASP_DYNASP_IHYPERGRAPHCONVERTERFACTORY_H_
#define DYNASP_DYNASP_IHYPERGRAPHCONVERTERFACTORY_H_

#include <dynasp/global>

#include <dynasp/IHypergraphConverter.hpp>

namespace dynasp
{
	class DYNASP_API IHypergraphConverterFactory
	{
	protected:
		IHypergraphConverterFactory &operator=(IHypergraphConverterFactory &)
		{ return *this; }

	public:
		virtual ~IHypergraphConverterFactory() = 0;

		virtual IHypergraphConverter *create() const = 0;

	}; // class IHypergraphConverterFactory

	inline IHypergraphConverterFactory::~IHypergraphConverterFactory() { }

} // namespace dynasp

#endif // DYNASP_DYNASP_IHYPERGRAPHCONVERTERFACTORY_H_
