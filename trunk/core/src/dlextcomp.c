#include <stdio.h>
#include <stdlib.h>
#include <complearn/complearn.h>

#if DLFCN_RDY

static void *dl_musthavesymbol(void *dlhandle, const char *str)
{
	void *result;
	result = dlsym(dlhandle, str);
	if (result == NULL) {
		clogError( "Error, symbol %s not defined for "
                    "dynamic library compressor\n", str);
		exit(1);
	}
	return result;
}

struct CompAdaptor *compaLoadDynamicLib(const char *libraryname)
{
	void *dlhandle;
  struct CompAdaptor *c;
  t_clnewca nca;
	dlhandle = dlopen(libraryname, RTLD_LAZY);
	if (dlhandle == NULL) {
		clogError( "Error opening dynamic library %s\n", libraryname);
		exit(1);
	}

  nca = (t_clnewca) dl_musthavesymbol(dlhandle, FUNCNAMENCA);
  c = nca();

	return c;
}

#else

struct CompAdaptor *compaLoadDynamicLib(const char *libraryname)
{
  return NULL;
}

#endif
