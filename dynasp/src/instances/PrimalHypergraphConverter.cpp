#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif
#include "../util/debug.hpp"

#include "PrimalHypergraphConverter.hpp"

#include <vector>
#include <typeinfo>
#include <stdexcept>

namespace dynasp
{
	using htd::vertex_t;
	using htd::IHypergraph;
	using htd::IMutableHypergraph;
	using htd::HypergraphFactory;

	using std::vector;

	PrimalHypergraphConverter::PrimalHypergraphConverter() { }

	PrimalHypergraphConverter::~PrimalHypergraphConverter() { }

	IHypergraph *PrimalHypergraphConverter::convert(
			const IGroundAspInstance &instance) const
	{
		if(typeid(instance) == typeid(GroundAspInstance))
			return this->convert(
					dynamic_cast<const GroundAspInstance &>(instance));
		
		//TODO: implement this method for the general case
		throw std::logic_error("not implemented yet");
	}

	IHypergraph *PrimalHypergraphConverter::convert(
			const GroundAspInstance &instance) const
	{
		HypergraphFactory &factory = HypergraphFactory::instance();
		IMutableHypergraph *hypergraph = factory.getHypergraph();

		for(size_t vertex = instance.maxAtom_; vertex > 0; --vertex)
			hypergraph->addVertex();

		for(IGroundAspRule * const &rule : instance.rules_)
		{
			vector<vertex_t> edge;
			edge.push_back(hypergraph->addVertex());

			for(const atom_t &atom : *rule)
				edge.push_back(atom);

			DBG("edge: "); DBG_COLL(edge); DBG(std::endl);

			hypergraph->addEdge(htd::ConstCollection<vertex_t>(
						edge.begin(), edge.end()));
		}

		return hypergraph;
	}

} // namespace dynasp
