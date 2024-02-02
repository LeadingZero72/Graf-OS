/*


  ZEICHENKETTEN UND TEXTFORMATIERUNG          16.Januar 2019
                                             Alexander Graf



  Wie erklÑre ich dem Assembler, was ein String ist und
  wie bringe ich sie im Code unter ?

  Die Programme brÑuchten folglich einen Header mit
  'Daten.

  Ich kann Strings allerdings auch aus Resourcen entnehmen,
  z.B. von Pladde.


  Die Stringroutinen Ñndern sich dadurch nicht.

  Die Struktur fÅr Strings enthÑlt nur den Header,
  die Daten sind explizit durch Zeigerverschiebung
  erreichbar.

*/

struct string
{
  long Length
}

//String in einen neuen String kopieren ---------------------
far long strc(string *quelle)
{
  string *ziel

  //Speicher fÅr Zielstring reservieren
  ziel = getmem(*quelle.Length+4)

  //String im Eiltempo kopieren
  copy(quelle,ziel,*quelle.Length+4)

  //Zeiger auf neuen String als Ergebnis zurÅckgeben
  return(ziel)
}

//Zwei Strings verbinden ------------------------------------
far long stra(string *erster,string *zweiter)
{
  string *ziel
  long summe

  //Die neue GesamtlÑnge ist die Summe der EinzellÑngen
  summe = (*erster.Length) + (*zweiter.Length)

  //Speicher fÅr Zielstring reservieren
  ziel = getmem(summe+4);
  *ziel.Length = summe

  //String1 im Eiltempo kopieren
  copy(erster+4,ziel+4,*erster.Length)

  //String2 hintendran kopieren
  copy(zweiter+4,ziel+4+(*erster.Length),*zweiter.Length)

  //Zeiger auf neuen String als Ergebnis zurÅckgeben
  return(ziel)
}

//Strings vergleichen ---------------------------------------
far short strcmp(string *quelle,string *vergleich)
{
  short *s1,*s2
  long  i,track

  track = *quelle.Length

  //Sind die LÑngen ungleich, dann wird das nichts mehr..
  if(track != (*vergleich.Length)) {return(0)}

  //Zeiger auf Daten vorbereiten
  s1 = quelle+4
  s2 = vergleich+4

  //Compare-Loop..kînnte noch optimiert werden
  for(i=0,i<track,i+=1)
  {
    //Bei der ersten Abweichung schon abbrechen
    if((*s1)!=(*s2)) {return(0)}
    s1+=1
    s2+=1
  }
  //Am Ende 'wahr' zurÅckgeben, so beide gleich sind
  return(1)
}


