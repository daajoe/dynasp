#ifndef DYNASP_DYNASP_ATOM_H_
#define DYNASP_DYNASP_ATOM_H_

#include <dynasp/global>

#include <htd/main>

namespace dynasp
{
	typedef htd::vertex_t atom_t;

	struct Atom
	{
		static const atom_t FALSE = 0;
	
	private:
		Atom();
	};

} // namespace dynasp

#endif // DYNASP_DYNASP_ATOM_H_
