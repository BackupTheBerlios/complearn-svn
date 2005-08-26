#include <malloc.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>


#include <complearn/complearn.h>

#define INIT_TS_SIZE 11

#if 0
struct TransformAdaptorStack {
	struct TransformAdaptor **ta;
	int size;
  int allocsize;
};
#endif

struct TransformAdaptorStack {
  struct DoubleA *da;
};

struct TransformAdaptorStack *newTAStack()
{
	struct TransformAdaptorStack *ts;
	ts = (struct TransformAdaptorStack*)gcalloc(sizeof(struct TransformAdaptorStack), 1);
  ts->da = newDoubleDoubler();
	return ts;
}

int freeTS(struct TransformAdaptorStack *ts)
{
	freeDoubleDoubler(ts->da);
	ts->da = NULL;
	free(ts);
	return CL_OK;
}

int pushTS(struct TransformAdaptorStack *ts, struct TransformAdaptor *ta)
{
  union PCTypes p;

  p.ta = ta;
  pushValue(ts->da, p);
  return CL_OK;
}

struct TransformAdaptor *shiftTS(struct TransformAdaptorStack *ts)
{
  union PCTypes p;
  memset(&p, 0, sizeof(p));
	if (sizeTS(ts) == 0) return p.ta;
	p = shiftDoubleDoubler(ts->da);
  return p.ta;
}

struct TransformAdaptor *popTS(struct TransformAdaptorStack *ts)
{
  union PCTypes p;
  memset(&p, 0, sizeof(p));
	if (sizeTS(ts) == 0) return p.ta;
	p = popDoubleDoubler(ts->da);
  return p.ta;
}

int isEmptyTS(struct TransformAdaptorStack *ts)
{
	return getSize(ts->da) == 0;
}

int sizeTS(struct TransformAdaptorStack *ts)
{
	return getSize(ts->da);
}

struct TransformAdaptor *searchTS(void *ts, void *s, t_searchfunc searchfunc)
{
	struct TransformAdaptor *curta = NULL;
	curta = searchfunc(ts,s);
	return curta;
}

/*
struct TransformAdaptor *sequentialSearchTS(void *ts, void *s)
{
	struct TransformAdaptorStack *curts = (struct TransformAdaptorStack *)ts;
	const char *curstr = (const char *)s;
  int i;
	struct TransformAdaptor *curta = NULL;
	struct TransformAdaptor *found = NULL;
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
