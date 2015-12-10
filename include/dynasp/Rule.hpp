#ifndef DYNASP_DYNASP_RULE_H_
#define DYNASP_DYNASP_RULE_H_

#include <dynasp/global>

#include <htd/main>

#include <vector>

namespace dynasp
{
	typedef htd::vertex_t rule_t;
	typedef std::vector<rule_t> rule_vector;

	struct Rule
	{

	private:
		Rule();
	};

} // namespace dynasp

#endif // DYNASP_DYNASP_RULE_H_
