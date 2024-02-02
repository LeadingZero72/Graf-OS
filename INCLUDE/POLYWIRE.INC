/*

  POLYGON-WIREFRAME                         14.Februar 2019
                                          Alexander Graf


  Diese Routine wurde vom 3d-Werkzeug getrennt, da
  hier noch Line und Box ben”tigt werden, ganz einfach
  um Programmspeicher zu sparen und etwas auszuprobieren^^

*/

//Zeichnet ein Wireframe-Polygon ----------------------------
far void polywire(long linewidth)
{
  edgestruct *e1,*e2

  long i

  e1 = poly+12
  e2 = e1+64

  for(i=*poly.NPoints,i>0,i-=1)
  {
    Line((*e1.x),(*e1.y),(*e2.x),(*e2.y),(*poly.Color),linewidth);

    e1+=64
    e2+=64
  }
}
//Ende POLYWIRE.INC