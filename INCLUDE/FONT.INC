/*

  GRAFIK-SCHRIFTARTEN                     14.Januar 2019
                                            Alexander Graf


  Die Glyphen werden VOR dem Zeichnen auf öberlappung
  getestet und der Cursor entsprechend korrigiert.

  Dadurch ist Text immer korrekt im Viewport eingeblendet.


  Die Glyphen sind unterschiedlich bemessen. So geht das 'g'
  eben etwas tiefer. Ich brauche also einen vernÅnftigen
  Rand zwischen den Zeilen. Es reicht nicht, einfach eine
  Hîhe tieferzusetzen, sondern da mu· noch ein Rand zwischen
  sein...



*/
struct fontstruct
{
  long *next
  long *name
  long *prev
  long *subnode
  long *Bitmap
}

struct fontchar
{
  int x0
  int y0
  int x1
  int y1
}

//Fontvariablen ---------------------------------------------
  public fontstruct *font,*fonts
  public bitmap     *fonttarget

  public long cursorx
  public long cursory
  public long fontsize
  public long fontspaceX
  public long fontspaceY
  public long fontheight
  public long fontborder
  public long fontoffset

//Cursor positionieren --------------------------------------
far void setcursor(long x,long y)
{
  cursorx = viewMinX + fontborder + x
  cursory = viewMinY + fontborder + y
}

//Cursor in neue Zeile setzen -------------------------------
far void newline()
{
  //Cursor zurÅck nach links setzen
  cursorx=viewMinX+fontborder

  //Cursor eine Zeile tiefer setzen
  cursory+=fontheight+fontspaceY
}

//Zeichen am Cursor einblenden ------------------------------
far void putc(long c)
{
  fontchar *f
  long a,b

  //Relative Fontadresse berechnen
  f = c*8+(font+20)

  //Nur weitermachen, wenn Glyph nicht leer
  if(*f.x1== 0)||(*f.y1== 0) {return()}
  if(*f.x0>255)||(*f.y0>255) {return()}
  if(*f.x1>255)||(*f.y1>255) {return()}

  //Breite der Abbildung im Zielbild
//  a = ((*f.x1) * fontsize) >> 16

  //Hîhe der Abbildung im Zielbild
//  b = ((*f.y1) * fontsize) >> 16

  b = fontsize
  a = (b*(*f.x1))/(*f.y1)

  if(a<  4)  {return()}
  if(b<  4)  {return()}
  if(a>255)  {return()}
  if(b>255)  {return()}

  //Wenn am rechten Bildrand angekommen
  if(cursorx+a+fontspaceX>=viewMaxX-fontborder) { return() }

  //Komplette Grafik-Abbildung hier in einem Schritt erledigen
  if(a!=0)&&(b!=0)
  {
    Blockcopy(*f.x0,*f.y0,*f.x1,*f.y1,*font.Bitmap,
              cursorx,cursory,a,b,fonttarget)
  }

  //Cursor nach rechts weiterbewegen
  cursorx+=a+fontspaceX
}


//Text am Cursor schreiben ----------------------------------
far void puts(string *quelle)
{
  long i
  short *adr

  //Laufadresse der Daten vorbereiten
  adr=quelle+4

  //Alle Textzeichen einzeln nacheinander anlegen
  i=0
  while(i<(*quelle.Length))
  {
    if(*adr==0x0d)
    {
      adr+=2
      i+=2
      newline()
      continue;
    }

    putc(*adr)
    adr+=1
    i+=1


    if(cursory+fontsize+fontspaceY>viewMaxY) {break;}
  }
}


/************************************************************
 *                   LADE FONT-DATEI(EN)                    *
 ************************************************************/
far long loadfont(string *filename,string *fontname)
{
  fontstruct *temp
  long       *buf

  //Neue Font-Node anlegen und registrieren
  temp = node(2052) place(temp,fonts)

  //Fontnamen speichern
  *temp.name = strc(fontname)


  //Dateiendung .FON fÅr die Fontdaten
  fsetext(filename,'.FON')

  //Jetzt die Fontdaten laden und in den Font kopieren
  buf = fread(filename) copy(buf,temp+20,2048) free(buf)



  //Dateiendung .BMP fÅr das Bitmap
  fsetext(filename,'.BMP')

  //Bitmap einzeln aus der Datei laden
  *temp.Bitmap = fLoadBMP(filename)

  font       = temp
/*
  fontsize   = 8
  fontspaceX = 0
  fontspaceY = 0
  fontheight = 8
  fontborder = 2
  fonttarget = targetbitmap
*/
  return(temp)
}



//Ende FONT.INC