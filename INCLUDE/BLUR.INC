/*

  BLUR.INC                                  24.September 2019
                                               Alexander Graf



  VERTIKALES REALTIME IMAGE-BLUR                                     2.August 2019
                                                                   Alexander Graf

  Angenommen es sollen immer 8 Pixel geblurrt werden.

  Zuerst werden 7 Pixel addiert, Pixel 0..6, um eine
  Zwischensumme zu bekommen.

  Jetzt beginnt die Hauptschleife.

  Hier wird nun das 8. Pixel hinzuaddiert, der Blurmittelwert
  berechnet (shr 3) und anschlie·end das erste Pixel wieder
  abgezogen, um wieder zu 7 Pixeln zu kommen.

  WÑhrend der Hauptschleife wird nun immer nur das 8. Pixel
  hinzuaddiert und nach der Pixelberechnung das erste wieder
  abgezogen.

  Ich brauche Quell und Zielpointer.

  Der Quellpointer beginnt bei Pixel 0, der Zielpointer bei
  Pixel 4.

  Wenn die Hauptschleife beginnt, befindet der Quellzeiger
  sich bereits bei Pixel 8, aber nur 7 Pixel in der Zwischen-
  summe.

  Es erscheint etwas unpraktisch, aber um das erste Pixel
  wieder aus der Zwischensumme zu entfernen, wird ein weiterer
  Quellzeiger benîtigt, der quasi immer langsam hinterherlÑuft.

  Dieser beginnt bei Pixel 0, sobald die Hauptschleife startet.


  Der Rand macht Probleme.

  Ich kann die Gewichte auf einzelne Variablen verteilen,
  z.B. G0..G7. Damit wird dann multipliziert und durch 256
  geteilt.

  Ich kann fÅr diese Variablen auch ein Array erstellen und
  dann in einer Schleife den Index weitersetzen. So gefÑllt
  es mir zumindest am Besten.

  Das fÅhrt natÅrlich zu mehr BÅrokratie. Das Array mu·
  dynamisch reserviert, mit Werten belegt und dann am Ende
  wieder freigegeben werden.

*/

//Array mit Blur-Gewichten fÅr die Randregion vorberechnen
public int *blurweights

void blurgetweights()
{
  int i,j,k,*g

  blurweights = getmem(512) //blast(blurweights,512,0)

  g = blurweights i=1

  for(j=0,j<64,j+=1)
  {
    k=16384 / i;
    *g = k g+=2;
    *g = k g+=2;
    *g = k g+=2;
    *g = 0 g+=2;
    i+=1
  }
}



