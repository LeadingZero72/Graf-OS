/************************************************************
 * DRAW BITMAP                   06.01.2019 Alexander Graf  *
 *                                                          *
 * Bei einer Box ist das Clipping wichtig, damit die Box    *
 * auch auáerhalb eines Bildes liegen kann.                 *
 *                                                          *
 * Liegt die Box links, ist Breite-x die neue Breite und x  *
 * wird Null.                                               *
 * Liegt die Box oben, geht das gleiche Spiel mit der H”he. *
 * Liegt sie rechts, dann ist ResX-x die neue Breite.       *
 * Liegt sie unten, dann ist ResY-y die neue H”he.          *
 *                                                          *
  Zeichnet ein Bitmap in eigener Gr”áe an die gegebenen
  Koordinaten.


 *                                                          *
 ************************************************************/
far void Sprite(long x0,long y0,bitmap *source)
{
  long *adr,*sadr,x1,y1,i,j,DeltaX,DeltaY
  long Jump,SJump,LinkerRand,RechterRand,ObererRand
  long todraw
  long width,height

  width = *targetbitmap.ResX
  height= *targetbitmap.ResY

  x1=x0+(*source.ResX)-1
  y1=y0+(*source.ResY)-1

  if(x1<viewMinX)||(y1<viewMinY)||(x0>=viewMaxX)||(y0>=viewMaxY) {return()}

  DeltaX = x1-x0 + 1
  DeltaY = y1-y0 + 1

  LinkerRand=0
  RechterRand=0
  ObererRand=0

  if(x0<viewMinX)
  {
    LinkerRand=(viewMinX-x0)*4
    DeltaX-=viewMinX-x0
    x0=viewMinX
  }

  if(y0<viewMinY)
  {
    ObererRand=(viewMinY-y0)*4*(*source.ResX)
    DeltaY-=viewMinY-y0
    y0=viewMinY
  }

  if(x0+DeltaX>=viewMaxX)
  {
    RechterRand = (x0+DeltaX-viewMaxX)*4
    DeltaX = viewMaxX - x0
  }

  if(y0+DeltaY>=viewMaxY)
  {
    DeltaY = viewMaxY - y0
  }

  //Adresse im Zielbild
  adr=targetbitmap+32+(y0*width+x0)*4

  //Adresse im Quellbild
  sadr=source+32+LinkerRand+ObererRand

//  SJump  = LinkerRand+RechterRand
//  Jump   = (width - DeltaX)*4

  //Sprung in Quellbild und Zielbild
  SJump  = (*source.ResX)*4
  Jump   = (*targetbitmap.ResX)*4
  todraw = DeltaX*4

  //Alle restlichen Pixel zeichnen
  for(j=0,j<DeltaY,j++)
  {
    copy(sadr,adr,todraw)
    sadr+=SJump
    adr+=Jump
  }
}
//Ende SPRITE.INC