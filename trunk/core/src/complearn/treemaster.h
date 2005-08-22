#ifndef __TREEMASTER_H
#define __TREEMASTER_H

#include <complearn/cltypes.h>

#include <gsl/gsl_matrix.h>
#include <gsl/gsl_blas.h>

/*! \file treemaster.h */

/** \brief searches for best-matching tree given a distance matrix
 *
 * \struct TreeMaster
 *
 * TreeMaster provides a way to search for best-fitting trees given a
 * predefined distance matrix.  The distance matrix must be in the gsl_matrix
 * format.  There are two variations in the type of trees to be searched;
 * rooted or unrooted.  In the case of unrooted trees, every node has
 * exactly one or three neighbors.  In the case of rooted trees, the
 * root has exactly two neighbors, and every other node has exactly 1 or
 * 3 neighbors.
 *
 * Callers have the option of registering a callback listener to receive
 * tree-changed events during the findTree() call.  This allows for interactive
 * or realtime visualizations to occur before the final termination condition
 * is reached.  In these cases abortTreeSearchTM() may be useful to cut short
 * a search while in-progress.
 *
 * There are three ways to create a new TreeMaster :
 * newTreeMaster() , newTreeMasterEx(), or newTreeMasterEz()
 *
 * \sa treemaster.h
 */
struct TreeMaster;

/** \brief Contains persistent configuration options to TreeMaster
 *
 * \struct TreeMasterConfig
 *
 * \sa treemaster.h
 */
struct TreeMasterConfig;

/** \brief allocates a new TreeMaster with default options and an explicit rootedness flag
 *
 * This function returns a pointer to a newly allocated TreeMaster that
 * searches for either unrooted (0) or rooted (nonzero) binary trees.
 * The tree scores will be computed based on the given distance matrix.
 *
 * \param gsl pointer to the gsl_matrix containing the distance matrix input
 * \param isRooted int flag indicating unrooted (0) or rooted (nonzero) trees
 * \return pointer to a new TreeMaster
 */
struct TreeMaster *newTreeMaster(gsl_matrix *gsl, int isRooted);
/** \brief allocates a new TreeMaster with custom options and an explicit rootedness flag
 *
 * This function returns a pointer to a newly allocated TreeMaster that
 * searches for either unrooted (0) or rooted (nonzero) binary trees.
 * The tree scores will be computed based on the given distance matrix.
 * Advanced parameters may be passed in using the EnvMap provided.
 *
 * \param gsl pointer to the gsl_matrix containing the distance matrix input
 * \param isRooted int flag indicating unrooted (0) or rooted (nonzero) trees
 * \param em pointer to an EnvMap containing additional custom parameters
 * \return pointer to a new TreeMaster
 */
struct TreeMaster *newTreeMasterEx(gsl_matrix *gsl, int isRooted, struct EnvMap *em);

/** \brief simplest all-defaults way to search for an unrooted binary tree
 * 
 * newTreeMasterEz() offers the maximum convenience in constructing a TreeMaster
 * by way of its extensive use of defaults.  It takes only the distance matrix
 * and can search only for the most common unrooted binary trees with no
 * custom parameters whatsoever.
 *
 * \param gsl pointer to the gsl_matrix containing the distance matrix input
 * \return pointer to a new TreeMaster
 */
struct TreeMaster *newTreeMasterEz(gsl_matrix *gsl);

/** \brief inspects the random "starting" tree before findTree is called
 *
 * This function allows visualization display layers and other users to
 * query the initial random starting configuration for one of the starting
 * TreeHolder instances used in the given TreeMaster.  There is no way to
 * specify which one is queried using this function; if you need to explicitly
 * control this, use getTreeAtIndex() instead.
 *
 * \param tm pointer to the TreeMaster to inspect
 * \return pointer to a TreeHolder with the starting tree in the TreeAdaptor
 */
struct TreeHolder *getStarterTree(struct TreeMaster *tm);

/** \brief Searches for the next best tree
 *
 *  findTree() will search for a new tree, and return a pointer to a TreeHolder
 *  if the tree it is holding has a better tree score than the tree previously
 *  found in the last call of findTree().
 *
 *  If needed, abortTreeSearchTM(), can be called in a separate thread to
 *  prematurely interrupt findTree().
 *
 *  \param tm pointer to TreeMaster
 *  \return pointer to the best TreeHolder that contains a best TreeAdaptor
 */
struct TreeHolder *findTree(struct TreeMaster *tm);

/** \brief inspects the tree at a given TreeHolder index 0 <= i < k
 *
 * This function allows the user to query the fine-grained structure in
 * the k-tree search.  Each TreeMaster contains k distinct TreeHolder
 * instances which evolve seperately and may be queried through this
 * function.
 *
 * \sa getKTM()
 * 
 * \param tm pointer to the TreeMaster to inspect
 * \param i index of TreeHolder to retrieve between 0 and k-1 inclusive
 * \return pointer to a TreeHolder with the starting tree in the TreeAdaptor
 */
struct TreeHolder *getTreeAtIndex(struct TreeMaster *tm, int i);

