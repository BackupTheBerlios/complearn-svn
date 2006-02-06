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
  struct DRA *da;
};

struct TransformAdaptorStack *clNewTAStack()
{
	struct TransformAdaptorStack *ts;
	ts = (struct TransformAdaptorStack*)clCalloc(sizeof(struct TransformAdaptorStack), 1);
  ts->da = clDraNew();
	return ts;
}

int clFreeTS(struct TransformAdaptorStack *ts)
{
	clDraFree(ts->da);
	ts->da = NULL;
	free(ts);
	return CL_OK;
}

int clPushTS(struct TransformAdaptorStack *ts, struct TransformAdaptor *ta)
{
  union PCTypes p;

  p.ta = ta;
  clDraPush(ts->da, p);
  return CL_OK;
}

struct TransformAdaptor *clShiftTS(struct TransformAdaptorStack *ts)
{
  union PCTypes p;
  memset(&p, 0, sizeof(p));
	if (clSizeTS(ts) == 0) return p.ta;
	p = clDraShift(ts->da);
  return p.ta;
}

struct TransformAdaptor *clPopTS(struct TransformAdaptorStack *ts)
{
  union PCTypes p;
  memset(&p, 0, sizeof(p));
	if (clSizeTS(ts) == 0) return p.ta;
	p = clDraPop(ts->da);
  return p.ta;
}

int clIsEmptyTS(struct TransformAdaptorStack *ts)
{
	return clDraSize(ts->da) == 0;
}

int clSizeTS(struct TransformAdaptorStack *ts)
{
	return clDraSize(ts->da);
}

struct TransformAdaptor *clSearchTS(void *ts, void *s, t_searchclFunc searchclFunc)
{
	struct TransformAdaptor *curta = NULL;
	curta = searchclFunc(ts,s);
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
