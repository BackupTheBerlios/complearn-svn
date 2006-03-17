/*
* Copyright (c) 2006 Rudi Cilibrasi, Rulers of the RHouse
* All rights reserved.     cilibrar@cilibrar.com
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the RHouse nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE RULERS AND CONTRIBUTORS "AS IS" AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE RULERS AND CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <complearn/complearn.h>

void clsetenv(struct CompAdaptor *ca, struct EnvMap *em);
double art_compclFunc(struct CompAdaptor *ca, struct DataBlock src);
void art_freecompclFunc(struct CompAdaptor *ca);
char *art_shortname(void);
char *art_longname(void);
char *art_compparam(struct CompAdaptor *ca);
int art_apiver(void);
/** \brief The arithmetic coder instance information
 *
 *  This structure holds all persistant instance information for an arithmetic
 *  coder.  There is only one example parameter, padding, which is ignored; it
 *  is only used for testing and demonstration.  For examples of a real
 *  parameter utilization as well, see the builtin zlib (builtinca-zlib.c)
 *  adaptor.
 *  \struct artCompInstance
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
    cf:   art_compclFunc,
    fcf:  art_freecompclFunc,
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
  gconf = clLoadDefaultEnvironment();
  assert(gconf);
  em = clGetEnvMap(gconf);
  assert(em);

  ca = calloc(sizeof(*ca), 1);
  *ca = c;

  ca->cptr = calloc(sizeof(*aci), 1);
  aci = (struct artCompInstance *) ca->cptr;

  aci->padding = 0;

  val = clEnvmapValueForKey(em, "padding");

  if (val) {
    aci->padding = atoi(val);
  }

  sprintf(aci->paramDesc, "padding:%d", aci->padding);

  return ca;
}

double art_compclFunc(struct CompAdaptor *ca, struct DataBlock src)
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

void art_freecompclFunc(struct CompAdaptor *ca)
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