far void vblur(bitmap *source,bitmap *dest,long blur)
{
  long *a,*b,*c
  long *sourceline,*destline
  long i,j
  long lineskip,todraw

  int  *g

  long blurhalf,blurhalfpred,bluroffset

  blurhalf     = blur>>1
  blurhalfpred = blurhalf - 1
  bluroffset   = (blurhalfpred << 3) + blurweights

  //Sprung zwischen zwei Zeilen (vertikal von oben nach unten)
  lineskip = (*source.ResX * 4)

  //Anzahl Zeilen in der Hauptschleife
  todraw   = (*source.ResY - blur)

  //Beginn der Bilddaten in Quelle und Ziel
  sourceline = source + 32
  destline   = dest   + 32

  //MM7 fÅr Byte-Extensions klarmachen
  asm
  {
    emms
    pxor mm7,mm7
  }

  //FöR ALLE VERTIKALEN SPALTEN ---------------------------------------
  for(j=*source.ResX,j>0,j-=1)
  {
    //Hier werden die ersten 3 Randpixel von oben summiert (BLUR/2)
    asm
    {
      pxor mm1,mm1
    }

    //Zeiger auf Pixel0 in A
    a = sourceline

    //Zeiger auf Zielpixel in C
    c = destline


/*
  Zuerst mÅssen 7 Randpixel summiert werden.

  In einer Schleife, welche die Zwischensumme vergrî·ert,
  werden dann die 8 Randpunkte gezeichnet, so da· am Ende
  15 Punkte in der Zwischensumme sind. Danach folgt dann
  die Hauptschleife. Das besondere an den Randpunkten ist,
  da· sie unterschiedlich gewichtet sind und da· von der
  Zwischensumme keine Punkte wieder abgezogen werden.


  Die Gewichte sind 16384 / x, das sind 14 Bits.
  FÅr jedes Pixel wird ein anderer Wert benutzt, der eben
  nicht durch 8 oder 16 teilt, sondern durch einen dazwischen
  liegenden Wert. Weil sich das schlecht dividieren lÑ·t,
  wird mit dem Kehrwert multipliziert, wobei viel zu viele
  Bits herauskommen. Es werden also erstmal Farbbits eliminiert,
  um öberlÑufe zu verhindern. Anschlie·end wird nochmals
  gekÅrzt, um die Multiplikation mit dem Kehrwert zu vollenden.
  Anschlie·end sollte alles okay sein, d.h. der Farbmittelwert
  als Pixel gespeichert werden kînnen.

  FÅr das erste Randpixel wurden 4 Pixel summiert, also wird
  durch 4 geteilt, bzw. mit 1/4 malgenommen.

  Die Gewichte werden also von Index 4 gebraucht und nicht
  von Index 0 !


  Wenn ich die Gewichte zu weit setze, wird mit kleineren
  Zahlen multipliziert, also mÅ·te ein dunkleres Bild entstehen...

  

*/

    //BLUR/2-1 Pixel in Zwischensumme voraddieren, das erste Randpixel hat dann schonmal einen Wert
    b = a
    g = bluroffset //(( (blur>>1) -1)<<3)
    asm
    {
        mov cx,blurhalfpred
lvsum:
        pld mm0,*b
        punpcklbw mm0,mm7
        paddusw mm1,mm0
        mov eax,lineskip
        add b,eax
        dec cx
        jne lvsum

        mov cx,blurhalf
lvr1:
        pld mm0,*b                    //Pixel7 zu Summe addieren
        punpcklbw mm0,mm7
        paddusw mm1,mm0
        pmov mm4,mm1                  //Summe in Zwischenregister kopieren 

        plq mm5,*g
        pshimw mm4,6                  //Farben vorweg etwas raus
        pmullw mm4,mm5
        pshimw mm4,8                  //Mittelwert korrigiert berechnen
        packuswb mm4,mm4              //RGB's wieder zusammenfassen
        pst mm4,*c                    //Fertiges Pixel schreiben

        mov eax,lineskip              //Datenpointer eine Zeile tiefersetzen
        add b,eax
        add c,eax
        add g,8
        dec cx
        jne lvr1
    }

    //Schleife fÅr den Hauptteil in der Mitte -----------------------------------
    asm
    {
        mov cx,todraw
        plq mm5,*g
lvblur:
        pld mm0,*b                    //Pixel7 zu Summe addieren
        punpcklbw mm0,mm7
        paddusw mm1,mm0
        pmov mm4,mm1                  //Summe in Zwischenregister kopieren 


        pshimw mm4,6                  //Farben vorweg etwas raus
        pmullw mm4,mm5
        pshimw mm4,8                  //Mittelwert korrigiert berechnen

        packuswb mm4,mm4              //RGB's wieder zusammenfassen
        pst mm4,*c                    //Fertiges Pixel schreiben

        pld mm0,*a                    //Pixel0 wieder von Summe abziehen
        punpcklbw mm0,mm7
        psubusw mm1,mm0

        mov eax,lineskip              //Datenpointer eine Zeile tiefersetzen
        add a,eax
        add b,eax
        add c,eax
        dec cx
        jne lvblur
    }


    /*
      DER UNTERE RAND

      Der untere Rand beginnt mit einer Zwischensumme, die
      bereits 7 Pixel enthÑlt. Da die 'gedachte Box' nun aber
      schon aus dem Bild herauslÑuft, in einen Bereich wo
      keine Daten mehr zur VerfÅgung stehen, werden keine
      neuen Pixel zur Zwischensumme hinzuaddiert, sondern immer
      ein altes Pixel heraussubtrahiert, so da· fÅr das letzte
      Pixel in der Reihe nur noch 8 Pixel in der Summe sind.
      Auch hier werden wieder unterschiedliche Gewichte benîtigt,
      die zum oberen Rand entgegengesetzt verlaufen. Man kann
      also vom aktuellen 'g' ausgehend immer 8 Bytes zurÅck
      setzen und bekommt automatisch die richtigen Gewichte.

      Was jetzt schwierig ist, ist das richtige Pixel aus der
      Zwischensumme herauszusubtrahieren.

      Wenn meine Vorstellung mich nicht trÅbt, ist es die
      gleiche Laufvariable, die vorher schon in der Hauptschleife
      zum heraussubtrahieren benutzt wurde, so da· ich sie
      einfach weiter mitlaufen lassen kann. Die Variable zum
      hinzuaddieren wird aber hinfÑllig und mu· nicht weiter
      mitlaufen.
    */
    asm
    {
        mov cx,blurhalf
lvr2:
        sub g,8                       //Da G bereits auf den folgenden Eintrag zeigt, mu· G 'zuerst' wieder zurÅckbewegt werden und nicht am Ende der Schleife!
        pmov mm4,mm1                  //Summe in Zwischenregister kopieren 

        plq mm5,*g
        pshimw mm4,6                  //Farben vorweg etwas raus
        pmullw mm4,mm5
        pshimw mm4,8                  //Mittelwert korrigiert berechnen
        packuswb mm4,mm4              //RGB's wieder zusammenfassen
        pst mm4,*c                    //Fertiges Pixel schreiben

        pld mm0,*a                    //Pixel0 von Summe abziehen
        punpcklbw mm0,mm7
        psubusw mm1,mm0

        mov eax,lineskip              //Datenpointer eine Zeile tiefersetzen
        add a,eax
        add c,eax
        dec cx
        jne lvr2
    }



    //Zeiger jetzt eine Spalte nach rechts bewegen
    sourceline += 4
    destline   += 4
  }
}


