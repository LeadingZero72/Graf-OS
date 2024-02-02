/*
  VIEWPORT-CLIPPING                           8.Januar 2019
                                            Alexander Graf

  Viewport-Clipping in 2d fr projizierte Polygone.

  Das Polygon wird gegen alle einzelnen R„nder des Viewports
  abgeschnitten.

  Das Polygon liegt in komplett aufgel”ster Form vor, d.h.
  die Edgepointer sind weg und stattdessen liegen alle
  berechneten Daten direkt in dem Polygon-Array.

  Es wird dieselbe Struktur verwendet, die auch zum Fllen
  des Polygons verwendet wird -> <poly>

  Um nicht st„ndig Parameter zu bergeben, wird davon
  ausgegangen, daá es zwei feste Polygonstrukturen im
  Programm gibt, zwischen denen quasi Pingpong-m„áig die
  Daten hin- und hergerechnet werden.

  Der Algo kann einmal fr die X-Achse verwendet werden,
  aber auch fr die Y-Achse. Hierfr wird ein Achsenoffset
  angegeben.

  Die Y-Werte k”nnen anhand des Abschnitts ausgerechnet
  werden.

  Die Trabanten mssen allerdings perspektivisch korrekt
  bewegt werden, weil hier wieder die Bildschirmregel gilt:

  UZ und VZ sind linear, U und V nicht.


  Eine Idee w„re jetzt, alle 1/x-Werte VOR dem Clipping
  zu berechnen und diese beizubehalten, statt jedesmal
  bei der Schnittpunktberechnung alles vor- und zurck
  zu rechnen.
*/
public polystruct *sutherland,*hodgeman


void intersect3d(edgestruct *this,edgestruct *next,long grenze,edgestruct *output)
{
  long deltax,deltay,deltaz,rand
  long midx,midy

  midx=videox/2
  midy=videoy/2

  deltax = *next.vxxx - (*this.vxxx)
  deltay = *next.vyyy - (*this.vyyy)
  deltaz = *next.vzzz - (*this.vzzz)

  rand   = grenze  - (*this.vzzz);

  *output.vxxx  = *this.vxxx + ((rand * deltax) / deltaz);
  *output.vyyy  = *this.vyyy + ((rand * deltay) / deltaz);
  *output.vzzz  = grenze;

  *output.x     =  *output.vxxx*midx/grenze + midx;
  *output.y     = -*output.vyyy*midy/grenze + midy;


//Die Koordinaten werden korrekt berechnet. Das ist geil!
//Ich h„tte so gerne sch”ne gutaussehende Abschnitte, am
//besten ganz weich.


  *output.u0    = *this.u0    + ((rand * (*next.u0    - (*this.u0   )) ) / deltaz);
  *output.v0    = *this.v0    + ((rand * (*next.v0    - (*this.v0   )) ) / deltaz);
  *output.fog   = *this.fog   + ((rand * (*next.fog   - (*this.fog  )) ) / deltaz);
  *output.red   = *this.red   + ((rand * (*next.red   - (*this.red  )) ) / deltaz);
  *output.green = *this.green + ((rand * (*next.green - (*this.green)) ) / deltaz);
  *output.blue  = *this.blue  + ((rand * (*next.blue  - (*this.blue )) ) / deltaz);
}



  public long       *thisX,*thisY
  public long       *nextX,*nextY
  public long       *outX ,*outY

  public edgestruct *thisedge,*nextedge,*outputedge


