/************************************************************
 * CIRCLE/FILLCIRCLE              3.01.2019 Alexander Graf  *
 *                                                          *
 * Wie zeichnet man einen perfekten Kreis ?                 *
 *                                                          *
 * Wenn ich alle Quadranten gespiegelt gleich mache,        *
 * muss ich nur 90 Grad berechnen.                          *
 *                                                          *
 * Wenn ich so anfange, daá der erste Punkt links in der    *
 * Zeile beim Mittelpunkt liegt, kann ich eine horizontale  *
 * Strecke fllen.                                          *
 *                                                          *
 * Die n„chste Frage ist, welchen Winkel ich brauche, damit *
 * die Zeile sich „ndert. Den benutze ich fr die n„chste   *
 * x-Koordinate.                                            *
 *                                                          *
 * So berspringe ich immer wieder Winkel, um zu den neuen  *
 * x-Koordinaten zu kommen.                                 *
 *                                                          *
 * Ein anderer Ansatz wrde den Umfang in Pixeln berechnen  *
 * und entsprechend hoch aufgel”ste Winkel nehmen.          *
 *                                                          *
 ************************************************************/

  public long fillcolor

void hline(long x0,long x1,long y,long col)
{
  long *adr

  if(x0>=viewMaxX)||(x1 <viewMinX)
  ||(y  <viewMinY)||(y >=viewMaxY)||(x0==x1) {return()}

  if(x0 <viewMinX) {x0=viewMinX}
  if(x1>=viewMaxX) {x1=viewMaxX-1}

  adr = targetbitmap+32+((y*(*targetbitmap.ResX)+x0)*4)

  blast(adr,(x1-x0)*4,col)
}



void setPixel(long x,long y)
{
  long *adr

  if(x>=viewMinX)&&(x<=viewMaxX)&&(y>=viewMinY)&&(y<=viewMaxY)
  {
    adr = ((*targetbitmap.ResX)*y+x)*4+32+targetbitmap;
  
    *adr = fillcolor
  }
}



far void elipse(long xm,long ym,long a,long b)
{
  long x,y,e2,DX,DY,err

  x=-a
  y=0

  e2=b
  DX=(1+2*x)*e2*e2
  DY=x*x
  err=DX+DY


  while(x<=0)
  {
    setPixel(xm-x,ym+y)
    setPixel(xm+x,ym+y)
    setPixel(xm+x,ym-y)
    setPixel(xm-x,ym-y)

    e2=2*err

    if(e2>=DX) {x+=1 DX+=2*b*b err+=DX}
    if(e2<=DY) {y+=1 DY+=2*a*a err+=DY}
  }
}


far void fillelipse(long xm,long ym,long a,long b)
{
  long x,y,e2,DX,DY,err

  x=-a
  y=0

  e2=b
  DX=(1+2*x)*e2*e2
  DY=x*x
  err=DX+DY

  while(x<=0)
  {
    if(y!=0) hline(xm+x,xm-x,ym-y,fillcolor)
             hline(xm+x,xm-x,ym+y,fillcolor)

    e2=2*err

    if(e2>=DX) {x+=1 DX+=2*b*b err+=DX}
    if(e2<=DY) {y+=1 DY+=2*a*a err+=DY}
  }
}
//Ende CIRCLE.INC