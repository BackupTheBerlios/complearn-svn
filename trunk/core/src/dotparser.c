#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <complearn/complearn.h>

#define MAXPAT 5

/* Token numbers 0-255 represent single characters */
#define TOK_STARTSTRING 256
#define TOK_ENDSTRING 257
#define TOK_STARTCOMMENT 258
#define TOK_ENDCOMMENT 259
#define TOK_WORD 260
#define TOK_STARTBLOCK 261
#define TOK_ENDBLOCK 262
#define TOK_STARTBRACKET 263
#define TOK_ENDBRACKET 264
#define TOK_PROPSET 265
#define TOK_ENDSTATEMENT 266
#define TOK_SEPARATOR 267
#define TOK_EDGEMARK 268
#define TOK_WHITESPACE 269
#define TOK_COMMENT 270
#define TOK_STRING 271
#define TOK_TREE 272
#define TOK_EDGEMARKER 273

#define STATE_NORMAL 0
#define STATE_INSTRING 1
#define STATE_AFTERBACKSLASH 2
#define STATE_AFTERSLASH 3
#define STATE_AFTERSTAR 4
#define STATE_INCOMMENT 5
#define STATE_AFTERDASH 6
#define STATE_GOTGRAPH 7
#define STATE_NEEDBLOCK 8
#define STATE_GOTBLOCK 9
#define STATE_NEEDENDPOINT 10
#define STATE_NEEDSEMI 11
#define STATE_NEEDPROPVAL 12

struct CLToken {
  int tokNum;
  struct DoubleA *d; /* Array of CLToken * */
};

struct CLYieldChain;
typedef void (*parse_func)(struct CLYieldChain *yc, struct CLToken *clt);

struct CLYieldChain {
  struct CLYieldChain *next;
  int passed;
  int state;
  struct CLToken *pasTok;
  struct CLToken *m;
  parse_func process;
};

struct DotParserInstance;
struct DotParserClass;

static struct DotParserInstance *currentArena = NULL;

typedef void (*tokenFunc)(struct DotParserInstance *dp,
  struct CLToken *cur, void *obj);

struct DotParserInstance {
  struct DotParserClass *dpc;
  struct DataBlock *db;
  struct DoubleA *commentBuf;
  struct DoubleA *tokensToFree;
};

struct DotParserClass {
};

struct YCStrings {
  struct CLYieldChain yc;
};

struct YCComments {
  struct CLYieldChain yc;
};

struct YCNames {
  struct CLYieldChain yc;
};

struct YCPunk {
  struct CLYieldChain yc;
};

struct YCWhiteSpace {
  struct CLYieldChain yc;
};

struct YCMerger {
  struct CLYieldChain yc;
};

struct YCTree {
  struct CLYieldChain yc;
  struct CLToken *curword, *lastword;
  int oldstate;
  int isNode;
  int nodenum, maxnodenum;
  struct DoubleA *nodeLabels, *edgeKeeper;
  struct TreeAdaptor *ta;
  struct TreeScore *ts;
  struct TreeHolder *th;
};

static void feedBytes(const struct DotParserInstance *dpi, struct CLYieldChain *toWhom);
static struct CLToken *newCharToken(unsigned char c);
static void receiveToken(struct CLYieldChain *yc, struct CLToken *clt);
static void pass(struct CLYieldChain *yc, struct CLToken *clt);
static void eject(struct CLYieldChain *yc, struct CLToken *clt);
static void ejectGobbleAndPass(struct CLYieldChain *yc, struct CLToken *clt);
static void ejectPassClear(struct CLYieldChain *yc, struct CLToken *clt);
static void ejectGobblePassClear(struct CLYieldChain *yc, struct CLToken *clt);
static void gobble(struct CLYieldChain *yc, struct CLToken *clt);
static void clear(struct CLYieldChain *yc);
//static void showResults(struct YCTree *yctree);
static void ejectGobble(struct CLYieldChain *yc, struct CLToken *clt);
static void transitTo(struct CLYieldChain *yc, int ns);
static void newToken(struct CLYieldChain *yc, int sym);
static int punkMap(unsigned char c);
static struct CLToken *newSemToken(int symval);
static void handleComment(struct YCTree *tree, struct CLToken *clt);
static void handleTreeName(struct YCTree *tree, struct CLToken *clt);
static void handleNewEdge(struct YCTree *tree, const char *leftstr, const char *rightstr);
static void handleSetProp(struct YCTree *tree, const char *leftstr, const char *rightstr);
static void handleNewNode(struct YCTree *tree, const char *str);

