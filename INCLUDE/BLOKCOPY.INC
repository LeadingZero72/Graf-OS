/*


   BLOCK-COPY                               13.Januar 2018
                                          Alexander Graf



  Hier ist eine generelle Routine, um Bilddaten von
  hier nach da zu kopieren.

  Es gibt ein Quellbild und ein Zielbild.

  Aus irgendeinem Quellbild soll ein Ausschnitt
  bestimmter Grî·e in den Viewport eines anderen
  Bildes geclippt und gezeichnet werden.

  Das kann man u.A dazu benutzen, Schriftarten
  darzustellen. Die Glyphen werden einem Bild
  entnommen und mit <blockcopy> in die Grafik
  kopiert.

  OPTIMIERUNGEN

  1.Wird der Bildausschnitt 1:1 kopiert, wird
    die schnelle SSE-128Bit Routine zum kopieren
    der Bilddaten verwendet.

  2.Sind beide Kanten vergrî·ert, wird bilinear
    gefiltert, um den Rastereffekt zu minimieren.

  3.Sind beide Kanten verkleinert, wird ein
    Sammelfilter verwendet, um dem Schimmereffekt
    entgegenzuwirken und den Charakter des Bildes
    zu erhalten.

  4.Es gibt 2 FÑlle, wo X 1:1 kopiert wird,
    aber Y vergrî·ert oder verkleinert.
    Die bilineare Routine kann den Fall der Vergrî·erung
    abdecken, da die Schrittweite dann 65536 ist
    und keinen Unterschied macht.
    Das Sammelfilter wird man allerdings nicht
    losbekommen, da sonst wieder das Schimmern
    einsetzt.
    Das bilineare 4-Punkteprogramm wird zum
    2-Punkteprogramm. Es gibt einen Punkt oben
    und einen in der Zeile darunter, zwischen
    denen linear Interpoliert wird. Nur mÅssen
    fÅr den oberen und den unteren Punkt alle
    horizontalen Punkte zusammenaddiert werden,
    was das Sammelfilter macht.
  5.Den gleichen Fall gibt es auch vertauscht,
    d.h. horizontal wird interpoliert, weil zu
    wenige Punkte da sind und vertikal wird
    gesammelt, um Schimmern zu beheben.

  Die optimierten Resampler machen das Kopieren
  von Grafikblîcken zum Traum. Es wird unfassbar
  viel darauf aufbauen, Schriftarten, Desktop,
  Fenster und Items...

*/

public long fontcolor

