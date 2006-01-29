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

double ncdfunc(struct DataBlock *a, struct DataBlock *b, struct GeneralConfig *cur)
{
  double result;
  if (a == NULL || b == NULL || cur == NULL) {
    clogError("NULL ptr in ncdfunc()\n");
  }
  if (cur->fJustSize) {
    struct DataBlock *c;
    c = datablockCatPtr(a, b);
    result = compaCompress(cur->ca, c);
    datablockFreePtr(c);
  }
  else {
    result = compaNCD(cur->ca, a, b);
  }
  return result;
}
