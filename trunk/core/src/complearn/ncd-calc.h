#ifndef __NCDCALC_H
#define __NCDCALC_H

#include <complearn/environment.h>

/*! \file ncd-calc.h */

/* should this be static? */
double mndf(double ca, double cb, double cab, double cba);
double ncdfunc(struct DataBlock *a, struct DataBlock *b, struct GeneralConfig *cur);

#endif
