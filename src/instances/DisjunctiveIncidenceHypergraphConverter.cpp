#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../util/debug.hpp"

#include "DisjunctiveIncidenceHypergraphConverter.hpp"

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

    DisjunctiveIncidenceHypergraphConverter::DisjunctiveIncidenceHypergraphConverter() {}

    DisjunctiveIncidenceHypergraphConverter::~DisjunctiveIncidenceHypergraphConverter() {}

    IHypergraph *DisjunctiveIncidenceHypergraphConverter::convert(
            const IGroundAspInstance &instance) const {
        if (typeid(instance) == typeid(GroundAspInstance))
            return this->convert(
                    dynamic_cast<const GroundAspInstance &>(instance));

        //TODO: implement this method for the general case
        throw std::logic_error("not implemented yet");
    }

    IHypergraph *DisjunctiveIncidenceHypergraphConverter::convert(
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
                    //Construct edges
                    for (const atom_t &atom2: rule->getHead()) {
			hypergraph->addEdge(atom2, rid);
                    }
                    for (const auto &atom2: rule->getPosBody()) {
                        if (disj_vertices.count(atom2.first) > 0) {
			    hypergraph->addEdge(atom2.first, rid);
                        }
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
