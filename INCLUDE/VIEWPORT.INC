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
  public long viewMinX
  public long viewMinY
  public long viewMaxX
  public long viewMaxY


far void setviewport(long minx,long miny,long maxx,long maxy)
{
  viewMinX=minx
  viewMinY=miny
  viewMaxX=maxx
  viewMaxY=maxy
}
//Ende VIEWPORT.INC