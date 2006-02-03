#include <complearn/complearn.h>
#include <assert.h>

double clMndf (double ca, double cb, double cab, double cba) {
  double maxk = ca < cb ? cb : ca;
	double kab = cba - cb;
	double kba = cab - ca;
  double maxck;

	maxck = kab < kba ? kba : kab;

	return ( maxck / maxk );
}

double clNcdclFunc(struct DataBlock *a, struct DataBlock *b, struct GeneralConfig *cur)
{
  double result;
  if (cur->fJustSize) {
    struct DataBlock *c;
    c = clDatablockCatPtr(a, b);
    result = clCompaCompress(cur->ca, c);
    clDatablockFreePtr(c);
  }
  else {
    result = clCompaNCD(cur->ca, a, b);
  }
  return result;
}
