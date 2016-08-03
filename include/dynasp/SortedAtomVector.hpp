//
// Created by hecher on 5/4/16.
//

#ifndef DYNASP_2ND_LEVEL_EXTENSIONS_SORTEDATOMVECTOR_H
#define DYNASP_2ND_LEVEL_EXTENSIONS_SORTEDATOMVECTOR_H

#include <dynasp/Atom.hpp>

namespace dynasp
{

	//TODO: move semantics
	template <class T, class Target>
	static bool insertMerged(const T& source, Target& dest)
	{
		dest.reserve(source.size() + dest.size());
		bool allInserted = true;
		for (auto& origin : source)
		{
			bool found = false;
			for (auto& existing : dest)
				if (origin == existing)
				{
					found = true;
					allInserted = false;
					break;
				}
			if (!found)
				dest.push_back(origin);
		}
		return allInserted;
	}

	template <class T, class Target>
	static bool insertMergedOnce(const T& source, Target& dest)
	{
		bool found = false;
		for (auto& existing : dest)
			if (source == existing)
			{
				found = true;
				break;
			}
		if (!found)
			dest.emplace_back(source);
		return !found;
	}

	//TODO: move semantics
	template <class T, class Target>
	static bool insertSet(const T& source, Target& dest)
	{
		dest.reserve(source.size() + dest.size());
		bool allInserted = true;
		for (auto& origin : source)
			if (!dest.insert(origin).second)
				allInserted = false;
		return allInserted;
	}

#ifndef INT_ATOMS_TYPE
	//TODO: move semantics
	/*template <class T, class Target>
	static bool insertSetOnce(const T& source, Target& dest)
	{
		return dest.insert(origin).second;
	}*/


	class SortedAtomVector : public atom_vector
	{

	public:
		SortedAtomVector::const_iterator find(const atom_t elem) const;
		void insert(const atom_t elem);
		template <class InputIterator>
		void insert(InputIterator begin, InputIterator end);
		template <class Input>
		void insertDup(const Input &coll);
	};

	template <class InputIterator>
	void SortedAtomVector::insert(InputIterator first, InputIterator last)
	{
		this->atom_vector::insert(end(), first, last);
	}

	template <class Input>
	void SortedAtomVector::insertDup(const Input& coll) //Iterator first, InputIterator last)
	{
		insertMerged(coll, *this);/*
		for (; first != last; ++first)
			insertMerged()
		this->atom_vector::insert(end(), first, last);*/
	}
#endif
}


#endif //DYNASP_2ND_LEVEL_EXTENSIONS_SORTEDATOMVECTOR_H
