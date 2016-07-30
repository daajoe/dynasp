#ifndef DYNASP_DYNASP_IPARSERFACTORY_H_
#define DYNASP_DYNASP_IPARSERFACTORY_H_

#include <dynasp/global>

#include <dynasp/IParser.hpp>

namespace dynasp
{
	class DYNASP_API IParserFactory
	{
	protected:
		IParserFactory &operator=(IParserFactory &) { return *this; }

	public:
		virtual ~IParserFactory() = 0;

		virtual IParser *create() const = 0;

	}; // class IParserFactory

	inline IParserFactory::~IParserFactory() { }

} // namespace dynasp

#endif // DYNASP_DYNASP_IPARSERFACTORY_H_
