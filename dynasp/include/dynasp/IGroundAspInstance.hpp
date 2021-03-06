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

namespace dynasp
{

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

		virtual bool isRule(htd::vertex_t vertex) const = 0;
		virtual bool isAtom(htd::vertex_t vertex) const = 0;

		virtual const IGroundAspRule &rule(rule_t rule) const = 0;

		virtual std::size_t weight(
				const atom_vector &trueAtoms,
				const atom_vector &falseAtoms) const = 0;

	}; // class IGroundAspInstance

	inline IGroundAspInstance::~IGroundAspInstance() { }

} // namespace dynasp

#endif // DYNASP_DYNASP_IGROUNDASPINSTANCE_H_
