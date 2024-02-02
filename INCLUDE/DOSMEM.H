/*
  640kB KONVENTIONELLER SPEICHER          14.Februar 2019
                                          Alexander Graf
*/

//Datenstruktur einzelner Speicherbereiche, im Unterverzeichnis liegen die einzelnen "<public> Far-calls" incl. ihrer Segmentadresse
struct segment
{
  long *next,*name,*prev,*interface
  long *base
  long user
}

//Globale Variablen zur Speicherreservierung unter DOS
extern segment *segments
extern long segcount

//RealmodeSpeicher reservieren -----------------------------
far long getseg();


//Anzahl freier Segmente ausz„hlen -------------------------
far long segfree();


//Ende DOSMEM.INC