static struct YCStrings *ycStringsNew(struct CLYieldChain *fm);
static void ycStringsPF(struct CLYieldChain *yc, struct CLToken *clt);
static struct YCComments *ycCommentsNew(struct CLYieldChain *fm);
static void ycCommentsPF(struct CLYieldChain *yc, struct CLToken *clt);
static struct YCNames *ycNamesNew(struct CLYieldChain *fm);
static void ycNamesPF(struct CLYieldChain *yc, struct CLToken *clt);
static struct YCPunk *ycPunkNew(struct CLYieldChain *fm);
static void ycPunkPF(struct CLYieldChain *yc, struct CLToken *clt);
static struct YCWhiteSpace *ycWhiteSpaceNew(struct CLYieldChain *fm);
static void ycWhiteSpacePF(struct CLYieldChain *yc, struct CLToken *clt);
static struct YCMerger *ycMergerNew(struct CLYieldChain *fm);
static void ycMergerPF(struct CLYieldChain *yc, struct CLToken *clt);
static struct YCTree *ycTreeNew(struct CLYieldChain *fm);
static void ycTreePF(struct CLYieldChain *yc, struct CLToken *clt);

#define upcastTo(t, y) struct t *ycs = (struct t *) y;
#define downcast(y) ((struct CLYieldChain *)y)
#define getState(y) (y->yc.state)

static int punkMap(unsigned char c)
{
  switch (c) {
    case '{': return TOK_STARTBLOCK;
    case ',': return TOK_SEPARATOR;
    case '}': return TOK_ENDBLOCK;
    case '[': return TOK_STARTBRACKET;
    case ']': return TOK_ENDBRACKET;
    case '=': return TOK_PROPSET;
    case ';': return TOK_ENDSTATEMENT;

    default:
      return 0;
  }
}

static void transitTo(struct CLYieldChain *yc, int ns)
{
  if (yc == NULL) {
    clogError("NULL ptr in TransitTo()\n");
  }
  yc->state = ns;
}

static void newToken(struct CLYieldChain *yc, int sym)
{
  if (yc == NULL) {
    clogError("NULL ptr in newToken()\n");
  }
  assert(yc->m == NULL && "Uncleared symbol");
  yc->m = newSemToken(sym);
  assert(yc->m->tokNum == sym);
  assert(yc->m->d != NULL);
}

void ycStringsPF(struct CLYieldChain *yc, struct CLToken *clt)
{
  if (yc == NULL || clt == NULL) {
    clogError("NULL ptr in ycStringsPF()\n");
  }
  upcastTo(YCStrings, yc);
  //struct YCStrings *ycs = (struct YCStrings *) yc;
  switch(getState(ycs))
  {
    case STATE_NORMAL:
      if (clt->tokNum == '"') {
        transitTo(yc, STATE_INSTRING);
        newToken(yc, TOK_STARTSTRING);
        ejectGobblePassClear(yc, clt);
      }
      break;

    case STATE_INSTRING:
      if (clt->tokNum == '\\') {
        transitTo(yc, STATE_AFTERBACKSLASH);
        eject(yc, clt);
      }
      if (clt->tokNum == '"') {
        transitTo(yc, STATE_NORMAL);
        newToken(yc, TOK_ENDSTRING);
        ejectGobblePassClear(yc, clt);
      }
      break;

    case STATE_AFTERBACKSLASH:
      pass(yc, clt);
      transitTo(yc, STATE_INSTRING);
      break;
  }
}

struct YCStrings *ycStringsNew(struct CLYieldChain *fm)
{
  struct YCStrings *ycs = clCalloc(sizeof(*ycs), 1);
  if (fm == NULL) {
    clogError("NULL ptr in ycStringsNew()\n");
  }
  ycs->yc.next = fm;
  ycs->yc.process = ycStringsPF;
  return ycs;
}

