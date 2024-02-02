/*

  DATEIROUTINEN                             18.Januar 2019
                                              Alexander Graf

  Darauf habe ich lange hingearbeitet oO

  Mit den String-Routinen im GepÑck sollte es kein Problem
  mehr sein, die INT21h-Funktionen des DOS fÅr Dateien zu
  benutzen.

  Hauptsache der INT-Befehl wird richtig Åbersetzt. Der
  Rest sollte laufen^^

  Wichtig ist, da· ich einen DTA finde, den ich als festen
  Puffer fÅr die realmem-Routinen finde,lol


  FACTS of LIFE

  - Die Funktion zum holen des aktuellen Pfades braucht
    die Laufwerksnumm+1, sonst wird das nix. Die DTA kann
    hier als Stringpuffer verwendet werden.


  - Die Funktion Findfirst akzeptiert auch die DTA als
    Eingangs-Stringpuffer, d.h. man kann seine Suchmaske
    in die DTA schreiben und bekommt das Ergebnis in der
    DTA.
*/

//Struktur fÅr einzelne Noden in einem Dateisystemabbild.
struct fileattributes
{
  long *next
  long *name
  long *prev,*subnode
  long  size
  long  icon
  long  radix
  long  posx
  long  posy
  long  select
  short attributes
}


  //Globale Variablen fÅr das Dateiwerkzeug
  public long dtaseg,dtaofs
  public long dtabase
  public long dtahandle
  public long currentharddrive
  public fileattributes *files


//DOS-DTA in EAX zurÅckgeben
far long getdta() { asm
{
        mov ah,0x2f
        intr 0x21
        pues
        pop ax
        mov dtaofs,bx
        mov dtaseg,ax
        shl eax,4
        movzx ebx,bx
        add eax,ebx
        mov dtabase,eax
}}



//Nullterminierten String in Realmem-String umwandeln
far long asciiz2string(short *asciiz)
{
  string *this
  short *chr
  long count

  //Erst alle Zeichen bis zur abschlie·enden Null zÑhlen
  chr   = asciiz
  count = 0
  while(*chr!=0) { chr+=1 count+=1 }

  //Stringspeicher reservieren und aus DTA kopieren
  this         = getmem(count+4);
  *this.Length = count

  copy(asciiz,this+4,count)

  return(this)
}


//Aktuellen Pfad auf dem aktuellen GerÑt ermitteln
far long root()
{
  string *this
  short  *chr

  this=0 blast(dtabase,128,0)

  asm
  {
        puds
        push si


        mov ah,0x19
        intr 0x21
        mov dl,al
        add dl,1
        mov currentharddrive,al
        mov ax,dtaseg
        mov si,dtaofs
        add si,3

        push ax
        pods
        mov ah,0x47
        intr 0x21
        pop si
        pods
        jc noroot
  }
  //Laufwerksnamen, Doppelpunkt und Backslash einbauen
  chr=dtabase;
  *chr=currentharddrive+65 chr+=1;
  *chr=0x3a                chr+=1;
  *chr=0x5c


  //Pfad in Realmemstring kopieren
  this = asciiz2string(dtabase)

  asm
  {
noroot:
  }
  return(this)
}



//Setzt neuen Pfad auf dem aktuellen GerÑt, entweder relativ oder absolut
far short cd(string *Pfad)
{
  short *chr
  long pos
  short driveletter

  //Kommt ein Doppelpunkt drin vor, dann mu· auch das GerÑt gewechselt werden. Der Doppelpunkt und das folgende Backslash werden Åbersprungen
  pos = 0
  chr = Pfad+5 if(*chr==0x3a)
  {
    pos=2
    chr=Pfad+4 driveletter=*chr-65
    asm
    {
      mov dl,driveletter
      mov ah,0x0e
      intr 0x21
    }
  }

  //Suchmaske in DOS-String kopieren
  copy(Pfad+4+pos,dtabase,*Pfad.Length-pos)

  //Dateimaske null-terminieren
  chr = *Pfad.Length+dtabase-pos; *chr=0

  asm
  {
        puds
        mov ax,dtaseg
        mov dx,dtaofs
        push ax
        pods
        mov ah,0x3b
        intr 0x21
        mov al,1
        jc cdnokay
        mov al,0
cdnokay:pods
  }
}


