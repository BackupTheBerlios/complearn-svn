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
#include <string.h>
#include <complearn/complearn.h>

double clCompaCompress(struct CompAdaptor *ca, struct DataBlock *src)
{
  return ca->cf(ca, src);
}

void clCompaFree(struct CompAdaptor *ca)
{
  clParamlistFree(ca->pl);
  ca->fcf(ca);
}

char *clCompaShortName(struct CompAdaptor *ca)
{
  return ca->sn();
}
char *clCompaLongName(struct CompAdaptor *ca)
{
  return ca->ln();
}
int clCompaAPIVer(struct CompAdaptor *ca)
{
  return ca->apiv();
}

double clCompaNCD(struct CompAdaptor *comp, struct DataBlock *a, struct DataBlock *b )
{
/* temporary hard-coded compressor: bzip */
	struct DataBlock *ab, *ba;
	double ca, cb, cab, cba;

	ab = clDatablockCatPtr(a,b);
	ba = clDatablockCatPtr(b,a);

	ca = clCompaCompress(comp, a);
	cb = clCompaCompress(comp, b);
	cab = clCompaCompress(comp, ab);
	cba = clCompaCompress(comp, ba);

  clDatablockFreePtr(ab);
  clDatablockFreePtr(ba);

/* temporary hard-coded ncd variation */
	return clMndf(ca,cb,cab,cba);
}

/** \brief Should be called from each CompAdaptor Implementation constructor
 */
void clCompaInitParameters(struct CompAdaptor *ca)
{
  ca->pl = clParamlistNew();
}

struct ParamList *clCompaParameters(struct CompAdaptor *comp)
{
  return clParamlistClone(comp->pl);
}

void clCompaPushParameter(struct CompAdaptor *ca, const char *key, const char *value, int type)
{
  struct ParamList *pl = ca->pl;
  pl->fields[pl->size] = clFielddescNew(key,value,type);
  assert(pl->fields[pl->size]);
  pl->size += 1;
}

void clCompaSetValueForKey(struct CompAdaptor *ca, const char *key, void *dest)
{
  struct ParamList *pl = ca->pl;
  char *value;
  int i;
  assert(pl->em);
  value = clEnvmapValueForKey(pl->em,key);
  for ( i = 0; i < pl->size; i += 1 ) {
    if (strcmp(pl->fields[i]->key,key) == 0) {
      if (value) {
        clFree(pl->fields[i]->value);
        pl->fields[i]->value = clStrdup(value);
      }
      clParamlistGetValue(pl, key, dest, clParamlistParamType(pl, key));
      return;
    }
  }
  clogError( "Error: can not set key %s without default value.\n", key);
  assert(0);
}
