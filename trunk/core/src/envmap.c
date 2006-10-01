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
#include <string.h>


#include <complearn/complearn.h>
#define MAXINDECES 10

struct EnvMap {
  struct DRA *d;
  struct CLNodeSet *marked;
  struct CLNodeSet *private;
};

#define MAXINDECES 10
struct EnvMap *clEnvmapNew() {
  struct EnvMap *em;
  em = clCalloc(sizeof(struct EnvMap), 1);
  em->d = clDraNew();
  em->marked = clNodesetNew(MAXINDECES);
  em->private = clNodesetNew(MAXINDECES);
  return em;
}

void clEnvmapPrint(struct EnvMap *uem)
{
  struct EnvMap *em;
  int i;
  if (uem == NULL) {
    clLogError("NULL ptr in clEnvmapPrint()\n");
  }
  em = clEnvmapClone(uem);
  printf("ES:\n");
  for (i = 0; i < clDraSize(em->d); ++i)
    printf("%s->%s\n", clDraGetValueAt(em->d,i).sp.key, clDraGetValueAt(em->d,i).sp.val);
  printf("Marked: ");
  clNodesetPrint(em->marked);
  printf("Private: ");
  clNodesetPrint(em->private);
  printf("ES END.\n");
  clEnvmapFree(em);
}

static union PCTypes cloneStringPair(struct StringPair sp)
{
  union PCTypes p;
  p.sp.key = clStrdup(sp.key);
  p.sp.val = clStrdup(sp.val);
  return p;
}

struct EnvMap *clEnvmapClone(struct EnvMap *em)
{
  struct EnvMap *nem;
  int i;
  int sz;
  if (em == NULL) {
    clLogError("NULL ptr in clEnvmapClone()\n");
  }
  sz = clEnvmapSize(em);
  nem = clCalloc(sizeof(struct EnvMap), 1);
  nem->d = clDraNew();
  for (i = 0; i < sz; ++i)
    clDraSetValueAt(nem->d, i, cloneStringPair(clDraGetValueAt(em->d, i).sp));
  nem->marked = clNodesetClone(em->marked);
  nem->private = clNodesetClone(em->private);
  return nem;
}

int clEnvmapIsEmpty(struct EnvMap *em)
{
  if (em == NULL) {
    clLogError("NULL ptr in clEnvmapIsEmpty()\n");
  }
  return clDraSize(em->d) == 0;
}

int clEnvmapSize(struct EnvMap *em)
{
  if (em == NULL) {
    clLogError("NULL ptr in clEnvmapSize()\n");
  }
  return clDraSize(em->d);
}

static int setKeyValAt(struct EnvMap *em, int where, const char *key, const char *val)
{
  union PCTypes p;
  if (em == NULL || key == NULL || val == NULL) {
    clLogError("NULL ptr in clEnvmapSize()\n");
  }
  p.sp.key = clStrdup(key);
  p.sp.val = clStrdup(val);
  clDraSetValueAt(em->d, where, p);
  return CL_OK;
}

void clEnvmapSetKeyMarked(struct EnvMap *em, const char *key)
{
  if (em == NULL || key == NULL) {
    clLogError("NULL ptr in clEnvmapSetKeyMarked()\n");
  }
  clNodesetAddNode(em->marked, clEnvmapIndexForKey(em, key));
}

int clEnvmapSetKeyVal(struct EnvMap *em, const char *key, const char *val)
{
  int i;
  if (em == NULL || key == NULL || val == NULL) {
    clLogError("NULL ptr in clEnvmapSetKeyVal()\n");
  }
  i = clEnvmapIndexForKey(em,key);
  if (i >= 0)
    setKeyValAt(em, i, key, val);
  else {
    union PCTypes p;
    p.sp.key = clStrdup(key);
    p.sp.val = clStrdup(val);
    clDraPush(em->d, p);
  }
  /* to ensure NodeSets are big enough */
  clNodesetRemoveNode(em->marked, clEnvmapSize(em)+1);
  clNodesetRemoveNode(em->private, clEnvmapSize(em)+1);
  return CL_OK;
}

int clEnvmapFree(struct EnvMap *em)
{
  int i, sz;
  static union PCTypes zeroblock;

  if (em == NULL) {
    clLogError("NULL ptr in clEnvmapFree()\n");
  }
  sz = clEnvmapSize(em);

  for (i = 0; i < sz; ++i) {
    union PCTypes p = clDraGetValueAt(em->d, i);
    clFreeandclear(p.sp.key);
    clFreeandclear(p.sp.val);
    clDraSetValueAt(em->d, i, zeroblock);
  }
  clDraFree(em->d);
  em->d = NULL;
  clNodesetFree(em->marked);
  em->marked = NULL;
  clNodesetFree(em->private);
  em->private = NULL;
  clFreeandclear(em);
  return CL_OK;
}

char *clEnvmapValueForKey(struct EnvMap *em, const char *key)
{
  int i;
  char *val = NULL;
  if (em == NULL || key == NULL) {
    clLogError("NULL ptr in clEnvmapValueForKey()\n");
  }
  i = clEnvmapIndexForKey(em,key);
  if (i >= 0) {
    union PCTypes p = clEnvmapKeyValAt(em,clEnvmapIndexForKey(em,key));
    val = p.sp.val;
    clEnvmapSetKeyMarked(em, key);
  }
  return val;
}

union PCTypes clEnvmapKeyValAt(struct EnvMap *em, int where)
{
  if (em == NULL) {
    clLogError("NULL ptr in clEnvmapKeyValAt()\n");
  }
  assert(where >= 0);
  return clDraGetValueAt(em->d, where);
}

int clEnvmapIndexForKey(struct EnvMap *em, const char *key)
{
  int i;
  if (em == NULL || key == NULL) {
    clLogError("NULL ptr in clEnvmapIndexForKey()\n");
  }
  for (i = 0; i < clEnvmapSize(em); i += 1) {
    union PCTypes p = clEnvmapKeyValAt(em,i);
    if (strcmp(p.sp.key, key) == 0) {
      return i;
    }
  }
  return -1;
}

void clEnvmapSetKeyPrivate(struct EnvMap *em, const char *key)
{
  if (em == NULL || key == NULL) {
    clLogError("NULL ptr in clEnvmapSetKeyPrivate()\n");
  }
  clNodesetAddNode(em->private, clEnvmapIndexForKey(em,key));
}

int clEnvmapIsMarkedAt(struct EnvMap *em, int where)
{
  if (em == NULL) {
    clLogError("NULL ptr in clEnvmapIsMarkedAt()\n");
  }
  return clNodesetHasNode(em->marked, where);
}

int clEnvmapIsPrivateAt(struct EnvMap *em, int where)
{
  if (em == NULL) {
    clLogError("NULL ptr in clEnvmapIsPrivateAt()\n");
  }
  return clNodesetHasNode(em->private, where);
}

int clEnvmapMerge(struct EnvMap *dest, struct EnvMap *src)
{
  union PCTypes p;
  int i;
  if (src == NULL) {
    clLogError("NULL ptr in clEnvmapMerge()\n");
  }
  for (i = 0; i < clEnvmapSize(src); i += 1) {
    p = clEnvmapKeyValAt(src,i);
    clEnvmapSetKeyVal(dest, p.sp.key, p.sp.val);
    if (clNodesetHasNode(src->marked, i))
      clEnvmapSetKeyMarked(dest, p.sp.key);
  }
  return CL_OK;
}