struct YCComments *ycCommentsNew(struct CLYieldChain *fm)
{
  struct YCComments *ycs = clCalloc(sizeof(*ycs), 1);
  if (fm == NULL) {
    clogError("NULL ptr in ycCommentsNew()\n");
  }
  ycs->yc.next = fm;
  ycs->yc.process = ycCommentsPF;
  return ycs;
}

struct YCNames *ycNamesNew(struct CLYieldChain *fm)
{
  struct YCNames *ycs = clCalloc(sizeof(*ycs), 1);
  if (fm == NULL) {
    clogError("NULL ptr in ycNamesNew()\n");
  }
  ycs->yc.next = fm;
  ycs->yc.process = ycNamesPF;
  return ycs;
}

static void ycNamesPF(struct CLYieldChain *yc, struct CLToken *clt)
{
  //upcastTo(YCNames, yc);
  if (clt->tokNum < 256) {
    if (yc->m == NULL) {
      newToken(yc, TOK_WORD);
    }
    ejectGobble(yc, clt);
  } else {
    if (yc->m != NULL) {
      pass(yc, yc->m);
      clear(yc);
    }
  }
}

void ycCommentsPF(struct CLYieldChain *yc, struct CLToken *clt)
{
  upcastTo(YCComments, yc);
  switch(getState(ycs))
  {
    case STATE_NORMAL:
      if (clt->tokNum == '/') {
        transitTo(yc, STATE_AFTERSLASH);
        newToken(yc, TOK_STARTCOMMENT);
        ejectGobble(yc, clt);
      }
      break;

    case STATE_AFTERSLASH:
      if (clt->tokNum == '*') {
        transitTo(yc, STATE_INCOMMENT);
        ejectGobblePassClear(yc, clt);
      } else {
        transitTo(yc, STATE_NORMAL);
        pass(yc, doubleaGetValueAt(yc->m->d, 0).ptr);
        clear(yc);
      }
      break;

    case STATE_INCOMMENT:
      if (clt->tokNum == '*') {
        transitTo(yc, STATE_AFTERSTAR);
        newToken(yc, TOK_ENDCOMMENT);
        eject(yc, clt);
        gobble(yc, clt);
      }
      break;

    case STATE_AFTERSTAR:
      if (clt->tokNum == '/') {
        transitTo(yc, STATE_NORMAL);
        ejectGobbleAndPass(yc, clt);
      } else {
        transitTo(yc, STATE_INCOMMENT);
        pass(yc, doubleaGetValueAt(yc->m->d, 0).ptr);
      }
      clear(yc);
      break;

    default:
      fprintf(stderr, "Unknown state %d in YCCommentPF\n", getState(ycs));
      exit(1);
  }
}

static struct YCPunk *ycPunkNew(struct CLYieldChain *fm)
{
  struct YCPunk *ycs = clCalloc(sizeof(*ycs), 1);
  ycs->yc.next = fm;
  ycs->yc.process = ycPunkPF;
  return ycs;
}

static void ycPunkPF(struct CLYieldChain *yc, struct CLToken *clt)
{
  int pm;
  upcastTo(YCPunk, yc);

  if (clt->tokNum > 255) {
    pass(yc, clt);
    return;
  }
  switch(getState(ycs)) {
    case STATE_NORMAL:
      if (clt->tokNum == '-') {
        transitTo(yc, STATE_AFTERDASH);
        newToken(yc, TOK_EDGEMARK);
        ejectGobble(yc, clt);
      }
      pm = punkMap(clt->tokNum);
      if (pm) {
        newToken(yc, pm);
        ejectGobblePassClear(yc, clt);
      }
      break;
    case STATE_AFTERDASH:
      if (clt->tokNum == '-')
        ejectGobblePassClear(yc, clt);
      else {
        pass(yc, doubleaGetValueAt(yc->m->d, 0).ptr);
        clear(yc);
      }
      transitTo(yc, STATE_NORMAL);
      break;

    default:
      assert("Illegal state in YCPunk" && 0);
  }
}

static struct YCWhiteSpace *ycWhiteSpaceNew(struct CLYieldChain *fm)
{
  struct YCWhiteSpace *ycs = clCalloc(sizeof(*ycs), 1);
  ycs->yc.next = fm;
  ycs->yc.process = ycWhiteSpacePF;
  return ycs;
}

