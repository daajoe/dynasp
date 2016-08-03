//
// Created by hecher on 5/4/16.
//

#include <dynasp/SortedAtomVector.hpp>

using namespace dynasp;

#ifndef INT_ATOMS_TYPE
SortedAtomVector::const_iterator dynasp::SortedAtomVector::find(const atom_t elem) const
{
	for (auto pos = cbegin(); pos != cend(); ++pos)
		if (*pos == elem)
			return pos;
	return cend();
}


void dynasp::SortedAtomVector::insert(const atom_t elem)
{
	push_back(elem);
}
#endif

/*void dynasp::SortedAtomVector::insert(const_iterator first, const_iterator last)
{
	reserve(last - first);
	//insert(end(), begin, end);
}*/