far void hblur(bitmap *source,bitmap *dest,long blur)
{
  long *a,*b,*c
  long *sourceline,*destline
  long i,j
  long lineskip,todraw
  int  *g
  long blurhalf,blurhalfpred,bluroffset

  //Hilfsvariablen vorberechnen
  blurhalf     = blur>>1
  blurhalfpred = blurhalf - 1
  bluroffset   = (blurhalfpred << 3) + blurweights

  //Sprung zwischen zwei Zeilen (vertikal von oben nach unten)
  lineskip = (*source.ResX * 4)

  //Anzahl Zeilen in der Hauptschleife
  todraw   = (*source.ResX - blur)

  //Beginn der Bilddaten in Quelle und Ziel
  sourceline = source + 32
  destline   = dest   + 32

  //MM7 fÅr Byte-Extensions klarmachen
  asm
  {
    emms
    pxor mm7,mm7
  }


  //FöR ALLE VERTIKALEN SPALTEN ---------------------------------------
  for(j=*source.ResY,j>0,j-=1)
  {
    //Hier werden die ersten 3 Randpixel von oben summiert (BLUR/2)
    asm
    {
      pxor mm1,mm1
    }

    //Zeiger auf Pixel0 in A
    a = sourceline

    //Zeiger auf Zielpixel in C
    c = destline

    //BLUR-1 Pixel in Zwischensumme addieren
    b = a
    g = bluroffset //blurweights + (( (16>>1) -1)<<3)
    asm
    {
        mov cx,blurhalfpred
lhsum:
        pld mm0,*b
        punpcklbw mm0,mm7
        paddusw mm1,mm0
        add b,4
        dec cx
        jne lhsum

        mov cx,blurhalf
lhr1:
        pld mm0,*b                    //Pixel7 zu Summe addieren
        punpcklbw mm0,mm7
        paddusw mm1,mm0
        pmov mm4,mm1                  //Summe in Zwischenregister kopieren 

        plq mm5,*g
        pshimw mm4,6                  //Farben vorweg etwas raus
        pmullw mm4,mm5
        pshimw mm4,8                  //Mittelwert korrigiert berechnen
        packuswb mm4,mm4              //RGB's wieder zusammenfassen
        pst mm4,*c                    //Fertiges Pixel schreiben

        add b,4                       //Datenpointer eine Zeile tiefersetzen
        add c,4
        add g,8
        dec cx
        jne lhr1
    }


    //Schleife fÅr den Hauptteil in der Mitte -----------------------------------
    asm
    {
        mov cx,todraw
        plq mm5,*g
lhblur:
        pld mm0,*b                    //Pixel7 zu Summe addieren
        punpcklbw mm0,mm7
        paddusw mm1,mm0
        pmov mm4,mm1                  //Summe in Zwischenregister kopieren 

        pshimw mm4,6                  //Farben vorweg etwas raus
        pmullw mm4,mm5
        pshimw mm4,8                  //Mittelwert korrigiert berechnen

        packuswb mm4,mm4              //RGB's wieder zusammenfassen
        pst mm4,*c                    //Fertiges Pixel schreiben
        pld mm0,*a                    //Pixel0 wieder von Summe abziehen
        punpcklbw mm0,mm7
        psubusw mm1,mm0

        mov eax,4                     //Datenpointer ein Pixel weitersetzen
        add a,eax
        add b,eax
        add c,eax
        dec cx
        jne lhblur
    }


    asm
    {
        mov cx,blurhalf
lhr2:
        sub g,8                       //Da G bereits auf den folgenden Eintrag zeigt, mu· G 'zuerst' wieder zurÅckbewegt werden und nicht am Ende der Schleife!
        pmov mm4,mm1                  //Summe in Zwischenregister kopieren 

        plq mm5,*g
        pshimw mm4,6                  //Farben vorweg etwas raus
        pmullw mm4,mm5
        pshimw mm4,8                  //Mittelwert korrigiert berechnen
        packuswb mm4,mm4              //RGB's wieder zusammenfassen
        pst mm4,*c                    //Fertiges Pixel schreiben

        pld mm0,*a                    //Pixel0 von Summe abziehen
        punpcklbw mm0,mm7
        psubusw mm1,mm0

        add a,4                       //Datenpointer eine Zeile tiefersetzen
        add c,4
        dec cx
        jne lhr2
    }



    //Zeiger jetzt eine Spalte nach rechts bewegen
    sourceline += lineskip
    destline   += lineskip
  }
}



