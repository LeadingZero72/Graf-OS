/*######################################################

  STRUKTUR FšR 3D-OBJEKTE

  #####################################################*/
struct object
{
  long *next,*reg,*prev,*subnode
  long x,y,z
  long rx,ry,rz
  long sx,sy,sz
  long radius
  long colr
}


/*######################################################

  STRUKTUR FšR KOORDINATEN IN 3D-OBJEKTEN

  #####################################################*/
struct vertex
{
  long *next,*reg,*prev,*subnode

  long   x,  y,  z
  long  xx, yy, zz
  long xxx,yyy,zzz

  long screenx,screeny

  long u ,v

  long u0,v0
  long u1,v1

  long r,g,b
  long fog
}



/*######################################################

  STRUKTUR FšR POLYGONE IN 3D-OBJEKTEN

  #####################################################*/
struct polygon
{
  long *next,*reg,*prev,*subnode

  long NPoints
  long *T0
  long Color
  long dummy

  long *e0
  long *e1
  long *e2
  long *e3
  long *e4
  long *e5
  long *e6
  long *e7
}


  public object  *objects
  public vertex  *Kvertices,*Kvertex
  public polygon *Kpolygons,*Kpoly
  public tree    *Klist

//Kameraposition/Blickrichtung/Geschwindigkeit ------------
  public long camspeed

  public long camx ,camy ,camz
  public long camrx,camry,camrz
  public long camsx,camsy,camsz

  public long camsinx,camcosx
  public long camsiny,camcosy
  public long camsinz,camcosz

  public long rotx,roty,rotz



/*######################################################

  KAMERAVARIABLEN FšR NŽCHSTEN RUN UPDATEN

  #####################################################*/
far void cameraangles(long x,long y,long z)
{
  camrx   = x
  camry   = y
  camrz   = z
  camsinx = sinus(camrx    ,256)
  camcosx = sinus(camrx+256,256)
  camsiny = sinus(camry    ,256)
  camcosy = sinus(camry+256,256)
  camsinz = sinus(camrz    ,256)
  camcosz = sinus(camrz+256,256)
}



/*######################################################

  PUNKTELISTE KOORDINATEN DREHEN

  #####################################################*/
far void rotate(object *o, vertex *p)
{
  long sinx,cosx
  long siny,cosy
  long sinz,cosz
  long x,y,z,xx,yy,zz
  long midx,midy
  midx=videox>>1
  midy=videoy>>1

  //Trigonometrische Werte in Variablen bef”rdern
  sinx = sinus(*o.rx   ,256)
  cosx = sinus(*o.rx+256,256)
  siny = sinus(*o.ry    ,256)
  cosy = sinus(*o.ry+256,256)
  sinz = sinus(*o.rz    ,256)
  cosz = sinus(*o.rz+256,256)

  //Alle Punkte nacheinander transformieren
  while(p!=0)
  {
    //Rotation um Y
    x=((*p.x * cosy) - (*p.z * siny)) / 256
    z=((*p.x * siny) + (*p.z * cosy)) / 256
    y= *p.y;

    //Rotation um X
    yy=((y * cosx) - (z * sinx)) / 256
    zz=((y * sinx) + (z * cosx)) / 256
    xx=  x;

    //Rotation um Z
    x=((xx * cosz) - (yy * sinz)) / 256
    y=((xx * sinz) + (yy * cosz)) / 256
    z=  zz;

    *p.xx=x-camx+(*o.x);
    *p.yy=y-camy+(*o.y);
    *p.zz=z-camz+(*o.z);

    x=((*p.xx * camcosy) - (*p.zz * camsiny)) / 256;
    z=((*p.xx * camsiny) + (*p.zz * camcosy)) / 256;
    y= *p.yy;

    *p.yyy=((y * camcosx) - (z * camsinx)) / 256;
    *p.zzz=((y * camsinx) + (z * camcosx)) / 256;
    *p.xxx= x;

    *p.fog=*p.zzz>>5 if(*p.fog>255) *p.fog=255;

    if(*p.zzz>31)
    {
      *p.screenx =  *p.xxx*midx/(*p.zzz) + midx;
      *p.screeny = -*p.yyy*midy/(*p.zzz) + midy;
    }
    p=*p.next
  }
}



