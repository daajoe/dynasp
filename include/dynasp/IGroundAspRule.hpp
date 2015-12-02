#ifndef DYNASP_DYNASP_IGROUNDASPRULE_H_
#define DYNASP_DYNASP_IGROUNDASPRULE_H_

#include <dynasp/global>

#include <dynasp/Atom.hpp>

#include <sharp/main>

#include <vector>
#include <cstddef>

namespace dynasp
{
	class DYNASP_API IGroundAspRule
	{
	protected:
		IGroundAspRule &operator=(IGroundAspRule &) { return *this; }

	public:
		enum Type
		{
			STANDARD,
			CHOICE
		};

		typedef sharp::IConstEnumerator<atom_t>::Iterator const_iterator;

		virtual ~IGroundAspRule() = 0;

		virtual void setHead(atom_t atom) = 0;
		virtual void setHead(const atom_vector &atoms) = 0;
		virtual void setPositiveBody(const atom_vector &atoms) = 0;
		virtual void setNegativeBody(const atom_vector &atoms) = 0;
		virtual void setMinimumTrueBodyAtoms(std::size_t count) = 0;

		virtual const_iterator begin() const = 0;
		virtual const_iterator end() const = 0;

	}; // class IGroundAspRule

	inline IGroundAspRule::~IGroundAspRule() { }

} // namespace dynasp

#endif // DYNASP_DYNASP_IGROUNDASPRULE_H_
