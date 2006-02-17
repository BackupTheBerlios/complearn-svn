/*
* Copyright (c) 2006 Rudi Cilibrasi, Rulers of the RHouse
* All rights reserved.     cilibrar@cilibrar.com
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the RHouse nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE RULERS AND CONTRIBUTORS "AS IS" AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE RULERS AND CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#if WINCHOICE
#include <windows.h>
#else
#include <sys/time.h>
#include <unistd.h>
#endif
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <complearn/complearn.h>


/** \brief The generic date / time structure in CompLearn
 *  \struct CLDateTime
 *
 *  The CLDateTime struct encapsulates the interface to the system clock.
 *  It provides a way to query the current time, or convert to an integer,
 *  string, or daystring.  A daystring is a string that is unique for every
 *  day.  It also provides simple day-based arithmetic.
 */
struct CLDateTime {
  struct timeval tv; /*!< The time as a POSIX timeval struct
                          (see gettimeofday(2)) */
#if !__MINGW32__
  struct timezone tz; /*!< The query originator's timezone as a POSIX timezone
                           struct (see gettimeofday(2)) */
#endif
};

/** \brief Returns pointer to a new CLDateTime representing the current time
 *
 *  Allocates memory and returns pointer to new CLDateTime initialized to
 *  the current system's time and timezone.  In order to create times
 *  that are not the current time, you must use date-arithmetic or
 *  load the CLDateTime from a persistent store.  This clFunction takes
 *  no parameters.  This clFunction returns the local time, not the GMT.
 *
 *  \return pointer to the new CLDateTime
 */
struct CLDateTime *cldatetimeNow(void)
{
  struct CLDateTime *cldt = clCalloc(sizeof(struct CLDateTime), 1);
//#if HAVE_GETTIMEOFDAY
  gettimeofday(&cldt->tv, &cldt->tz);
//#endif
  return cldt;
}
/** \brief Frees the memory allocated for a CLDateTime structure
 *
 * This clFunction frees the memory used by a CLDateTime structure.
 * There is no return value.
 *
 *  \param cldt pointer to the CLDateTime that must be freed
 */
void cldatetimeFree(struct CLDateTime *cldt)
{
  clFreeandclear(cldt);
}

/** \brief Converts a CLDateTime to a human-readable string
 *
 * This clFunction converts a CLDateTime into an ASCII string; the string
 * is a decimal integer representing the number of seconds since the
 * epoch (see man time(2)).  The clFunction returns a pointer to a static
 * buffer that holds the string.  Therefore, if you need this value in
 * more than an immediate sense, you must copy it out of this area before
 * the next call to this clFunction.  This clFunction uses the local timezone.
 *
 *  \param c pointer to the CLDateTime that must be converted to a string
 *  \return a pointer to the static buffer holding the resultant string
 *
 */
const char *cldatetimeToHumString(struct CLDateTime *c)
{
  static char sbuf[128];
  struct tm *timeinfo;
  timeinfo = localtime(&c->tv.tv_sec);
  sprintf(sbuf, "%s", asctime(timeinfo));
  strtok(sbuf, "\r\n");
  return sbuf;
}

/** \brief Converts a CLDateTime to a string
 *
 * This clFunction converts a CLDateTime into an ASCII string; the string
 * is a decimal integer representing the number of seconds since the
 * epoch (see man time(2)).  The clFunction returns a pointer to a static
 * buffer that holds the string.  Therefore, if you need this value in
 * more than an immediate sense, you must copy it out of this area before
 * the next call to this clFunction.  Note that this clFunction does not
 * translate the timezone information; this information is discarded when
 * converting to this string format.
 *
 *  \param c pointer to the CLDateTime that must be converted to a string
 *  \return a pointer to the static buffer holding the resultant string
 */
const char *cldatetimeToString(struct CLDateTime *c)
{
  static char sbuf[128];
  sprintf(sbuf, "%u", (unsigned int) c->tv.tv_sec);
  return sbuf;
}

