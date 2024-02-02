/*

  DATEIROUTINEN                             18.Januar 2019
                                              Alexander Graf
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
  extern long dtaseg,dtaofs
  extern long dtabase
  extern long dtahandle
  extern long currentharddrive


//DOS-DTA in EAX zurÅckgeben
far long getdta();

//Nullterminierten String in Realmem-String umwandeln
far long asciiz2string(short *asciiz);

//Aktuellen Pfad auf dem aktuellen GerÑt ermitteln
far long root();

//Setzt neuen Pfad auf dem aktuellen GerÑt, entweder relativ oder absolut
far short cd(string *Pfad);

//Erstellt Dateiliste in <files>
far long findfirst(string *mask,long attribute);

//Datei îffnen, 3dh=lesen, 3ch=neu erstellen
far short open(string *filename,long mode);

//Aktuelle Datei schlie·en
far void close();

//Setze Dateizeiger relativ zum Dateibeginn
far void seek(long position);

//Aus Datei lesen und Daten in Realmem hochladen
far void read(long *buffer,long size);

//Daten vom Realmem in Dos-Datei schreiben
far void write(long *buffer,long size);


//Ende FILES.H