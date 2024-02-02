/*

   POLY.INC                                  29.Januar 2019
                                         Alexander Graf


   Dieses Malwerkzeug stellt ein paar schnelle Routinen
   bereit, so da· jeder Polygonzeichner einfach erstellt
   werden kann:

   void mypolygondrawer()
   {
     polySetup()
     while(left!=right)
     {
       ..do stuff

       polyNextline()
     }
   }

   -------------------------------------------------------
*/


public long eins,kommastellen



//64 Bytes incl. Dummy-Variablen
struct edgestruct
{
  long x
  long y

  long vxxx
  long vyyy
  long vzzz

  long u0,v0
  long u1,v1
  long u2,v2 

  long fog
  long red,green,blue
}

struct polystruct
{
  long   NPoints
  long   *T0
  long   Color
}

//Speicher reservieren --------------------------------------
public polystruct *poly,*poly2


//Schlu·ecke kopieren
void polyFinish(polystruct *this)
{
  edgestruct *e1,*e2

  e1 = this+12
  e2 = e1+(*this.NPoints<<6);

  *e2.x     =   *e1.x;
  *e2.y     =   *e1.y;
  *e2.u0    =   *e1.u0;
  *e2.v0    =   *e1.v0;

  *e2.red   =   *e1.red;
  *e2.green =   *e1.green;
  *e2.blue  =   *e1.blue;

  *e2.fog   =   *e1.fog;

  *e2.vxxx   =   *e1.vxxx;
  *e2.vyyy   =   *e1.vyyy;
  *e2.vzzz   =   *e1.vzzz;
}


//12 Bytes Header + 15 Ecken
void polyinit()
{
  poly  = getmem(974)
  poly2 = getmem(974)
}



  //Kantenvariablen
  public edgestruct *polyL,*polyR

  public long polyY
  public long polyStop

  //Bildschirm-Scan-Variablen
  public long XfL,XiL,MfL,MiL,DeltaXL,DeltaYL,StepXL,CountL
  public long XfR,XiR,MfR,MiR,DeltaXR,DeltaYR,StepXR,CountR

  //Textur 1 Scan-Variablen
  public long U0 ,V0 ,DeltaU0 ,DeltaV0

  //Farbvariablen
  public long RL,GL,BL
  public long DeltaRL,DeltaGL,DeltaBL

  public long RR,GR,BR
  public long DeltaRR,DeltaGR,DeltaBR

  public long R0,G0,B0,F0
  public long DeltaR0,DeltaG0,DeltaB0,DeltaF0

  //Fogging-Variablen
  public long FL,FR
  public long DeltaFL,DeltaFR

  //Tiefenwert-Variablen
  public long Z0,DeltaZ0

  //Variablen fÅr perspektivische Korrektur
  public long uzl0,vzl0
  public long uzl1,vzl1
  public long uzr0,vzr0
  public long uzr1,vzr1
  public long duzl,dvzl
  public long duzr,dvzr
  public long zzl0,zzr0,dzzl,dzzr
  public long zzl1,zzr1



//Links weitersetzen, am Ende Array zyklisch halten
void polyNewL()
{
  polyL+=64
  if(polyL>=polyStop) { polyL=poly+12 }
}

//Schon links am Arrayanfang angekommen ? Dann ans Ende springen.
void polyNewR()
{
  if(polyR==poly+12) { polyR=polyStop-64 }
  else               { polyR-=64         }
}



//Berechnet die Steigung fÅr linke Kante ####################
void polyPrepL()
{
  edgestruct *this

  //Der Start der linken Seite befindet sich rechts vom linken Zielpunkt
  this = polyL+64

  //DeltaY ist immer positiv
  DeltaYL = *this.y - (*polyL.y)

  while(DeltaYL==0)
  {
    polyNewL()
    if(polyL==polyR) { return() }

    this    = polyL+64
    DeltaYL = *this.y - (*polyL.y)
  }

  //DeltaY muss immer mindestens 1 sein!
  if(DeltaYL>0)
  {
    //Konkretes X startet hier:
    XiL     = *polyL.x

    //Fraktalteil ist -DeltaY
    XfL     = -DeltaYL

    //SchleifenzÑhler entspricht dem Betrag von DeltaY, DeltaY ist immer positiv :-P
    CountL  = DeltaYL

    //Delta X berechnen
    DeltaXL = *this.x - (*polyL.x)

    //Steigung als 32:32 Fixkommazahl
    MiL = DeltaXL / DeltaYL
    MfL = DeltaXL % DeltaYL

    //Angenommen, der zusÑtzliche öberlauf geht nach rechts
    if(DeltaXL<0) { StepXL=-1 MfL=-MfL }
    else          { StepXL=1           }

    //Farbsteigung linke Seite berechnen
    RL      = *polyL.red <<16
    DeltaRL = ( (*this.red - (*polyL.red) ) << 16) / DeltaYL

    GL      = *polyL.green <<16
    DeltaGL = ( (*this.green - (*polyL.green) ) << 16) / DeltaYL

    BL      = *polyL.blue<<16
    DeltaBL = ( (*this.blue - (*polyL.blue) ) << 16) / DeltaYL

    //Fogging-Steigung links berechnen
    FL      = *polyL.fog<<16
    DeltaFL = ( (*this.fog - (*polyL.fog) ) << 16) / DeltaYL


    uzl0 = *polyL.u0
    vzl0 = *polyL.v0
    zzl0 = *polyL.vzzz

    uzl1 = *this.u0
    vzl1 = *this.v0
    zzl1 = *this.vzzz

    duzl = (uzl1 - uzl0) / DeltaYL
    dvzl = (vzl1 - vzl0) / DeltaYL
    dzzl = (zzl1 - zzl0) / DeltaYL
  }
}