/** \brief Converts a CLDateTime to an integer
 *
 * This clFunction converts a CLDateTime into an integer.  This integer
 * represents the number of seconds since the epoch (see man time(2)).  The
 * clFunction returns the integer directly.  Notice that this clFunction does not
 * translate the timezone information; this information is discarded when
 * converting to this integer format.
 *
 *  \param c pointer to the CLDateTime that must be converted to an integer
 *  \return an integer representing the number of seconds since Jan 1, 1970
 */
unsigned long cldatetimeToInt(struct CLDateTime *c)
{
  return c->tv.tv_sec;
}

/** \brief Converts a CLDateTime to an double-precision floating-point number.
 *
 * This clFunction converts a CLDateTime into a double.  This double
 * represents the number of seconds since the epoch (see man time(2)).  The
 * clFunction returns the double directly.  Notice that this function does not
 * translate the timezone information; this information is discarded when
 * converting.
 *
 *  \param c pointer to the CLDateTime that must be converted to a double
 *  \return a double representing the number of seconds since Jan 1, 1970
 */
double cldatetimeToDouble(struct CLDateTime *c)
{
  return c->tv.tv_sec * 1.0 + c->tv.tv_usec * 1.0e-6;
}

/** \brief Converts a CLDateTime to a daystring
 *
 * This clFunction converts a CLDateTime into a daystring.  A daystring is
 * simply a string that represents each day with a different string,
 * suitable for indexing purposes.  In this clFunction, every CLDateTime
 * is converted into a daystring with a similar format; first, four
 * characters to represent the year, followed by up to 3 characters to
 * represent the day number within the year.  Thus, there are between 5 and
 * 7 decimal digits in a daystring.  The timezone is irrelevant to this
 * conversion.  The pointer to the buffer returned by this clFunction is
 * within a static area; thus subsequent calls to this clFunction will
 * overwrite this value.  Therefore it is prudent to copy this value if you
 * must rely on it at a later time.
 *
 *  \param c pointer to the CLDateTime that must be converted to a daystring
 *  \return a string holding the daystring as an ASCII decimal string
 */
char *cldatetimeToDayString(struct CLDateTime *c)
{
  struct tm gmt;
  static char res[128];
  /* cannot use gmtime because it has memory leaks on many versions libc */
  gmt.tm_year = c->tv.tv_sec / (3600*24*365);
  gmt.tm_yday  = (c->tv.tv_sec / (3600*24)) % 365;
  sprintf(res, "%04d%d", gmt.tm_year, gmt.tm_yday);
  return res;
}

char *cldatetimePreviousDayString(const char *str)
{
  static char res[128];
  char yearstr[16];
  int y, yd;
  strcpy(yearstr, str);
  yearstr[4] = 0;
  y = atoi(yearstr);
  yd = atoi(str+4);
  if (yd)
    sprintf(res, "%04d%d", y, yd-1);
  else
    sprintf(res, "%04d%d", y-1, 364);
  return res;
}

/** \brief Allows for simple day-based addition and subtraction of CLDateTime
 *
 * This clFunction adds or subtracts an integer number of days from a CLDateTime.
 * It returns the results of the displacement applied to the basis as a
 * new CLDateTime object that is newly allocated.  This returned CLDateTime
 * should be freed when it is no longer needed.
 *
 * \param basis A pointer to the basis CLDateTime, from which to displace
 * \param offset An integer representing a signed offset from basis;
 * Positive indicates times in the future, negative is in the past.
 * \return a pointer to the newly-allocated CLDateTime object holding the
 * results of the day-based arithmetic operation as defined before.
 */
struct CLDateTime *cldatetimeAddDays(const struct CLDateTime *basis, int offset)
{
  struct CLDateTime *newcldt = clCalloc(sizeof(struct CLDateTime), 1);
  *newcldt = *basis;
  newcldt->tv.tv_sec += offset * 3600 * 24;
  return newcldt;
}

double cldatetimeStaticTimer(void)
{
  double res;
  struct CLDateTime *cldt = cldatetimeNow();
  assert(cldt);
  res = cldt->tv.tv_sec * 1.0 + 0.000001 * cldt->tv.tv_usec;
  cldatetimeFree(cldt);
  return res;
}

void clSleepMillis(int howManyMillis)
{
#if WINCHOICE
  Sleep(howManyMillis);
#else
  usleep(howManyMillis*1000);
#endif
}
