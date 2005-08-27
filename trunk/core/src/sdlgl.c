/*
 * Rudi Cilibrasi
 * sphere node tree GL demo springball sim for complearn
 * Distributed under terms of the LGPL.
 */

/*! \file sdlgl.c */

#include <assert.h>
#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <complearn/complearn.h>
#include <complearn/springball.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MIDIBONUS 0

#define SDL_TTF 1

#if !LINUX
#include <windows.h>
#else
#if GTK_RDY
#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <gdk/gdkkeysyms.h>
#endif
#endif

#undef SDL_PROTOTYPES_ONLY
#undef __CYGWIN__
#define WIN32_LEAN_AND_MEAN 1
#include <SDL/SDL_syswm.h>


#if MIDIBONUS
#include <windows.h>
#include <mmsystem.h>
#else
#define HMIDIOUT int
#endif

#if SDL_TTF
#include <complearn/textsdl.h>
#endif

#define SPRINGCYLRAD 0.16

#define RADMIN 1.0
#define RADMAX 1000.0
#define RADSPEED 10.0

struct TransformAdaptor *builtin_UNBZIP(void);
struct TransformAdaptor *builtin_UNGZ(void);
struct TransformAdaptor *builtin_UNZLIB(void);

struct DataBlock *testCompression(struct DataBlock *db, struct TransformAdaptor *t) {
  struct DataBlock *result = db;
  if (!t) return result;
  if (t->pf(*db)) {
    result = calloc(sizeof(struct DataBlock), 1);
    *result = t->tf(*db);
    assert(result->size > 0);
    assert(result->ptr != NULL);
  } /* TODO: consider fixing the memory leak pattern here with incoming db */
  return result;
}

void addAndProcessDataBlock(struct IncrementalDistMatrix *idm, struct DataBlock *db) {

#if 1
  static struct TransformAdaptor *buz, *bgz, *bbzip;
  if (!buz) {
    buz = builtin_UNZLIB();
    bgz = builtin_UNGZ();
    bbzip = builtin_UNBZIP();
  }
  struct DataBlock *oldresult, *result = db;
  do {
    oldresult = result;
    result = testCompression(result, buz);
    result = testCompression(result, bbzip);
    result = testCompression(result, bgz);
  } while (result != oldresult);
#endif
  addDataBlock(idm, result);
}

static double myPI;
static double dummyChanger;
const double angularSpeed = 3; /* radians / sec */
static double prematrixSpeed = 5;
static double orbitRadius = 6;
static int fIsCalculatingDM;

int F1 = 1, F2 = 1, F3=1, F4=1, F5=1;

static void draw_screen(void);
static void process_events(void);

struct TreeAdaptor *ta, *nextbest;
struct StringStack *labels;
struct SpringBallSystem *sbs;
SDL_Surface *screen;
static int fShowLabels;
static int fShowHelp;
static int fTwoDForce;
static int LoD=2;   /* Level of Detail, 1 = Low, 2 = High */
struct TreeMaster *tm;

int isCalculatingDM(void);
static void doDroppedFile(char *buf);
static void realDoDroppedFile(char *buf);
static void setup_shading(void);

int countThreeSlashes(char *buf)
{
  int i;
  int sc = 0;
  for (i = 0; buf[i]; i += 1)
    if (buf[i] == '/') {
      sc += 1;
      if (sc == 3)
        return i;
    }
  return -1;
}


/** \brief viewer (camera) settings and parameters
 *
 * \struct Camera
 *
 * This function maintains all state associated with viewer position and
 * orientation. The orientation is controlled by two angles, angle1 and
 * angle2.  angle1 represents horizontal heading (as on a horizontal compass)
 * and angle2 represents vertical pitch above or below the horizon.
 *
 * There is also a parameter radius representing the distance from the
 * viewer to the origon.  The viewer is always looking diretly towards the
 * origin of the coordinate system.
 *
 * \sa sdlgl.c
 *
 */
struct Camera {
  double angle1, angle2;
  double *curChanging;
  double lastTime;
  double width, height;
  double radChanging;
  double radius;
  int curDir;
};

