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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <math.h>

#ifdef WINCHOICE
#include <windows.h>
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include "textsdl.h"

#define GL_BGR 0x80E0
#define GL_BGRA 0x80E1

/*! \file textsdl.c */

/** \brief contains an SDL Text Rendering style with a given color and font
 *
 * The Simple DirectMedia Layer text rendering subroutines use this for styling.
 *
 * \sa textsdl.c
 * \struct CLTextStyle
 */
struct CLTextStyle {
  SDL_Color *color;
  TTF_Font *font;
  int size;
};
char *subfontpath = "%s/Fonts/arial.ttf";
int screenwidth = 640;
int screenheight = 480;
TTF_Font *font;

int cl_round(double x) { return (int)(x + 0.5); }

void doProject(double objX, double objY, double objZ, double *outx, double *outy)
{
  GLdouble model[16] ;
  GLdouble proj[16] ;
  GLint view[4] ;
  GLdouble winX, winY, winZ ;
  glGetDoublev(GL_MODELVIEW_MATRIX, model) ;
  glGetDoublev(GL_PROJECTION_MATRIX, proj) ;
  glGetIntegerv(GL_VIEWPORT, view) ;
  gluProject(objX, objY, objZ, model, proj, view, &winX, &winY, &winZ ) ;
  *outx = winX;
  *outy = winY;
}

int nextpoweroftwo(int x)
{
  double logbase2 = log(x) / log(2);
  return cl_round(pow(2,ceil(logbase2)));
}

void init_sdltext()
{
  if(TTF_Init()==-1) {
    printf("TTF_Init: %s\n", TTF_GetError());
    exit(2);
    }
  atexit(TTF_Quit);
  static char fontpath[2048];
#ifdef WINCHOICE
  static char wdirspace[2048];
  GetWindowsDirectory(wdirspace, 1024);
  sprintf(fontpath, subfontpath, wdirspace);
#else
#ifdef __APPLE__
  strcpy(fontpath, "/Library/Fonts/Arial.ttf");
#else
  strcpy(fontpath, "/usr/share/fonts/truetype/ttf-bitstream-vera/Vera.ttf");
#endif
#endif
  if(!(font = TTF_OpenFont(fontpath, 14))) {
    printf("Error loading font\n");
    printf("error msg: %s\n", TTF_GetError());
    exit (1);
  }
}

void SDL_GL_RenderText(char *text,
                      TTF_Font *font,
                      SDL_Color color,
                      SDL_Rect *location,
                      struct CLTexture *clt)
{

  if (!clt->texName) {
    glGenTextures(1, &clt->texName);
  }

  /* Use SDL_TTF to render our text */
  if (!clt->initial) {
    clt->initial = TTF_RenderText_Blended(font, text, color);

    /* Convert the rendered text to a known format */
    clt->w = nextpoweroftwo(clt->initial->w);
    clt->h = nextpoweroftwo(clt->initial->h);

    clt->intermediary = SDL_CreateRGBSurface(0, clt->w, clt->h, 32,
        0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);

//    glTextEnv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    SDL_BlitSurface(clt->initial, 0, clt->intermediary, 0);

    /* Tell GL about our new texture */
//    glGenTextures(2, textures);
    glBindTexture(GL_TEXTURE_2D, clt->texName);
    glTexImage2D(GL_TEXTURE_2D, 0, 4, clt->w, clt->h, 0, GL_BGRA,
      GL_UNSIGNED_BYTE, clt->intermediary->pixels );

    /* GL_NEAREST looks horrible, if scaled... */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  }

  /* prepare to render our texture */
  glBindTexture(GL_TEXTURE_2D, clt->texName);
  glDisable(GL_LIGHTING);

  /* Draw a quad at location */
  glBegin(GL_QUADS);
    /* Recall that the origin is in the lower-left corner
       That is why the TexCoords specify different corners
       than the Vertex coors seem to. */
    glTexCoord2f(0.0f, 1.0f);
      glVertex2f(location->x    , location->y);
    glTexCoord2f(1.0f, 1.0f);
      glVertex2f(location->x + clt->w, location->y);
    glTexCoord2f(1.0f, 0.0f);
      glVertex2f(location->x + clt->w, location->y + clt->h);
    glTexCoord2f(0.0f, 0.0f);
      glVertex2f(location->x    , location->y + clt->h);
  glEnd();
  glEnable(GL_LIGHTING);

