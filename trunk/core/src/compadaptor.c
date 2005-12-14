#include <string.h>
#include <complearn/complearn.h>

double compaCompress(struct CompAdaptor *ca, struct DataBlock *src)
{
  return ca->cf(ca, src);
}

void compaFree(struct CompAdaptor *ca)
{
  paramlistFree(ca->pl);
  ca->fcf(ca);
}

char *compaShortName(struct CompAdaptor *ca)
{
  return ca->sn();
}
char *compaLongName(struct CompAdaptor *ca)
{
  return ca->ln();
}
int compaAPIVer(struct CompAdaptor *ca)
{
  return ca->apiv();
}

double compaNCD(struct CompAdaptor *comp, struct DataBlock *a, struct DataBlock *b )
{
/* temporary hard-coded compressor: bzip */
	struct DataBlock *ab, *ba;
	double ca, cb, cab, cba;

	ab = datablockCatPtr(a,b);
	ba = datablockCatPtr(b,a);

	ca = compaCompress(comp, a);
	cb = compaCompress(comp, b);
	cab = compaCompress(comp, ab);
	cba = compaCompress(comp, ba);

  datablockFreePtr(ab);
  datablockFreePtr(ba);

/* temporary hard-coded ncd variation */
	return mndf(ca,cb,cab,cba);
}

/** \brief Should be called from each CompAdaptor Implementation constructor
 */
void compaInitParameters(struct CompAdaptor *ca)
{
  ca->pl = paramlistNew();
}

struct ParamList *compaParameters(struct CompAdaptor *comp)
{
  return paramlistClone(comp->pl);
}

void compaPushParameter(struct CompAdaptor *ca, const char *key, const char *value, int type)
{
  struct ParamList *pl = ca->pl;
  pl->fields[pl->size] = fielddescNew(key,value,type);
  assert(pl->fields[pl->size]);
  pl->size += 1;
}

void compaSetValueForKey(struct CompAdaptor *ca, const char *key, void *dest)
{
  struct ParamList *pl = ca->pl;
  char *value;
  int i;
  assert(pl->em);
  value = envmapValueForKey(pl->em,key);
  for ( i = 0; i < pl->size; i += 1 ) {
    if (strcmp(pl->fields[i]->key,key) == 0) {
      if (value) {
        clFree(pl->fields[i]->value);
        pl->fields[i]->value = clStrdup(value);
      }
      paramlistGetValue(pl, key, dest, paramlistParamType(pl, key));
      return;
    }
  }
  clogError( "Error: can not set key %s without default value.\n", key);
  assert(0);
}