far void mblur(bitmap *new,bitmap *old,bitmap *dest,long newmix,long oldmix) { asm
{
        //Pixelmenge ausrechnen
        emms
        pxor mm7,mm7
        xor eax,eax
        xor ecx,ecx
        mov ax,*dest.ResX
        mov cx,*dest.ResY
        imul ecx
        mov ecx,eax

        //Zeiger auf Daten vorbereiten
        add new,32
        add old,32
        add dest,32

        //Blendfaktoren in MM2 und MM3 als Words parallel laden
        pld       mm2,newmix
        punpcklbw mm2,mm2
        punpcklwd mm2,mm2
        punpcklbw mm2,mm7
        pld       mm3,oldmix
        punpcklbw mm3,mm3
        punpcklwd mm3,mm3
        punpcklbw mm3,mm7


        //Nur halb so viele dank MMX-Twinpixel-Routine Oo
        shr ecx,1

        //32Bit-AND-Mask in MM4 laden
        mov       eax,0xffffffff
        pldr      mm4,eax

        //2 Pixel aus jeder Quelle laden
lCopyMB:
        plq       mm5,*new
        plq       mm0,*old

        //Low und high Dword in zwei MMX-Register aufspalten
        pmov      mm1,mm0
        pmov      mm6,mm5
        punpckhdq mm1,mm7
        punpckhdq mm6,mm7
        pand      mm0,mm4
        pand      mm5,mm4

        //Alle Bytes in Words konvertieren, um zu multiplizieren
        punpcklbw mm0,mm7
        punpcklbw mm1,mm7
        punpcklbw mm5,mm7
        punpcklbw mm6,mm7

        //Altgewicht einberechnen
        pmullw    mm0,mm2
        pmullw    mm1,mm2
        pmullw    mm5,mm3
        pmullw    mm6,mm3

        //Entsprechende Bits rauskÅrzen
        pshimw    mm0,8
        pshimw    mm1,8
        pshimw    mm5,8
        pshimw    mm6,8

        //Neu + Alt zusammenmischen
        paddusw   mm0,mm5
        paddusw   mm1,mm6

        //16Bit-RGBWerte wieder in Bytes umwandeln
        packuswb  mm0,mm0
        packuswb  mm1,mm1

        //Pixel 1&2 zusammenpacken und speichern
        punpckldq mm0,mm1
        psq       mm0,*dest

        //Zeiger weitersetzen und Schleife abschlie·en
        add new,8
        add old,8
        add dest,8
        dec ecx
        jne lCopyMB
}}


//Ende BLUR.INC