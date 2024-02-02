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


  extern object  *objects
  extern vertex  *points,*thispoint,*lastpoint
  extern polygon *surfaces,*thispoly
  extern tree    *description


  extern long camspeed

  extern long camx ,camy ,camz
  extern long camrx,camry,camrz
  extern long camsx,camsy,camsz

  extern long camsinx,camcosx
  extern long camsiny,camcosy
  extern long camsinz,camcosz

  extern long rotx,roty,rotz

  far void cameraangles(long x,long y,long z);
  far void rotate(vertex *p);
  far void drawobjects(object *obj);
  far void movecamera(short scancode);
  far void setpoint(vertex *this,long x,long y, long z,long u,long v, long r,long g,long b);
  far void setface(long NPoints,long Texture,long Color,long E0,long E1,long E2,long E3);

//Ende 3D.H