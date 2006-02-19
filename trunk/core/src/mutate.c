/*
* Copyright (c) 2006 Rudi Cilibrasi, Rulers of the RHouse
* All rights reserved.     cilibrar@cilibrar.com
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the RHouse nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE RULERS AND CONTRIBUTORS "AS IS" AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE RULERS AND CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
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

#define MAXMUT 30
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
      k = i + 4; /* to make single-mutations somewhat less common */
      curp = 1.0 / (k * (log(k) / log(2.0)) * (log(k)/log(2.0)));
      p[i] = curp;
    }
    d = gsl_ran_discrete_preproc(MAXMUT, p);
    r = gsl_rng_alloc(gsl_rng_taus);
    clFreeifpresent(p);
  }
  return gsl_ran_discrete(r, d) + 1;
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

