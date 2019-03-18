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

	using ExtensionPointerID = std::vector<std::size_t>;
	using AnswerSetContributionCache = std::set<std::size_t>;
	
	struct TreeNodeInfo;
	class DYNASP_API IGroundAspInstance : public sharp::IInstance
	{
	protected:
		IGroundAspInstance &operator=(IGroundAspInstance &) { return *this; }

	public:
		virtual ~IGroundAspInstance() = 0;

		virtual float cost(const atom_vector &trueAtoms,
				const atom_vector &falseAtoms, const TreeNodeInfo& info) const = 0;

		virtual void addAtomName(atom_t atom, const std::string &name) = 0;
		virtual void addAtom(atom_t atom) = 0;
		virtual void addRule(IGroundAspRule *rule) = 0;
		virtual void addWeight(
				atom_t atom,
				bool negated,
				std::size_t weight) = 0;

		virtual bool isRule(htd::vertex_t vertex) const = 0;
		virtual bool isAtom(htd::vertex_t vertex) const = 0;

		virtual bool isConstrainedFactAtom(htd::vertex_t vertex) = 0;
		virtual bool isConstrainedNFactAtom(htd::vertex_t vertex) = 0;
		virtual bool isNegatedAtom(htd::vertex_t vertex) = 0;
		virtual bool isNegatedChoiceAtom(htd::vertex_t vertex) = 0;

		virtual bool isProjectionAtom(htd::vertex_t vertex) = 0;
		virtual void setSpeedup(bool s) = 0;
		virtual unsigned int getAtomCount() const = 0;

		virtual void fetchDependencyGraph(htd::DirectedGraph& graph) const = 0;

		virtual std::string toString() const = 0;

		virtual const IGroundAspRule &rule(rule_t rule) const = 0;


		virtual const std::string& getName(atom_t atom) const = 0;

		virtual std::size_t weight(
				const atom_vector &trueAtoms,
				const atom_vector &falseAtoms, const TreeNodeInfo& info) const = 0;
		virtual std::string toQBFString() const = 0;
		virtual void cleanTmps() = 0;
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
		virtual bool hasNodeData(size_t node) const = 0;

	#ifdef BNB_CRAFT_ALGO
		virtual std::size_t newAnswerSet(const std::vector<htd::vertex_t>& aset) = 0;
		virtual void addNextExtensionPtr(const std::size_t p) = 0;
		virtual std::size_t currentAnswerSetID() const = 0;
		virtual const std::vector<htd::vertex_t>* currentAnswerSet() const = 0;
	#endif



		//dynasp::atom_vector getNodeBag(size_t node);
	//#endif
		
		
	}; // class IGroundAspInstance

	inline IGroundAspInstance::~IGroundAspInstance() { }

} // namespace dynasp

#endif // DYNASP_DYNASP_IGROUNDASPINSTANCE_H_