/*######################################################

  3D-OBJEKT-RENDERING

  #####################################################*/
far void drawobjects(object *obj)
{
  object     *o
  tree       *description
  polygon    *surfaces,*s
  vertex     *points,*p
  edgestruct *edge
  long       *ep,i

  o=*obj.next
  while(o!=0)
  {
    //Konstruktionsdaten laden
    description = *o.subnode
    if(description)
    {
      points = *description.subnode

      rotate(o,*points.next)


      //#####################################################
      //ALLE POLYGONE BEARBEITEN
      //#####################################################
      surfaces=*description.next
      s=*surfaces.subnode
      s=*s.next
      
      while(s!=0)
      {
        //Randdaten des Polygone definieren!
        *poly.NPoints=*s.NPoints    ;
        *poly.Color=*s.Color        ;
        *poly.T0=*s.T0              ;

        //Ecken inclusive der Schluáecke definieren!
        edge=poly+12
        ep=s+32
        for(i=0,i<(*poly.NPoints),i+=1)
        {
          p=*ep                     ;
          *edge.vxxx  = *p.xxx      ;
          *edge.vyyy  = *p.yyy      ;
          *edge.vzzz  = *p.zzz      ;
          *edge.x     = *p.screenx  ;
          *edge.y     = *p.screeny  ;
          *edge.u0    = *p.u        ;
          *edge.v0    = *p.v        ;
          *edge.red   =   0         ;
          *edge.green =   0         ;
          *edge.blue  =   0         ;
          *edge.fog   = *p.fog      ;
          edge+=64
          ep=ep+4
        }
        polyFinish(poly)

        //Clipping am linken Bildrand
        sutherland = poly
        hodgeman   = poly2

        //Far-Plane-Clipping
        cut2farplane(0,1024)
        if(*hodgeman.NPoints<3)   {s=*s.next continue;}

        //Near-Plane-Clipping
        cut2nearplane(0,128)
        if(*sutherland.NPoints<3) {s=*s.next continue;}

        //Perspektivische Korrektur fr UV und Z
        edge=sutherland+12

        for(i=0,i<*sutherland.NPoints,i++)
        {
          *edge.vzzz = (65536 / (*edge.vzzz))<<16;
          *edge.u0   = ((*edge.u0)*(*edge.vzzz>>16));
          *edge.v0   = ((*edge.v0)*(*edge.vzzz>>16));

          edge+=64
        }
        polyFinish(poly)

        //Clipping an den 4 Bildr„ndern
        cut2minimum(0,4,viewMinX)
        if(*hodgeman.NPoints  <3) {s=*s.next continue;}

        cut2maximum(0,4,viewMaxX)
        if(*sutherland.NPoints<3) {s=*s.next continue;}

        cut2minimum(4,0,viewMinY)
        if(*hodgeman.NPoints  <3) {s=*s.next continue;}

        cut2maximum(4,0,viewMaxY)
        if(*sutherland.NPoints<3) {s=*s.next continue;}

        //Fllalgorithmen aufrufen
        polyfill()
        s=*s.next
      }
    }
    //N„chstes Objekt
    o=*o.next
  }
}


/*######################################################

  3D-KAMERA MIT TASTATUR/MAUS BEWEGEN

  #####################################################*/
