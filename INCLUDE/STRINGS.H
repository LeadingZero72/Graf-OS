/*


  ZEICHENKETTEN UND TEXTFORMATIERUNG          16.Januar 2019
                                             Alexander Graf

*/

struct string
{
  long Length
}

//Zeiger auf Dokumentenarray
extern long *ffptr
extern string *fstring




//String in einen neuen String kopieren ---------------------
far long strc(string *quelle);

//Zwei Strings verbinden ------------------------------------
far long stra(string *erster,string *zweiter);

//Strings vergleichen ---------------------------------------
far short strcmp(string *quelle,string *vergleich);

//Textformatierung durchfÅhren ------------------------------
far long strf(string *text,long *daten);

//Format-Array fÅllen
far void ft(string *text,long value);

//Format-Array fÅllen
far void fc(long value);

//Format-Array zurÅcksetzen
far void ff(long value);

//Format-Array zurÅcksetzen
far void fb(string *text,long value);

//Ende STRINGS.H