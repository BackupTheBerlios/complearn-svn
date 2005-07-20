#include <complearn/complearn.h>
#include <stdio.h>
#include <string.h>
#include <complearn/ppmencoder.h>
#include <complearn/modelnode.h>
#include <malloc.h>

//#define MAXALLOCNODES (1024*128)
#define MAXALLOCNODES (128000)

struct ModelNode {
  struct ModelNode *children[SYMCOUNT];
  unsigned int total;
  unsigned int freq;
  int childCount;
  int maxDepth;
};

/*
int getFrequency(const struct ModelNode *mn)
{
  assert(mn);
  return mn->freq;
}
*/

static int rescaleChildren(struct ModelNode *mn, int factor, int incr) {
  unsigned int total = 0;
  int i;
//  printf("Recsaling...\n");
  for (i = 0; i < SYMCOUNT; i += 1) {
    unsigned int smallfreq;
    if (mn->children[i] == NULL)
      continue;
    smallfreq = rescaleDeep(mn->children[i], factor, incr);
    if (smallfreq == 0)
      removeChildNode(mn, i);
    else
      total += smallfreq;
  }
  return total;
}

int rescaleDeep(struct ModelNode *mn, int factor, int incr)
{
  long long unsigned int f = factor;
  f *= mn->freq;
  f /= incr;
  mn->freq = (unsigned int) f;
  mn->total = rescaleChildren(mn, factor, incr);
  return mn->freq;
}

int encodeASymbol(struct ModelNode *mn, struct PPMEncoder *ppmenc,
                  unsigned char symbol, int shouldEscape, int incr)
{
  struct ModelNode *cmn = findChildForSymbol(mn, symbol);
  int q = 0;
  if (shouldEscape)
    q = incr * mn->childCount;
  if (cmn == NULL) {
    if (q > 0)
      PPMencode(ppmenc, ((double) q) / ((double) (mn->total + q)));
  }
    else {
      PPMencode(ppmenc, ((double) (cmn->freq) / ((double) (mn->total + q))));
  }
  return (cmn == NULL);
}

#define MAXFREQNUM 80000000

void updateFrequency(struct ModelNode *mn, unsigned char symbol, int incr)
{
//  printf("Updating frequency for %d in %p\n",  symbol, mn);
  struct ModelNode *cmn = findChildForSymbol(mn, symbol);
  if (cmn == NULL && mn->maxDepth > 0) {
    cmn = newModelNode(mn->maxDepth - 1);
    if (cmn)
      addChildSymbol(mn, symbol, cmn);
  }
  if (cmn) {
    cmn->freq += incr;
//    printf("The new freq for symbol %c is %d\n", symbol, cmn->freq);
    mn->total += incr;
    if (cmn->freq > MAXFREQNUM)
      rescaleDeep(mn, 10, 7);
  }
}

static int modelNodeCounter;

int getAllocCounter(void)
{
  return modelNodeCounter;
}

struct ModelNode *newModelNode(int maxDepth)
{
  struct ModelNode *mn = NULL;
  if (modelNodeCounter < MAXALLOCNODES) {
    mn = gcalloc(sizeof(struct ModelNode), 1);
    mn->maxDepth = maxDepth;
    modelNodeCounter += 1;
  }
  return mn;
}

static void checkChildBounds(int whichInd)
{
  assert(whichInd >= 0 && whichInd < SYMCOUNT);
}

void removeChildNode(struct ModelNode *mn, unsigned char symbol)
{
  struct ModelNode *victim = findChildForSymbol(mn, symbol);
  assert(victim);
  assert(victim->childCount == 0);
  freeModelNode(victim);
  mn->children[symbol] = NULL;
  mn->childCount -= 1;
}

struct ModelNode *findChildForSymbol(struct ModelNode *mn, unsigned char symbol)
{
  checkChildBounds(symbol);
  return mn->children[symbol];
}

void addChildSymbol(struct ModelNode *mn, unsigned char symbol, struct ModelNode *child)
{
  assert(child);
  checkChildBounds(symbol);
  assert(mn->children[symbol] == NULL);
  mn->children[symbol] = child;
  mn->childCount += 1;
}

void freeModelNodeDeep(struct ModelNode *mn)
{
  int i;
  for (i = 0; i < SYMCOUNT; i += 1)
    if (mn->maxDepth > 0 && mn->children[i]) {
      freeModelNodeDeep(mn->children[i]);
      mn->children[i] = NULL;
    }
  freeModelNode(mn);
}

void freeModelNode(struct ModelNode *mn)
{
  modelNodeCounter -= 1;
  assert(modelNodeCounter >= 0);
  memset(mn, 0, sizeof(*mn));
  free(mn);
}

void modelNodeCopy(struct ModelNode *mna, struct ModelNode *mnb)
{
  memcpy(mna, mnb, sizeof(*mna));
}

