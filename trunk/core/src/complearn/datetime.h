#ifndef __DATETIME_H
#define __DATETIME_H

/*! \file datetime.h */

struct CLDateTime;

struct CLDateTime *cldtNow(void);
double cldtGetStaticTimer(void);
void cldtfree(struct CLDateTime *cldt);
const char *cldt_to_s(struct CLDateTime *c);
const char *cldt_to_humstr(struct CLDateTime *c);
unsigned long cldt_to_i(struct CLDateTime *c);
char *cldt_daystring(struct CLDateTime *c);
struct CLDateTime *cldtAddDays(const struct CLDateTime *basis, int offset);
void sleepMillis(int howManyMillis);

#endif
