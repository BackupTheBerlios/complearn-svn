#ifndef __MQUARTET_H
#define __MQUARTET_H

#define MAXNODES 1024

/*! \file quartet.h */

struct UnrootedBinary;
struct DRA;

#include <sys/types.h>
/**
 * The basic type for node labels.
 * In this system, node labels are just unsigned chars to save memory space
 */
typedef unsigned char qbase_t;

/**
 * The basic cost accumulator type, used for weighting quartets.
 * This is just a double precision floating point number.
 */
typedef double weight_t;

/**
 * The basic distance type, used for the distance matrix.
 * This is the same as a weight_t, a double precision floating point number.
 */
typedef double dist_t;

/** \brief a labelled undirected binary tree with four leaf labels in a
 * particular one of three possible configurations
 *
 * \struct Quartet
 *
 * This structure holds an array of four elements.  The quartet is
 * arranged like so:
 *
 *.0---1.<br>
 *....|...<br>
 *....|...<br>
 *.2---3.<br>
 *
 * That is to say, q[0] and q[1] form a subtree, as do q[2] and q[3].
 * In a quartet, all four leaf-labels must be unique.
 * A quartet is said to be consistent with (or embedded within) a larger tree
 * if and only if the paths connecting q[0]-q[1] and q[2]-q[3] do not
 * intersect.  For any group of four leaf-labels, there are only three possible
 * configurations; for any tree containing all four leaf-labels, exactly
 * one of the three possible quartets is consistent at any time, and
 * the other two are inconsistent.
 *
 * If q[0] < q[1] and q[0] < q[2] and q[2] < q[3], then we say that the
 * Quartet is in normal form.  Every quartet may be converted to a normal
 * form without changing its meaning by swaps.
 */
struct Quartet {
  qbase_t q[4]; /*!< The quartet labels as an array of four elements */
};

/** \brief Holds the information necessary to evaluate a tree for its cost
 * within the context of a given distance matrix
 * \struct TreeScore
 */
struct TreeScore;

struct Quartet permuteLabels(qbase_t lab[4], int which); /* between 0, 1, 2 */
struct Quartet permuteLabelsDirect(qbase_t i, qbase_t j, qbase_t k, qbase_t m, int which);
void normalizeQuartet(struct Quartet *q);
void freeSPMSingle(struct DRA *d);
void freeSPMSet(struct DRA *d);

#endif