//Textformatierung durchfÅhren ------------------------------
far long strf(string *text,long *daten)
{
  string *temp,*ergebnis,*inserttext
  short *quelle,*ziel
  long  i,j
  long  wert,faktor
  short betrag
  long  inserttextlength

  //GenÅgend Speicher fÅr Text vorausreservieren
  temp = getmem(1000000)

  //Laufadressen in Quelle und Ziel vorbereiten
  quelle = text+4
  ziel   = temp+4

  //FÅr alle Quellzeichen
  i=0
  while(i<(*text.Length))
  {
    //'%' wird als Kennung fÅr die Formatsymbole benutzt
    if((*quelle)==37)
    {
      i+=1 quelle+=1














      //*****************************************************
      //*
      //*  '%g' Dezimalzahle Darstellung von DWORD-Zahlen
      //*
      //*****************************************************
      if((*quelle)==103)
      {
        i+=1 quelle+=1

        //NÑchstes Datenfeld lesen
        wert=*daten
        daten+=4

        //Vorzeichen beachten und mit anzeigen
        if(wert<0) { *ziel=45 ziel+=1 wert=-wert }

        //Anzahl Dezimalstellen ermitteln
        faktor=1000000000
        while(wert/faktor==0)
        {
          faktor/=10
          if(faktor==1) {break;}
        }

        //StellenbetrÑge ausdividieren und ins Ziel kopieren
        while(faktor>0)
        {
          //Stellenbetrag ausdividieren
          betrag = wert / faktor

          //Ascii-Wert speichern
          *ziel = betrag+48
          ziel+=1

          //Betrag von Wert abziehen
          wert -= faktor * betrag

          //Faktor auf nÑchstkleinere Stelle
          faktor/=10
        }

        continue;
      }





















      //*****************************************************
      //*
      //*  '%a' ZusÑtzlich Text einfÅgen
      //*
      //*****************************************************
      if((*quelle)==97)
      {
        i+=1 quelle+=1

        //NÑchstes Datenfeld als String kopieren
        inserttext=*daten
        inserttextlength=*inserttext.Length
        daten+=4

        if(inserttextlength!=0)
        {
          copy(inserttext+4,ziel,inserttextlength)
          ziel+=inserttextlength
        }

        continue;
      }








      //*****************************************************
      //*
      //*  '%l' BinÑre Darstellung von DWORD-Zahlen
      //*
      //*****************************************************
      if((*quelle)==108)
      {
        i+=1 quelle+=1

        //NÑchstes Datenfeld lesen
        wert=*daten
        daten+=4

        //Bits einzeln oben rausschieben und 0 oder 1 nehmen
        faktor=32
        while(faktor>0)
        {
          //MSB ganz nach unten schieben (0/1)
          betrag = wert>>31
 
          //Ascii-Wert speichern
          *ziel = betrag+48
          ziel+=1

          //nÑchstkleinere Stelle in Bit 31 hochschieben
          wert=wert<<1
          faktor=faktor-1
        }
        continue;
      }

















      //*****************************************************
      //*
      //*  '%x' Hex-Darstellung von DWORD-Zahlen
      //*
      //*****************************************************
      if((*quelle)==120)
      {
        i+=1 quelle+=1

        //NÑchstes Datenfeld lesen
        wert=*daten
        daten+=4

        //Nibbels einzeln oben rausschieben und 0-F nehmen
        faktor=8
        while(faktor!=0)
        {
          //Oberstes Nibble ganz nach unten schieben (0..15)
          j=(wert>>28)&15

          //Ascii-Wert speichern
          if(j<10) {*ziel=j+48}
          else     {*ziel=j+87}

          //nÑchstkleinere Stelle in Bit 31 hochschieben
          ziel+=1
          wert=wert<<4
          faktor=faktor-1
        }
        continue;
      }

























      //*****************************************************
      //*
      //*  '%i' Neue Zeile
      //*
      //*****************************************************
      if((*quelle)==105)
      {
        i+=1 quelle+=1;
        *ziel=0x0d
        ziel+=1;
        *ziel=0x0a
        ziel+=1
        continue;
      }




      //*****************************************************
      //*
      //*  '%w' Hochkomma einbauen
      //*
      //*****************************************************
      if((*quelle)==119)
      {
        i+=1 quelle+=1;
        *ziel=39
        ziel+=1;
        continue;
      }








      //*****************************************************
      //*
      //*  '%tx' Leerzeichen-Paare einfÅgen
      //*
      //*****************************************************
      if((*quelle)==116)
      {
        i+=2 quelle+=1; j=*quelle-48 quelle+=1
        while(j!=0)
        {
          *ziel=0x20
          ziel+=1;
          *ziel=0x20
          ziel+=1
          j-=1
        }
        continue;
      }
















    }

    //Normale Textzeichen werden einfach Åbernommen
    *ziel=*quelle
    quelle+=1
    ziel+=1
    i+=1
  }



  //TextlÑnge steht jetzt fest
  *temp.Length = ziel-temp-4

  //Anschlie·end den TemporÑren in den finalen String kopieren
  ergebnis = strc(temp)

  //TemporÑren Speicher freigeben
  free(temp)

  //Zeiger auf fertigen Text als Ergebnis zurÅckgeben
  return(ergebnis)
}


/*###########################################################

   DOKUMENTEN-VERARBEITUNG

   Es gibt ein FÅllhorn mit Daten, welche nacheinander
   auf einen Stack gelegt werden. Dieser Stack wird
   dann mit dem Textformatierer abgearbeitet.

 ###########################################################*/


//Zeiger auf Dokumentenarray
public long *ffptr
public string *fstring


//Format-Array fÅllen
far void ft(string *text,long value)
{
  string *t

  if(fstring==0) fstring=strc(text)
  else
  {
    t = fstring
    fstring = stra(fstring,text)
    free(t)
  }
  *ffptr=value;
  ffptr+=4;
}



//Format-Array fÅllen
far void fc(long value)
{
  *ffptr=value;
  ffptr+=4;
}

//Format-Array zurÅcksetzen
far void ff(long value)
{
  long *i
  i=4194304+63000;
  ffptr=i
  fc(value)
}

//Format-Array zurÅcksetzen
far void fb(string *text,long value)
{
  long *i
  i=4194304+63000;
  ffptr=i
  fstring=0
  ft(text,value)
}
//Ende STRINGS.INC