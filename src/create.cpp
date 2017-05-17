#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "instances/GroundAspRule.hpp"
#include "instances/GroundAspInstance.hpp"
#include "instances/PrimalHypergraphConverter.hpp"
#include "instances/DisjunctivePrimalHypergraphConverter.hpp"
#include "instances/DisjunctiveIncidenceHypergraphConverter.hpp"
#include "instances/DisjunctiveIncidencePrimalHypergraphConverter.hpp"
#include "instances/IncidenceHypergraphConverter.hpp"
#include "instances/IncidencePrimalHypergraphConverter.hpp"
//#include "algorithms/FullDynAspTuple.hpp"
#include "algorithms/SimpleDynAspTuple.hpp"
#include "algorithms/RuleSetDynAspTuple.hpp"
//#include "algorithms/InverseSimpleDynAspTuple.hpp"
#include "algorithms/DynAspCountingSolutionExtractor.hpp"

#include <dynasp/create.hpp>

#include <stdexcept>

namespace dynasp {
    namespace {
        create::ConfigurationType type_ = create::PRIMAL_SIMPLETUPLE;
        bool nonnorm_ = false, redspeed_ = true, compr_ = true;
        unsigned passes_ = 3;
        IGroundAspRuleFactory *ruleFactory_ = nullptr;
        IGroundAspInstanceFactory *instanceFactory_ = nullptr;
        IHypergraphConverterFactory *hypergraphConverterFactory_ = nullptr;
        IDynAspTupleFactory *tupleFactory_ = nullptr;
    }

    create::ConfigurationType create::get() {
        return type_;
    }

    unsigned create::passes() {
        return passes_;
    }

    void create::setPasses(unsigned p) {
        passes_ = p;
    }

    bool create::reductSpeedup() {
        return redspeed_;
    }

    void create::setReductSpeedup(bool rs) {
        redspeed_ = rs;
    }

    bool create::isCompr() {
        return compr_;
    }

    void create::setCompr(bool c) {
        compr_ = c;
    }

    bool create::isNon() {
        return nonnorm_;
    }

    void create::setNon(bool non) {
        nonnorm_ = non;
    }

    void create::set(ConfigurationType type) {
        switch (type) {
            case create::PRIMAL_FULLTUPLE:
            case create::PRIMAL_SIMPLETUPLE:
            case create::PRIMAL_INVERSESIMPLETUPLE:
            case create::INCIDENCE_FULLTUPLE:
            case create::INCIDENCEPRIMAL_FULLTUPLE:
            case create::INCIDENCEPRIMAL_RULESETTUPLE:
            case create::DISJ_PRIMAL_SIMPLETUPLE:
            case create::DISJ_INCIDENCEPRIMAL_FULLTUPLE:
            case create::DISJ_INCIDENCE_FULLTUPLE:
                type_ = type;
                break;

            default:
                throw std::domain_error("Invalid type1.");
                break;
        }
    }

    void create::set(IGroundAspRuleFactory *factory) {
        if (ruleFactory_)
            delete ruleFactory_;
        ruleFactory_ = factory;
    }

    void create::set(IGroundAspInstanceFactory *factory) {
        if (instanceFactory_)
            delete instanceFactory_;
        instanceFactory_ = factory;
    }

    void create::set(IHypergraphConverterFactory *factory) {
        if (hypergraphConverterFactory_)
            delete hypergraphConverterFactory_;
        hypergraphConverterFactory_ = factory;
    }

    void create::set(IDynAspTupleFactory *factory) {
        if (tupleFactory_)
            delete tupleFactory_;
        tupleFactory_ = factory;
    }

    IGroundAspRule *create::rule() {
        if (ruleFactory_) return ruleFactory_->create();
        return create::rule(type_);
    }

    IGroundAspRule *create::rule(ConfigurationType type) {
        switch (type) {
            default:
                return new GroundAspRule();
        }
    }

    IGroundAspInstance *create::instance() {
        if (instanceFactory_) return instanceFactory_->create();
        return create::instance(type_);
    }

    IGroundAspInstance *create::instance(ConfigurationType type) {
        switch (type) {
            default:
                return new GroundAspInstance();
        }
    }

    IHypergraphConverter *create::hypergraphConverter() {
        if (hypergraphConverterFactory_)
            return hypergraphConverterFactory_->create();
        return create::hypergraphConverter(type_);
    }

    IHypergraphConverter *create::hypergraphConverter(ConfigurationType type) {
        switch (type) {
            case create::PRIMAL_FULLTUPLE:
            case create::PRIMAL_SIMPLETUPLE:
            case create::PRIMAL_INVERSESIMPLETUPLE:
                return new PrimalHypergraphConverter();
            case create::INCIDENCE_FULLTUPLE:
                return new IncidenceHypergraphConverter();
            case create::INCIDENCEPRIMAL_FULLTUPLE:
            case create::INCIDENCEPRIMAL_RULESETTUPLE:
                return new IncidencePrimalHypergraphConverter();
            case create::DISJ_PRIMAL_SIMPLETUPLE:
                return new DisjunctivePrimalHypergraphConverter();
            case create::DISJ_INCIDENCE_FULLTUPLE:
                return new DisjunctiveIncidenceHypergraphConverter();
            case create::DISJ_INCIDENCEPRIMAL_FULLTUPLE:
                return new DisjunctiveIncidencePrimalHypergraphConverter();


            default:
                throw std::domain_error("Invalid type2.");
        }
    }

    IDynAspTuple *create::tuple(bool leaf) {
        if (tupleFactory_) return tupleFactory_->create(leaf);
        return create::tuple(leaf, type_);
    }

    IDynAspTuple *create::tuple(bool leaf, ConfigurationType type) {
        switch (type) {
            case create::PRIMAL_FULLTUPLE:
            case create::INCIDENCE_FULLTUPLE:
            case create::INCIDENCEPRIMAL_FULLTUPLE:
            case create::DISJ_INCIDENCE_FULLTUPLE:
            case create::DISJ_INCIDENCEPRIMAL_FULLTUPLE:
                //return new FullDynAspTuple(leaf);

            case create::PRIMAL_SIMPLETUPLE:
            case create::DISJ_PRIMAL_SIMPLETUPLE:
                return new SimpleDynAspTuple(/*leaf*/);

            case create::PRIMAL_INVERSESIMPLETUPLE:
                //return new InverseSimpleDynAspTuple(leaf);

            case create::INCIDENCEPRIMAL_RULESETTUPLE:
                //TODO: remove this stuff here!
                return new RuleSetDynAspTuple(leaf);

            default:
                throw std::domain_error("Invalid type3.");
        }
    }

    IDynAspCountingSolutionExtractor *create::countingSolutionExtractor() {
        //TODO: implement and use factory for this (in sharp)
        return new DynAspCountingSolutionExtractor();
    }

} // namespace dynasp

