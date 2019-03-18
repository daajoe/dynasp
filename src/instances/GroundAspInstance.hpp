#ifndef DYNASP_INSTANCES_GROUNDASPINSTANCE_H_
#define DYNASP_INSTANCES_GROUNDASPINSTANCE_H_

#include <dynasp/global>

#include <dynasp/IGroundAspInstance.hpp>

#include <unordered_map>
#include <vector>

namespace dynasp
{
	class PrimalHypergraphConverter;
	class IncidenceHypergraphConverter;

	class DYNASP_LOCAL GroundAspInstance : public IGroundAspInstance
	{
	public:
		GroundAspInstance();
		virtual ~GroundAspInstance();

		virtual float cost(const atom_vector &trueAtoms,
				const atom_vector &falseAtoms, const TreeNodeInfo& info) const;


		virtual void addAtomName(atom_t atom, const std::string &name);
		virtual void addAtom(htd::vertex_t vertex);
		virtual void addRule(IGroundAspRule *rule);
		virtual void addWeight(
				atom_t atom,
				bool negated,
				std::size_t weight);

		virtual htd::IHypergraph *toHypergraph() const;

		virtual void cleanTmps();

		virtual bool isRule(htd::vertex_t vertex) const;
		virtual bool isAtom(htd::vertex_t vertex) const;
 		virtual bool isNegatedAtom(htd::vertex_t vertex);
		virtual bool isProjectionAtom(htd::vertex_t vertex);
 		virtual bool isNegatedChoiceAtom(htd::vertex_t vertex);
		virtual bool isConstrainedFactAtom(htd::vertex_t vertex);
		virtual bool isConstrainedNFactAtom(htd::vertex_t vertex);
 		virtual void setSpeedup(bool speed);
		virtual unsigned int getAtomCount() const;

		virtual void fetchDependencyGraph(htd::DirectedGraph& graph) const;


		virtual std::string toString() const;

		virtual const IGroundAspRule &rule(htd::vertex_t rule) const;

		virtual std::size_t weight(
				const atom_vector &trueAtoms,
				const atom_vector &falseAtoms, const TreeNodeInfo& info) const;

	//#ifdef INT_ATOMS_TYPE
		/*struct NodeData
		{
			size_t int_introducedAtoms,
				int_rememberedAtoms;
		};*/
		
		void setNodeData(size_t node, TreeNodeInfo&& data);
		const TreeNodeInfo& getNodeData(size_t node) const;
		bool hasNodeData(size_t node) const;
		TreeNodeInfo& getNodeData(size_t node);
	//#endif

		void addNextExtensionPtr(const std::size_t p) { e.push_back(p); }
		std::size_t newAnswerSet(const std::vector<htd::vertex_t>& aset) { ++answers_; e.clear(); aset_ = &aset; return answers_; }

		std::size_t currentAnswerSetID() const { return answers_; }
		const std::vector<htd::vertex_t>* currentAnswerSet() const { return aset_; }

		virtual std::string toQBFString() const;
		virtual const std::string& getName(atom_t atom) const { return atomNames_.find(atom)->second; }
	private:
		ExtensionPointerID e;
		atom_t maxAtom_; bool negativesComputed_;
		std::unordered_map<atom_t, std::string> atomNames_;
		std::unordered_map<atom_t, std::size_t> positiveAtomWeights_;
		std::unordered_map<atom_t, std::size_t> negativeAtomWeights_;
		std::unordered_map<atom_t, float> costs_;
		std::unordered_map<std::string, float> tmpcosts_;
		std::vector<IGroundAspRule *> rules_;
		std::unordered_set<atom_t> negatives_, negativeChoices_, constrainedFactAtoms_, constrainedNFactAtoms_;
	//#ifdef INT_ATOMS_TYPE
		std::unordered_map<size_t, TreeNodeInfo> nodeData;
		
		std::unordered_map<atom_t, IGroundAspRule*> cacheCOMP_;

	protected:
		std::size_t answers_;
		const std::vector<htd::vertex_t>* aset_;
		//bool speed;
	//#endif
	public:
		friend class PrimalHypergraphConverter;
		friend class IncidenceHypergraphConverter;
		friend class IncidencePrimalHypergraphConverter;

	}; // class GroundAspInstance

} // namespace dynasp

#endif // DYNASP_INSTANCES_GROUNDASPINSTANCE_H_