far void Blockcopy(long AusschnittX,long AusschnittY,long AusschnittDX,long AusschnittDY,bitmap *source,
                   long x0,long y0,long DeltaX,long DeltaY,bitmap *dest)
{
  long *a1,*a2
  long *adr
  long *sadr
  long x1,y1,i,j
  long Jump,SJump

  long sx,sy
  long u,v
  long fracU,fracV
  long invU,invV

  long *A,*B,*C,*D

  long Au,Av,Bu,Bv,Cu,Cv,Du,Dv

  long m,n,filterX,filterY
  long Red,Green,Blue
  long pixel

  long PixelDivider

  long blend
  long Odx,Ody

  //Total-AusscheidungsfÑlle ------------------
  x1=x0+DeltaX-1
  y1=y0+DeltaY-1
  if(x1<viewMinX)||(y1<viewMinY)||(x0>=viewMaxX)||(y0>=viewMaxY) { return() }


  //Schrittweite XY im 16:16-Fixkommaformat aus ungeclippten Koordinaten berechnen
  sx = (AusschnittDX<<16) / DeltaX
  sy = (AusschnittDY<<16) / DeltaY


  //U0 und V0 ins 16:16-Fixkommaformat hochschieben
  AusschnittX = AusschnittX << 16
  AusschnittY = AusschnittY << 16


  //Original-DeltaXY beibehalten, weil der Bilinear-Mapper sonst beim Clipping zoomt
  Odx = DeltaX
  Ody = DeltaY


  //Wenn x0 links vom Bildrand
  if(x0<viewMinX)
  {
    //öberstehenden Rand berechnen
    i = viewMinX - x0

    //U-Koordinaten Åberspringen
    AusschnittX += i * sx

    //Entsprechend weniger Pixel fÅllen
    DeltaX -= i

    //x0 am linken Bildrand festsetzen
    x0=viewMinX
  }

  //Wenn y0 oberhalb des Bildrandes
  if(y0<viewMinY)
  {
    //öberstehenden Rand berechnen
    i = viewMinY - y0

    //V-Koordinaten Åberspringen
    AusschnittY += ((viewMinY-y0)*sy)

    //Entsprechend weniger Zeilen fÅllen
    DeltaY-=viewMinY-y0

    //y0 am oberen Bildrand festsetzen
    y0=viewMinY
  }

  //Der rechte/untere Rand scheint problemlos, da hier lediglich die Deltas reduziert werden.
  if(x0+DeltaX>=viewMaxX)
  {
    DeltaX = viewMaxX - x0 +1
  }

  if(y0+DeltaY>=viewMaxY)
  {
    DeltaY = viewMaxY - y0 +1
  }

  //Adresse im Zielbild
  adr = dest+32 + (y0*(*dest.ResX)+x0)*4


  //Wenn sich an den KantenlÑngen nichts Ñndert:
  if(sx==65536)&&(sy==65536)
  {
    sadr   = source+32 + ((AusschnittY>>16)*(*source.ResX)+(AusschnittX>>16))*4
    SJump  = *source.ResX*4
    Jump   = *dest.ResX*4

    for(j=0,j<DeltaY,j+=1)
    {
      a1 = sadr
      a2 =  adr

      //copy(sadr,adr,ToDraw)
      for(i=0,i<DeltaX,i+=1)
      {
        if(*a1 != 0) { *a2 = *a1 }

        a1+=4 a2+=4
      }
      sadr+=SJump
      adr+=Jump
    }
    return(0)
  }



  //Wenn beide Kanten lÑnger werden: Bilineares Filter verwenden
  if(sx<65536)||(sy<65536)
  {
    sx = ((AusschnittDX-1)<<16) / (Odx)
    sy = ((AusschnittDY-1)<<16) / (Ody)

    blend = fontcolor

    //MMX-Einheit vorbereiten
    asm {
          emms
          pxor mm7,mm7

          pld       mm5,blend
          punpcklbw mm5,mm7
        }

    //Startkoordinate<V> in 16:16-Fixkommaformat
    v = AusschnittY

    //Sprung zwischen den Zeilen des Zielbildes
    Jump  = ((*dest.ResX) - DeltaX)*4

    for(j=0,j<DeltaY,j++)
    {
      //Startkoordinate<U> in 16:16-Fixkomma umwandeln
      u = AusschnittX

      for(i=0,i<DeltaX,i++)
      {
        //Zeiger auf die 4 Punkte ABCD 
        A = source+32+((v>>16)*(*source.ResX)+(u>>16))*4
        B = A +4
        C = B +(*source.ResX)*4
        D = C -4

        //Fraktalteil der aktuellen UV-Koordinate entlang X und Y
        fracU = u&65535
        fracV = v&65535
        invU  = 65535 - fracU
        invV  = 65535 - fracV

        //Die Wahrscheinlichkeiten aus den Achsen werden multipliziert, um einen Gesamtaussagewert fÅr den jeweiligen Quartett-Punkt zu bekommen.
        Au = invU  * invV
        Bu = fracU * invV
        Cu = fracU * fracV
        Du = invU  * fracV

        //Punkte ABCD mit ihren Gewichten multiplizieren und Summe speichern
        asm
        {
          pld       mm6,*A
          pld       mm1,Au
          pshimd    mm1,24
          punpcklbw mm1,mm1
          punpcklwd mm1,mm1
          punpcklbw mm1,mm7
          punpcklbw mm6,mm7
          pmullw    mm6,mm1
          pshimw    mm6,8

          pld       mm0,*B
          pld       mm1,Bu
          pshimd    mm1,24
          punpcklbw mm1,mm1
          punpcklwd mm1,mm1
          punpcklbw mm1,mm7
          punpcklbw mm0,mm7
          pmullw    mm0,mm1
          pshimw    mm0,8
          paddusw   mm6,mm0

          pld       mm0,*C
          pld       mm1,Cu
          pshimd    mm1,24
          punpcklbw mm1,mm1
          punpcklwd mm1,mm1
          punpcklbw mm1,mm7
          punpcklbw mm0,mm7
          pmullw    mm0,mm1
          pshimw    mm0,8
          paddusw   mm6,mm0

          pld       mm0,*D
          pld       mm1,Du
          pshimd    mm1,24
          punpcklbw mm1,mm1
          punpcklwd mm1,mm1
          punpcklbw mm1,mm7
          punpcklbw mm0,mm7
          pmullw    mm0,mm1
          pshimw    mm0,8
          paddusw   mm6,mm0

          pmullw    mm6,mm5
          pshimw    mm6,8

          packuswb  mm6,mm6
          pst       mm6,pixel
        }
        if(pixel!=0) { *adr=pixel }
        adr+=4 u+=sx
      }
      v+=sy adr+=Jump
    }
    return(0)
  }



  //Wenn beide Kanten kÅrzer werden: Sammelfilter verwenden
  if(sx>65536)||(sy>65536)
  {
    //Sammelboxgrî·e m und n Pixel. Dabei wird eins aufgerundet.
    filterX=(sx>>16)+1
    filterY=(sy>>16)+1

    if(filterX==0) filterX=1
    if(filterY==0) filterY=1

    //Startkoordinate<V> in 16:16-Fixkomma umwandeln
    v = AusschnittY

    //Sprung zwischen den Zeilen des Zielbildes
    Jump  = ((*dest.ResX) - DeltaX)*4

    //Divisionsfaktor zum ausdividieren der Farbwerte
    PixelDivider = 0xffff / (filterX*filterY)

    for(j=0,j<DeltaY,j++)
    {
      //Startkoordinate<U> in 16:16-Fixkomma umwandeln
      u = AusschnittX

      for(i=0,i<DeltaX,i++)
      {
        //Zeiger auf die Texel (linke/obere Ecke des Sammelfilters)
        A = source+32+((v>>16)*(*source.ResX)+(u>>16))*4;

        //Sammelfilter Zwischensumme beginnt bei Null
        Red=0
        Green=0
        Blue=0

        //Sammelfilter fÅr X*Y Texel ausfÅhren
        for(n=0,n<filterY,n++)
        {
          for(m=0,m<filterX,m++)
          {
            asm
            {
              mov   eax,*A
              movzx edx,al
              add   Blue  ,edx
              movzx edx,ah
              add   Green,edx
              shr   eax,16
              movzx edx,al
              add   Red,edx
            }
            A+=4
          }
          A+=((*source.ResX)-filterX)*4
        }

        //Finalen Farbwert berechnen, indem durch die Gesamtzahl Pixel im Filter dividiert wird.
        asm
        {
          mov  ecx,PixelDivider

          mov  eax,Red
          imul ecx
          mov  ebx,eax

          mov  eax,Green
          imul ecx
          shr  eax,16
          mov  bh,al

          mov  eax,Blue
          imul ecx
          shr  eax,16
          mov  bl,al

          test ebx,ebx
          je   nofpixl
          mov  *adr,ebx
        nofpixl:
        }
        adr+=4 u+=sx
      }
      v+=sy adr+=Jump
    }
    return(0)
  }
}
//Ende BLOCKCOPY.INC