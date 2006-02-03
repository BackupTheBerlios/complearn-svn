#ifndef __NCDCALC_H
#define __NCDCALC_H

#include <complearn/environment.h>

/*! \file ncd-calc.h */

/* should this be static? */
double clMndf(double ca, double cb, double cab, double cba);
double clNcdclFunc(struct DataBlock *a, struct DataBlock *b, struct GeneralConfig *cur);

#endif
