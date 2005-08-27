#include <complearn.h>

double compaCompress(struct CompAdaptor *ca, struct DataBlock src)
{
  return ca->cf(ca, src);
}

void compaFree(struct CompAdaptor *ca)
{
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

double compaNCD(struct CompAdaptor *comp, struct DataBlock a, struct DataBlock b )
{
/* temporary hard-coded compressor: bzip */
	struct DataBlock ab, ba;
	double ca, cb, cab, cba;

	ab = datablockCat(a,b);
	ba = datablockCat(b,a);

	ca = compaCompress(comp, a);
	cb = compaCompress(comp, b);
	cab = compaCompress(comp, ab);
	cba = compaCompress(comp, ba);

  datablockFree(ab);
  datablockFree(ba);

/* temporary hard-coded ncd variation */
	return mndf(ca,cb,cab,cba);
}

