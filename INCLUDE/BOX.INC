/************************************************************
 * DRAW BOX                      18.12.2018 Alexander Graf  *
 *                                                          *
 * Bei einer Box ist das Clipping wichtig, damit die Box    *
 * auch au·erhalb eines Bildes liegen kann.                 *
 *                                                          *
 * Liegt die Box links, ist Breite-x die neue Breite und x  *
 * wird Null.
 * Liegt die Box oben, geht das gleiche Spiel mit der Hîhe. *
 * Liegt sie rechts, dann ist ResX-x die neue Breite.       *
 * Liegt sie unten, dann ist ResY-y die neue Hîhe.          *
 *                                                          *
 *                                                          *
 ************************************************************/

far void Box(long x0,long y0,long x1,long y1, long q)
{
  long *adr,i,j,DeltaX,DeltaY
  long todraw
  long width,height

  //Abmessungen des Zielbildes
  width = *targetbitmap.ResX
  height= *targetbitmap.ResY

  //Im gÅnstigsten Fall nichts zeichnen
  if(x0==x1)        {return()}
  if(y0==y1)        {return()}
  if(x1<viewMinX)   {return()}
  if(y1<viewMinY)   {return()}
  if(x0>=viewMaxX)  {return()}
  if(y0>=viewMaxY)  {return()}

  //Abmessungen des FÅllbereichs berechnen
  DeltaX = x1-x0 + 1
  DeltaY = y1-y0 + 1

  //Clipping am Viewport
  if(x0<viewMinX)
  {
    DeltaX-=viewMinX-x0
    x0=viewMinX
  }

  if(y0<viewMinY)
  {
    DeltaY-=viewMinY-y0
    y0=viewMinY
  }

  if(x0+DeltaX>=viewMaxX)
  {
    DeltaX = viewMaxX - x0
  }

  if(y0+DeltaY>=viewMaxY)
  {
    DeltaY = viewMaxY - y0
  }

  //Startadresse im Zielbild berechnen
  adr=targetbitmap+32+(y0*width+x0)*4

  //Bytes die gefÅllt werden
  todraw=DeltaX<<2

  //Bytes die zwischen den Zeilen liegen
  width=width<<2

  //Alle Zeilen nacheinander fÅllen
  for(j=0,j<DeltaY,j++)
  {
    blast(adr,todraw,q)
    adr+=width
  }
}
//Ende BOX.INC