struct Camera cam;
struct IncrementalDistMatrix *distmatglob;
static void setupCameraAngle(void);
GLUquadricObj *qobj1, *qobj2, *qobj3, *qspringcyl;
static GLboolean should_rotate = GL_TRUE;

GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };

GLfloat light_position1[] = { 8.0, 10.0, 1.0, 0.0 };
GLfloat light_position2[] = { 10.0, -10.0, 10.0, 0.0 };
GLfloat light_position3[] = { -10.0, 1.0, 9.0, 0.0 };
GLfloat light_position4[] = { 10.0, -10.0, -10.0, 0.0 };
GLfloat model_ambient[] = { 0.5, 0.5, 0.5, 1.0 };

GLfloat matball_diff[] = { 0.1, 0.1, 0.8, 1.0 };
GLfloat matball_spec[] = { 0.4, 0.4, 0.4, 1.0 };
GLfloat matball_shine[] = { 80.0 };

GLfloat matkern_diff[] = { 0.9, 0.2, 0.5, 1.0 };
GLfloat matkern_spec[] = { 0.2, 0.2, 0.2, 1.0 };
GLfloat matkern_shine[] = { 70.0 };

GLfloat matspring_diff[] = { 0.8, 0.8, 0.1, 1.0 };
GLfloat matspring_spec[] = { 1.0, 1.0, 0.8, 1.0 };
GLfloat matspring_shine[] = { 20.0 };

static void quit_tutorial( int code )
{
    /*
     * Quit SDL so we can release the fullscreen
     * mode and restore the previous video settings,
     * etc.
     */
    SDL_Quit( );

    /* Exit program. */
    exit( code );
}

static void adjustRadius(double whichWay)
{
  cam.radChanging = whichWay;
}

static void realAdjustRadius(double dt)
{
  if (cam.radChanging != 0) {
    if (dt > 0.3)
      dt = 0.3;
    cam.radius += cam.radChanging * dt * RADSPEED;
    if (cam.radius < RADMIN)
      cam.radius = RADMIN;
    if (cam.radius > RADMAX)
      cam.radius = RADMAX;
  }
}

static void handle_key_changed( SDL_keysym* keysym, int isDown )
{

    /*
     * We're only interested if 'Esc' has
     * been presssed.
     *
     * EXERCISE:
     * Handle the arrow keys and have that change the
     * viewing position/angle.
     */
    int choseDir = 0;
    int fIsShifted = keysym->mod & KMOD_SHIFT;
    double *choseAxis = NULL;
//    printf("Got key event %d with %d\n", keysym->sym, isDown);
    switch( keysym->sym ) {
    case SDLK_ESCAPE:
        quit_tutorial( 0 );
        break;
    case SDLK_f:
        if (isDown) {
          fTwoDForce = !fTwoDForce;
          if (sbs)
            set2DForce(sbs, fTwoDForce);
        }
        break;
    case SDLK_m:
        if (ta) {
          nextbest = treecloneTRA(ta);
          treemutateTRA(nextbest);
        }
        break;
    case SDLK_l:
        if (isDown)
          fShowLabels = !fShowLabels;
        break;
    case SDLK_h:
        if (isDown)
          fShowHelp = !fShowHelp;
        break;
    case SDLK_DOWN:
        if (fIsShifted) {
          cam.curChanging = isDown ? &dummyChanger : NULL;
          if (isDown)
            adjustRadius(1.0);
          else
            adjustRadius(0.0);
        }
        else {
          choseAxis = &cam.angle2;
          choseDir = -1;
        }
        break;
    case SDLK_UP:
        if (fIsShifted) {
          cam.curChanging = isDown ? &dummyChanger : NULL;
          if (isDown)
            adjustRadius(-1.0);
          else
            adjustRadius(0.0);
        }
        else {
          choseAxis = &cam.angle2;
          choseDir = 1;
        }
        break;
    case SDLK_LEFT:
        choseAxis = &cam.angle1;
        choseDir = 1;
        break;
    case SDLK_RIGHT:
        choseAxis = &cam.angle1;
        choseDir = -1;
        break;
    case SDLK_SPACE:
        should_rotate = !should_rotate;
        break;
    default:
        break;
    }

  if (choseDir) {
    if (isDown) {
      cam.curChanging = choseAxis;
      cam.curDir = choseDir;
      cam.lastTime = cldtGetStaticTimer();
    } else {
      cam.curChanging = NULL;
      cam.curDir = 0;
    }
  }
}

