#ifndef __PARAMLIST_H
#define __PARAMLIST_H

#define MAXFIELDS 128

#include <complearn/envmap.h>
#include <complearn/datablock.h>
#include <complearn/cltypes.h>

struct FieldDesc {
  char *key;
  char *value;
  int type;
};

struct ParamList {
  int size;
  struct EnvMap *em;
  struct FieldDesc *fields[MAXFIELDS];
};

int clParamlistParamType(struct ParamList *pl, const char *key);
char *clParamlistGetString(struct ParamList *pl, const char *key);
int clParamlistGetInt(struct ParamList *pl, const char *key);
double clParamlistGetDouble(struct ParamList *pl, const char *key);
char *clParamlistToString(struct ParamList *pl);
struct ParamList *clParamlistNew(void);
void clParamlistFree(struct ParamList *pl);
struct ParamList *clParamlistClone(struct ParamList *pl);
struct FieldDesc *clFielddescNew(const char *key, const char *value, int type);
void clFielddescFree(struct FieldDesc *fd);
int clParamlistGetValue(struct ParamList *pl, const char *key, void *dest, int type);


#endif