static void ycWhiteSpacePF(struct CLYieldChain *yc, struct CLToken *clt)
{
  int isWS;
  isWS = (clt->tokNum == ' ' || clt->tokNum == '\n' ||
          clt->tokNum == '\r' || clt->tokNum == '\t');
  if (yc->m == NULL) {
    if (isWS) {
      newToken(yc, TOK_WHITESPACE);
      ejectGobble(yc, clt);
    }
  }
  if (yc->m) {
    if (isWS) {
      gobble(yc, clt);
    } else {
      if (doubleaSize(yc->m->d) > 0)
        pass(yc, yc->m);
      clear(yc);
    }
  }
}

static struct YCMerger *ycMergerNew(struct CLYieldChain *fm)
{
  struct YCMerger *ycs = clCalloc(sizeof(*ycs), 1);
  ycs->yc.next = fm;
  ycs->yc.process = ycMergerPF;
  return ycs;
}

static void ycMergerPF(struct CLYieldChain *yc, struct CLToken *clt)
{
  //upcastTo(YCMerger, yc);
  if (clt->tokNum > 255) {
    switch (clt->tokNum) {
      case TOK_STARTCOMMENT:
        newToken(yc, TOK_COMMENT);
        eject(yc, clt);
        break;

      case TOK_STARTSTRING:
        newToken(yc, TOK_STRING);
        eject(yc, clt);
        break;

      case TOK_ENDCOMMENT:
      case TOK_ENDSTRING:
        ejectPassClear(yc, clt);
        break;
    }
  }
  if (clt->tokNum < 256) {
    if (yc->m)
      ejectGobble(yc, clt);
    return;
  }
}

static struct YCTree *ycTreeNew(struct CLYieldChain *fm)
{
  struct YCTree *ycs = clCalloc(sizeof(*ycs), 1);
  ycs->yc.next = fm;
  ycs->yc.process = ycTreePF;
  ycs->nodeLabels = doubleaNew();
  ycs->edgeKeeper = doubleaNew(); /* uses .ip.x and .ip.y */
  return ycs;
}

void convertNodeToStringRecursive(struct CLToken *t, struct DoubleA *da)
{
  int i;
  if (t->tokNum < 255) {
      union PCTypes p = zeropct;
      p.i = t->tokNum;
      doubleaPush(da, p);
      return;
  }
  for (i = 0; i < doubleaSize(t->d); i += 1) {
    struct CLToken *c = (struct CLToken *) doubleaGetValueAt(t->d, i).ptr;
    convertNodeToStringRecursive(c, da);
  }
}

static char *convertNodeToString(struct CLToken *t)
{
  static char *retval;
  int i;
  struct DoubleA *da = doubleaNew();
  assert(t->tokNum > 255 && "Invalid CharToken for convert to String");
  convertNodeToStringRecursive(t, da);
  if (retval)
    free(retval);
  retval = clCalloc(doubleaSize(da)+1,1);

  for (i = 0; i < doubleaSize(da); i += 1)
    retval[i] = doubleaGetValueAt(da, i).i;

//  printf("Returning <%s> from cnts for %p with dasize %d:%p\n", retval, t, doubleaSize(da));
  doubleaFree(da);
  return retval;
}

