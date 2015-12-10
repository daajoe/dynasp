#ifndef DYNASP_DYNASP_TREENODEINFO_H_
#define DYNASP_DYNASP_TREENODEINFO_H_

#include <dynasp/global>

#include <dynasp/Atom.hpp>
#include <dynasp/IGroundAspInstance.hpp>

#include <htd/main>

namespace dynasp
{
	struct TreeNodeInfo
	{
		const IGroundAspInstance &instance;
		//htd::vertex_container atoms;
		//htd::vertex_container rules;
		htd::vertex_container introducedAtoms;
		htd::vertex_container introducedRules;
		htd::vertex_container rememberedAtoms;
		htd::vertex_container rememberedRules;
		htd::vertex_container forgottenAtoms;
		htd::vertex_container forgottenRules;
	};

} // namespace dynasp

#endif // DYNASP_DYNASP_TREENODEINFO_H_