#define MAXTREESIZE 100

/** \brief holds a small array of DataBlock
 *
 * \struct DataBlockKeeper
 *
 * \sa sdlgl.c
 * \sa DataBlock
 */
struct DataBlockKeeper {
  struct DataBlock db[MAXTREESIZE];
  int size;
};

static struct DataBlockKeeper *curFiles;
static void doNextTree(void)
{
  assert(curFiles);
  assert(curFiles->size > 0);
  if (tm) {
    abortTreeSearchTM(tm);
    tm = NULL;
  }
}

#ifdef WINCHOICE

static struct GeneralConfig *gc;

static void doDroppedFiles(HANDLE dragDrop)
{
  int i;
  char buf[16384];
  int fileCount;
  if (gc == NULL) {
    gc = loadDefaultEnvironment();
    gc->ca = compaLoadBuiltin("blocksort");
  }
  fileCount = DragQueryFile(dragDrop, 0xffffffff, NULL, 0);
  for (i = 0; i < fileCount; ++i) {
    DragQueryFile(dragDrop, i, buf, 16384);
    doDroppedFile(buf);
  }
  DragFinish( dragDrop );
  doNextTree();
}

#else

static gboolean process_idle_events(gpointer data)
{
  data = data;
  process_events();
  draw_screen();
  return TRUE;
}

enum {
        TARGET_URI
};

static GtkTargetEntry target_list[] = {
        { "text/uri-list",     0, TARGET_URI },
};

static guint n_targets = G_N_ELEMENTS (target_list);

static void
drag_data_received_handl
(GtkWidget *widget, GdkDragContext *context, gint x, gint y,
        GtkSelectionData *selection_data, guint target_type, guint time,
        gpointer data)
{
        gboolean dnd_success = FALSE;
        gboolean delete_selection_data = FALSE;
        if((selection_data != NULL) && (selection_data-> length >= 0))
        {
          char *seldatastr = (char *) selection_data->data;
                switch (target_type)
                {
                        case TARGET_URI:
                                dnd_success = TRUE;
                                doDroppedFile(seldatastr);
                                doNextTree();

                                break;
                }
        }
        gtk_drag_finish (context, dnd_success, delete_selection_data, time);
}

static SDL_keysym *convertToSDL_keysym(GdkEvent *event)
{
  static SDL_keysym k;
  memset(&k, 0, sizeof(k));
  if (((GdkEventKey *)event)->state & GDK_SHIFT_MASK)
    k.mod |= KMOD_SHIFT;
  switch (((GdkEventKey *) event)->keyval) {
    case GDK_Escape: k.sym = SDLK_ESCAPE; break;
    case GDK_f: k.sym = SDLK_f; break;
    case GDK_m: k.sym = SDLK_m; break;
    case GDK_l: k.sym = SDLK_l; break;
    case GDK_h: k.sym = SDLK_h; break;
    case GDK_Down: k.sym = SDLK_DOWN; break;
    case GDK_Up: k.sym = SDLK_UP; break;
    case GDK_Left: k.sym = SDLK_LEFT; break;
    case GDK_Right: k.sym = SDLK_RIGHT; break;
    case GDK_space: k.sym = SDLK_SPACE; break;
    default:
          break;
  }
  return &k;
}

static void handle_keyb_pressed(GtkWidget *widget, GdkEvent *event, gpointer *cbdata)
{
  SDL_keysym *k = convertToSDL_keysym(event);
  handle_key_changed(k, 1);
}

static void handle_keyb_released(GtkWidget *widget, GdkEvent *event, gpointer *cbdata)
{
  SDL_keysym *k = convertToSDL_keysym(event);
  handle_key_changed(k, 0);
}

