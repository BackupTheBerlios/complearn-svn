#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <math.h>

#include <complearn/complearn.h>

#define MAXSTATES 13
#define CREDULITY 52
#define STATELOGBASE 1.532

/*! \file builtinca-blocksort.c */

/** \brief blocksort compressor instance
 *
 * \struct BlockSortCompInstance
 *
 * \sa builtinca-blocksort.c
 */
struct BlockSortCompInstance {
  int code2state[256];
  int nstates;
  int *x, *p, allocated;
};

void clSuffixsort(int *x, int *p, int n, int k, int l);

static double bs_compress(struct BlockSortCompInstance *CI,
			  unsigned char *data, int size) {
  int *x, *p;
  int i, j, mass, av;
  unsigned char code2byte[256];
  int num[256], total[MAXSTATES], state, oldstate;
  int statetrans[MAXSTATES][MAXSTATES], ntrans[MAXSTATES];
  double cl = 0;

  /* Obtain workspace in x and p */
  if (CI->allocated < size+1) {
    CI->allocated = 10 + size * 1.2;
    CI->x = clRealloc(CI->x, CI->allocated * sizeof(int));
    CI->p = clRealloc(CI->p, CI->allocated * sizeof(int));
    if (CI->x==NULL || CI->p==NULL) { clogError("do_compress"); }
  }
  x = CI->x;
  p = CI->p;

  /* Suffix sort the data (permutes x and p) */
  for (i=0; i<size; i++) x[i] = data[i];
  clSuffixsort(x, p, size, UCHAR_MAX+1, 0);

  /* Initialise state transition statistics */
  for (j=0; j<MAXSTATES; j++) {
    total[j]  = 0;
    ntrans[j] = MAXSTATES;
    for (i=0; i<MAXSTATES; i++) statetrans[j][i] = 1;
  }
  statetrans[0][0]--; ntrans[0]--;

  /* Initialise the move to front codebook and the symbol frequencies */
  for (i=0; i<256; i++) {
    code2byte[i] = i;
    num[i] = 1;
    total[CI->code2state[i]]++;
  }

  state = CI->nstates-1;

  /* Code the block sorted sequence */
  for (i=0; i<size+1; i++) {
    unsigned char c;
    unsigned char carry1, carry2;
    int code;

    c = p[i] ? data[(p[i]+size)%(size+1)] : code2byte[0];

    /* Move to front:
       - if the symbol is at position 1 of the code book, then move it
         to position 0
       - otherwise move it to position 1.
       (Why? Because it improves compression. Why? No-one knows.)
    */
    code = 0;
    carry2 = code2byte[0];
    if (carry2!=c) {
      carry1 = code2byte[++code];
      if (carry1==c) {
	code2byte[0] = (unsigned char)c;
	code2byte[1] = carry2;
      } else {
	code2byte[1] = (unsigned char)c;
	for (;;) {
	  carry2 = code2byte[++code];
	  code2byte[code] = carry1;
	  if (carry2 == c) break;
	  carry1 = code2byte[++code];
	  code2byte[code] = carry2;
	  if (carry1 == c) break;
	}
      }
    }


    /* Encoding takes place in three stages:

       1. Encode a state transition.
          The state depends on the symbol to be encoded through the
	  lookup table CI->code2state[]. We keep statistics on state
	  transition frequencies through statetrans[<state>][<state>]
	  and ntrans[<state>].

       2. Encode the symbol.
          Decoder already knows it must be one of the symbols that map
	  to the current state. We keep statistics on those as well,
	  this time through the arrays num[<symbol>] and total[<state>].
	  Notice that some states only contain a single symbol; if we are
	  in such a state then automatically zero bits are used in this
	  stage.

       3. Run length encode zeroes.
          If the symbol was a zero, then encode the number of zeroes that
          follow, instead of coding each of them separately. This is necessary
	  because often sequences of zeroes occur that are highly dependent:
	  the probability that the next symbol is also a zero is often much
	  higher if the previous TWO symbols are zero than if only the
	  previous symbol is a zero, etc.

    */

    /* Stage 1. Encode the state transition. */
    oldstate = state;
    state = CI->code2state[code];

    cl += -log(statetrans[oldstate][state])+log(ntrans[oldstate]);
    statetrans[oldstate][state]++;
    ntrans[oldstate]++;

    /* 2. Encode the symbol from the range that belongs to this state */
    cl += -log(num[code])+log(total[state]);

    mass = CREDULITY;
    for (j=code; j>=0 && num[j]*(code-j)<mass; j--)
      mass += num[j];
    av = mass / (code-j);
    for (j++; j<=code; j++) {
      total[CI->code2state[j]] += av - num[j];
      num[j] = av;
    }

    /* Stage 3. Run length encode zeroes.
       We take as a probability distribution on the integers: P(n)=1/(n(n+1))
       It is easy to check that this sums to one for 1 <= n < infinity.
       The corresponding code uses -log P(n) bits to encode n.
       Motivation: the codelength is logarithmic in n, so it can never be
       extremely inefficient. At the same time, a relatively high probability
       is assigned to low numbers.
    */
    if (code==0 && i<size) {
      int runlength = i;
      i++;
      while (i<size+1) {
	if (p[i] && data[(p[i]+size)%(size+1)] != code2byte[0]) break;
	i++;
      };
      runlength = i - runlength;
      i--;

      cl += log(runlength) + log(runlength+1);
    }
  }

  return (cl + log(size)) / M_LN2;
}

void clBs_freecompclFunc(struct CompAdaptor *ca) {
  struct BlockSortCompInstance *bsci =
    (struct BlockSortCompInstance *)ca->cptr;
  if (bsci->allocated > 0) {
    free(bsci->x);
    free(bsci->p);
  }
  free(ca->cptr);
  free(ca);
}

static double bs_compclFunc(struct CompAdaptor *ca, struct DataBlock *src) {
  struct BlockSortCompInstance *bsci =
    (struct BlockSortCompInstance *)ca->cptr;
  return bs_compress(bsci, clDatablockData(src), clDatablockSize(src));
}

static char *bs_shortname(void) { return "blocksort"; }
static char *bs_longname(void) { return "block-sorting compressor"; }
static int   bs_apiver(void) { return APIVER_V1; }

struct CompAdaptor *clBuiltin_blocksort(void) {
  int i, d, m, s;
  double prev, cur;
  const static struct CompAdaptor c = {
    cptr: NULL,
    cf:   bs_compclFunc,
    fcf:  clBs_freecompclFunc,
    sn:   bs_shortname,
    ln:   bs_longname,
    apiv: bs_apiver
  };
  struct CompAdaptor *ca;
  struct BlockSortCompInstance *bsci;

  ca   = clCalloc(sizeof(*ca), 1);
  bsci = clCalloc(sizeof(*bsci), 1);

  if (ca==NULL || bsci==NULL) { clogError("clBuiltin_blocksort"); }
  *ca = c;
  ca->cptr= bsci;

  bsci->x = bsci->p = NULL;
  bsci->allocated = 0;
  d = m = 0;
  prev = 0;
  s = 0;
  for (i=0; i<256; i++) {
    cur = log(i+1.0)/log(STATELOGBASE);
    d = (int)cur-(int)prev;
    if (d>1) m+=(d-1);
    s = (int)cur - m;
    bsci->code2state[i] = s;
    prev = cur;
  }
  bsci->nstates = s+1;
  if (bsci->nstates > MAXSTATES) {
    clogError( "MAXSTATES should be at least %d\n", s+1);
    exit(1);
  }

  clCompaInitParameters(ca);

  return ca;
}
