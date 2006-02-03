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