static void initDragDropSubsystem(GtkWidget *window)
{
        gtk_drag_dest_set
        (
                window, GTK_DEST_DEFAULT_ALL, target_list,       n_targets,
                GDK_ACTION_COPY
        );
        g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);
        g_signal_connect (window, "drag-data-received",
                G_CALLBACK(drag_data_received_handl), NULL);
        g_signal_connect (window, "key-press-event", G_CALLBACK (handle_keyb_pressed), NULL);
        g_signal_connect (window, "key-release-event", G_CALLBACK (handle_keyb_released), NULL);
}

static int findXWindowID(GtkWidget *window) {
  int xwin = GDK_WINDOW_XWINDOW(GTK_WIDGET(window)->window);
  printf("xwin is %d\n", xwin);
  return xwin;
}

#endif

static void process_events( void )
{
    /* Our SDL event placeholder. */
    SDL_Event event;
#ifdef WINCHOICE
    struct SDL_SysWMEvent *wmevt;
#endif
    int xGrid, yGrid, buttonDown;

    /* Grab all the events off the queue. */
    while( SDL_PollEvent( &event ) ) {

        switch( event.type ) {
#ifdef WINCHOICE
        case SDL_SYSWMEVENT:
            wmevt = (struct SDL_SysWMEvent *) &event;
            if (wmevt->msg->msg == WM_DROPFILES) {
              doDroppedFiles((HANDLE) wmevt->msg->wParam);
            }
            break;
#endif
        case SDL_MOUSEBUTTONDOWN:
            buttonDown = 1;
            //printf("button down\n");
            break;
        case SDL_MOUSEBUTTONUP:
            buttonDown = 0;
            //printf("button up\n");
            break;
        case SDL_MOUSEMOTION:
            xGrid = event.motion.x;
            yGrid = event.motion.y;
            //printf("mouse at %d,%d\n",xGrid,yGrid);
            break;
        case SDL_KEYDOWN:
            /* Handle key presses. */
            handle_key_changed( &event.key.keysym, 1);
            break;
        case SDL_KEYUP:
            handle_key_changed( &event.key.keysym, 0);
            break;
        case SDL_QUIT:
            /* Handle quit requests (like Ctrl-c). */
            quit_tutorial( 0 );
            break;
        }
//        if(buttonDown == 1)
    }

}

static void draw_sphere(double posx, double posy, double posz );
static void draw_mediumsphere(double posx, double posy, double posz );
static void draw_spring(double posx1, double posy1, double posz1,
                        double posx2, double posy2, double posz2 );
static void setupDemo(void);

