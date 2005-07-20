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
