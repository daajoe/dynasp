#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif
#include "debug.hpp"

#ifdef DEBUG
std::ostream &dynasp::Debug::stream = std::cout;
#endif // DEBUG
