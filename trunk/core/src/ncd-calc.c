#include <complearn/complearn.h>
#include <assert.h>

double mndf (double ca, double cb, double cab, double cba) {
  double maxk = ca < cb ? cb : ca;
	double kab = cba - cb;
	double kba = cab - ca;
  double maxck;

	maxck = kab < kba ? kba : kab;

	return ( maxck / maxk );
}

double ncdPair(struct CompAdaptor *comp, struct DataBlock a, struct DataBlock b )
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

double ncdfunc(struct DataBlock *a, struct DataBlock *b, struct GeneralConfig *cur)
{
  double result;
  if (cur->fJustSize) {
    struct DataBlock c;
    c = catDataBlock(*a, *b);
    result = compfuncCA(cur->ca, c);
    freeDataBlock(c);
  }
  else {
    result = ncdPair(cur->ca, *a,*b);
  }
  return result;
}
