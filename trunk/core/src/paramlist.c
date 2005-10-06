#include <string.h>
#include <complearn/complearn.h>

struct ParamList *paramlistNew()
{
   struct ParamList *pl;
   pl = clCalloc(sizeof(struct ParamList), 1);
   return pl;
}

char *paramlistValForKey(struct ParamList *pl, const char *key)
{
  int i;
  for ( i = 0; i < pl->size; i += 1) {
    if (strcmp(pl->fields[i].key, key) == 0)
      return clStrdup(pl->fields[i].value);
  }
  return NULL;
}

int paramlistParamType(struct ParamList *pl, const char *key)
{
  int i;
  for ( i = 0; i < MAXFIELDS; i += 1) {
    if (strcmp(pl->fields[i].key, key) == 0)
      return pl->fields[i].type;
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

static int paramlistGetValue(struct ParamList *pl, const char *key, void *dest, int type)
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
    incr+=sprintf(buff+incr, "%s: %s; ",pl->fields[i].key,pl->fields[i].value);
  result = clCalloc(incr, 1);
  memcpy(result, buff, incr);
  result[incr-2]='\0';
  return result;
}

void paramlistPushField(struct ParamList *pl, const char *key, const char *value, int type)
{
  pl->fields[pl->size].key = clStrdup(key);
  pl->fields[pl->size].value = clStrdup(value);
  pl->fields[pl->size].type = type;
  pl->size += 1;
}

void paramlistSetValueForKey(struct ParamList *pl, struct EnvMap *em, const char *key, void *dest)
{
  char *value;
  int i;
  if((value = envmapValueForKey(em,key))) {
    for ( i = 0; i < pl->size; i += 1 ) {
      if (strcmp(pl->fields[i].key,key) == 0) {
        pl->fields[i].value = clStrdup(value);
        paramlistGetValue(pl, key, dest, paramlistParamType(pl, key));
        break;
      }
    }
  }
}

void paramlistFree(struct ParamList *pl)
{
  clFreeandclear(pl);
}
