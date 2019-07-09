#ifndef __FONTPOOL_H_
#define __FONTPOOL_H_

#include <SDL2/SDL.h>
#include "texture.h"

#define NUM_FONT_CHARS 96

/* FONT TEMPLATE FOR THE STORED CHARACTERS WITH THEIR NUMBERS IN THE ARRAY

        //    !  "  #  $  %  &  '  (  )  *  +  ,  -  .  /
           0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,

        // 0  1  2  3  4  5  6  7  8  9  :  ;  <  =  >  ?
          16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,

        // @  A	 B  C  D  E  F  G  H  I  J  K  L  M  N  O
          32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,

        // P  Q  R  S  T  U  V  W  X  Y  Z  [  \  ]  ^  _
          48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,

        // `  a  b  c  d  e  f  g  h  i  j  k  l  m  n  o
          64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,

        // p  q  r  s  t  u  v  w  x  y  z  {  |  }  ~
          80,81,82,83,84,86,86,87,88,89,90,91,92,93,94,95,
*/

//structure of a font
typedef struct fontT
{
    int cWidth;                     //the width of each character in the texture
    int cHeight;                    //the height of each character in the texture
    int space[NUM_FONT_CHARS];      //how many pixels to move forward after each character
    textureT texture;               //the texture holding the characters
    SDL_Rect c[NUM_FONT_CHARS];     //clip rectangle for every character
    char name[255];                 //name of the font
}fontT;

//structure of the font pool
typedef struct fontPoolT
{
    fontT *fonts;       //pointer to the fonts
    int numFonts;       //current number of fonts in the font pool
    int maxFonts;       //current max number of fonts in the font pool
}fontPoolT;

fontPoolT *fontPoolNewFontPool(int initialNumFonts);

int fontPoolAddFont(fontPoolT *fontPool,char *textureFilename,char *fontName,int cWidth,int cHeight,int fontSpace);
void fontPoolSetFontPoolPointer(fontPoolT *newFontPool);
fontPoolT *fontPoolGetFontPoolPointer();
void fontPoolFreeFontPool(fontPoolT *fontPool);
fontT *fontPoolGetFontFromFontPoolByName(fontPoolT *fontPool,char *fontName);
void fontPoolSetCharacterFontSpace(fontPoolT *fontPool,char *fontName,char character,int space);
void fontPoolSetFontPoolPointer(fontPoolT *newFontPool);
void bitmapFontString(fontT *font,char *string,int x,int y);
void bitmapFontStringColor(fontT *font,char *string,int x,int y,SDL_Color color);
void bitmapFontStringScale(fontT *font,char *string,int x,int y,float scale);
void bitmapFontStringScaleColor(fontT *font,char *string,int x,int y,float scale,SDL_Color color);
void bitmapFontStringCenterScale(fontT *font,char *string,int x,int y,float scale);
void bitmapFontStringCenterScaleColor(fontT *font,char *string,int x,int y,float scale,SDL_Color color);
SDL_Color fontPoolGetFontColor(int r,int g,int b);


#endif // __FONTPOOL_H_