static void draw_screen(void)
{
  int i, j, n;
  double x, y, z;
  double curtime = cldtGetStaticTimer();
  /* Clear the color and depth buffers. */
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  setup_shading();


  glMaterialfv(GL_FRONT, GL_DIFFUSE, matspring_diff);
  glMaterialfv(GL_FRONT, GL_SPECULAR, matspring_spec);
  glMaterialfv(GL_FRONT, GL_SHININESS, matspring_shine);


//  glEnable(GL_COLOR_MATERIAL);
  double t = cldtGetStaticTimer();
  matspring_diff[0] = 0.4 + 0.4 * cos(t * 7 * isCalculatingDM());
  matspring_diff[1] = 0.4 + 0.4 * cos(t * 4 * isCalculatingDM());
//  glColor3fv(matspring_diff);

  if (ta && sbs) {
    for (i = 0; i < treeGetNodeCountTRA(ta); i += 1) {
      gsl_vector *p = getBallPosition(sbs, i);
      for (j = i+1; j < treeGetNodeCountTRA(ta); j += 1) {
        if (getSpringSmoothSBS(sbs, i, j) < 0.7)
          continue;
        gsl_vector *p2 = getBallPosition(sbs, j);
      draw_spring(gsl_vector_get(p, 0), gsl_vector_get(p, 1), gsl_vector_get(p, 2) , gsl_vector_get(p2, 0), gsl_vector_get(p2, 1), gsl_vector_get(p2, 2) );
      }
    }

  glDisable(GL_COLOR_MATERIAL);
//  glColor3f(1.0,1.0,1.0);

    for (i = 0; i < treeGetNodeCountTRA(ta); i += 1) {
      gsl_vector *p = getBallPosition(sbs, i);
      if (treeIsQuartettable(ta, i)) {
        glMaterialfv(GL_FRONT, GL_DIFFUSE, matball_diff);
        glMaterialfv(GL_FRONT, GL_SPECULAR, matball_spec);
        glMaterialfv(GL_FRONT, GL_SHININESS, matball_shine);

        draw_sphere(gsl_vector_get(p, 0), gsl_vector_get(p, 1), gsl_vector_get(p, 2));
      } else {

        glMaterialfv(GL_FRONT, GL_DIFFUSE, matkern_diff);
        glMaterialfv(GL_FRONT, GL_SPECULAR, matkern_spec);
        glMaterialfv(GL_FRONT, GL_SHININESS, matkern_shine);

//        glColor3fv(matkern_diff);
        draw_mediumsphere(gsl_vector_get(p, 0), gsl_vector_get(p, 1), gsl_vector_get(p, 2));
      }
    }
    glDisable(GL_COLOR_MATERIAL);
//    glColor3f(1.0,1.0,1.0);
  #if SDL_TTF
    if (fShowLabels) {
      for (i = 0; i < treeGetNodeCountTRA(ta); i += 1) {
        gsl_vector *p = getBallPosition(sbs, i);
        if (treeIsQuartettable(ta, i)) {
          int colind = getColumnIndexForNodeIDLP(treegetlabelpermTRA(ta), i);
          static struct CLTexture texLabels[MAXTEX];
          draw_sdltext(readAtSS(labels, colind), &texLabels[colind], p);
        }
      }
    }
  #endif
  sleepMillis(30);
  }
  if (fShowHelp || !ta )
    draw_sdlhelp();

  if ( !ta && (n = sizeSS(labels))) {
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matball_diff);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matball_spec);
    glMaterialfv(GL_FRONT, GL_SHININESS, matball_shine);
    for (i = 0; i < n; i += 1 ) {
      x = orbitRadius * cos((prematrixSpeed*curtime) + ((i*2*M_PI)/n));
      z = orbitRadius * sin((prematrixSpeed*curtime) + ((i*2*M_PI)/n));
      y = z;
      draw_sphere(x,y,z);
    }
  }

  SDL_GL_SwapBuffers( );
  setupCameraAngle();
  if (sbs)
    evolveForward(sbs);
  if (nextbest) {
    if (sbs == NULL || treeGetNodeCountTRA(nextbest) != getNodeCountSBS(sbs)) {
      sbs = newSBS(nextbest);
      setModelSpeedSBS(sbs, 4.0);
    }
    else
      changeTargetTreeSBS(sbs, nextbest);
    ta = nextbest;
    nextbest = NULL;
  }
}

void handleBetterTree(struct TreeObserver *tob, struct TreeHolder *th)
{
  double score = getCurScore(th);
  if (getTreeIndexTH(th) == 0)
    nextbest = treecloneTRA(getCurTree(th));
}

struct TreeObserver tob = {
    ptr:NULL,
    treeimproved:handleBetterTree,
    treedone:handleBetterTree,
    treesearchstarted:NULL,
    treerejected:NULL,
};

int calcThreadFunc(void *unused)
{
  for (;;) {
    if (distmatglob == NULL) {
      sleepMillis(40);
      continue;
    }
    if (tm == NULL && curFiles != NULL && curFiles->size >= 4) {
      int dmsize;
      fIsCalculatingDM = 1;
      for (;;) {
        dmsize = getSizeIDM(distmatglob);
        if (dmsize < sizeSS(labels))
          addAndProcessDataBlock(distmatglob, &curFiles->db[dmsize]);
        else
          break;
      }
      fIsCalculatingDM = 0;
      assert(distmatglob);
      if (getSizeIDM(distmatglob) < 4) {
        sleepMillis(40);
        continue;
      }
      tm = newTreeMaster(getDistMatrixIDM(distmatglob), 0);
      nextbest = getCurTree(getStarterTree(tm));
      setTreeObserverTM(tm, &tob);
    }
    if (tm) {
      findTree(tm);
    }
    else {
      sleepMillis(40);  /* display help / status info here */
    }
  }
  return 0;
}

