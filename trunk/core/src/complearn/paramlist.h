#ifndef __PARAMLIST_H
#define __PARAMLIST_H

#define MAXFIELDS 128

#include <complearn/envmap.h>
#include <complearn/datablock.h>
#include <complearn/cltypes.h>

struct FieldDesc {
  const char *key;
  const char *value;
  int type;
  int fIsPrivate;
};

struct ParamList {
  struct FieldDesc fields[MAXFIELDS];
  int size;
};

int paramlistParamType(struct ParamList *pl, const char *key);
char *paramlistGetString(struct ParamList *pl, const char *key);
int paramlistGetInt(struct ParamList *pl, const char *key);
double paramlistGetDouble(struct ParamList *pl, const char *key);
char *paramlistToString(struct ParamList *pl);
void paramlistPushField(struct ParamList *pl, const char *key, const char *value, int type);
void paramlistSetValueForKey(struct ParamList *pl, struct EnvMap *em, const char *key, void *dest);
struct ParamList *paramlistNew(void);
void paramlistFree(struct ParamList *pl);

#endif