//Berechnet die Steigung fÅr rechte Kante ###################
void polyPrepR()
{
  edgestruct *this

  //Der Start der linken Seite befindet sich rechts vom linken Zielpunkt
  this = polyR+64

  //DeltaY ist immer positiv
  DeltaYR = *polyR.y - (*this.y)

  while(DeltaYR==0)
  {
    polyNewR()
    if(polyR==polyL) {return()}

    this    = polyR+64
    DeltaYR = *polyR.y - (*this.y)
  }

  //DeltaY muss immer mindestens 1 sein!
  if(DeltaYR>0)
  {
    //Konkretes X startet hier:
    XiR     = *this.x

    //Fraktalteil ist -DeltaY
    XfR     = -DeltaYR

    //SchleifenzÑhler entspricht dem Betrag von DeltaY, DeltaY ist immer positiv :-P
    CountR  = DeltaYR

    //Delta X berechnen
    DeltaXR = *polyR.x - (*this.x)

    //Steigung als 32:32-Fixkommazahl
    MiR     = DeltaXR / DeltaYR
    MfR     = DeltaXR % DeltaYR

    //Angenommen, der zusÑtzliche öberlauf geht nach rechts
    if(DeltaXR<0) { StepXR=-1 MfR=-MfR }
    else          { StepXR=1           }

    //Farbsteigung rechte Seite berechnen
    RR      = *this.red <<16
    DeltaRR = ( (*polyR.red   - (*this.red))   << 16) / DeltaYR

    GR      = *this.green <<16
    DeltaGR = ( (*polyR.green - (*this.green)) << 16) / DeltaYR

    BR      = *this.blue<<16
    DeltaBR = ( (*polyR.blue  - (*this.blue))  << 16) / DeltaYR

    //Fogging-Steigung rechts berechnen
    FR      = *this.fog<<16
    DeltaFR = ( (*polyR.fog  - (*this.fog))  << 16) / DeltaYR

    //U/Z
    uzr0 = *this.u0
    vzr0 = *this.v0
    zzr0 = *this.vzzz

    uzr1 = *polyR.u0
    vzr1 = *polyR.v0
    zzr1 = *polyR.vzzz

    duzr = (uzr1 - uzr0) / DeltaYR
    dvzr = (vzr1 - vzr0) / DeltaYR
    dzzr = (zzr1 - zzr0) / DeltaYR
  }
}








//Setup fÅr die den Polygon-Scankonverter alltogether #######
void polySetup()
{
  long i
  edgestruct *edge
  long randoben



  eins=65536
  kommastellen=16



  //Hîchsten Punkt finden: Immer von oben nach unten zeichnen
  polyY=999999
  edge=poly+12

  for(i=0,i<*poly.NPoints,i+=1)
  {
    if(*edge.y<polyY) { polyY=*edge.y polyL=edge }
    edge+=64
  }

  //Arrayende vorausberechnen
  polyStop = *poly.NPoints*64+poly+12

  //Wenn am Arrayanfang, rechts ans andere Ende setzen, sonst links davon
  if(polyL!=poly+12) { polyR=polyL-64    }
  else               { polyR=polyStop-64 }

  //Kantensteigungen berechnen
  polyPrepL()
  polyPrepR()
}


