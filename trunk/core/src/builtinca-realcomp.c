#include <complearn/envmap.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <complearn/datablock.h>
#include <complearn/complearn.h>
#include "clalloc.h"
#include <string.h>
#include <unistd.h>
#include <signal.h>

/** \brief a structure containing instance information for a real compressor
 * \struct RealCompInstance
 *
 * This structure holds all information relevant for a real compressor.
 * A real compressor is invoked in a way similar to the virtual compressor.
 * That is, a process is forked, a given external command filename is
 * executed, and the uncompressed input data are fed to standard input.
 * Then the resultant output from the child process (the real compressor)
 * is counted to determine its length, in bytes and then converted to
 * bits.  This resultant length is returned to the CompLearn system.
 * A real compressor is often the most convenient interface with which to
 * use a custom compressor, as often a simple one-line shell script is
 * sufficient to make a real compressor out of any clFunctional compression
 * program.
 */
struct RealCompInstance {
	char *cmd;
  int bytecount;
};


//static void rc_clsetenv(struct CompAdaptor *ca, struct EnvMap *em);
static double rc_compclFunc(struct CompAdaptor *ca, struct DataBlock *src);
static void rc_freecompclFunc(struct CompAdaptor *ca);
static char *rc_shortname(void);
static char *rc_longname(void);
static int rc_apiver(void);

struct CompAdaptor *clBuiltin_RealComp(const char *cmd)
{
	struct CompAdaptor c =
	{
    cptr: NULL,
//    se:   rc_clsetenv,
    cf:   rc_compclFunc,
    fcf:  rc_freecompclFunc,
    sn:   rc_shortname,
    ln:   rc_longname,
    apiv: rc_apiver,
  };
  struct CompAdaptor *ca;
	struct RealCompInstance *rci;
  ca = clCalloc(sizeof(*ca), 1);
  *ca = c;

  ca->cptr = clCalloc(sizeof(struct RealCompInstance), 1);
  rci = (struct RealCompInstance *) ca->cptr;

  if (cmd) {
    rci->cmd = clStrdup(cmd);
  }
  else {
    clogError( "Error, no command specified for realcomp\n");
    exit(1);
  }
  clCompaInitParameters(ca);
  return ca;
}

void clZombie_reaper(int q)
{
  int stat;
  while(waitpid(-1, &stat, WNOHANG) > 0) ;
}

/** \brief Returns an fd to read the output of given command with DataBlock
 * fed in through stdin.
 * \param inp pointer to DataBlock to feed in to external program
 * \param cmd string indicating pathname for external program
 * \return fd that may be read to get data from external program stdout
 */
int clForkPipeExecAndFeed(struct DataBlock *inp, const char *cmd)
{
	int pout[2], pin[2];
  int retval;
  int childid;
  retval = pipe(pout);
  if (retval)
    clogError("pipe");
  retval = pipe(pin);
  if (retval)
    clogError("pipe");
  signal(SIGCHLD, (void(*)(int))clZombie_reaper);
  childid = fork();
  if (childid < 0) { // An error
        clogError("fork");
      }
  if (childid) { // parent
    int wlen, wtot = 0, wleft;
    wleft = clDatablockSize(inp);
    while (wleft > 0) {
      wlen = write(pout[1], clDatablockData(inp)+wtot, wleft);
      if (wlen < 0) {
        clogError("write");
        continue;
      }
      wtot += wlen;
      wleft -= wlen;
    }
    close(pout[1]);
    close(pout[0]);
    close(pin[1]);
  } else       { // child
    close(pin[0]);
    close(pout[1]);
    retval = dup2(pout[0],0);
    if (retval < 0)
      clogError("dup2");
    retval = dup2(pin[1],1);
    if (retval < 0)
      clogError("dup2");
    execl(cmd, cmd, NULL);
    printf("Shouldn't be here, wound up returning from exec!!\n");
    exit(1);
  }
  return pin[0];
}

static double rc_compclFunc(struct CompAdaptor *ca, struct DataBlock *src)
{
	struct RealCompInstance *ci = (struct RealCompInstance *) ca->cptr;
#define READBLOCKSIZE 16384
  static char dummy[READBLOCKSIZE];
  int readfd, readlen;

  ci->bytecount = 0;
  readfd = clForkPipeExecAndFeed(src, ci->cmd);
  while ((readlen = read(readfd, &dummy[0], READBLOCKSIZE)) > 0) {
    ci->bytecount += readlen;
  }
  close(readfd);
//  printf("Got bytecount %f\n", (float) ci->bytecount);
  return ci->bytecount * 8.0;
}

static void rc_freecompclFunc(struct CompAdaptor *ca)
{
	struct RealCompInstance *ci = (struct RealCompInstance *) ca->cptr;
  clFreeandclear(ci->cmd);
  clFreeandclear(ca->cptr);
	clFreeandclear(ca);
}

static char *rc_shortname(void)
{
	return "realcomp";
}

static char *rc_longname(void)
{
	return "real external compressor command";
}

static int rc_apiver(void)
{
	return APIVER_V1;
}

