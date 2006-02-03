#include <complearn/complearn.h>
#include <math.h>

#include <gsl/gsl_randist.h>


void clFlipCrosswise(struct AdjAdaptor *aa,
    qbase_t i1, qbase_t n1, qbase_t i2, qbase_t n2)
{
  clAdjaSetConState(aa, i1, n1, 0);
  clAdjaSetConState(aa, i2, n2, 0);
  clAdjaSetConState(aa, i1, n2, 1);
  clAdjaSetConState(aa, i2, n1, 1);
}

void clMutateSpecies(struct AdjAdaptor *aa, struct LabelPerm *labelperm)
{
  clLabelpermMutate(labelperm);
}

/*
double tryNumAdjustment(struct TreeHolder *th)
{
  double fc = clTreehFailCount;
  double n = treeh
  return log(clTreehFailCount(th)) / 
}
*/

#define MAXMUT 1000
int clHowManyMutationsWeirdLogFormula(void)
{
  static gsl_ran_discrete_t *d;
  static gsl_rng *r;
  if (d == NULL) {
    int i;
    double curp;
    double *p = clCalloc(sizeof(*d), MAXMUT);
    double k;
    for (i = 0; i < MAXMUT; i += 1) {
      k = i + 2;
      curp = 1.0 / (k * (log(k) / log(2.0)) * (log(k)/log(2.0)));
      p[i] = curp;
    }
    d = gsl_ran_discrete_preproc(MAXMUT, p);
    r = gsl_rng_alloc(gsl_rng_taus);
    clFreeifpresent(p);
  }
  return gsl_ran_discrete(r, d) + 2;
 // + tryNumAdjustment();
}

int clHowManyMutationsTwoMinusExp(void)
{
  int res = 0;
  do {
    res += 1;
  } while (rand() % 2 == 1);
  return res;
}

