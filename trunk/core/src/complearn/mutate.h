#ifndef __MUTATE_H
#define __MUTATE_H

/*! \file mutate.h */

struct AdjAdaptor;
struct LabelPerm;

#include <complearn/cltypes.h>

/* Does a "crosswise" switch, transforming || to X */
void flipCrosswise(struct AdjAdaptor *aa,
    qbase_t i1, qbase_t n1, qbase_t i2, qbase_t n2);

/* Does a simple random inversion */
void mutateSpecies(struct AdjAdaptor *aa, struct LabelPerm *labelperm);

int howManyMutationsWeirdLogFormula(void);

int howManyMutationsTwoMinusExp(void);


#endif
