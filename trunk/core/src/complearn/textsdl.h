#ifndef __TEXTSDL_H
#define __TEXTSDL_H

#include <complearn/complearn.h>
#if HAVE_SDL_SDL_TTF_H

#include <gsl/gsl_vector.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

#define MAXTEX 30

/*! \file textsdl.h */

/** \brief Contains a single texture bitmap
 *
 * \struct CLTexture
 *
 * This structure is used each time a new texture bitmap is needed.
 */
struct CLTexture {
  SDL_Surface *initial;
  SDL_Surface *intermediary;
  unsigned int texName;
  int w;
  int h;
};

void init_sdltext(void);
void draw_sdltext(char *str, struct CLTexture *clt, gsl_vector *p);
void setTextPos(double x, double y, double z);
void draw_sdlhelp(void);

#endif

#endif
