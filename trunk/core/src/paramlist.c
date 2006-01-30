#include <string.h>
#include <complearn/complearn.h>


struct ParamList *paramlistNew()
{
   struct ParamList *pl;
   pl = clCalloc(sizeof(struct ParamList), 1);
   pl->em = loadDefaultEnvironment()->em;
   return pl;
}

char *paramlistValForKey(struct ParamList *pl, const char *key)
{
  int i;
  for ( i = 0; i < pl->size; i += 1) {
    if (strcmp(pl->fields[i]->key, key) == 0)
      return clStrdup(pl->fields[i]->value);
  }
  return NULL;
}

int paramlistParamType(struct ParamList *pl, const char *key)
{
  int i;
  for ( i = 0; i < pl->size; i += 1) {
    if (strcmp(pl->fields[i]->key, key) == 0)
      return pl->fields[i]->type;
  }
  return 0;
}

char *paramlistGetString(struct ParamList *pl, const char *key)
{
  return paramlistValForKey(pl, key);
}

int paramlistGetInt(struct ParamList *pl, const char *key)
{
  char *result;
  if ((result = paramlistValForKey(pl, key)))
      return atoi(result);
  return -1;
}

double paramlistGetDouble(struct ParamList *pl, const char *key)
{
  char *result;
  if ((result = paramlistValForKey(pl, key)))
      return atof(result);
  return -1;
}

int paramlistGetValue(struct ParamList *pl, const char *key, void *dest, int type)
{
  switch (type) {
    case PARAMSTRING:
      *((char **) dest) = paramlistGetString(pl, key);
      break;
    case PARAMINT:
      *((int *) dest) = paramlistGetInt(pl, key);
      break;
    case PARAMDOUBLE:
      *((double *) dest) =  paramlistGetDouble(pl, key);
      break;
  }
  return CL_OK;
}

char *paramlistToString(struct ParamList *pl)
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

struct FieldDesc *fielddescNew(const char *key, const char *value, int type)
{
  struct FieldDesc *fd;
  fd = clCalloc(sizeof(struct FieldDesc), 1);
  fd->key = clStrdup(key);
  fd->value = clStrdup(value);
  fd->type = type;
  return fd;
}

struct FieldDesc *fielddescClone(struct FieldDesc *fd)
{
  return fielddescNew(fd->key, fd->value, fd->type);
}

void fielddescFree(struct FieldDesc *fd)
{
  assert(fd);
  clFreeandclear(fd->key);
  clFreeandclear(fd->value);
  clFreeandclear(fd);
}
struct ParamList *paramlistClone(struct ParamList *pl)
{
   struct ParamList *result;
   int i;
   result = clCalloc(sizeof(struct ParamList), 1);
   result->em = pl->em;
   result->size = pl->size;
   for ( i = 0 ; i < pl->size ; i += 1)
     result->fields[i] = fielddescClone(pl->fields[i]);
   return result;
}

void paramlistFree(struct ParamList *pl)
{
  int i;
  for (i = 0; i < pl->size ; i += 1)
    fielddescFree(pl->fields[i]);
  clFreeandclear(pl);
}
