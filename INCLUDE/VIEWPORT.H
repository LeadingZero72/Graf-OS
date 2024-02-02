/*

  VIEWPORT.INC                        6.Januar 2019
                                      Alexander Graf

  Alles rund um den Viewport. Der Viewport ist eine
  Struktur, welche die linke/obere und rechte/untere
  Ecke des Zeichen-Bildausschnitts innerhalb des
  Bitmaps angibt.

  Der Viewport ist ein globales Werkzeug und muá
  nach jeder Kooperationsphase neu gesetzt werden.

*/
extern long viewMinX
extern long viewMinY
extern long viewMaxX
extern long viewMaxY

far void setviewport(long minx,long miny,long maxx,long maxy);


//Ende VIEWPORT.INC