  /* Bad things happen if we delete the texture before it finishes */
//  glFinish();

  /* return the deltas in the unused w,h part of the rect */
//  location->w = initial->w;
//  location->h = initial->h;

  /* Clean up */
//  SDL_FreeSurface(initial);
//  SDL_FreeSurface(intermediary);
//  glDeleteTextures(1, &texture);

}

void glEnable2D()
{
  int vPort[4];

  glGetIntegerv(GL_VIEWPORT, vPort);

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();

  glOrtho(0, vPort[2], 0, vPort[3], -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
}

void glDisable2D()
{
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
}

void draw_sdlpreparetodrawlabel(struct CLTexture *clt, gsl_vector *p, SDL_Rect *position)
{
    double textPosX = 0.0, textPosY = 0.0, textPosZ = 0.0;

//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    glTranslatef(0.2f, 0.2f, 0.2f);
    glDisable(GL_TEXTURE_2D);

    /* Go in HUD-drawing mode */
    double outx, outy;
    textPosX = gsl_vector_get(p,0);
    textPosY = gsl_vector_get(p,1);
    textPosZ =  gsl_vector_get(p,2);
    doProject(textPosX, textPosY, textPosZ, &outx, &outy);
    glEnable2D();
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    /* Draw some text */
    /** A quick note about position.
     * Enable2D puts the origin in the lower-left corner of the
     * screen, which is different from the normal coordinate
     * space of most 2D api's. position, therefore,
     * gives the X,Y coordinates of the lower-left corner of the
     * rectangle **/
    position->y = outy;
    position->x = outx;

    glAlphaFunc(GL_GEQUAL, 0.0625);
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);

//  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBlendFunc(GL_ONE, GL_ONE);
}

void draw_sdltextteardown()
{
  glDisable(GL_BLEND);
  glEnable(GL_ALPHA_TEST);
  glEnable(GL_LIGHTING);

    /* Come out of HUD mode */
    glEnable(GL_DEPTH_TEST);
    glDisable2D();
    glDisable(GL_TEXTURE_2D);

    /* Show the screen */
//    SDL_GL_SwapBuffers( );
}

void draw_sdltext(char *str, struct CLTexture *clt, gsl_vector *p)
{
  SDL_Rect position;
  draw_sdlpreparetodrawlabel( clt, p, &position);
  SDL_Color color = {200,200,200,55};
  SDL_GL_RenderText(str, font, color, &position, clt);
  draw_sdltextteardown();
}

#define MAXLINES 128
void draw_sdlhelp()
{
  SDL_Rect position;
  static gsl_vector *p;
  SDL_Color color = {0,255,0,55};
  static struct CLTexture helplines[MAXLINES];
  int origx, origy, yincr;
  int ind = 0;
  origy = 100;
  origx = 5; yincr = -14;

  if (!p) { p = gsl_vector_calloc(3); }

//  draw_sdlpreparetodrawlabel(&helplines[ind], p, &position);
//  position.x = origx;
//  SDL_GL_RenderText("line 1", font, color, &position, &helplines[ind++]);

#define ADDLINE(mx) do { char *locstr = mx; draw_sdlpreparetodrawlabel(&helplines[ind], p, &position); position.x = origx; position.y = origy + yincr*ind; SDL_GL_RenderText(locstr, font, color, &position, &helplines[ind++]); } while (0)
  ADDLINE("HELP MENU");
  ADDLINE("Up/Down/Right/Left keys - camera");
  ADDLINE("Shift-Up key/Shift-Down key - zoom in/out");
  ADDLINE("m - mutate                l - labels on/off");
  ADDLINE("f - flatten on/off         h - help on/off");
  ADDLINE("Esc - exit                  Don't try more than 24 files.");
  ADDLINE("Drag and drop at least 4 files (or a directory) into this window to begin");

  draw_sdltextteardown();
}
