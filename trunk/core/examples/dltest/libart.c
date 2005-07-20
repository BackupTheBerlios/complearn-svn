#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <complearn/complearn.h>

void clsetenv(struct CompAdaptor *ca, struct EnvMap *em);
double art_compfunc(struct CompAdaptor *ca, struct DataBlock src);
void art_freecompfunc(struct CompAdaptor *ca);
char *art_shortname(void);
char *art_longname(void);
char *art_compparam(struct CompAdaptor *ca);
int art_apiver(void);
/** \brief The arithmetic coder instance information
 *  \struct artCompInstance
 *
 *  This structure holds all persistant instance information for an arithmetic
 *  coder.  There is only one example parameter, padding, which is ignored; it
 *  is only used for testing and demonstration.  For examples of a real
 *  parameter utilization as well, see the builtin zlib (builtinca-zlib.c)
 *  adaptor.
 */
struct artCompInstance {
	int padding;
  char paramDesc[256];
};

struct CompAdaptor *newCompAdaptor(void)
{
  int i;
	const static struct CompAdaptor c =
	{
    cptr: NULL,
    cf:   art_compfunc,
    fcf:  art_freecompfunc,
    sn:   art_shortname,
    ln:   art_longname,
    cp:   art_compparam,
    apiv: art_apiver
  };
  struct CompAdaptor *ca;

  static char result[1024];
  struct artCompInstance *aci;

  char *val;
  double d;
  struct GeneralConfig *gconf;
  struct EnvMap *em;
  gconf = loadDefaultEnvironment();
  assert(gconf);
  em = getEnvMap(gconf);
  assert(em);

  ca = calloc(sizeof(*ca), 1);
  *ca = c;

  ca->cptr = calloc(sizeof(*aci), 1);
  aci = (struct artCompInstance *) ca->cptr;

  aci->padding = 0;

  val = readValForEM(em, "padding");

  if (val) {
    aci->padding = atoi(val);
  }

  sprintf(aci->paramDesc, "padding:%d", aci->padding);

  return ca;
}

double art_compfunc(struct CompAdaptor *ca, struct DataBlock src)
{
  int c[256];
	int sum = 0;
	int i;
	double codelen = 0.0;

	for (i = 0; i < 256 ; i++) {
		c[i] = 1;
	}
	sum = 256;

	for (i = 0; i < src.size; i++) {
		unsigned char cur;
		double prob;
		cur = src.ptr[i];
		prob = ((double) c[cur]) / ((double) sum);
		codelen -= log(prob);
		c[cur] += 1;
		sum += 1;
	}
	return codelen/log(2);
}

void art_freecompfunc(struct CompAdaptor *ca)
{
  free(ca->cptr);
	free(ca);
}

char *art_shortname(void)
{
  return "art";
}

char *art_longname(void)
{
	return "order-0 adaptive arithmetic encoder";
}

char *art_compparam(struct CompAdaptor *ca)
{
  struct artCompInstance *aci = (struct artCompInstance *) ca->cptr;
  return aci->paramDesc;
}

int art_apiver(void)
{
  return 1;
}
