#ifndef DYNASP_ASPINSTANCE_HPP_
#define DYNASP_ASPINSTANCE_HPP_

#include <set>
#include "AspRules.hpp"
#include <map>

using namespace std;

namespace dynasp
{
	typedef std::map<std::string, atom_t> variable_map;	

	class AspInstance
	{
		friend class LParseProblem;
	public:
		// implementieren, aber nicht verändern:
		virtual ~AspInstance();
		
		const rule_set& getCoveredRules(const atom_set& atoms) const; // alle regeln r, wo body(r)+head(r) ein subset von atoms ist; caller has to free set!
		const rule_set& getRulesContaining(const atom_set& atoms) const; // alle regeln, wo atoms ein subset von body(r)+head(r) ist; caller has to free set!

		const atom_set& getBody(const AspRule& rule) const; // die atome die im body von rule vorkommen
		const atom_set& getPositiveBody(const AspRule& rule) const; // die atome die im positiven body von rule vorkommen
		const atom_set& getNegativeBody(const AspRule& rule) const; // die atome die im negativen body von rule vorkommen
		const atom_set& getHead(const AspRule& rule) const; // die atome die im head von rule vorkommen
		const atom_set& getAtoms(const AspRule& rule) const; // die atome die in rule vorkommen

		bool isRuleSatisfied(const AspRule& rule, const atom_set& positiveAtoms, const atom_set& negativeAtoms) const; // true wenn rule mit diesem assignment satisfied; false wenn nicht, oder wenn unbekannt/zu wenig information

		int compareAtoms(const atom_t& atom1, const atom_t& atom2) const; // < 0 wenn atom1 kleiner, 0 wenn gleich oder unvergleichbar, und > 0 wenn atom2 kleiner. kann im moment eine dummy-implementierung sein, soll aber schlussendlich die partielle reihenfolge im dependency graph widerspiegeln

		// hier dann zeug einfügen (z.B. Konstruktor), das du für die implementierung brauchst (entweder private oder public, je nachdem, obs nach außen hin nützlich sein könnte)
		
		//methods for getting information concerning variable mapping
		const atom_t& getAtom(const std::string& name) const;
		const std::string& getAtomName(const atom_t& atom) const;
	
		static const std::string ATOM_NAME_NOT_FOUND;
		static const atom_t ATOM_NOT_FOUND;
	private:
		variable_map vmap;
		rule_set rules;

		void addRule(AspRule& rule);
		void addVariableMapping(const atom_t& atom, const std::string& name);
	};

	// deine Parser-Klasse braucht noch eine Methode AspInstance *getInstance() oder so, um die AspInstance zu erzeugen: 1) instanz erzeugen, 2) parser laufen lassen und instanz befüllen, 3) instanz zurückgeben
}

#endif // DYNASP_ASPINSTANCE_HPP_
