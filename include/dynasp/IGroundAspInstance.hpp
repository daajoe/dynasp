#ifndef DYNASP_DYNASP_IGROUNDASPINSTANCE_H_
#define DYNASP_DYNASP_IGROUNDASPINSTANCE_H_

#include <dynasp/global>

#include <dynasp/Atom.hpp>
#include <dynasp/Rule.hpp>
#include <dynasp/IGroundAspRule.hpp>

#include <sharp/main>

#include <htd/main.hpp>

#include <string>
#include <utility>
#include <cstddef>
#include <unordered_map>

namespace dynasp
{
	struct TreeNodeInfo;
	class DYNASP_API IGroundAspInstance : public sharp::IInstance
	{
	protected:
		IGroundAspInstance &operator=(IGroundAspInstance &) { return *this; }

	public:
		virtual ~IGroundAspInstance() = 0;

		virtual void addAtomName(atom_t atom, const std::string &name) = 0;
		virtual void addAtom(atom_t atom) = 0;
		virtual void addRule(IGroundAspRule *rule) = 0;
		virtual void addWeight(
				atom_t atom,
				bool negated,
				std::size_t weight) = 0;

        virtual std::size_t getNumAtoms() = 0;
        virtual std::size_t getNumRules() = 0;
        virtual std::size_t getNumConstraints() = 0;
        virtual std::size_t getNumBinaryConstraints() = 0;
        virtual std::size_t getNumTernaryConstraints() = 0;
        virtual std::size_t getNumOtherConstraints() = 0;

        virtual std::size_t getNumCardinalityRules() = 0;
        virtual std::size_t getNumChoiceRules() = 0;
        virtual std::size_t getNumDisjunctiveRules() = 0;
        virtual std::size_t getNumNormalRules() = 0;
        virtual std::size_t getNumWeightedRules() = 0;


        virtual bool isRule(htd::vertex_t vertex) const = 0;
		virtual bool isAtom(htd::vertex_t vertex) const = 0;
		virtual bool isNegatedAtom(htd::vertex_t vertex) = 0;
		virtual void setSpeedup(bool s) = 0;

		virtual const IGroundAspRule &rule(rule_t rule) const = 0;

		virtual std::size_t weight(
				const atom_vector &trueAtoms,
				const atom_vector &falseAtoms, const TreeNodeInfo& info) const = 0;

		//#ifdef INT_ATOMS_TYPE
		//typedef TreeNodeInfo NodeData;
		/*struct NodeData {
			dynasp::atom_vector int_introducedAtoms,
				int_rememberedAtoms;

			const htd::vertex_container introducedAtoms, rememberedAtoms;
			AtomPositionMap atomAtPosition;

			NodeData(dynasp::atom_vector& i, dynasp::atom_vector& r, htd::vertex_container&& ia, htd::vertex_container&& ra, AtomPositionMap&& ap) : int_introducedAtoms(i), int_rememberedAtoms(r), introducedAtoms(ia), rememberedAtoms(ra), atomAtPosition(ap) { }
		};*/

		virtual void setNodeData(size_t node, TreeNodeInfo&& data) = 0;
		virtual const TreeNodeInfo& getNodeData(size_t node) const = 0;
		virtual TreeNodeInfo& getNodeData(size_t node) = 0;
		//dynasp::atom_vector getNodeBag(size_t node);
	//#endif


	}; // class IGroundAspInstance

	inline IGroundAspInstance::~IGroundAspInstance() { }

} // namespace dynasp

#endif // DYNASP_DYNASP_IGROUNDASPINSTANCE_H_
