#include <complearn/envmap.h>
#include <sys/types.h>
#include "clalloc.h"
#include <complearn/complearn.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

/** \brief A structure containing the instance state for a virtual compressor
 * \struct VirtualCompInstance
 *
 * This structure holds all status information necessary to perform the
 * virtual compression interface.  This includes forking a new process,
 * executing a given compressor command, feeding the input file through
 * a unix pipe, and reading the child process standard output, scanning
 * for an ASCII decimal formatted floating-point number indicating the
 * compressed size, in bits, of the imaginary compressed file.  Note that,
 * unlike the real compressor instance, no actual compression is necessary.
 */
struct VirtualCompInstance {
	char *cmd;
  char sbuf[1024];
  int curpt;
};

//static void vc_clsetenv(struct CompAdaptor *ca, struct EnvMap *em);
int clForkPipeExecAndFeed(const struct DataBlock *inp, const char *cmd);

static double vc_compclFunc(struct CompAdaptor *ca, struct DataBlock *src);
static void vc_freecompclFunc(struct CompAdaptor *ca);
static char *vc_shortname(void);
static char *vc_longname(void);
static int vc_apiver(void);

struct CompAdaptor *clCompaLoadVirtual(const char *cmd)
{
	struct CompAdaptor c =
  {
    cptr: NULL,
//    se:   vc_clsetenv,
    cf:   vc_compclFunc,
    fcf:  vc_freecompclFunc,
    sn:   vc_shortname,
    ln:   vc_longname,
    apiv: vc_apiver,
  };
  struct CompAdaptor *ca;
	struct VirtualCompInstance *vci;
  ca = clCalloc(sizeof(*ca), 1);
  *ca = c;

  ca->cptr = clCalloc(sizeof(struct VirtualCompInstance), 1);
  vci = (struct VirtualCompInstance *) ca->cptr;
  if (cmd) {
    vci->cmd = clStrdup(cmd);
  }
  else {
    clogError( "Error, no command specified for virtcomp\n");
    exit(1);
  }

  clCompaInitParameters(ca);

	return ca;
}

static double vc_compclFunc(struct CompAdaptor *ca, struct DataBlock *src)
{
	struct VirtualCompInstance *ci = (struct VirtualCompInstance *) ca->cptr;
  int readfd;
  char ch;
  readfd = clForkPipeExecAndFeed(src, ci->cmd);
  while (read(readfd, &ch, 1) == 1 && ci->curpt < 512) {
    ci->sbuf[ci->curpt++] = ch;
  }
  ci->sbuf[ci->curpt++] = 0;
  return atof(ci->sbuf);
}

static void vc_freecompclFunc(struct CompAdaptor *ca)
{
	struct VirtualCompInstance *ci = (struct VirtualCompInstance *) ca->cptr;
  clFreeandclear(ci->cmd);
  clFreeandclear(ca->cptr);
	clFreeandclear(ca);
}

static char *vc_shortname(void)
{
	return "virtcomp";
}

static char *vc_longname(void)
{
	return "virtual external compressor command";
}

static int vc_apiver(void)
{
	return APIVER_V1;
}

