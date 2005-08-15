#ifndef __SPRINGBALL_H
#define __SPRINGBALL_H

#if GSL_RDY
#include <gsl/gsl_odeiv.h>
#include <gsl/gsl_vector.h>

/*! \file springball.h */

/** \brief charged ball and spring physical simulation for animation effect
 * \struct SpringBallSystem
 *
 * The SpringBallSystem evolves a multibody phsyical simulation over time
 * using a numerical approximation of the differential equations due to
 * Newton's Law of Acceleration, Coulomb's Law of Electrostatic Repulsion,
 * and Hook's Spring Law.  Additionally, the SpringBallSystem is under the
 * influence of a simple viscous drag to allow for stability.
 *
 * Each ball has a 3-d coordinate storing its position and another 3d
 * coordinate for its velocity, for a total of six elements of state per ball.
 * Each ball may also be attached (or not) from every other ball by a
 * spring.  This allows for simple tree-type structures to be shown as a
 * smooth animation.  Each ball has a charge equal to that of every other
 * ball, so all balls naturally repel one another.  Only the springs may hold
 * them within proximity of one another.
 */
struct SpringBallSystem;

/** \brief Converts an AdjA into a GSL-style matrix with 1's for connected nodes
 *
 * convertAdjAToGSLMatrix() takes a pointer to an AdjA adjacency matrix
 * adaptor and converts it into a standard gsl_matrix.  This is the standard
 * adjacency-matrix representation where each entry at location i, j is
 * 1 if nodes i and j are connected, or 0 otherwise.
 *
 *  \param aa pointer to AdjA
 *  \return pointer to new gsl_matrix
 */
gsl_matrix *convertAdjAToGSLMatrix(struct AdjA *aa);

/** \brief stepTowardsTree() allows for smooth time-splicing of dynamic springs
 *
 * When displaying a 3D version of a tree as a springball diagram, it may
 * happen that a new tree is discovered that is better than the currently
 * displayed one.  In this situation, at least one spring must disappear
 * and another must appear somewhere new in order to transform the old tree
 * into a new best one.  Doing this instantaneously would result in visually
 * uncomfortable jerkiness and is better handled using a smoothing matrix.
 * The smoothing matrix is the standard binary adjacency matrix (as returned
 * instantaneously from convertAdjAToGSLMatrix() at any point in time) put
 * through an exponential smoothing-filter so that when an adjacency changes
 * status it does so with a time-constant that is not instantaneous.  This
 * results in a matrix of smoothed adjacency values that are then directly
 * usable in the phsyical simulation as k-values in Hook's law springs to
 * allow for a smooth morphing from one tree to the next.  A decaying average
 * is used in combination with a duration parameter dt to adjust the smooth
 * matrix towards the adjacency matrix of the tree.  The dt represents the
 * amount of time elapsed since the last call to this function so that it
 * may know how far to tend towards the new tree at this point.
 *
 * \param smooth the old "smoothed" k gsl_matrix from which to adjust
 * \param ta a new TreeAdaptor to tend towards
 * \param dt the amount of time since the last stepTowardsTree() call
 *
 * This function does not return any values.  It simply modifies the smooth
 * matrix in place.
 */
void stepTowardsTree(gsl_matrix *smooth, struct TreeAdaptor *ta, double dt);

/** \brief Allocates a new SpringBallSystem big enough for the given tree
 *
 * This function allocates a new SpringBallSystem with a number of nodes equal
 * to the number of nodes in the TreeAdaptor.  It returns a pointer to the
 * new SpringBallSystem.
 *
 * \param ta pointer to a TreeAdaptor to be modelled in the SpringBallSystem
 * \return point to the newly allocated SpringBallSystem
 */
struct SpringBallSystem *newSBS(struct TreeAdaptor *ta);

/** \brief frees a SpringBallSystem
 *
 * This function has no return value.
 *
 * \param sbs pointer to the SpringBallSystem to be freed
 */
void freeSBS(struct SpringBallSystem *sbs);

