#include <assert.h>
#include <math.h>
#include <malloc.h>
#include <complearn/ppmencoder.h>
#include <complearn/complearn.h>

struct PPMEncoder {
  double codelen;
  double logtwo;
};

struct PPMEncoder *newPPMEncoder(void)
{
  struct PPMEncoder *result = gcalloc(sizeof(struct PPMEncoder), 1);
  result->codelen = 0;
  result->logtwo = log(2.0);
  return result;
}

double PPMlen(struct PPMEncoder *ppmenc)
{
  return ppmenc->codelen / ppmenc->logtwo;
}

void PPMencode(struct PPMEncoder *ppmenc, double c)
{
  ppmenc->codelen -= log(c);
}

