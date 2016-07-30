#ifndef DYNASP_DYNASP_IPARSER_H_
#define DYNASP_DYNASP_IPARSER_H_

#include <dynasp/global>

#include <dynasp/IGroundAspInstance.hpp>

#include <iostream>

namespace dynasp
{
	class DYNASP_API IParser
	{
	protected:
		IParser &operator=(IParser &) { return *this; }

	public:
		virtual ~IParser() = 0;

		virtual IGroundAspInstance *parse(std::istream *in) = 0;
		virtual IGroundAspInstance *parse(
				std::istream *in,
				std::ostream *out) = 0;

	}; // class IParser

	inline IParser::~IParser() { }

} // namespace dynasp

#endif // DYNASP_DYNASP_IPARSER_H_
