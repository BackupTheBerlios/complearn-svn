#include <complearn/complearn.h>
#include <math.h>

#if GSL_RDY
#include <gsl/gsl_randist.h>
#endif


void flipCrosswise(struct AdjAdaptor *aa,
    qbase_t i1, qbase_t n1, qbase_t i2, qbase_t n2)
{
  adjaSetConState(aa, i1, n1, 0);
  adjaSetConState(aa, i2, n2, 0);
  adjaSetConState(aa, i1, n2, 1);
  adjaSetConState(aa, i2, n1, 1);
}

void mutateSpecies(struct AdjAdaptor *aa, struct LabelPerm *labelperm)
{
  labelpermMutate(labelperm);
}

/*
double tryNumAdjustment(struct TreeHolder *th)
{
  double fc = treehFailCount;
  double n = treeh
  return log(treehFailCount(th)) / 
}
*/

#define MAXMUT 1000
int howManyMutationsWeirdLogFormula(void)
{
#if GSL_RDY
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
#else
	assert (0 && "gsl not supported");
	exit(1);
	return -1;
#endif
}

int howManyMutationsTwoMinusExp(void)
{
  int res = 0;
  do {
    res += 1;
  } while (rand() % 2 == 1);
  return res;
}

