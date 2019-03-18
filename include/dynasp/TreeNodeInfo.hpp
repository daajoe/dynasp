#ifndef DYNASP_DYNASP_TREENODEINFO_H_
#define DYNASP_DYNASP_TREENODEINFO_H_

#include <dynasp/global>

#include <dynasp/Atom.hpp>
#include <dynasp/Rule.hpp>

#include <htd/main.hpp>

#include <unordered_map>
//#include <dynasp/IGroundAspInstance.hpp>

namespace dynasp
{
	class IGroundAspInstance;
	typedef	std::unordered_map<htd::vertex_t, size_t> AtomPositionMap;

	using atom_container = std::unordered_set<htd::vertex_t>; 

	struct TreeNodeInfo
	{
		IGroundAspInstance *instance;
		htd::vertex_container introducedAtoms;
		rule_vector introducedRules;
		htd::vertex_container rememberedAtoms;
		rule_vector rememberedRules;
		
		#ifdef INT_ATOMS_TYPE
			atom_vector int_introducedAtoms;
			atom_vector int_rememberedAtoms;
			atom_vector int_negatedAtoms;
			atom_vector int_negatedChoiceAtoms;
			atom_vector int_projectionAtoms;
			atom_vector int_constrainedFactAtoms;
			atom_vector int_constrainedNFactAtoms;
			AtomPositionMap atomAtPosition;

			atom_vector int_introducedRules;
			atom_vector int_rememberedRules;
			AtomPositionMap ruleAtPosition;


			atom_vector getAtoms() const { return int_introducedAtoms | int_rememberedAtoms; }
			#ifdef NON_NORM_JOIN
			atom_vector int_joinAtoms, int_joinRules;

			atom_vector getJoinAtoms() const { return int_joinAtoms; }
			atom_vector getJoinRules() const { return int_joinRules; }
			#endif
			atom_vector getRules() const { return int_introducedRules | int_rememberedRules; }
		#endif

		TreeNodeInfo(IGroundAspInstance &i,
		htd::vertex_container &&ia,
		rule_vector &&ir,
		htd::vertex_container &&ra,
		rule_vector &&rr

		#ifdef INT_ATOMS_TYPE
			,/*atom_vector &iia,
			atom_vector &ira,*/
			AtomPositionMap &&aap,
			AtomPositionMap &&rap
		#endif
		) : instance(&i), introducedAtoms(ia), introducedRules(ir), rememberedAtoms(ra), rememberedRules(rr)
#ifdef INT_ATOMS_TYPE
		//,/*int_introducedAtoms(iia), int_rememberedAtoms(ira),*/ atomAtPosition(aap)
		,int_introducedAtoms(0), int_rememberedAtoms(0), int_negatedAtoms(0), int_negatedChoiceAtoms(0), int_projectionAtoms(0), int_constrainedFactAtoms(0), int_constrainedNFactAtoms(0), atomAtPosition(aap), int_introducedRules(0), int_rememberedRules(0), ruleAtPosition(rap)
		#ifdef NON_NORM_JOIN
			,int_joinAtoms(0), int_joinRules(0) 
		#endif
#endif
		{}

		TreeNodeInfo() : instance(nullptr)  { }


	#ifdef INT_ATOMS_TYPE
		dynasp::atom_vector project(const atom_container& atoms) const;
		dynasp::atom_vector project(htd::ConstCollection<htd::vertex_t>& atoms) const;
		void transformAndExtract(const dynasp::atom_vector& atoms, htd::vertex_container& atoms_out) const;
		dynasp::atom_vector transform(size_t child) const;
		dynasp::atom_vector transform(const atom_vector &atoms, size_t child) const;
		dynasp::atom_vector rule_transform(const atom_vector &atoms, size_t child) const;
		htd::vertex_t ruleIdOfPosition(size_t pos) const;
		//dynasp::atom_vector transform(const atom_vector &&atoms, size_t child) const;

	#endif
		
	};

} // namespace dynasp

#endif // DYNASP_DYNASP_TREENODEINFO_H_
