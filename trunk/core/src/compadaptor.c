#include <complearn.h>

double compfuncCA(struct CompAdaptor *ca, struct DataBlock src)
{
  return ca->cf(ca, src);
}

void freeCA(struct CompAdaptor *ca)
{
  ca->fcf(ca);
}

char *shortNameCA(struct CompAdaptor *ca)
{
  return ca->sn();
}
char *longNameCA(struct CompAdaptor *ca)
{
  return ca->ln();
}
int apiverCA(struct CompAdaptor *ca)
{
  return ca->apiv();
}

double ncdCA(struct CompAdaptor *comp, struct DataBlock a, struct DataBlock b )
{
/* temporary hard-coded compressor: bzip */
	struct DataBlock ab, ba;
	double ca, cb, cab, cba;

	ab = catDataBlock(a,b);
	ba = catDataBlock(b,a);

	ca = compfuncCA(comp, a);
	cb = compfuncCA(comp, b);
	cab = compfuncCA(comp, ab);
	cba = compfuncCA(comp, ba);

  freeDataBlock(ab);
  freeDataBlock(ba);

/* temporary hard-coded ncd variation */
	return mndf(ca,cb,cab,cba);
}