/** \brief sets or clears the 2 dimensional flattening force
 *
 * In order to support a flattened tree view, an experimental flattening
 * force is possible as a toggle.  It attracts all balls to a plane.
 * It is unstable and likely to cause the differential equations to become
 * unsolveable in all but the simplest cases, so general use should be
 * avoided.
 *
 * \param sbs pointers to the SpringBallSystem to be adjusted
 * \param newval int indicating to flatten (nonzero) or not (0)
 *
 * There is no return value.
 */
void set2DForce(struct SpringBallSystem *sbs, int newval);

/** \brief returns the 3 dimensional position of a given ball
 *
 * The main purpose of the SpringBallSystem is to smoothly track the
 * position of these charged balls over time.  This function is the window
 * into their position at any point in time.  The balls are indexed by
 * consecutive integers starting at 0.
 *
 * \param sbs pointer to a SpringBallSystem to inspect
 * \param whichBall integer index for which ball is under consideration
 * \return pointer to a gsl_vector of 3 dimensions indicating the ball position
 */
gsl_vector *getBallPosition(struct SpringBallSystem *sbs, int whichBall);

/** \brief adjusts the speed of the simulation relative to realtime
 *
 * The SpringBallSystem evolves forward by using the realtime clock from
 * the operating system.  This starts from the moment of construction.  In
 * order to control the speed of simulation this function may be used.  The
 * modelSpeed is a multiplicative speed parameter; higher values make it go
 * faster, and the value 1 makes it go at "realtime" speed.  There is no
 * return value.
 * 
 * \param sbs pointer to a SpringBallSystem to adjust
 * \param modelSpeed speedup factor to switch to
 */
void setModelSpeedSBS(struct SpringBallSystem *sbs, double modelSpeed);

/** \brief Adjusts the current "target" tree for this SpringBallSystem
 *
 * At any point in time, the SpringBallSystem is tending towards a given
 * "target" tree via an exponential decay path.  This tree is initially
 * that one that it is constructed with, but may be changed only with this
 * function.  Once this is called, over time, the springs will adjust to
 * match the most recently changed-to tree.
 *
 * \param sbs pointer to a SpringBallSystem to retarget
 * \param ta new tree to be set as a target to strive towards
 */
void changeTargetTreeSBS(struct SpringBallSystem *sbs, struct TreeAdaptor *ta);

/** \brief Inspects the smoothed-k matrix for instantaneously smoothed values
 *
 * At any point in time, the SpringBallSystem is tending towards a given
 * "target" tree via an exponential decay path, and each spring in the
 * system is represented by a single double-precision floating point number
 * within a matrix.  This function allows you to find the value of that
 * number between 0 and 1.   0 means fully disconnected, 1 means fully
 * connected, and as the tree changes you may find many values in between.
 *
 * \param sbs pointer to a SpringBallSystem to inspect
 * \param i first node index to consider
 * \param j second node index to consider
 * \return value between 0 and 1 indicating how connected node i and node j is
 */
double getSpringSmoothSBS(struct SpringBallSystem *sbs, int i, int j);

/** \brief Steps the system forward some small amount of time
 *
 * This function should be called for each frame of animation.  It will
 * get the current time from the OS and step the SpringBallSystem forward
 * as much as is appropriate to catch up with realtime.
 *
 * This is the only way to change the positions and velocities of the
 * balls in the system.  There is no return value.
 *
 * Be careful!  If there are too many balls in the system (or they are in
 * an unsolveable tangle) this function may infinite loop.
 *
 * \param sbs pointer to a SpringBallSystem to evolve
 */
void evolveForward(struct SpringBallSystem *sbs);

/** \brief returns a count of the number of balls in this SpringBallSystem
 *
 * This value should equal the number of balls used in the construction of
 * this SpringBallSystem.
 *
 * \param sbs pointer to a SpringBallSystem to evolve
 * \return number of balls in this SpringBallSystem
 */
int getNodeCountSBS(struct SpringBallSystem *sbs);

#endif

#endif