far void movecamera(short scancode)
{
  if(scancode==75)
  {
    camsx-=camspeed
  }
  if(scancode==77)
  {
    camsx+=camspeed
  }
  if(scancode==72)
  {
    camsz+=camspeed
  }
  if(scancode==80)
  {
    camsz-=camspeed
  }

  //Blickrichtung mit der Maus ver„ndern
  camry=(camry+MausDX)&1023
  camrx=(camrx-MausDY)&1023
  cameraangles(camrx,camry,camrz)

  //Kamera bewegt sich in seine Richtung
  camx+=( (camsx*camcosy)+(camsz*camsiny))/256
  camy+=   camsy
  camz+=(-(camsx*camsiny)+(camsz*camcosy))/256

  //Geschwindigkeit wird bed„mpft
  camsx=camsx*96/100
  camsy=camsy*96/100
  camsz=camsz*96/100
}


/*******************************************************
 *     OBJEKT-KONSTRUKTIONSWERKSZEUG INITIALISIEREN    *
 *******************************************************/
far void beginobject(long x,long y,long z)
{
  object *o

  //Objektnode anlagen und an Objekteliste h„ngen
  o = node(1000) place(o,objects)

  //Objektvariablen initialisieren
  *o.x     =   x;
  *o.y     =   y;
  *o.z     =   z;
  *o.rx    =   0;
  *o.ry    =   0;
  *o.rz    =   0;
  *o.sx    =   0;
  *o.sy    =   0;
  *o.sz    =   0;
  *o.radius= 100;
  *o.colr  = 0xffffff;

  //Node fr die Unterliste anlegen und im Objekt einschreiben
  Klist = node(0); *o.subnode = Klist

  //Polygonliste anlegen und hinter die Vertexliste legen
  Kpolygons = node(0) place(Kpolygons, Klist);

  //Punkteliste anlegen und hier ist noch dieses Headerless-Design, was ich bald „ndern m”chte, so daá da noch ein Header ist
  Kvertices = node(0); *Klist.subnode = Kvertices Kvertex=Kvertices
}


//Besetzt Punkt mit neuen Werten
far void setpoint(vertex *this,long x,long y, long z,long u,long v, long r,long g,long b)
{
  *this.x=x;
  *this.y=y;
  *this.z=z;
  *this.u=u;
  *this.v=v;
  *this.r=r;
  *this.g=g;
  *this.b=b;
}


/*******************************************************
  Fgt dem aktuellen Objekt einen weiteren Vertex hinzu
 *******************************************************/
far long addvertex(long x,long y,long z,long u,long v,long r,long g,long b)
{
  vertex *p

  //Neue Vertexnode anlegen und an letzten Punkt dranh„ngen
  p = node(488) place(p, Kvertex)

  //Koordinaten belegen
  setpoint(p, x, y, z, u,v, r,g,b)

  //Neuen Punkt als letzten in der Liste markieren, damit die Reihenfolge stimmt.
  Kvertex = p
  
  //Zeiger auf neuen Vertex returnen
  return(p)
}


/*******************************************************
 *  POLYGONLISTEN-KONSTRUKTIONSWERKZEUG INITIALISIEREN *
 *******************************************************/
far void beginpolygons()
{
  //Erste Polygon-Node erzeugen und global ver”ffentlichen
  Kpoly = node(256)

  //Polygonlisten-Subnode des Objekts mit dieser Liste verknpfen
  *Kpolygons.subnode = Kpoly
}



//Besetzt Objektoberfl„che mit Werten
far void addpolygon(long NPoints,long Texture,long Color,long E0,long E1,long E2,long E3)
{
  polygon *p

  p     = node(256) place(p,Kpoly)
  Kpoly = p

  //
  *Kpoly.NPoints = NPoints;
  *Kpoly.T0      = Texture;
  *Kpoly.Color   = Color;
  *Kpoly.e0      = search(Kvertices,E0);
  *Kpoly.e1      = search(Kvertices,E1);
  *Kpoly.e2      = search(Kvertices,E2);
  *Kpoly.e3      = search(Kvertices,E3);
}

//Ende 3D.INC