#ifndef DYNASP_DYNASP_IDYNASPTUPLEFACTORY_H_
#define DYNASP_DYNASP_IDYNASPTUPLEFACTORY_H_

#include <dynasp/global>

#include <dynasp/IDynAspTuple.hpp>

namespace dynasp
{
	class DYNASP_API IDynAspTupleFactory
	{
	protected:
		IDynAspTupleFactory &operator=(IDynAspTupleFactory &) { return *this; }

	public:
		virtual ~IDynAspTupleFactory() = 0;

		virtual IDynAspTuple *create(bool leaf) const = 0;

	}; // class IDynAspTupleFactory

	inline IDynAspTupleFactory::~IDynAspTupleFactory() { }

} // namespace dynasp

#endif // DYNASP_DYNASP_IDYNASPTUPLEFACTORY_H_
