#ifndef DYNASP_PARSERS_LPARSEPARSER_H_
#define DYNASP_PARSERS_LPARSEPARSER_H_

#include <dynasp/global>

#include <dynasp/IParser.hpp>

namespace dynasp
{
	class DYNASP_LOCAL LParseParser : public IParser
	{
	public:
		LParseParser();
		virtual ~LParseParser();

		virtual IGroundAspInstance *parse(std::istream *in);
		virtual IGroundAspInstance *parse(std::istream *in, std::ostream *out);

	private:
		virtual IGroundAspInstance *createEmptyInstance();

	}; // class LParseParser

} // namespace dynasp

#endif // DYNASP_PARSERS_LPARSEPARSER_H_
