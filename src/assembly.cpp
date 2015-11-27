#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif

#include <dynasp/global>

extern "C"
{
	DYNASP_API const char *dynasp_version();
}

const char *dynasp_version()
{
	return VERSION;
}
