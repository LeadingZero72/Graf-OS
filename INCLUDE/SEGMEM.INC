/*
  640kB KONVENTIONELLER SPEICHER          14.Februar 2019
                                          Alexander Graf


  Memory Map:

  0x00000000 - 0x000003ff 1   KiB   Interrupts
  0x00000400 - 0x000004ff 256 Byte  Bios Data Area
  0x00000500 - 0x00007bff <30 KiB   frei
  0x00007c00 - 0x00007fff 512 Byte  OS-Bootloader
  0x00007e00 - 0x0007ffff 480,5 KiB frei
  0x00080000 - 0x0009ffff 128 KiB   Extended Bios Data Area
  0x000a0000 - 0x000fffff 384 KiB   verschiedenes (Videoram/Cassette-ROM)



  Der konventionelle Speicher wird in 32kB-HÑppchen
  aufgeteilt, die in einer Liste arrangiert sind.

  Das Memory-Management ist hier denkbar einfach.
  Jeder Block ist entweder reserviert oder frei.
  Bei der Reservierung wird immer der nÑchstbeste
  freie Block zurÅckgegeben.

  Um Programme nicht stÑndig neu von Platte laden
  zu mÅssen, gibt es fÅr jeden Block einen Shadow
  im hohen Speicher, so da· Programme > 640kB
  nebeneinander ausgefÅhrt werden kînnen.


  Scheinbar kann ich den Bereich zwischen 28000h bis 38000h
  nicht nutzen...wo landet denn KERNEL.BIN ? Bei 30000h !
  Gerade nachgesehen, es ist 30000h!

  Der Bereich zwischen 28000-30000h kann aber ebenfalls noch
  nicht genutzt werden, warum auch immer .-(


*/

//Datenstruktur einzelner Speicherbereiche, im Unterverzeichnis liegen die einzelnen "<public> Far-calls" incl. ihrer Segmentadresse
struct segment
{
  long *next,*name,*prev,*interface
  long *base
  long user
}

//Globale Variablen zur Speicherreservierung unter DOS
public segment *segments
public long segcount


//Gesamten DOS-Speicher nach leeren Stellen (256-32768 Bytes) durchsuchen und diese als BlÑtter in der Dos-Liste anlegen
void initsegs()
{
  segment  *this
  long     *base
  long     size

  //Erste Node erstellen
  segments = node(0)
  segcount = 0
  size     = 32768
  base     = 0x8000

//  while(base<0x28000)

  while(base<0x30000)
  {
    this=node(8) 

    //Blîcke von mindestens 256 Bytes in die Liste nehmen
    *this.base     = base; blast(base,size,0);
    *this.user     = 0;

    place(this,segments)

    base     += size
    segcount += 1
  }



  base     = 0x38000



  //Gesamten DOS-Speicher abtasten (bis zum EBDA-Bereich, welcher an 0x80000 beginnen kann und aber definitiv bei 0xa0000 endet)
  //655360 Bytes=640kB
  while(base<0x80000)
  {
    this=node(8) 

    //Blîcke von mindestens 256 Bytes in die Liste nehmen
    *this.base     = base; blast(base,size,0);
    *this.user     = 0;

    place(this,segments)

    base     += size
    segcount += 1
  }
}


//RealmodeSpeicher reservieren -----------------------------
far long getseg()
{
  segment *what

  //Kleinsten passenden Block auswÑhlen
  what = *segments.next
  while(what!=0)
  {
    if(*what.user==0) { *what.user=1 return(what) }
    what=*what.next
  }
  return(0)
}


//Anzahl freier Segmente auszÑhlen -------------------------
far long segfree()
{
  segment *s
  long     i

  s = *segments.next i=0

  while(s!=0)
  {
    if(*s.user==0) { i+=1 }
    s = *s.next
  }
  return(i)
}
//Ende DOSMEM.INC