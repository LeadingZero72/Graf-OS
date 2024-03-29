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

//Textformatierung durchf�hren ------------------------------
far long strf(string *text,long *daten);

//Format-Array f�llen
far void ft(string *text,long value);

//Format-Array f�llen
far void fc(long value);

//Format-Array zur�cksetzen
far void ff(long value);

//Format-Array zur�cksetzen
far void fb(string *text,long value);

//Ende STRINGS.H