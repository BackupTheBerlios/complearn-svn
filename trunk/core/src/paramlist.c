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


struct ParamList *clParamlistNew()
{
   struct ParamList *pl;
   pl = clCalloc(sizeof(struct ParamList), 1);
   pl->em = clLoadDefaultEnvironment()->em;
   return pl;
}

char *clParamlistValForKey(struct ParamList *pl, const char *key)
{
  int i;
  for ( i = 0; i < pl->size; i += 1) {
    if (strcmp(pl->fields[i]->key, key) == 0)
      return clStrdup(pl->fields[i]->value);
  }
  return NULL;
}

int clParamlistParamType(struct ParamList *pl, const char *key)
{
  int i;
  for ( i = 0; i < pl->size; i += 1) {
    if (strcmp(pl->fields[i]->key, key) == 0)
      return pl->fields[i]->type;
  }
  return 0;
}

char *clParamlistGetString(struct ParamList *pl, const char *key)
{
  return clParamlistValForKey(pl, key);
}

int clParamlistGetInt(struct ParamList *pl, const char *key)
{
  char *result;
  if ((result = clParamlistValForKey(pl, key)))
      return atoi(result);
  return -1;
}

double clParamlistGetDouble(struct ParamList *pl, const char *key)
{
  char *result;
  if ((result = clParamlistValForKey(pl, key)))
      return atof(result);
  return -1;
}

int clParamlistGetValue(struct ParamList *pl, const char *key, void *dest, int type)
{
  switch (type) {
    case PARAMSTRING:
      *((char **) dest) = clParamlistGetString(pl, key);
      break;
    case PARAMINT:
      *((int *) dest) = clParamlistGetInt(pl, key);
      break;
    case PARAMDOUBLE:
      *((double *) dest) =  clParamlistGetDouble(pl, key);
      break;
  }
  return CL_OK;
}

char *clParamlistToString(struct ParamList *pl)
{
  char buff[1024];
  int incr = 0;
  int i;
  char *result;
  for ( i = 0; i < pl->size ; i += 1)
    incr+=sprintf(buff+incr,"%s: %s; ",pl->fields[i]->key,pl->fields[i]->value);
  result = clCalloc(incr, 1);
  memcpy(result, buff, incr);
  result[incr-2]='\0';
  return result;
}

struct FieldDesc *clFielddescNew(const char *key, const char *value, int type)
{
  struct FieldDesc *fd;
  fd = clCalloc(sizeof(struct FieldDesc), 1);
  fd->key = clStrdup(key);
  fd->value = clStrdup(value);
  fd->type = type;
  return fd;
}

struct FieldDesc *clFielddescClone(struct FieldDesc *fd)
{
  return clFielddescNew(fd->key, fd->value, fd->type);
}

void clFielddescFree(struct FieldDesc *fd)
{
  assert(fd != NULL);
  clFreeandclear(fd->key);
  clFreeandclear(fd->value);
  clFreeandclear(fd);
}
struct ParamList *clParamlistClone(struct ParamList *pl)
{
   struct ParamList *result;
   int i;
   result = clCalloc(sizeof(struct ParamList), 1);
   result->em = pl->em;
   result->size = pl->size;
   for ( i = 0 ; i < pl->size ; i += 1)
     result->fields[i] = clFielddescClone(pl->fields[i]);
   return result;
}

void clParamlistFree(struct ParamList *pl)
{
  int i;
  for (i = 0; i < pl->size ; i += 1)
    clFielddescFree(pl->fields[i]);
  clFreeandclear(pl);
}
