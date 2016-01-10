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

		virtual void addAtomName(atom_t atom, const std::string &name);
		virtual void addAtom(htd::vertex_t vertex);
		virtual void addRule(IGroundAspRule *rule);
		virtual void addWeight(
				atom_t atom,
				bool negated,
				std::size_t weight);

		virtual htd::IHypergraph *toHypergraph() const;

		virtual bool isRule(htd::vertex_t vertex) const;
		virtual bool isAtom(htd::vertex_t vertex) const;

		virtual const IGroundAspRule &rule(htd::vertex_t rule) const;

		virtual std::size_t weight(
				const atom_vector &trueAtoms,
				const atom_vector &falseAtoms) const;

	private:
		atom_t maxAtom_;
		std::unordered_map<atom_t, std::string> atomNames_;
		std::unordered_map<atom_t, std::size_t> positiveAtomWeights_;
		std::unordered_map<atom_t, std::size_t> negativeAtomWeights_;
		std::vector<IGroundAspRule *> rules_;

	public:
		friend class PrimalHypergraphConverter;
		friend class IncidenceHypergraphConverter;

	}; // class GroundAspInstance

} // namespace dynasp

#endif // DYNASP_INSTANCES_GROUNDASPINSTANCE_H_