/*##########################################################

  FIND-FIRST

  DS:DX mu· auf die DTA fÅr die Dateiattribute zeigen, die
  aber nur Åber DOS selbst reserviert werden kann, sonst
  klappt es nicht. Das Åblichen DTA-Segment bekommt man
  Åber <int21h,ah=2fh> in ES:BX zurÅckgeliefert.

  ########################################################*/
far long findfirst(string *mask,long attribute)
{
  string *searchstring
  short *chr
  long  *size
  fileattributes *this,*files


  //Annahme, da· keine Datei zu finden
  files=0

  //Suchmaske in (null-terminierten) DOS-String kopieren
  copy(mask+4,dtabase,*mask.Length)
  chr=*mask.Length+dtabase; *chr=0

  //Findfirst Åber Interrupt 21h aufrufen
  asm
  {
        puds
        mov ecx,attribute
        mov ax,dtaseg
        mov dx,dtaofs
        push ax
        pods
        mov ah,0x4e
        intr 0x21
        pods
        jc nofirst
  }

  //Bei Erfolg, neue Dateiliste anlegen
  files = node(64)
  this=files

  asm
  {
    findnxt:
  }


//  size=dtabase+0x1e
//  if(*size&0xffff==0x002e) 


  //Dateinamen-Space ins Reg-Feld generieren und aus DTA kopieren
  *this.name      = asciiz2string(dtabase+0x1e);


  //Dateiendung und Anfangsbuchstaben fÅr radix-sort mischen


  //Dateigrî·e festhalten
  size            = dtabase + 0x1a;
  *this.size      = *size

  //Dateiattribute festhalten
  chr             = dtabase + 0x15;
  *this.attributes= *chr

  //Bisher noch keine Desktopverwendung ersichtlich, also Icon=Null
  *this.icon      = 0

  //Bisher noch keine Desktopverwendung ersichtlich, also Sortierkriterium=Null
  *this.radix = 0



  //Weiteres Element finden
  asm
  {
       mov ah,0x4f
       intr 0x21
       jc nofirst
  }

  //Neue Node erzeugen und Liste verlÑngern
  this=node(64) place(this,files)

  asm
  {
    jmp findnxt
    nofirst:
  }
  return(files)
}


//Datei îffnen, 3dh=lesen, 3ch=neu erstellen
far short open(string *filename,long mode)
{
  short *chr

  //Dateinamen in null-terminierten DOS-String kopieren
  copy(filename+4,dtabase,*filename.Length)
  chr = *filename.Length+dtabase; *chr=0
  asm
  {
        puds
        mov ah,mode
        mov cx,dtaseg
        mov dx,dtaofs
        push cx
        pods
        xor cx,cx
//        xor al,al
        mov al,2

        intr 0x21
        jnc openkay
        xor ax,ax
openkay:pods
        mov dtahandle,ax
  }
}

//Aktuelle Datei schlie·en
far void close() { asm
{
        mov bx,dtahandle
        mov ah,0x3e
        intr 0x21
}}


//Setze Dateizeiger relativ zum Dateibeginn
far void seek(long position) { asm
{
        mov ecx,position
        mov dx,cx
        shr ecx,16
        mov bx,dtahandle
        mov ax,0x4200
        intr 0x21
}}



//Aus Datei lesen und Daten in Realmem hochladen
far void read(long *buffer,long size)
{
  long dtasize

  dtasize = 128

  while(size>0)
  {
    if(size<dtasize) dtasize=size
    asm
    {
        puds
        mov ah,0x3f
        mov bx,dtahandle
        mov cx,dtasize
        mov di,dtaseg
        mov dx,dtaofs
        push di
        pods
        intr 0x21
        pods
        jc readnkay
    }
    copy(dtabase,buffer,dtasize)

    buffer+=dtasize
    size-=dtasize
  }
  asm
  {
readnkay:
  }
}

//Daten aus Realmem in Datei schreiben
far void write(long *buffer,long size)
{
  long dtasize

  dtasize = 128

  while(size>0)
  {
    if(size<dtasize) dtasize=size
    copy(buffer,dtabase,dtasize)
    asm
    {
        puds
        mov ah,0x40
        mov bx,dtahandle
        mov cx,dtasize
        mov di,dtaseg
        mov dx,dtaofs
        push di
        pods
        intr 0x21
        pods
        jc writenkay
    }
    buffer+=dtasize
    size-=dtasize
  }
  asm
  {
writenkay:
  }
}




//Ende FILES.INC