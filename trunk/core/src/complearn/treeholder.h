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
#ifndef __TREEHOLDER_H
#define __TREEHOLDER_H

/*! \file treeholder.h */

/** \brief holds a single candidate tree and tries to improve it monotonically
 *
 * A TreeHolder stores a TreeAdaptor as well as a distance matrix for scoring
 * the tree.  It uses this to try to step the tree towards progressively better
 * trees until finally reaching some maximum.  The maximum value is impossible
 * to determine in general and therefore is typically inferred by agreement
 * over a large number of concurrent TreeHolder trial threads.  A single
 * TreeHolder can give but little indication of when it is done, because
 * every tree search gets progressively slower for some amount of time
 * before finally reaching a maximum.  For most distance matrices, this
 * maximum is below 1 and thus not convenient for a termination criterion.
 * In order to improve the existing tree simply call clTreehImprove over and
 * over.  Occasionally it will succeed in improving the tree and it will
 * automatically retain the best improvements.
 *
 * \sa treeholder.h
 * \struct TreeHolder
 */
struct TreeHolder;

struct TreeAdaptor;

/** \brief allocates a new TreeHolder given a gsl_matrix distmat and TreeAdaptor
 *
 * This is the basic constructor for the TreeHolder struct.  After calling
 * this function the next step is typically clTreehScramble() if
 * randomization is required (for instance, in Monte Carlo agreement search)
 * and then many successive clTreehImprove() calls.  The TreeHolder must
 * eventually be deallocated with clTreehFree().
 *
 * \param distmat pointer to a distance matrix in the form of a gsl_matrix
 * \param tra pointer to a TreeAdaptor to use as a starting tree
 * \return pointer to the newly allocated TreeHolder
 */
struct TreeHolder *clTreehNew(const gsl_matrix *distmat, struct TreeAdaptor *tra);

/** \brief Returns the current scaled S(T) tree benefit score between 0 and 1
 *
 * At any given point in time, a TreeHolder contains a TreeAdaptor in a
 * particular configuration.  This configuration may be scored in the
 * context of the distance matrix given at construction with clTreehNew().
 * The result of this scoring is available through the clTreehScore() function.
 * This scaled tree benefit score should be at least 0 and no more than 1.
 * 1 represents a perfect tree.  A typical random tree would score near
 * 1/3 most commonly.
 *
 * \param th pointer to the TreeHolder to inspect
 * \return scaled S(T) tree benefit score value between 0 and 1
 */
double clTreehScore(const struct TreeHolder *th);

/** \brief Returns the current TreeAdaptor stored in this TreeHolder
 *
 * This function is used to retrieve the current best-fitting tree
 * stored within the TreeHolder.
 *
 * \param th pointer to the TreeHolder to inspect
 * \return pointer to a TreeAdaptor that represents the current tree
 */
struct TreeAdaptor *clTreehTreeAdaptor(const struct TreeHolder *th);

/** \brief Tries a single new random tree and with luck improves
 *
 * This function must be called over and over to evolve a best-fitting
 * tree.  It will usually return 0 indicating no improvement.  But it
 * will occasionally return 1, indicating that it tried a random tree
 * mutation and found a new better "best" tree.  A simple single-threaded
 * termination condition might be receiving 100,000 consecutive 0's as
 * returns from this function.  At that point you might guess there was no
 * longer any improvement possible and consider the current tree the best.
 *
 * \param th pointer to the TreeHolder to inspect
 * \return 0 indicating no improvement or 1 indicating a successful mutation
 */
int clTreehImprove(struct TreeHolder *th);

/** \brief makes a new copy of an existing TreeHolder
 *
 * This function clones a TreeHolder, making a whole new TreeAdaptor as
 * well based on the original.  It should be similar in every way.  You
 * must free this using clTreehFree() just as you must in clTreehNew().
 *
 * \param th pointer to the TreeHolder to be cloned
 * \return pointer to the new copy of the old TreeHolder
 */
struct TreeHolder *clTreehClone(const struct TreeHolder *th);

/** \brief returns the total number of trees searched so far
 *
 * This function lets the user query how many candidate trees in total have
 * been searched.  Most of these trials normally result in failures to
 * improve.
 * \param th pointer to the TreeHolder to be inspected
 * \return integer number of trees examined in the TreeHolder full lifetime
 */
int clTreehTreeCount(const struct TreeHolder *th);

/** \brief returns the running number of consecutive failed improvement attempts
 *
 * This function counts how many failed attempts at improvement have occured
 * in a row up to this point.  When a success occurs this number is reset
 * to 0, and then goes up one with each consecutive failure.  This can be
 * used as a simple termination condition.
 *
 * \param th pointer to the TreeHolder to be inspected
 * \return integer number of successive failures up to the present
 */
int clTreehFailCount(const struct TreeHolder *th);

/** \brief Randomly scrambles the TreeAdaptor within this TreeHolder
 *
 * This function is used for Monte-Carlo agreement type termination or
 * stability criteria.  It does many random mutations on the underlying
 * TreeAdaptor, thus ensuring with high likelihood that the tree is
 * different than any other trees that have also been scrambled.
 * It is usually used as the first step after construction but before
 * attempts at improvement through clTreehImprove().
 * This function always succeeds, there is no return value.
 *
 * \param th pointer to the TreeHolder to be scrambled
 */
void clTreehScramble(struct TreeHolder *th);

/** \brief This function retrieves the distance matrix used for scoring
 *
 * Sometimes it is useful to retrieve the distance matrix that was used
 * for scoring the TreeAdaptor candidate trees in this TreeHolder.  In
 * that case the clTreehDistMatrix function can return a gsl_matrix for
 * general use.
 *
 * \param th pointer to the TreeHolder to be inspected
 * \return pointer to the gsl_matrix holding distance values
 */
gsl_matrix *clTreehDistMatrix(const struct TreeHolder *th);

/** \brief Sets the "tree index" opaque integer field for this TreeHolder
 *
 * This function provides access to a simple opaque integer field usually
 * used for indexing within a large array of TreeHolder instances.  This
 * can be used as a simple label for instance for one of many concurrent
 * TreeHolder searches.  This function has no return value.
 *
 * \sa clTreehGetTreeIndex()
 *
 * \param th pointer to the TreeHolder to be inspected
 * \param treeind integer indicating value to be stored
 */
void clTreehSetTreeIndex(struct TreeHolder *th, int treeind);

/** \brief Gets the "tree index" opaque integer field for this TreeHolder
 *
 * This function reads the value previously stored using clTreehSetTreeIndex()
 * for this TreeHolder.  It is most commonly used to label distinct but
 * concurrently running TreeHolder instances.  This can be used for
 * agreement tests in Monte Carlo style searches.
 *
 * \param th pointer to the TreeHolder to be inspected
 * \return integer indicating value that was most recently stored
 */
int clTreehGetTreeIndex(struct TreeHolder *th);

/** \brief frees memory associated with a TreeHolder
 *
 * This function frees the memory associated with a TreeHolder.
 * There is no return value.
 *
 * \param th pointer to the TreeHolder to be freed
 */
void clTreehFree(struct TreeHolder *th);

int clTreehMutationCount(struct TreeHolder *th);

#endif
