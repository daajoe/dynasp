#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif
#include "../util/debug.hpp"

#include "IncidencePrimalHypergraphConverter.hpp"

#include <vector>
#include <typeinfo>
#include <stdexcept>
#include <fstream>
#include "dynasp/create.hpp"

extern htd::LibraryInstance* inst;

namespace dynasp
{
	using htd::vertex_t;
	using htd::IHypergraph;
	using htd::IMutableHypergraph;
	using htd::HypergraphFactory;

	using std::vector;

	IncidencePrimalHypergraphConverter::IncidencePrimalHypergraphConverter() { }

	IncidencePrimalHypergraphConverter::~IncidencePrimalHypergraphConverter()
	{ }

	IHypergraph *IncidencePrimalHypergraphConverter::convert(
			const IGroundAspInstance &instance) const
	{
		if(typeid(instance) == typeid(GroundAspInstance))
			return this->convert(
					dynamic_cast<const GroundAspInstance &>(instance));
		
		//TODO: implement this method for the general case
		throw std::logic_error("not implemented yet");
	}

	IHypergraph *IncidencePrimalHypergraphConverter::convert(
			const GroundAspInstance &instance) const
	{
		HypergraphFactory factory(inst);
		//HypergraphFactory &factory = HypergraphFactory::instance();
		IMutableHypergraph *hypergraph = factory.createInstance();

		for(size_t vertex = instance.maxAtom_; vertex > 0; --vertex)
			hypergraph->addVertex();

		std::string& pace = dynasp::create::getPaceOut();

		std::ofstream fo;
		if (pace.length())
		{
			fo.open(pace.c_str());
			fo << "p" << " graphedge " << "                                                         " << std::endl;
		}

		size_t edgeCount = 0;
		for(IGroundAspRule * const &rule : instance.rules_)
		{
			if(rule->hasWeightedBody())
			{
				vector<vertex_t> edge;
				edge.push_back(hypergraph->addVertex());

				for(const atom_t &atom : *rule)
					edge.push_back(atom);

				DBG("edge: "); DBG_COLL(edge); DBG(std::endl);

				hypergraph->addEdge(htd::ConstCollection<vertex_t>(
							edge.begin(), edge.end()));
			}
			else if(rule->hasChoiceHead())
			{
				vertex_t ruleVertex = hypergraph->addVertex();
				vector<vertex_t> headEdge;
				headEdge.push_back(ruleVertex);

				for(atom_t atom : *rule)
					if(rule->isHeadAtom(atom))
						headEdge.push_back(atom);
					else
					{
						DBG("edge: ["); DBG(ruleVertex); DBG(","); DBG(atom);
						DBG("]"); DBG(std::endl);

						hypergraph->addEdge(ruleVertex, atom);
					}

				DBG("edge: "); DBG_COLL(headEdge); DBG(std::endl);
				hypergraph->addEdge(htd::ConstCollection<vertex_t>(
							headEdge.begin(), headEdge.end()));
			}
			else
			{
				vertex_t ruleVertex = hypergraph->addVertex();
				
				for(atom_t atom : *rule)
				{
					DBG("edge: ["); DBG(ruleVertex); DBG(","); DBG(atom);
					DBG("]"); DBG(std::endl);

					hypergraph->addEdge(ruleVertex, atom);
				}

				if (pace.length())
				{
					for (atom_t atom : *rule)
					{
						fo << "e " << atom << " " << ruleVertex << std::endl;
						edgeCount++;
					}
				}
			}
		}

		if (pace.length())
		{
			fo.seekp(13, std::ios_base::beg);
			fo << instance.maxAtom_ << " " << edgeCount;
			fo.close();
		}

		return hypergraph;
	}

} // namespace dynasp
