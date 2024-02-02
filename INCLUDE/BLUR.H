/*
  BLUR.H                                    24.September 2019
                                               Alexander Graf


*/

//Gewichtsfaktoren fÅr einzelne Pixelmengen als Kehrwerte zum Multiplizieren mit MMX-Words
extern int *blurweights

//Vertikales Blur-Filter mit variabler Kernelgrî·e
far void vblur(bitmap *source,bitmap *dest,long blur);

//Horizontales Blur mit variabler Kernelgrî·e
far void hblur(bitmap *source,bitmap *dest,long blur);

//Image-Crossfading aka Motion-Blur
far void mblur(bitmap *new,bitmap *old,bitmap *dest,long newmix,long oldmix);

//Ende BLUR.H