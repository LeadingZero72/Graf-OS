/*
  640kB KONVENTIONELLER SPEICHER          14.Februar 2019
                                          Alexander Graf


  Der konventionelle Speicher wird in 32kB-H„ppchen
  aufgeteilt, die in einer Liste arrangiert sind.

  Das Memory-Management ist hier denkbar einfach.
  Jeder Block ist entweder reserviert oder frei.
  Bei der Reservierung wird immer der n„chstbeste
  freie Block zurckgegeben.

  Um Programme nicht st„ndig neu von Platte laden
  zu mssen, gibt es fr jeden Block einen Shadow
  im hohen Speicher, so daá Programme > 640kB
  nebeneinander ausgefhrt werden k”nnen.

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


//Gesamten DOS-Speicher nach leeren Stellen (256-32768 Bytes) durchsuchen und diese als Bl„tter in der Dos-Liste anlegen
void initsegs()
{
  segment  *this
  long     *base
  long     size

  //Erste Node erstellen
  segments = node(0)
  segcount = 0
//48000?
  base     = 0x38000

  //Gesamten DOS-Speicher abtasten
  //655360 Bytes=640kB
  while(base<0x80000)
  {
    //Finde die Gr”áe des Blocks bis max 32768 Bytes (wegen den near-Sprngen^^)
    size=32768

    this=node(8) 

    //Bl”cke von mindestens 256 Bytes in die Liste nehmen
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

  //Kleinsten passenden Block ausw„hlen
  what = *segments.next
  while(what!=0)
  {
    if(*what.user==0) { *what.user=1 return(what) }
    what=*what.next
  }
  return(0)
}


//Anzahl freier Segmente ausz„hlen -------------------------
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