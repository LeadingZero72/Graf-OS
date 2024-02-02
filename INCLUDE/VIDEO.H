/*

   FRAMEBUFFER                                15.Januar 2019
                                               Alexander Graf
*/

extern bitmap *frame1
extern bitmap *frame2
extern bitmap *frame3
extern bitmap *frame4
extern bitmap *frame5
extern bitmap *frame6
extern bitmap *frame7
extern bitmap *frame8
extern bitmap *zbuffer
extern bitmap *videotarget


extern long *videoram
extern long videox,videoy,videosize
extern int  videopage


far long newbitmap(long resx,long resy);
far long clearbitmap(bitmap *this,long colr);
far void setresolution(long x,long y);
far void videomode(long ResX,long ResY);


//Ende VIDEO.H