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

#define DEFMINTERMLENGTH 3
#define DEFMAXTERMS     10
#define DEFMAXCHARS     80
#define DEFFULLSTRINGWEIGHTING     0.25

struct IndDist {
  int index;
  double distance;
};

static int sidcompare(const void *a, const void *b)
{
  struct IndDist *ia = ((struct IndDist *)a), *ib = ((struct IndDist *)b);
  if (ia->distance < ib->distance)
    return -1;
  if (ia->distance > ib->distance)
    return 1;
  return 0;
}

static struct StringStack *splitIntoParts(struct SearchSettings *sparm, char *str)
{
  struct StringStack *res = clStringstackNew();
  unsigned char *swork = (unsigned char *) clStrdup(str);
  int i;
  for (i = 0; str[i]; i += 1)
    if (!sparm->wordChar[swork[i]])
      swork[i] = 0;
  for (i = 0; str[i]; i += 1)
    if (sparm->wordChar[swork[i]]) {
      clStringstackPush(res, (char *) (swork+i));
      i += strlen((char *) (swork+i))-1;
    }
  clFree(swork);
  return res;
}

static char *convertStr(struct SearchSettings *sparm, char *str)
{
  int i;
  static char *res, *cur;
  if (res)
    clFree(res);
  res = clCalloc(strlen(str)+1,1);
  cur = res;
  for (i = 0; str[i]; i++) {
    int c = str[i];
    int t = sparm->transTable[c];
    if (t)
      *cur++ = t;
  }
  *cur++ = '\0';
  return res;
}

/* Pass in an array of count strings in str and count entries in res; it
 * will return a sorted list with closest matches first.
 */
int clFindClosestMatchCB(struct CompressionBase *cb, struct SearchSettings *sparm, char *target,  char **str, int count, struct StringWithDistance *res)
{
  int i, j;
  struct DataBlock *dbtar;
  struct IndDist *sid;
  if (sparm == NULL)
    sparm = clSearchSettingsFoldCase();
  if (count < 0)
    clLogError("Invalid negative count specified in approximate match.");
  if (count == 0)
    return 0;
  if (target == NULL)
    clLogError("Must pass in target string to find closest match.");
  if (str == NULL)
    clLogError("Must pass in array of strings to match.");
  if (res == NULL)
    clLogError("Must pass in array to hold results.");
  dbtar = clStringToDataBlockPtr(convertStr(sparm, target));
  sid = clCalloc(sizeof(*sid), count);
  for (i = 0; i < count; i += 1) {
    struct DataBlock *dbcur;
    int pc;
    int tooLong = 0;
    struct StringStack *parts;
    char *convstr;
    sid[i].index = i;
    convstr = convertStr(sparm, str[i]);
    if (strlen(convstr) > sparm->maxChars)
      tooLong = 1;
    parts = splitIntoParts(sparm,convstr);
    dbcur = clStringToDataBlockPtr(convstr);
    pc = clStringstackSize(parts);
    if (pc == 0)
      sid[i].distance = 99999 + strlen(str[i]);
    else {
      double minsingle = 1.0;
      struct DataBlock *dbp;
      double distterm = clNcdFuncCB(cb, dbtar, dbcur);
      for (j= 0; !tooLong && j < clStringstackSize(parts) && j < sparm->maxTerms; j += 1) {
        double d;
        dbp = clStringToDataBlockPtr(clStringstackReadAt(parts, j));
        d = clNcdFuncCB(cb, dbtar, dbp);
        if (d < minsingle)
          minsingle = d;
        clDatablockFreePtr(dbp);
      }
      sid[i].distance = distterm * sparm->fullStringWeighting + minsingle * (1.0-sparm->fullStringWeighting);
    }
    clStringstackFree(parts);
    clDatablockFreePtr(dbcur);
  }
  qsort(sid, count, sizeof(sid[0]), sidcompare);
  for (i = 0; i < count; i += 1) {
    res[i].str = str[sid[i].index];
    res[i].distance = sid[i].distance;
  }
  clFree(sid);
  clDatablockFreePtr(dbtar);
  return 0;
}

struct SearchSettings *clSearchSettingsFoldCase(void)
{
  int i;
  static struct SearchSettings *res;
  if (res == NULL) {
    res = clCalloc(sizeof(struct SearchSettings), 1);
    res->minTermLength = DEFMINTERMLENGTH;
    res->maxTerms = DEFMAXTERMS;
    res->maxChars = DEFMAXCHARS;
    res->fullStringWeighting = DEFFULLSTRINGWEIGHTING;
    for (i = 0; i < 256; i += 1) {
      int c = i;
      if (c >= 'A' && c <= 'Z')
        c = c - 'A' + 'a';
      res->transTable[i] = c;
    }
    for (i = 0; i < 256; i += 1)
      res->wordChar[i] = (i >= 'A' && i <= 'Z') || (i >= 'a' && i <= 'z') ||
                         (i >= '0' && i <= '9');
  }
  return res;
}

struct SearchSettings *clSearchSettingsKeepCase(void)
{
  int i;
  static struct SearchSettings *res;
  if (res == NULL) {
    res = clCalloc(sizeof(struct SearchSettings), 1);
    res->minTermLength = DEFMINTERMLENGTH;
    res->maxTerms = DEFMAXTERMS;
    res->maxChars = DEFMAXCHARS;
    res->fullStringWeighting = DEFFULLSTRINGWEIGHTING;
    for (i = 0; i < 256; i += 1)
      res->transTable[i] = i;
    for (i = 0; i < 256; i += 1)
      res->wordChar[i] = (i >= 'A' && i <= 'Z') || (i >= 'a' && i <= 'z') ||
                         (i >= '0' && i <= '9');
  }
  return res;
}