static void ycTreePF(struct CLYieldChain *yc, struct CLToken *clt)
{
  char *str;
  upcastTo(YCTree, yc);
  if (clt->tokNum > 255) {
    if (clt->tokNum == TOK_WHITESPACE)
      eject(yc, clt);
    if (
        clt->tokNum == TOK_STARTBRACKET ||
        clt->tokNum == TOK_ENDBRACKET ||
        clt->tokNum == TOK_EDGEMARK ||
        clt->tokNum == TOK_SEPARATOR
       ) {
      if (clt->tokNum == TOK_SEPARATOR)
        ycs->curword = NULL;
      eject(yc, clt);
    }
  }
  if (clt->tokNum == TOK_COMMENT) {
    handleComment(ycs, clt);
    ejectGobble(yc, clt);
  }
  if (clt->tokNum < 256) {
    fprintf(stderr, " Unrecognized character: %d (%c)\n", clt->tokNum, clt->tokNum);
    exit(1);
  }
  switch(getState(ycs)) {
    case STATE_NORMAL:
      switch (clt->tokNum) {
        case TOK_WORD:
          str = convertNodeToString(clt);
//          printf("Converted the word to %s\n", str);
          if (strcmp(str, "graph") == 0) {
            transitTo(yc, STATE_GOTGRAPH);
            newToken(yc, TOK_TREE);
            ejectGobble(yc, clt);
          }
          break;
        default:
          fprintf(stderr, "Unrecognized token in tree:%s(%d)\n", convertNodeToString(clt), clt->tokNum);
          exit(1);
      }

    case STATE_GOTGRAPH:
      if (clt->tokNum == TOK_STRING) {
        handleTreeName(ycs, clt);
        ejectGobble(yc, clt);
        transitTo(yc, STATE_NEEDBLOCK);
      }
      break;

    case STATE_NEEDBLOCK:
      if (clt->tokNum == TOK_STARTBLOCK) {
        transitTo(yc, STATE_GOTBLOCK);
        ejectGobble(yc, clt);
      }
      break;

    case STATE_NEEDENDPOINT:
      if (clt->tokNum == TOK_WORD) {
        char *leftStr = clStrdup(convertNodeToString(ycs->curword));
        char *rightStr = clStrdup(convertNodeToString(clt));
        handleNewEdge(ycs, leftStr, rightStr);
        clFree(leftStr);
        clFree(rightStr);
        ejectGobble(yc, clt);
        transitTo(yc, STATE_GOTBLOCK);
        ycs->curword = NULL; // TODO: catch leak
      }
      break;

    case STATE_NEEDSEMI:
      if (clt->tokNum == TOK_ENDSTATEMENT) {
        ejectGobble(yc, clt);
        transitTo(yc, ycs->oldstate);
        ycs->oldstate = 0;
      }
      break;

    case STATE_NEEDPROPVAL:
      if (clt->tokNum == TOK_STRING) {
        char *leftStr = clStrdup(convertNodeToString(ycs->curword));
        char *rightStr = clStrdup(convertNodeToString(clt));
        handleSetProp(ycs, leftStr, rightStr);
        clFree(leftStr);
        clFree(rightStr);
        ycs->curword = NULL;
        transitTo(yc, STATE_GOTBLOCK);
        ejectGobble(yc, clt);
      }
      else {
        fprintf(stderr, "Expected string instead of %d\n", clt->tokNum);
        exit(1);
      }
      break;

    case STATE_GOTBLOCK:
      if (clt->tokNum == TOK_WORD) {
        if (ycs->curword) {
          char *leftStr = clStrdup(convertNodeToString(ycs->curword));
          char *rightStr = clStrdup(convertNodeToString(clt));
          handleNewEdge(ycs, leftStr, rightStr);
          clFree(leftStr);
          clFree(rightStr);
          ejectGobble(yc, clt);
          transitTo(yc, STATE_GOTBLOCK);
          ycs->curword = NULL;
        } else {
          ycs->curword = clt;
        }
        eject(yc, clt);
      }
      if (clt->tokNum == TOK_STARTBRACKET) {
        ycs->lastword = ycs->curword;
        if (ycs->curword) {
          handleNewNode(ycs, convertNodeToString(ycs->curword));
        }
        ycs->curword = NULL;
      }
      if (clt->tokNum == TOK_ENDBRACKET) {
        ycs->curword = ycs->lastword;
        ycs->lastword = NULL;
      }
      if (clt->tokNum == TOK_PROPSET) {
        ycs->oldstate = getState(ycs);
        transitTo(yc, STATE_NEEDPROPVAL);
        ejectGobble(yc, clt);
      }
      if (clt->tokNum == TOK_ENDSTATEMENT) {
        ycs->curword = NULL;
        eject(yc, clt);
      }

      break;

    default:
      fprintf(stderr, "Bad tree state: %d\n", getState(ycs));
      exit(1);
  }
}

struct CLToken *newSemToken(int symval)
{
  struct CLToken *clt = clCalloc(sizeof(*clt), 1);
  union PCTypes p;
  p.ptr = clt;
  assert(symval > 255 && "Invalid semantic token code");
  clt->tokNum = symval;
  clt->d = doubleaNew();
  if (currentArena)
    doubleaPush(currentArena->tokensToFree, p);
  return clt;
}