static void setupDemo(void)
{
  static int haveStartedCalcThread;

  if (qobj1 == NULL) {
    qobj1 = gluNewQuadric();
    qobj2 = gluNewQuadric();
    qobj3 = gluNewQuadric();
    qspringcyl = gluNewQuadric();
    gluQuadricDrawStyle(qobj1, GLU_FILL);
    gluQuadricNormals(qobj1, GLU_SMOOTH);
    glNewList(1, GL_COMPILE);  /* create leaf sphere display list */
    gluSphere(qobj1, /* radius */ 1.0, /* slices */ 16*LoD,  /* stacks */ 16*LoD);
    glEndList();
    glNewList(3, GL_COMPILE);  /* create kernel sphere display list */
    gluSphere(qobj3, /* radius */ 0.65, /* slices */ 11*LoD,  /* stacks */ 7*LoD);
    glEndList();
    glPushMatrix();
    glNewList(4, GL_COMPILE); /* create spring cylinder display list */
    glPushMatrix();
    glTranslatef(0,0,-0.5);
    gluCylinder(qspringcyl, SPRINGCYLRAD, SPRINGCYLRAD, 1.0, 16*LoD, 4*LoD);
    glPopMatrix();
    glEndList();
    glPopMatrix();
  }

  if (!haveStartedCalcThread) {
    SDL_CreateThread(calcThreadFunc, NULL);
    haveStartedCalcThread = 1;
  }
}

static void draw_mediumsphere(double posx, double posy, double posz )
{
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity( );

  /* Move down the z-axis. */
  glTranslatef( posx, posy, posz );
  glCallList(3);
}

static void draw_sphere(double posx, double posy, double posz )
{
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity( );

  /* Move down the z-axis. */
  glTranslatef( posx, posy, posz );
  glCallList(1);
}

void setRotParms(int uF1, int uF2, int uF3, int uF4, int uF5)
{
  F1 = uF1; F2 = uF2; F3 = uF3; F4=uF4; F5=uF5;
}

static void draw_spring(double posx1, double posy1, double posz1,
                        double posx2, double posy2, double posz2 )
{
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity( );
  double x, y, z;
  double ang1, ang2;
  double len, xzlen;
  glPushMatrix();
  glTranslatef( (posx1+posx2)/2, (posy1+posy2)/2, (posz1+posz2)/2 );
  x = posx2 - posx1; y = posy2 - posy1; z = posz2 - posz1;
  xzlen = sqrt(x*x+z*z);
  len = sqrt(x*x+y*y+z*z);
  ang2 = atan2(y, xzlen) * 180 / myPI;
  ang1 = -atan2(x, -z) * 180 / myPI;
  glRotatef(ang1, 0, 1, 0);
  glRotatef(ang2, 1, 0, 0);
  glScalef(1, 1, 0.9*len);
  glCallList(4);
  glPopMatrix();
}

static void setupCameraAngle(void) {
    float ratio = (float) cam.width / (float) cam.height;
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity( );
    /*
     * EXERCISE:
     * Replace this with a call to glFrustum.
     */
    gluPerspective( 60.0, ratio, 1.0, 1024.0 );
    gluLookAt(cam.radius*sin(cam.angle1)*cos(cam.angle2),cam.radius*sin(cam.angle2), cam.radius*cos(cam.angle1), 0, 0, 0, 0, 1, 0);
  double curtime = cldtGetStaticTimer();
  if (cam.curChanging) {
    double dt = curtime - cam.lastTime;
    cam.lastTime = curtime;
    cam.curChanging[0] += cam.curDir * dt * angularSpeed;
    realAdjustRadius(dt);
  }
}

static void setup_shading1( void )
{
    glShadeModel( GL_SMOOTH );
    glCullFace( GL_BACK );
    glFrontFace( GL_CCW );
    glEnable( GL_CULL_FACE );
    glEnable(GL_DEPTH_TEST);
}

