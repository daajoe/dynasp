#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif
#include "../util/debug.hpp"

#include "PrimalHypergraphConverter.hpp"

#include <vector>
#include <typeinfo>
#include <stdexcept>
#include "dynasp/create.hpp"
#include <fstream>

extern htd::LibraryInstance* inst;

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

		//std::cout << instance.maxAtom_ << std::endl;

		size_t edgeCount = 0;
		for(IGroundAspRule * const &rule : instance.rules_)
		{
			vector<vertex_t> edge;
			edge.push_back(hypergraph->addVertex());

			size_t atomCnt = 0;
			for(const atom_t &atom : *rule)
			{
				edge.push_back(atom);
				atomCnt++;
			}

			DBG("edge: "); DBG_COLL(edge); DBG(std::endl);

			hypergraph->addEdge(htd::ConstCollection<vertex_t>(
						edge.begin(), edge.end()));
			if (pace.length())
			{
				edgeCount += (atomCnt * (atomCnt - 1)) / 2;
				for(const atom_t &atom : *rule)
				{
					for(const atom_t &atom2 : *rule)
					{
						if (atom2 > atom)
							fo << "e " << atom << " " << atom2 << std::endl;
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