void insekt(long grenze)
{
  long deltax,deltay,rand

  deltax = *nextX - (*thisX)
  deltay = *nextY - (*thisY)
  rand   = grenze - (*thisX);

  *outX             = grenze;
  *outY             = *thisY      + ((rand * deltay) / deltax);
  *outputedge.u0    = *thisedge.u0    + ((rand * (*nextedge.u0    - (*thisedge.u0   )) ) / deltax);
  *outputedge.v0    = *thisedge.v0    + ((rand * (*nextedge.v0    - (*thisedge.v0   )) ) / deltax);
  *outputedge.vzzz  = *thisedge.vzzz  + ((rand * (*nextedge.vzzz  - (*thisedge.vzzz )) ) / deltax);
  *outputedge.fog   = *thisedge.fog   + ((rand * (*nextedge.fog   - (*thisedge.fog  )) ) / deltax);
  *outputedge.red   = *thisedge.red   + ((rand * (*nextedge.red   - (*thisedge.red  )) ) / deltax);
  *outputedge.green = *thisedge.green + ((rand * (*nextedge.green - (*thisedge.green)) ) / deltax);
  *outputedge.blue  = *thisedge.blue  + ((rand * (*nextedge.blue  - (*thisedge.blue )) ) / deltax);

  outputedge+=64
  outX+=64
  outY+=64
}

void copyedge()
{
  copy(thisedge,outputedge,64)
  outputedge+=64
  outX+=64
  outY+=64
}


void cut2minimum(long axisX,long axisY,long begrenzung)
{
  short i

  //Minimum-Clipping immer von poly -> poly2 (die Maximum-Tests liegen dazwischen und machen es dann andersherum!)
  thisedge  = sutherland +12
  nextedge  = thisedge  +64
  outputedge= hodgeman   +12

  thisX=thisedge+axisX
  thisY=thisedge+axisY

  nextX=nextedge+axisX
  nextY=nextedge+axisY

  outX =outputedge+axisX
  outY =outputedge+axisY


  for(i=*sutherland.NPoints,i!=0,i-=1)
  {
    //Ist der Startpunkt noch innen ?
    if(*thisX>=begrenzung)
    {
      //Sind beide innen, dann Startpunkt in die Liste
      if(*nextX>=begrenzung)
      {
        copyedge()
      }
      //Ansonsten Startpunkt kopieren und Schnittpunkt zum Endpunkt berechnen und in die Liste
      else
      {
        copyedge()
        insekt(begrenzung)
      }
    }
    //Ist der Startpunkt bereits auáen ?
    else
    {
      //Wenn der Endpunkt wieder innen liegt, dann Schnittpunkt und Endpunkt in die Liste
      if(*nextX>=begrenzung)
      {
        insekt(begrenzung)
      }
    }
    thisedge+=64
    nextedge+=64
    thisX+=64
    thisY+=64
    nextX+=64
    nextY+=64
  }
  *hodgeman.NPoints = (outputedge-12 - hodgeman) >> 6

  polyFinish(hodgeman)
}



void cut2maximum(long axisX,long axisY,long begrenzung)
{
  short i

  //Minimum-Clipping immer von poly -> poly2 (die Maximum-Tests liegen dazwischen und machen es dann andersherum!)
  thisedge  = hodgeman  +12
  nextedge  = thisedge +64
  outputedge= sutherland+12

  thisX=thisedge+axisX
  thisY=thisedge+axisY

  nextX=nextedge+axisX
  nextY=nextedge+axisY

  outX =outputedge+axisX
  outY =outputedge+axisY

  for(i=*hodgeman.NPoints,i!=0,i-=1)
  {
    //Ist der Startpunkt noch innen ?
    if(*thisX<=begrenzung)
    {
      //Sind beide innen, dann Startpunkt in die Liste
      if(*nextX<=begrenzung)
      {
        copyedge()
      }
      //Ansonsten Startpunkt kopieren und Schnittpunkt zum Endpunkt berechnen und in die Liste
      else
      {
        copyedge()
        insekt(begrenzung)
      }
    }
    //Ist der Startpunkt bereits auáen ?
    else
    {
      //Wenn der Endpunkt wieder innen liegt, dann Schnittpunkt und Endpunkt in die Liste
      if(*nextX<=begrenzung)
      {
        insekt(begrenzung)
      }
    }
    thisedge+=64
    nextedge+=64
    thisX+=64
    thisY+=64
    nextX+=64
    nextY+=64
  }
  *sutherland.NPoints = (outputedge-12 - sutherland) >> 6

  polyFinish(sutherland)
}