static void setup_shading2( void )
{

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity( );

    glEnable(GL_LIGHTING);

    /* Our shading model--Gouraud (smooth). */
    glLightfv(GL_LIGHT0, GL_POSITION, light_position1);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

    glLightfv(GL_LIGHT1, GL_POSITION, light_position2);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT2, GL_POSITION, light_position3);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT2, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT3, GL_POSITION, light_position4);
    glLightfv(GL_LIGHT3, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT3, GL_SPECULAR, light_specular);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, model_ambient);

    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHT2);
    glEnable(GL_LIGHT3);
}

static void setup_shading(void)
{
  setup_shading1();
  setup_shading2();
}

static void setup_opengl( int width, int height )
{
    setup_shading1();

    setup_shading2();

    /* Set the clear color. */
    glClearColor( 0, 0, 0, 0 );

    /* Setup our viewport. */
    glViewport( 0, 0, width, height );

    /*
     * Change to the projection matrix and set
     * our viewing volume.
     */
    cam.width = width;
    cam.height = height;
  setupCameraAngle();
}

void init_wm()
{
#ifdef WINCHOICE
  HWND hwnd;
#endif
  SDL_SysWMinfo wminfo;
  SDL_VERSION(&wminfo.version);
  SDL_GetWMInfo(&wminfo);
/* or window for X11 */
#ifdef WINCHOICE
  hwnd = wminfo.window;
#endif
  SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);
#ifdef WINCHOICE
  DragAcceptFiles(hwnd, TRUE);
#endif
}

int isCalculatingDM(void)
{
  return fIsCalculatingDM;
}

static char *fixbuf(char *inpbuf)
{
  static char tmpbuf[16384];
  if (strncmp(inpbuf, "file:", 5) == 0) {
    int offset = countThreeSlashes(inpbuf);
    strcpy(tmpbuf, inpbuf+offset);
  }
  else
    strcpy(tmpbuf, inpbuf);
//  strtok(tmpbuf, "\r\n");
  return tmpbuf;
}

static void doDroppedFile(char *buf)
{
  const char *LDELIMS = "\n\r";
  char *curline;
  printf("Got large file drop: <%s>\n", buf);
  for (curline = strtok(buf, LDELIMS); curline; curline = strtok(NULL, LDELIMS)) {
    realDoDroppedFile(curline);
  }
}

static char * findLastPart(char *buf)
{
  char *curptr;
  printf("Finding last part of <%s>\n", buf);
  curptr = buf + strlen(buf);
  while (*curptr != '/' && *curptr != '\\')
    curptr -= 1;
  curptr += 1;
  printf("Got <%s>\n", curptr);
  return curptr;
}

static void realDoDroppedFile(char *buf)
{
  char *lastpart = NULL;
  if (buf[0] == 0)
    return;
  buf = fixbuf(buf);
  if (buf[0] == 0)
    return;
  if (curFiles == NULL) {
    curFiles = gcalloc(sizeof(struct DataBlockKeeper), 1);
  }
  if (isDirectory(buf)) {
    struct DataBlockEnumeration *dbe = loadDirectoryDBE(buf);
    struct DataBlockEnumerationIterator *dbi;
    struct DataBlock *cur;
    dbi = dbe->newenumiter(dbe);
    assert(dbi);
    while ( ( cur = dbe->istar(dbe, dbi) ) ) {
      curFiles->db[curFiles->size++] = *cur;
      addAndProcessDataBlock(distmatglob, cur);
      pushSS(labels, dbe->ilabel(dbe, dbi));
      dbe->istep(dbe, dbi);
      datablockFreePtr(cur);
    }
  } else {
    curFiles->db[curFiles->size++] =  fileToDataBlock(buf);
    lastpart = findLastPart(buf);
    pushSS(labels, lastpart);
  }
  //gfree(buf);
}

int main( int argc, char* argv[] )
{
#if WINCHOICE
  if( SDL_Init( SDL_INIT_VIDEO) < 0 ) {
#else
  GtkWidget       *window;
  guint           win_xsize       = 800;
  guint           win_ysize       = 600;
  gtk_init (&argc, &argv);
  window  = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_widget_realize(window);
 { char SDL_windowhack[32];
         sprintf(SDL_windowhack,"SDL_WINDOWID=%ld",
                      findXWindowID(window));
               putenv(SDL_windowhack);
                  }
  if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE ) < 0 ) {
#endif
      /* Failed, exit. */
      fprintf( stderr, "Video initialization failed: %s\n",
           SDL_GetError( ) );
      quit_tutorial( 1 );
  }
