#ifndef __DOUBLEA_H
#define __DOUBLEA_H

#include <assert.h>
#include <complearn/datablock.h>
#include <complearn/cltypes.h>
/*! \file doublea.h */

/** \brief a dynamically resizing, doubling polymorphic array
 * \struct DoubleA
 *
 * This structure represents the primary mode of dynamic allocation in
 * the CompLearn system.  It keeps track of its highest-referenced
 * element index, and returns one more than this value for its size.
 * The DoubleA automatically doubles its sizes and reallocates with a
 * (flat, shallow) copy of all the old information whenever it is
 * necessary.  The DoubleA supports a variety of different types, of
 * sizes up to 8 bytes.  The union pctypes contains all possible
 * value types that may be used within this dynamic container class.
 */
struct DoubleA;

/** \brief a basic key, value pair
 * \struct strpair
 * This structure holds a key and an associated value as two associated pointers */
struct strpair { char *key; char *val; };
struct intpair { int x; int y; };
struct intdbpair { int tnum ; struct DataBlock db; };

/** \brief the basic polymorphic types supported by DoubleA
 * \union pctypes
 * a DoubleA can hold any number of different objects.  The choices include
 * a double <b>d</b> <br>
 * an integer <b>i</b> <br>
 * a character pointer or string <b>str</b> <br>
 * a pointer to a nested DoubleA as <b>ar</b> <br>
 * a strpair <b>sp</b> containing <b>sp.key</b> and <b>sp.val</b> <br>
 * a pointer to a TransAdaptor <b>ta</b>
 *
 * When using a pctypes, it is important to remember that you may only
 * store one of the above types (excepting the strpair, wherein the pair
 * counts as a single object).  If you store using one type then attempt
 * to retrieve using a different type, then an uncertain future awaits.
 */
union pctypes {
  double d;
  int i;
  char *str;
  struct DoubleA *ar;
  struct strpair sp;
  struct intpair ip;
  struct intdbpair idbp;
  struct DataBlock db;
  struct TransAdaptor *ta;
  void *ptr;
};

/**
 * These constants are for your convenience.
 * The first, zeropct, is simply a block of all-zeros that you may
 * conveniently copy to clear a pctypes.  The second is an integer i
 * with the value 1.  You may use this for boolean, set, or other operations.
 */
const extern union pctypes zeropct, onepcti;

struct DoubleA *newDoubleDoubler(void);
void freeDoubleDoubler(struct DoubleA *ptr);
struct DoubleA *loadDoubleDoubler(struct DataBlock d, int fmustbe);
struct DataBlock dumpDoubleDoubler(const struct DoubleA *d);
struct DataBlock dumpDeepDoubleDoubler(const struct DoubleA *d, int level);
void freeDeepDoubleDoubler(struct DoubleA *ptr, int lvl);
double getDValueAt(struct DoubleA *da, int where);
void setDValueAt(struct DoubleA *a, int where, double val);
int getSize(const struct DoubleA *a);
struct DoubleA *deepCopyLvl(const struct DoubleA *ptr, int lvl);
struct DoubleA *cloneDoubler(const struct DoubleA *ptr);
void makeSizeFor(struct DoubleA *da, int where);
union pctypes getValueAt(const struct DoubleA *da, int where);
void setValueAt(struct DoubleA *da, int where, union pctypes p);
void unshiftValue(struct DoubleA *da, union pctypes p);
void pushValue(struct DoubleA *da, union pctypes p);
union pctypes shiftDoubleDoubler(struct DoubleA *da);
union pctypes popDoubleDoubler(struct DoubleA *da);
union pctypes getRandomElement(const struct DoubleA *da);
int swapValues(struct DoubleA *da, int inda, int indb);
void printNodeList(const struct DoubleA *da);
struct DataBlock dumpString(const char *s);
/* if fmustbe, function exits when tagnum does not match. else, returns NULL */
char *loadString(struct DataBlock d, int fmustbe);
void verifyDoubleDoubler(const struct DoubleA *da);
void addDoubleDestructor(struct DoubleA *d, t_doublefreefunc func, void *udata);

int isInDAqb(const struct DoubleA *da, qbase_t which);
void printIntPairList(const struct DoubleA *da);
void addIfNewqb(struct DoubleA *da, qbase_t which);
int sizeCLNodeSet(const struct CLNodeSet *cl);

#define ALLNODES(sz, i) \
  for (i = 0; i < sz; i += 1)

#define ALLPAIRS(sz, i, j) \
  for (i = 0; i < sz; i += 1) \
    for (j = i+1; j < sz; j += 1)

#define ALLTRIPLETS(sz, i, j, k) \
  for (i = 0; i < sz; i += 1) \
    for (j = i+1; j < sz; j += 1) \
      for (k = j+1; k < sz; k += 1)

#define ALLQUARTETS(sz, i, j, k, m) \
  for (i = 0; i < sz; i += 1) \
    for (j = i+1; j < sz; j += 1) \
      for (k = j+1; k < sz; k += 1) \
        for (m = k+1; m < sz; m += 1)

#endif
