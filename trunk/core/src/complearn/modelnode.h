#ifndef __MODELNODE_H
#define __MODELNODE_H

#define SYMCOUNT 256
struct ModelNode;
struct PPMEncoder;

struct ModelNode *newModelNode(int maxDepth);
struct ModelNode *findChildForSymbol(struct ModelNode *mn, unsigned char symbol);
void freeModelNode(struct ModelNode *mn);
void freeModelNodeDeep(struct ModelNode *mn);
void addChildSymbol(struct ModelNode *mn, unsigned char symbol, struct ModelNode *child);
void removeChildNode(struct ModelNode *mn, unsigned char symbol);
void updateFrequency(struct ModelNode *mn, unsigned char symbol, int incr);
int getFrequency(const struct ModelNode *mn);
int getAllocCounter(void);
int encodeASymbol(struct ModelNode *mn, struct PPMEncoder *ppmenc,
                  unsigned char symbol, int shouldEscape, int incr);
int rescaleDeep(struct ModelNode *mn, int factor, int incr);
void modelNodeCopy(struct ModelNode *mna, struct ModelNode *mnb);

#endif
