#include <dynasp/TreeNodeInfo.hpp>
#include <dynasp/IGroundAspInstance.hpp>
#include <dynasp/create.hpp>
#include "../util/debug.hpp"


namespace dynasp
{
	#ifdef INT_ATOMS_TYPE
		dynasp::atom_vector TreeNodeInfo::transform(size_t child) const
		{
			/*const atom_vector atoms = instance->getNodeData(child).getAtoms();
			return transform(atoms, child);*/
			return transform(instance->getNodeData(child).getAtoms(), child) | (create::get() == create::INCIDENCEPRIMAL_RULESETTUPLE ? rule_transform(instance->getNodeData(child).getRules(), child) : 0);
			//return transform(std::move(instance->getNodeData(child).getAtoms()), child);

			/*const atom_vector atoms = instance->getNodeData(child).getAtoms();
			atom_vector childData;
			TreeNodeInfo& childInfo = instance->getNodeData(child);
			size_t pos = introducedAtoms.size();
			for (auto & i : rememberedAtoms)
				childData |= ((atoms >> childInfo.atomAtPosition[i]) & atom_vector(1)) << pos++;
			return childData;*/
		}


		//TODO: keep code only on one place!
		/*dynasp::atom_vector TreeNodeInfo::transform(const atom_vector &&atoms, size_t child) const
		{
			atom_vector childData;
			TreeNodeInfo& childInfo = instance->getNodeData(child);
			size_t pos = introducedAtoms.size();
			for (auto & i : rememberedAtoms)
				childData |= ((atoms >> childInfo.atomAtPosition[i]) & atom_vector(1)) << pos++;
			return childData;
		}*/

		dynasp::atom_vector TreeNodeInfo::rule_transform(const atom_vector &atoms, size_t child) const
		{
			atom_vector childData = 0;
			TreeNodeInfo& childInfo = instance->getNodeData(child);
			size_t pos = introducedAtoms.size() + rememberedAtoms.size() + introducedRules.size();
			DBG("rememb "); DBG_COLL(rememberedRules);
			for (auto & i : rememberedRules)
			{
				const auto it = childInfo.ruleAtPosition.find(i);
				if (it != childInfo.ruleAtPosition.end())
				{
					DBG("bit "); DBG_COLL(atoms); DBG( " -> "); DBG(it->second); DBG(std::endl);
					childData |= ((atoms >> it->second) & atom_vector(1)) << pos;
				}
				else
				{
					DBG("bla");
				}
				++pos;
			}
			return childData;// | (atoms & (1 << (INT_ATOMS_TYPE - 1)));
		}

		htd::vertex_t TreeNodeInfo::ruleIdOfPosition(size_t pos) const
		{
			assert(pos >= introducedAtoms.size() + rememberedAtoms.size());
			pos -= introducedAtoms.size() + rememberedAtoms.size();
			assert(pos < introducedRules.size() + rememberedRules.size());
			return pos < introducedRules.size() ? introducedRules[pos] : rememberedRules[pos - introducedRules.size()];
		}

		dynasp::atom_vector TreeNodeInfo::transform(const atom_vector &atoms, size_t child) const
		{
			atom_vector childData = 0;
			TreeNodeInfo& childInfo = instance->getNodeData(child);
			size_t pos = introducedAtoms.size();
			DBG("rememb "); DBG_COLL(rememberedAtoms);
			for (auto & i : rememberedAtoms)
			{
				const auto it = childInfo.atomAtPosition.find(i);
				if (it != childInfo.atomAtPosition.end())
					childData |= ((atoms >> it->second) & atom_vector(1)) << pos;
				pos++;
			}
			return childData;// | (atoms & (1 << (INT_ATOMS_TYPE - 1)));
		}
	#endif
} // namespace dynasp

