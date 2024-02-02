/*

  GRAFIK-SCHRIFTARTEN                     14.Januar 2019
                                            Alexander Graf


  Die Glyphen werden VOR dem Zeichnen auf šberlappung
  getestet und der Cursor entsprechend korrigiert.

  Dadurch ist Text immer korrekt im Viewport eingeblendet.


*/


struct fontstruct
{
  long *next
  long *name
  long *prev
  long *subnode
  long *Bitmap
}

struct fontchar
{
  int x0
  int y0
  int x1
  int y1
}


//Fontvariablen ---------------------------------------------
extern fontstruct *font
extern bitmap     *fonttarget
extern long       cursorx
extern long       cursory
extern long       fontsize
extern long       fontspaceX
extern long       fontspaceY
extern long       fontheight
extern long       fontborder
extern long       fontoffset



//Cursor positionieren --------------------------------------
far void setcursor(long x,long y);

//Cursor in neue Zeile setzen -------------------------------
far void newline();

//Zeichen am Cursor einblenden ------------------------------
far void putc(long c);

//Text am Cursor schreiben ----------------------------------
far void puts(string *quelle);

//Fontdatei(en) laden ---------------------------------------
far long loadfont(string *filename,string *fontname);

//Ende FONT.H