/** \brief counts the total number of trees examined so far in this search
 *
 * This function returns the total number of random trees examined so far in
 * this search.
 *
 * \param tm pointer to the TreeMaster to inspect
 * \return count of the number of trees so far examined in the latest search
 */
int totalTreesExamined(struct TreeMaster *tm);

/** \brief returns the number of distinct TreeHolder instances in a TreeMaster
 *
 * This function returns the "k" parameter determining the number of distinct
 * concurrent TreeHolder searches are going to occur in dovetail fashion.
 * This number counts the number of distinct threads.  For smaller node sizes
 * larger k values must be used to get similar accuracies.  This number
 * bounds the maximum allowable range of getTreeAtIndex() .
 *
 * \param tm pointer to the TreeMaster to inspect
 * \return number of TreeHolder instances in use
 */
int getKTM(struct TreeMaster *tm);

/** \brief simple opaque data pointer tunnel system for user expansion
 *
 * Provides a simple single void * for the user's arbitrary use.  There
 * is no return value, but see getUserDataTM() for the use.
 *
 * \param tm pointer to the TreeMaster to store opaque pointer into
 * \param udata the opaque pointer (to your stuff) to store
 */
void setUserDataTM(struct TreeMaster *tm, void *udata);

/** \brief simple opaque data pointer tunnel system reader for your use
 *
 * This is the function to retrieve your "tunneled" data.  In this way
 * you can attach arbitrary bits of your own code or state to a given
 * TreeMaster and run several different TreeMaster instances in different
 * threads at the same time.  setUserDataTM() must be called before
 * calling this function.
 *
 * \param tm pointer to the TreeMaster where an opaque pointer was stored
 * \return the pointer that was stored earlier with setUserDataTM()
 */
void *getUserDataTM(struct TreeMaster *tm);

/** \brief frees a TreeMaster
 *
 * This function deallocates memory association with the given TreeMaster .
 * There is no return value.
 *
 * \param tm pointer to the TreeMaster to be freed
 */
void freeTreeMaster(struct TreeMaster *tm);

/** \brief callback system for realtime tree search progress feedback
 *
 * \struct TreeObserver
 *
 * This struct contains four callback functions for use in multithreaded
 * programming.
 *
 * treesearchstarted is called when a new search first begins with findTree()
 * in another thread.  This function will be called just once per findTree().
 *
 * treeimproved is called each time a better tree is found in the search
 * this function may be called very many times.
 *
 * treerejected is called after each new random tree is rejected because its
 * score is worse than the current best score.
 *
 * treedone is called once when the final tree is found after a termination
 * condition has been reached.
 *
 * The TreeObserver structure contains one opaque pointer for general use, ptr.
 */
struct TreeObserver {
  void *ptr;
  t_treesearchstarted treesearchstarted;
  t_treeimproved treeimproved;
  t_treerejected treerejected;
  t_treedone treedone;
};

/** \brief attaches a TreeObserver to a TreeMaster prior to findTree()
 *
 * This function must be called before findTree is called in order to
 * enable realtime feedback of tree progress before findTree returns in
 * the main computation thread.
 *
 * Before using this function, you must set unused function pointers in
 * the TreeObserver tob to 0 or NULL and set at least one of these to a custom
 * function of your choosing.  You may also choose to use the ptr opaque
 * pointer.
 *
 * There is no return value for this function.
 *
 * \param tm pointer to the TreeMaster instance to observe
 * \param tob pointer to an already filled-in TreeObserver structure
 */
void setTreeObserverTM(struct TreeMaster *tm, struct TreeObserver *tob);

/** \brief returns the currently attached TreeObserver or NULL if none
 *
 * \param tm pointer to the TreeMaster instance to inspect
 * \return pointer to a TreeObserver structure or NULL if none has been set
 */
struct TreeObserver *getTreeObserverTM(struct TreeMaster *tm);

/** \brief returns the time when the most recent findTree() call finished
 *
 * This function may be used to determine the saved time when the last
 * tree search ended.  This is for historical purposes.
 *
 * \param tm pointer to the TreeMaster instance to inspect
 * \return pointer to a CLDateTime containing the finish time
 */
struct CLDateTime *getEndTimeTM(struct TreeMaster *tm);

/** \brief returns the time when the most recent findTree() call started
 *
 * This function may be used to determine the saved time when the last
 * tree search began.  This is for statistical and verification purposes.
 *
 * \param tm pointer to the TreeMaster instance to inspect
 * \return pointer to a CLDateTime containing the starting time
 */
struct CLDateTime *getStartTimeTM(struct TreeMaster *tm);

/** \brief Aborts current search for better tree
 *
 *  Interrupts search for better tree. Used for multi-threaded processing.
 *  Assumes findTree() has already been called in another thread for the same
 *  TreeMaster.
 *  \param tm pointer to TreeMaster
 */
void abortTreeSearchTM(struct TreeMaster *tm);

/** \brief returns the number of labelled nodes in this TreeMaster
 *
 * This function may be used to determine the number of labelled objects
 * in the TreeMaster.
 *
 * \param tm pointer to the TreeMaster instance to inspect
 * \return integer indicating number of labelled objects
 */
int getLabelCountTM(struct TreeMaster *tm);
#endif