struct CLToken *newCharToken(unsigned char c)
{
  struct CLToken *clt = clCalloc(sizeof(*clt), 1);
  union PCTypes p;
  p.ptr = clt;
  clt->tokNum = c;
  if (currentArena)
    doubleaPush(currentArena->tokensToFree, p);
  return clt;
}

static void freeCLToken(struct CLToken *c)
{
  if (c->d) {
    doubleaFree(c->d);
    c->d = NULL;
  }
  free(c);
}

static void ejectPassClear(struct CLYieldChain *yc, struct CLToken *clt)
{
  eject(yc, clt);
  pass(yc, yc->m);
  clear(yc);
}

static void ejectGobblePassClear(struct CLYieldChain *yc, struct CLToken *clt)
{
  ejectGobbleAndPass(yc, clt);
  clear(yc);
}

static void ejectGobbleAndPass(struct CLYieldChain *yc, struct CLToken *clt)
{
  eject(yc, clt);
  gobble(yc, clt);
  pass(yc, yc->m);
}

void clear(struct CLYieldChain *yc)
{
  yc->m = NULL; // TODO: fix mem leak
}

void gobble(struct CLYieldChain *yc, struct CLToken *clt)
{
  union PCTypes p = zeropct;
  p.ptr = clt;
  assert(yc->m && "No token set");
  assert(yc->m->d && "No doublea set!");
  doubleaPush(yc->m->d, p);
}

void ejectGobble(struct CLYieldChain *yc, struct CLToken *clt)
{
  eject(yc, clt);
  gobble(yc, clt);
}

void eject(struct CLYieldChain *yc, struct CLToken *clt)
{
  if (clt == yc->pasTok)
    yc->passed = 1;
}

void pass(struct CLYieldChain *yc, struct CLToken *clt)
{
  assert(yc);
  assert(clt);
  eject(yc, clt);
  if (yc->next)
    receiveToken(yc->next, clt);
}

void receiveToken(struct CLYieldChain *yc, struct CLToken *clt)
{
  yc->passed = 0;
  yc->pasTok = clt;
  yc->process(yc, clt);
  if (!yc->passed)
    pass(yc, clt);
}

void feedBytes(const struct DotParserInstance *dpi, struct CLYieldChain *toWhom)
{
  int i;
  for (i = 0; i < datablockSize(dpi->db); i += 1) {
//    printf("Feeding byte %d: %c\n", i, datablockData(dpi->db)[i]);
    receiveToken(toWhom, newCharToken(datablockData(dpi->db)[i]));
  }
}

void freeDotParser(struct DotParserInstance *dp)
{
  if (currentArena && currentArena == dp) {
    int i;
    currentArena = NULL;
    for (i = 0; i < doubleaSize(dp->tokensToFree); i += 1) {
      union PCTypes p;
      p = doubleaGetValueAt(dp->tokensToFree, i);
      freeCLToken((struct CLToken *) p.ptr);
    }
    doubleaFree(dp->tokensToFree);
    dp->tokensToFree = NULL;
    doubleaFree(dp->commentBuf);
    dp->commentBuf = NULL;
  }
  datablockFreePtr(dp->db);
  dp->db = NULL;
  free(dp);
}

struct DotParserInstance *newDotParser(struct DataBlock *db)
{
  struct DotParserInstance *dp = clCalloc(sizeof(struct DotParserInstance), 1);
  assert(dp);
  dp->commentBuf = doubleaNew();
  dp->tokensToFree = doubleaNew();
  dp->db = datablockClonePtr(db);
  currentArena = dp;
  return dp;
}

static void handleSetProp(struct YCTree *tree, const char *leftstr, const char *rightstr) {
//  printf("PROP    for (%s) -> (%s)\n", leftstr, rightstr);
  if (tree->isNode) {
    union PCTypes p = zeropct;
    p.ptr = clStrdup(rightstr);
    doubleaSetValueAt(tree->nodeLabels, tree->nodenum, p);
  }
}
static void handleNewEdge(struct YCTree *tree, const char *leftstr, const char *rightstr) {
  tree->isNode = 0;
  if (leftstr[0] == 'i' || rightstr[0] == 'i')
    return;
  int leftnum = atoi(leftstr);
  int rightnum = atoi(rightstr);
  assert(leftnum <= tree->maxnodenum);
  assert(rightnum <= tree->maxnodenum);
  union PCTypes p = zeropct;
  p.ip.x = leftnum;
  p.ip.y = rightnum;
  doubleaPush(tree->edgeKeeper, p);
//  printf("EDGE    for (%d) -- (%d)\n", leftnum, rightnum);
}