//Polygonzeichner in nÑchste Zeile setzen -------------------
void polyNextLine()
{
  //Links/Rechts Fraktalteil und Integerteil updaten
  XfL+=MfL if(XfL>0) { XiL+=StepXL XfL-=DeltaYL }
  XiL+=MiL
  XfR+=MfR if(XfR>0) { XiR+=StepXR XfR-=DeltaYR }
  XiR+=MiR

  //NÑchsttiefere Y-Koordinate setzen
  polyY+=1

  //Linkes Ende erreicht ? Dann evtl. neue Linie links..
  CountL-=1
  if(CountL==0) { polyNewL() polyPrepL()  if(polyL==polyR) {return()} }

  //Rechtes Ende erreicht ? Dann evtl. neue Linie rechts..
  CountR-=1
  if(CountR==0) { polyNewR() polyPrepR()  if(polyR==polyL) {return()} }

  //Farbvariablen in nÑchste Zeile setzen
  RL += DeltaRL
  GL += DeltaGL
  BL += DeltaBL

  RR += DeltaRR
  GR += DeltaGR
  BR += DeltaBR

  //Fogging-Werte fÅr nÑchste Zeile
/*
  asm
  {
    plq mm0,uzl0
    plq mm1,duzl
    plq mm2,uzr0
    plq mm3,duzr
    plq mm4,zzl0
    plq mm5,dzzl
    plq mm6,FL
    plq mm7,DeltaFL

    paddd mm0,mm1
    paddd mm2,mm3
    paddd mm4,mm5
    paddd mm6,mm7

    psq mm0,uzl0
    psq mm2,uzr0
    psq mm4,zzl0
    psq mm6,FL
  }
*/
  uzl0+=duzl
  vzl0+=dvzl
  uzr0+=duzr
  vzr0+=dvzr
  zzl0+=dzzl
  zzr0+=dzzr
  FL  +=DeltaFL
  FR  +=DeltaFR
}


//Horizontale FÅllroutine -----------------------------------
void txhline(long x0,long x1)
{
  long *adr,*txr,i,DeltaX
  bitmap *Textur0
  int tempred,tempgreen,tempblue,tempalpha
  long tempfog,tempz

  long U1,V1
  long ZZL,ZZR
  long U,V,Z

  DeltaX = x1-x0+1

//  if(DeltaX<=0) {return()}

  adr = targetbitmap+32+((polyY*(*targetbitmap.ResX)+x0)*4)

  R0 = RL
  G0 = GL
  B0 = BL
  F0 = FL

  U0 = uzl0
  V0 = vzl0  
  DeltaU0 = (uzr0 - uzl0) / DeltaX
  DeltaV0 = (vzr0 - vzl0) / DeltaX

  Z0 = zzl0
  DeltaZ0 = (zzr0 - zzl0) / DeltaX

  DeltaR0 = (RR  -  RL) / DeltaX
  DeltaG0 = (GR  -  GL) / DeltaX
  DeltaB0 = (BR  -  BL) / DeltaX
  DeltaF0 = (FR  -  FL) / DeltaX

  Textur0 = *poly.T0

  asm {emms
       pxor mm7,mm7
      }

  for(i=0,i<DeltaX,i+=1)
  {
    Z = eins/(Z0>>16)
    U = (U0 * Z)>>16
    V = (V0 * Z)>>16

    txr=( (V * (*Textur0.ResX) + U) *4) + Textur0 + 32

    tempred   = R0>>16
    tempgreen = G0>>16
    tempblue  = B0>>16
    tempfog   = F0>>16

    asm 
    {
      pld       mm0,*txr
      punpcklbw mm0,mm7
      plq       mm1,tempred
//      pmullw    mm0,mm1
//      pshimw    mm0,6
      paddusw   mm0,mm1

      //Fogging-Wert in 3 KanÑle ausbreiten und auf Pixel addieren
      pld       mm2,tempfog
      punpcklbw mm2,mm2
      punpcklwd mm2,mm2

      packuswb  mm0,mm0
      psubusb   mm0,mm2
      pst       mm0,*adr
    }
    U0 += DeltaU0
    V0 += DeltaV0
    R0 += DeltaR0
    G0 += DeltaG0
    B0 += DeltaB0
    F0 += DeltaF0
    Z0 += DeltaZ0
    adr+= 4
  }
}



//Einfacher PolygonfÅller -----------------------------------
void polyfill()
{
  polySetup()

  while(polyL!=polyR)&&(XiL<=XiR)
  {
    txhline(XiL,XiR)

    polyNextLine()
  }
}



/*
void demopoly()
{
  edgestruct *edge
  long *adr,i

  //Randdaten des Polygone definieren!
  *poly.NPoints=3             ;
  *poly.Color=0xff8000        ;
  *poly.T0=ImageA
  edge=poly+12

  //Ecken inclusive der Schlu·ecke definieren!
  *edge.x     = 300  ;
  *edge.y     =  10  ;
  *edge.u0    = 255  ;
  *edge.v0    =   0  ;
  *edge.red   = 255  ;
  *edge.green =   0  ;
  *edge.blue  =   0  ;
  *edge.fog   =   0  ;
  edge+=64           ;  

  *edge.x     =  10  ;
  *edge.y     = 300  ;
  *edge.u0    =   0  ;
  *edge.v0    =   0  ;
  *edge.red   =   0  ;
  *edge.green = 255  ;
  *edge.blue  =   0  ;
  *edge.fog   =   0  ;
  edge+=64           ;

  *edge.x     = 210+x;
  *edge.y     = 400+y;
  *edge.u0    =   0  ;
  *edge.v0    = 255  ;
  *edge.red   =   0  ;
  *edge.green =   0  ;
  *edge.blue  = 255  ;
  *edge.fog   =   0  ;
  edge+=64           ;

  polyFinish(poly)
  polyfill()
//  polywire()
}
*/
//Ende POLY.INC