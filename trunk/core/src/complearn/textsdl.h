#ifndef __TEXTSDL_H
#define __TEXTSDL_H

#include <complearn/complearn.h>
#if SDL_RDY

#include <gsl/gsl_vector.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

#define MAXTEX 30

struct CLTexture {
  SDL_Surface *initial;
  SDL_Surface *intermediary;
  int texName;
  int w;
  int h;
};

void init_sdltext(void);
void close_sdltext(TTF_Font *font);
void draw_sdltext(char *str, struct CLTexture *clt, gsl_vector *p);
void setTextPos(double x, double y, double z);
void  draw_sdlhelp(void);

#endif

#endif