/*


  Die Problematik beim Near-plane-Clipping sieht nun
  etwas anders aus.


  Da die polystruct aber noch 3d-Koordinaten bietet,
  gehe ich vor, wie gehabt und hole die Vektordaten
  einfach mit in die Polygonstruktur und kann dann auch
  in 3D clippen.


  Soooo....nach langem rumprobieren mit den scheiss Variablen
  habe ich einen guten Namen fr sie gefunden und kann
  einzeln auf sie zugreifen.


  Das Clipping fr die Nearplane beruht auf Z, doch Z
  liegt nur als 1/z vor. Ich muá also mit 1/nearplane
  vergleichen und den Vergleich in der Richtung umkehren.

  Die Werte mssen jetzt kleiner sein, um gr”áer zu sein^^

  Auáerdem ist jetzt so, daá die Z-Werte im 3d-Raum linear
  sein mssen, was fr 1/z nicht gilt.

  Die Umwandlung von z auf 1/z soll also nicht mehr in den
  Vertexberechnungen stattfinden, sondern nach dem 3d-Clipping.

  So bleiben die Tests dann auch 'rational' und ich kann
  weiterhin Z verwenden O.o

*/
void cut2nearplane(long axis,long begrenzung)
{
  edgestruct *this,*next,*output
  short i

  //Minimum-Clipping immer von poly -> poly2 (die Maximum-Tests liegen dazwischen und machen es dann andersherum!)
  this  = hodgeman   +12
  next  = this     +64
  output= sutherland +12

  for(i=*hodgeman.NPoints,i>0,i-=1)
  {
    //Ist der Startpunkt noch innen ?
    if(*this.vzzz>=begrenzung)
    {
      //Sind beide innen, dann Startpunkt in die Liste
      if(*next.vzzz>=begrenzung)
      {
        copy(this,output,64) output+=64
      }

      //Ansonsten Startpunkt kopieren und Schnittpunkt zum Endpunkt berechnen und in die Liste
      else
      {
        copy(this,output,64) output+=64
        intersect3d(this,next,begrenzung,output) output+=64
      }
    }
    //Ist der Startpunkt bereits auáen ?
    else
    {
      //Wenn der Endpunkt wieder innen liegt, dann Schnittpunkt und Endpunkt in die Liste
      if(*next.vzzz>=begrenzung)
      {
        intersect3d(this,next,begrenzung,output) output+=64
      }
    }
    this+=64
    next+=64
  }
  *sutherland.NPoints = (output-12 - sutherland) >> 6

  polyFinish(sutherland)
}




void cut2farplane(long axis,long begrenzung)
{
  edgestruct *this,*next,*output
  short i

  //Minimum-Clipping immer von poly -> poly2 (die Maximum-Tests liegen dazwischen und machen es dann andersherum!)
  this  = sutherland +12
  next  = this     +64
  output= hodgeman   +12

  for(i=*sutherland.NPoints,i>0,i-=1)
  {
    //Ist der Startpunkt noch innen ?
    if(*this.vzzz<=begrenzung)
    {
      //Sind beide innen, dann Startpunkt in die Liste
      if(*next.vzzz<=begrenzung)
      {
        copy(this,output,64) output+=64
      }

      //Ansonsten Startpunkt kopieren und Schnittpunkt zum Endpunkt berechnen und in die Liste
      else
      {
        copy(this,output,64) output+=64
        intersect3d(this,next,begrenzung,output) output+=64
      }
    }
    //Ist der Startpunkt bereits auáen ?
    else
    {
      //Wenn der Endpunkt wieder innen liegt, dann Schnittpunkt und Endpunkt in die Liste
      if(*next.vzzz<=begrenzung)
      {
        intersect3d(this,next,begrenzung,output) output+=64
      }
    }
    this+=64
    next+=64
  }
  *hodgeman.NPoints = (output-12 - hodgeman) >> 6

  polyFinish(hodgeman)
}
//Ende CLIPVIEW.INC