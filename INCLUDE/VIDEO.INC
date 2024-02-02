/*

   FRAMEBUFFER                                15.Januar 2019
                                               Alexander Graf
*/
public bitmap *frame1
public bitmap *frame2
public bitmap *frame3
public bitmap *frame4
public bitmap *frame5
public bitmap *frame6
public bitmap *frame7
public bitmap *frame8
public bitmap *zbuffer
public bitmap *videotarget


public long *videoram

public long videox,videoy,videosize
public int  videopage



//###########################################################
//      Praktische Routine, um neue Bitmaps zu erzeugen
//###########################################################
far long newbitmap(long resx,long resy)
{
  bitmap *b
/*
  b = getmem(resx*resy*4+32);

  *b.next    = 0;
  *b.name    = 0;
  *b.prev    = 0;
  *b.subnode = 0;
*/
  b = node(resx*resy*4+16);

  *b.ResX = resx;
  *b.ResY = resy;

  return(b)
}



//###########################################################
//      Praktische Routine, um Bitmaps auszuf„rben
//###########################################################
far long clearbitmap(bitmap *this,long colr)
{
  blast(this+32,(*this.ResX) * (*this.ResY) * 4,colr)
}



void setresolution(long x,long y)
{
  videox=x
  videoy=y
  videosize=x*y*4
  ;
  *frame1.ResX=x;
  *frame1.ResY=y;

  *frame2.ResX=x;
  *frame2.ResY=y;

  *frame3.ResX=x;
  *frame3.ResY=y;

  *frame4.ResX=x;
  *frame4.ResY=y;

  *frame5.ResX=x;
  *frame5.ResY=y;

  *frame6.ResX=x;
  *frame6.ResY=y;

  *frame7.ResX=x;
  *frame7.ResY=y;

  *frame8.ResX=x;
  *frame8.ResY=y;

  *zbuffer.ResX=x;
  *zbuffer.ResY=y;
}


void framebuffers()
{
  //Framebuffers erzeugen ------------------------------
  frame1  = newbitmap(1024,768)
  frame2  = newbitmap(1024,768)
  frame3  = newbitmap(1024,768)
  frame4  = newbitmap(1024,768)
  frame5  = newbitmap(1024,768)
  frame6  = newbitmap(1024,768)
  frame7  = newbitmap(1024,768)
  frame8  = newbitmap(1024,768)
  zbuffer = newbitmap(1024,768)

  setresolution(videox,videoy)
}

//Display-Start auf andere Page setzen
void videoflip() { asm
{
        mov  ax,0x4f07
        xor  bx,bx
        xor  cx,cx
        mov  dx,videopage
        intr 0x10
        mov  ax,1024
        xor  videopage,ax
}}


struct vesamodestruct
{
  int Attributes
  short WinAAttributes,WinBAttributes
  int   WinGranularity,WinSize,WinASegment,WinBSegment
  long  WinFuncPtr
  int   BpL,ResX,ResY
  short CharX,CharY
  short Planes
  short Bpp
  short Banks,MemoryModel,BankSize,BankCount,rsvd1
  short rms,rmp,gms,gmp,bms,bmp,ams,amp  
  short DirectColorModeInfo
  long  PhysBasePtr
  long  OffScreenMemOffset
  int   OffScreenMemSize
}


far void videomode(long ResX,long ResY)
{
  segment        *temp
  long            BytesPerLine
  vesamodestruct *Mode
  long            ModeSeg

  BytesPerLine = ResX<<2

  temp = getseg()

  Mode = *temp.base

  ModeSeg = Mode>>4

  asm
  {
//®ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¯
//  Suche nach dem passenden Videomode
//®ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¯
        mov ax,ModeSeg                  //ES fr INT 10 initialisieren
        push ax                         //
        poes                            //
        mov cx,0x100                    //Annahme: Kein passender Grafikmodus!

lModes: mov ax,0x4f01                   //Funktion 4f01: Modus-Information
        xor di,di                       //Ziel-Infopuffer setzen
        cmp cx,0x200                    //Ende der Modeliste testen
        ja  ModeErr                     //Evtl. alle Modes getestet..
        intr 0x10                       //Infos holen bers Bios.
        add cx,1                        //N„chste Modusnummer setzen
//        cmp ax,4fh                      //AX = 4f ?
//        jne lModes                      //Wenn Modenummer unbekannt..
        mov ax,*Mode.BpL                //Bytes per Scanline laden
        cmp ax,BytesPerLine             //Farbtiefe testen
        jne lModes                      //Wenn Farbtiefe falsch
        mov ax,*Mode.ResX               //Danach die Test-Aufl”sung laden
        mov dx,*Mode.ResY               //X und Y.
        cmp ax,ResX                     //Nur sofern der Mode mit
        jne lModes                      //den angegebenen Abmessungen
        cmp dx,ResY                     //bereinstimmt
        jne lModes                      //ist der Mode ok..  (min. 9*n Clock)

//®ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¯
//  Mode fertig ausgew„hlt, Modusnummer in SI.
//®ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¯
        mov videox,ax                 //Gew„hlte Aufl”sung speichern
        mov videoy,dx                 //

        mov eax,*Mode.PhysBasePtr     //Frontbuffer-Startadresse laden
        mov videoram,eax              //

        mov ax,0x4f02                 //Das wird vom VESA-Bios
        mov bx,cx                     //BX=Mode
        sub bx,1                      //Modusnummer korrigieren
        or bx,0x4000                  //Linear Framebuffer 
        intr 0x10                     //VBE 2.0
ModeErr:
  }
  *temp.user=0
}

//Ende VIDEO.INC