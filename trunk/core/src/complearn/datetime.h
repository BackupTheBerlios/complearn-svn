#ifndef __DATETIME_H
#define __DATETIME_H

/*! \file datetime.h */

struct CLDateTime;

struct CLDateTime *cldatetimeNow(void);
double cldatetimeStaticTimer(void);
void cldatetimeFree(struct CLDateTime *cldt);
const char *cldatetimeToString(struct CLDateTime *c);
const char *cldatetimeToHumString(struct CLDateTime *c);
unsigned long cldatetimeToInt(struct CLDateTime *c);
char *cldatetimeToDayString(struct CLDateTime *c);
struct CLDateTime *cldatetimeAddDays(const struct CLDateTime *basis, int offset);
void clSleepMillis(int howManyMillis);

#endif
