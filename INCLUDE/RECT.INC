/************************************************************
 * RECTANGLE.INC                   9.01.2019 Alexander Graf *
 ************************************************************
 * Bei einer Box ist das Clipping wichtig, damit die Box    *
 * auch auáerhalb eines Bildes liegen kann.                 *
 *                                                          *
 * Liegt die Box links, ist Breite-x die neue Breite und x  *
 * wird Null.                                               *
 * Liegt die Box oben, geht das gleiche Spiel mit der H”he. *
 * Liegt sie rechts, dann ist ResX-x die neue Breite.       *
 * Liegt sie unten, dann ist ResY-y die neue H”he.          *
 *                                                          *
 *                                                          *
 ************************************************************/
far void Rectangle(long x0,long y0,long x1,long y1, long q)
{
  long *adr,i,j,DeltaX,DeltaY
  long todraw,jump
  long width,height
  long clippingbits

  //Bisher noch kein Clipping (Bits:1=LEFT,2=TOP,4=RIGHT,8=BOTTOM)
  clippingbits=0

  width = *targetbitmap.ResX
  height= *targetbitmap.ResY

  if(x0==x1)||(y0==y1)||(x1<0)||(y1<0)||(x0>=width)||(y0>=height) {return()}

  DeltaX = x1-x0 + 1
  DeltaY = y1-y0 + 1


//---------------------
  if(x0<0)
  {
    DeltaX+=x0
    x0=0
    clippingbits|=1
  }
  if(y0<0)
  {
    DeltaY+=y0
    y0=0
    clippingbits|=2
  }

  if(x0+DeltaX>=width)
  {
    DeltaX = width - x0
    clippingbits|=4
  }

  if(y0+DeltaY>=height)
  {
    DeltaY = height - y0
    clippingbits|=8
  }

  adr=targetbitmap+32+(y0*width+x0)*4

  //Sprung vom Ende der Zeile bis zum x0 in der n„chsten (4 Byte extra wegen dem einzelnen Pixel an der rechten Seite)
  jump=(width-DeltaX)<<2+4

  todraw=DeltaX<<2
  width=width<<2

  //Oberste Zeile durchg„ngig malen
  if(clippingbits&2==0) blast(adr,todraw,q)
  adr+=width

  //Von der H”he allgemein 2 durchg„ngig gefllte Zeilen abziehen
  DeltaY-=2

  //Sind noch einzelne Zeilen dazwischen (die den Rahmen fr den Inhalt des Rechtecks links und rechts bilden)
  if(DeltaY>0)
  {
    //Innerhalb der Zeile ist der Sprung etwas anders
    todraw-=4

    //Alle Zeilen des Recheckrahmens zeichnen
    for(j=0,j<DeltaY,j++)
    {
      if(clippingbits&1==0) { *adr=q }
      adr+=todraw
      if(clippingbits&4==0) { *adr=q }
      adr+=jump
    }
  }
  if(clippingbits&8==0) blast(adr,todraw+4,q)
}
//Ende RECT.INC