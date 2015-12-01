#ifndef DYNASP_DYNASP_IDYNASPTUPLE_H_
#define DYNASP_DYNASP_IDYNASPTUPLE_H_

#include <dynasp/global>

#include <sharp/main>

#include <htd/main>

#include <string>
#include <cstddef>

namespace dynasp
{

	class DYNASP_API IDynAspTuple : public sharp::ITuple
	{
	protected:
		IDynAspTuple &operator=(IDynAspTuple &) { return *this; }

	public:
		virtual ~IDynAspTuple() = 0;

	}; // class IDynAspTuple

	inline IDynAspTuple::~IDynAspTuple() { }

} // namespace dynasp

#endif // DYNASP_DYNASP_IDYNASPTUPLE_H_
