#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../util/debug.hpp"

#include "DisjunctiveIncidencePrimalHypergraphConverter.hpp"

#include <vector>
#include <typeinfo>
#include <stdexcept>

extern htd::LibraryInstance *inst;

namespace dynasp {
    using htd::vertex_t;
    using htd::IHypergraph;
    using htd::IMutableHypergraph;
    using htd::HypergraphFactory;

    using std::vector;

    DisjunctiveIncidencePrimalHypergraphConverter::DisjunctiveIncidencePrimalHypergraphConverter() {}

    DisjunctiveIncidencePrimalHypergraphConverter::~DisjunctiveIncidencePrimalHypergraphConverter() {}

    IHypergraph *DisjunctiveIncidencePrimalHypergraphConverter::convert(
            const IGroundAspInstance &instance) const {
        if (typeid(instance) == typeid(GroundAspInstance))
            return this->convert(
                    dynamic_cast<const GroundAspInstance &>(instance));

        //TODO: implement this method for the general case
        throw std::logic_error("not implemented yet");
    }

    IHypergraph *DisjunctiveIncidencePrimalHypergraphConverter::convert(
            const GroundAspInstance &instance) const {
        HypergraphFactory factory(inst);
        //HypergraphFactory &factory = HypergraphFactory::instance();
        IMutableHypergraph *hypergraph = factory.getHypergraph();

#ifdef DEBUG
        instance.printSymbolTable();
#endif

        std::unordered_set<atom_t> disj_vertices;
        int pre_size = -1;
        bool unchanged = true;
        while (pre_size - (int) disj_vertices.size() < 0) {
            pre_size = disj_vertices.size();
            for (IGroundAspRule *const &rule : instance.rules_) {
                if (rule->getHeadSize() > 1 && !rule->hasChoiceHead()) {
                    disj_vertices.insert(rule->getHead().begin(), rule->getHead().end());
                };

                for (auto atom : disj_vertices) {
                    if (rule->getPosBody().count(atom) > 0) {
                        disj_vertices.insert(rule->getHead().begin(), rule->getHead().end());
                    }
                }
            }
        }

        for (size_t vertex = instance.maxAtom_; vertex > 0; --vertex) {
            hypergraph->addVertex();
        }

	vertex_t rid = instance.maxAtom_;

        for (IGroundAspRule *const &rule : instance.rules_) {
            hypergraph->addVertex();
            ++rid;

            for (auto atom: disj_vertices) {
                if (rule->getHead().count(atom) > 0) {
                    for (const atom_t &atom2: rule->getHead()) {
			hypergraph->addEdge(atom2, rid);
                    }
                    for (const auto &atom2: rule->getPosBody()) {
                        if (disj_vertices.count(atom2.first) > 0) {
			    hypergraph->addEdge(atom2.first, rid);
                        }
                    }
                    //Construct cliques
		    if (rule->hasWeightedBody()) {
            		vector<vertex_t> edge;
			for (const auto &atom2: rule->getPosBody()) {
                        	if (disj_vertices.count(atom2.first) > 0)
					edge.push_back(atom2.first);
                    	}
			hypergraph->addEdge(htd::ConstCollection<vertex_t>(
                            edge.begin(), edge.end()));
		    }
		    if (rule->hasChoiceHead()) {
            		vector<vertex_t> edge;
			for (const atom_t &atom2: rule->getHead())
				edge.push_back(atom2);
			hypergraph->addEdge(htd::ConstCollection<vertex_t>(
                            edge.begin(), edge.end()));
		    }
                    break;
                }
            }

        }

#ifdef DEBUG
        for (auto e: hypergraph->hyperedges()) {
            std::cout << e.elements() << std::endl;
        }
#endif

        DBG("edge: "); DBG_COLL(edge); DBG(std::endl);
        return hypergraph;
    }

} // namespace dynasp