static void handleNewNode(struct YCTree *yctree, const char *str)
{
//  printf("NODE    for (%s)\n", str);
  if (str[0] == 'i')
    return;
  yctree->nodenum = atoi(str);
  yctree->isNode = 1;
  if (yctree->nodenum > yctree->maxnodenum)
    yctree->maxnodenum = yctree->nodenum;
  doubleaSetValueAt(yctree->nodeLabels, yctree->nodenum, zeropct);
}

static void handleComment(struct YCTree *tree, struct CLToken *clt)
{
//  printf("COMMENT for %s\n", convertNodeToString(clt));
}

static void handleTreeName(struct YCTree *tree, struct CLToken *clt)
{
//  printf("TREENAME is %s\n", convertNodeToString(clt));
}

#if 0
static void showResults(struct YCTree *yctree)
{
  int i;
  for (i = 0; i <= yctree->maxnodenum; i += 1) {
    union PCTypes p = doubleaGetValueAt(yctree->nodeLabels, i);
    printf("Node #%d:     (%s)\n", i, treeaIsQuartettable(yctree->ta,i) ? (char *) p.ptr : "(kernel)");
  }
}
#endif

struct DotParseTree *parseDotDB(struct DataBlock *db, struct DataBlock *matdb)
{
  int i, j, leafcount;
  struct AdjAdaptor *aa;
  struct StringStack *labels;

  struct DotParseTree *dpt = clCalloc(sizeof(*dpt), 1);
  struct DotParserInstance *dp = newDotParser(db);

  struct YCTree *yctree = ycTreeNew(NULL);
  struct YCNames *names = ycNamesNew(downcast(yctree));
  struct YCPunk *punk = ycPunkNew(downcast(names));
  struct YCWhiteSpace *whitespace = ycWhiteSpaceNew(downcast(punk));
  struct YCMerger *merger = ycMergerNew(downcast(whitespace));
  struct YCComments *comdetector = ycCommentsNew(downcast(merger));
  struct YCStrings *strdetector = ycStringsNew(downcast(comdetector));
  feedBytes(dp, downcast(strdetector));
    leafcount = (yctree->maxnodenum+3)/2;
    yctree->ta = treeaNew(0, leafcount);
    aa = treeaAdjAdaptor(yctree->ta);
    for (i = 0; i <= yctree->maxnodenum; i += 1) {
      for (j = 0; j <= yctree->maxnodenum; j += 1) {
        adjaSetConState(aa, i, j, 0);
      }
    }
    for (i = 0; i < doubleaSize(yctree->edgeKeeper); i += 1) {
      union PCTypes p;
      p = doubleaGetValueAt(yctree->edgeKeeper, i);
      adjaSetConState(aa, p.ip.x, p.ip.y, 1);
    }
    doubleaFree(yctree->edgeKeeper);
    yctree->edgeKeeper = NULL;
    if (matdb) {
      dpt->dm = clbDBDistMatrix(matdb);
      /*
      for (i = 0; i < dpt->dm->size1; i += 1) {
        for (j = 0; j < dpt->dm->size2; j += 1) {
          printf("%f ", gsl_matrix_get(dpt->dm, i, j));
        }
        printf("\n");
      }
      */
      labels = clbDBLabels(matdb);
      assert(labels);
      dpt->labels = labels;
      for (i = 0; i <= yctree->maxnodenum; i += 1) {
        if (treeaIsQuartettable(yctree->ta,i)) {
          for (j = 0; j < leafcount; j += 1) {
            if (strcmp(stringstackReadAt(labels, j), doubleaGetValueAt(yctree->nodeLabels, i).ptr) == 0) {
              treealabelpermSetColumnIndexToNodeNumber(yctree->ta, j, i);
  //            printf("Must set %d to point to %d\n", i, j);
            }
          }
        }
      }
    }
  dpt->tree = yctree->ta;
  freeDotParser(dp);
  clFree(yctree);
  clFree(names);
  clFree(punk);
  clFree(whitespace);
  clFree(merger);
  clFree(comdetector);
  clFree(strdetector);
  return dpt;
}
