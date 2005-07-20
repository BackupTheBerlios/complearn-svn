#ifndef __QUARTETOBS_H
#define __QUARTETOBS_H

/** \brief a QuartetObserver allows for an ordered traversal of quartets
 *
 * \struct QuartetObserver
 *
 * The QuartetObserver interface provides for a method-pointer callback
 * external iteration over quartet groupings or topologies.  The inside of
 * the iteration loop is a Visitor-style pattern.
 */

#include <complearn/quartet.h>

struct QuartetObserver {
  void *udata;
  t_quartetvisitor qv;
};
typedef int (*t_quartetvisitor)(struct QuartetObserver *qo, struct Quartet q, int qphase);
#define QTVISIT_NEXT 0
#define QTVISIT_NEXTQUARTET 1
#define QTVISIT_BREAK 2

