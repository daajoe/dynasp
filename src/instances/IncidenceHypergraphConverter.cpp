#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif
#include "../util/debug.hpp"

#include "IncidenceHypergraphConverter.hpp"

#include <typeinfo>
#include <stdexcept>

namespace dynasp
{
	using htd::vertex_t;
	using htd::IHypergraph;
	using htd::IMutableHypergraph;
	using htd::HypergraphFactory;

	IncidenceHypergraphConverter::IncidenceHypergraphConverter() { }

	IncidenceHypergraphConverter::~IncidenceHypergraphConverter() { }

	IHypergraph *IncidenceHypergraphConverter::convert(
			const IGroundAspInstance &instance) const
	{
		if(typeid(instance) == typeid(GroundAspInstance))
			return this->convert(
					dynamic_cast<const GroundAspInstance &>(instance));
		
		//TODO: implement this method for the general case
		throw std::logic_error("not implemented yet");
	}

	IHypergraph *IncidenceHypergraphConverter::convert(
			const GroundAspInstance &instance) const
	{
		HypergraphFactory &factory = HypergraphFactory::instance();
		IMutableHypergraph *hypergraph = factory.getHypergraph();

		for(size_t vertex = instance.maxAtom_; vertex > 0; --vertex)
			hypergraph->addVertex();

		for(IGroundAspRule * const &rule : instance.rules_)
		{
			vertex_t ruleVertex = hypergraph->addVertex();
			
			for(atom_t atom : *rule)
			{
				DBG("edge: ["); DBG(ruleVertex); DBG(","); DBG(atom); DBG("]");
				DBG(std::endl);

				hypergraph->addEdge(ruleVertex, atom);
			}
		}

		return hypergraph;
	}

} // namespace dynasp
