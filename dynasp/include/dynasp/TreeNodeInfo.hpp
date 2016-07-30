#ifndef DYNASP_DYNASP_TREENODEINFO_H_
#define DYNASP_DYNASP_TREENODEINFO_H_

#include <dynasp/global>

#include <dynasp/Atom.hpp>
#include <dynasp/Rule.hpp>
#include <dynasp/IGroundAspInstance.hpp>

#include <htd/main.hpp>

namespace dynasp
{
	struct TreeNodeInfo
	{
		const IGroundAspInstance &instance;
		atom_vector introducedAtoms;
		rule_vector introducedRules;
		atom_vector rememberedAtoms;
		rule_vector rememberedRules;
	};

} // namespace dynasp

#endif // DYNASP_DYNASP_TREENODEINFO_H_
