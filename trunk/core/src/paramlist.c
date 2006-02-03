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
  assert(fd);
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