#if LINUX
  gtk_window_set_default_size (GTK_WINDOW(window), win_xsize, win_ysize);

  initDragDropSubsystem(window);
#endif
  distmatglob = newIDM(NULL);
  myPI = atan(1.0)*4;
  /* Information about the current video settings. */
  const SDL_VideoInfo* info = NULL;
  /* Dimensions of our window. */
  int width = 0;
  int height = 0;
  cam.lastTime = cldtGetStaticTimer();
  /* Color depth in bits of our window. */
  int bpp = 0;
  /* Flags we will pass into SDL_SetVideoMode. */
  int flags = 0;
  fShowLabels = 1;
  fShowHelp = 1;
setRotParms(1, -1, -1, 1, 1);

  cam.angle1 = cam.angle2 = 0.0;
  cam.radius = 25.0;
  labels = newStringStack();
  glutInit(&argc, argv);

  /* Let's get some video information. */
  info = SDL_GetVideoInfo( );

  if( !info ) {
      /* This should probably never happen. */
      fprintf( stderr, "Video query failed: %s\n",
           SDL_GetError( ) );
      quit_tutorial( 1 );
  }

  /*
   * Set our width/height to 640/480 (you would
   * of course let the user decide this in a normal
   * app). We get the bpp we will request from
   * the display. On X11, VidMode can't change
   * resolution, so this is probably being overly
   * safe. Under Win32, ChangeDisplaySettings
   * can change the bpp.
   */
  width = 800;
  height = 600;
  bpp = info->vfmt->BitsPerPixel;

  /*
   * Now, we want to setup our requested
   * window attributes for our OpenGL window.
   * We want *at least* 5 bits of red, green
   * and blue. We also want at least a 16-bit
   * depth buffer.
   *
   * The last thing we do is request a double
   * buffered window. '1' turns on double
   * buffering, '0' turns it off.
   *
   * Note that we do not use SDL_DOUBLEBUF in
   * the flags to SDL_SetVideoMode. That does
   * not affect the GL attribute state, only
   * the standard 2D blitting setup.
   */
  SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
  SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 );
  SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
  SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
  SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

  SDL_WM_SetCaption("CompLearn 3D Tree Visualisation", 0);

  /*
   * We want to request that SDL provide us
   * with an OpenGL window, in a fullscreen
   * video mode.
   *
   * EXERCISE:
   * Make starting windowed an option, and
   * handle the resize events properly with
   * glViewport.
   */
 // flags = SDL_OPENGL | SDL_FULLSCREEN;
  flags = SDL_OPENGL;

  /*
   * Set the video mode
   */
  screen = SDL_SetVideoMode( width, height, bpp, flags );
#if 0
  if( !screen ) {
      /*
       * This could happen for a variety of reasons,
       * including DISPLAY not being set, the specified
       * resolution not being available, etc.
       */
      fprintf( stderr, "Video mode set failed: %s\n",
           SDL_GetError( ) );
      quit_tutorial( 1 );
  }
  */
#endif

  init_wm();
  /*
   * At this point, we should have a properly setup
   * double-buffered window for use with OpenGL.
   */
  setup_opengl( width, height );


#if SDL_TTF
  init_sdltext();
#endif

  /*
   * Now we want to begin our normal app process--
   * an event loop with a lot of redrawing.
   */
  setupDemo();
//  drawHelpPane();
#if LINUX
  gtk_widget_show_all (window);
  gtk_idle_add(process_idle_events, window);
  gtk_main();
  exit(0);
#endif
  while( 1 ) {
      /* Process incoming events. */
      process_events( );
      /* Draw the screen. */
      draw_screen( );
  }

  /*
   * EXERCISE:
   * Record timings using SDL_GetTicks() and
   * and print out frames per second at program
   * end.
   */

  /* Never reached. */
  return 0;
}
