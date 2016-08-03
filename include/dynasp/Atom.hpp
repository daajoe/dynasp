#ifndef DYNASP_DYNASP_ATOM_H_
#define DYNASP_DYNASP_ATOM_H_

#include <dynasp/global>

#include <htd/main.hpp>

#include <vector>
#include <cstdint>
#include <bitset>

namespace dynasp
{
	typedef htd::vertex_t atom_t;

#ifndef INT16_ATOMS_TYPE
#ifndef INT32_ATOMS_TYPE
#ifndef INT64_ATOMS_TYPE
#ifndef INT8_ATOMS_TYPE
	typedef std::vector<atom_t> atom_vector;
#else
#ifdef DO_NOT_USE_BITSET
	//typedef uint_fast8_t atom_vector;
	typedef uint8_t atom_vector;
#else
	typedef std::bitset<8> atom_vector;
#endif
#endif
#else
#ifdef DO_NOT_USE_BITSET
	typedef uint_fast64_t atom_vector;
#else
	typedef std::bitset<64> atom_vector;
#endif
#endif
#else
#ifdef DO_NOT_USE_BITSET
	typedef uint_fast32_t atom_vector;
#else
	typedef std::bitset<32> atom_vector;
#endif
#endif
#else
#ifdef DO_NOT_USE_BITSET
	typedef uint_fast16_t atom_vector;
#else
	typedef std::bitset<16> atom_vector;
#endif
#endif

#ifdef DO_NOT_USE_BITSET
#define TO_INT(x) (x)
#define POP_CNT(x) (__builtin_popcount(x))
#else

#define TO_INT(x) ((x).to_ulong())
#define POP_CNT(x) ((x).count())
#endif

	struct Atom
	{
		static const atom_t FALSE = 0;
	
	private:
		Atom();
	};

} // namespace dynasp

#endif // DYNASP_DYNASP_ATOM_H_
