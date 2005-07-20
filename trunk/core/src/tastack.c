#include <malloc.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>


#include <complearn/complearn.h>

#define INIT_TS_SIZE 11

#if 0
struct TAstack {
	struct TransAdaptor **ta;
	int size;
  int allocsize;
};
#endif

struct TAstack {
  struct DoubleA *da;
};

struct TAstack *newTAStack()
{
	struct TAstack *ts;
	ts = (struct TAstack*)gcalloc(sizeof(struct TAstack), 1);
  ts->da = newDoubleDoubler();
	return ts;
}

int freeTS(struct TAstack *ts)
{
	freeDoubleDoubler(ts->da);
	ts->da = NULL;
	free(ts);
	return CL_OK;
}

int pushTS(struct TAstack *ts, struct TransAdaptor *ta)
{
  union pctypes p;

  p.ta = ta;
  pushValue(ts->da, p);
  return CL_OK;
}

struct TransAdaptor *shiftTS(struct TAstack *ts)
{
  union pctypes p;
  memset(&p, 0, sizeof(p));
	if (sizeTS(ts) == 0) return p.ta;
	p = shiftDoubleDoubler(ts->da);
  return p.ta;
}

struct TransAdaptor *popTS(struct TAstack *ts)
{
  union pctypes p;
  memset(&p, 0, sizeof(p));
	if (sizeTS(ts) == 0) return p.ta;
	p = popDoubleDoubler(ts->da);
  return p.ta;
}

int isEmptyTS(struct TAstack *ts)
{
	return getSize(ts->da) == 0;
}

int sizeTS(struct TAstack *ts)
{
	return getSize(ts->da);
}

struct TransAdaptor *searchTS(void *ts, void *s, t_searchfunc searchfunc)
{
	struct TransAdaptor *curta = NULL;
	curta = searchfunc(ts,s);
	return curta;
}

/*
struct TransAdaptor *sequentialSearchTS(void *ts, void *s)
{
	struct TAstack *curts = (struct TAstack *)ts;
	const char *curstr = (const char *)s;
  int i;
	struct TransAdaptor *curta = NULL;
	struct TransAdaptor *found = NULL;
  for (i = 0 ; i < curts->size ; i++) {
	  curta = curts->ta[i];
		if (curta && strcmp(curta->sn(),curstr) == 0) {
			found = curta; 
		  break;
		}
	}
	return found;
}
*/
