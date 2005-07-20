#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include <complearn/ppmencoder.h>
#include <complearn/modelnode.h>
#include <complearn/ppmcomp.h>
#include <complearn/complearn.h>

#define MAXCONTEXT 32

#define GENERALWEIGHT 250

#define SYMBOLSIZE 256

struct PPMComp
{
  struct ModelNode *context[MAXCONTEXT];
  int maxConInd;
  int maxOrder;
  int maxMem;
  int frozen, escape;
  struct PPMEncoder *ppmenc;
  struct ModelNode *root;
};


struct PPMComp *newPPMComp(void)
{
  struct PPMComp *ppmc = gcalloc(sizeof(struct PPMComp), 1);
  int i;
  ppmc->escape = 1;
  ppmc->maxOrder = 4;
  ppmc->maxMem = 512*1024*1024;
  ppmc->ppmenc = newPPMEncoder();
  ppmc->root = newModelNode(ppmc->maxOrder);
  for (i = 0; i < ppmc->maxOrder; i += 1)
    ppmc->context[i] = NULL;
  return ppmc;
}

static void nextContext(struct PPMComp *ppmc)
{
  ppmc->context[ppmc->maxConInd] = ppmc->root;
  ppmc->maxConInd += 1;
  if (ppmc->maxConInd == ppmc->maxOrder)
    ppmc->maxConInd = 0;
}

void updateModel(struct PPMComp *ppmc, unsigned int model, unsigned char sym)
{
//  printf("Updating letter %c for model %d\n", sym, model);
//  struct ModelNode *mn = findChildForSymbol(ppmc->context[model], sym);
//  if (mn)
//    printf("The freq for it is %d\n", getFrequency(mn));
  if (!ppmc->frozen)
    updateFrequency(ppmc->context[model], sym, GENERALWEIGHT);
  ppmc->context[model] = findChildForSymbol(ppmc->context[model], sym);
}

void encodeSymbol(struct PPMComp *ppmc, unsigned char sym)
{
  int escaped = 1;
  unsigned int model = ppmc->maxConInd;
  do {
    if (ppmc->context[model] != NULL) {
      if (escaped)
        escaped = encodeASymbol(ppmc->context[model],
            ppmc->ppmenc, sym, ppmc->escape, GENERALWEIGHT);
      updateModel(ppmc, model, sym);
    }
    model += 1;
    if (model == ppmc->maxOrder)
      model = 0;
  } while (model != ppmc->maxConInd);
  if (escaped)
    PPMencode(ppmc->ppmenc, 1.0 / (double) SYMBOLSIZE);
}

void encodeSymbolArray(struct PPMComp *ppmc, unsigned char *syms, unsigned int len)
{
  unsigned int i;
  for (i = 0; i < len; i += 1) {
    nextContext(ppmc);
    encodeSymbol(ppmc, syms[i]);
  }
}

double getPPMCompSize(struct PPMComp *ppmc)
{
  return PPMlen(ppmc->ppmenc);
}

/* XXX Memory leaks.. */
void freePPMComp(struct PPMComp *ppmc)
{
  freeModelNodeDeep(ppmc->root);
  ppmc->root = NULL;
  memset(ppmc, 0, sizeof(*ppmc));
  free(ppmc);
}
