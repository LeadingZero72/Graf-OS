/*
  Der Header der Bitmapdatei ist 54 Bytes lang
  und enthÑlt an Offsets 18 und 22 die Abmessungen
  als DWORD.

  Es soll Speicher entsprechender Grî·e reserviert werden.

  Die Datei wird geladen und mu· dann aber noch konvertiert
  werden, da das 32BPP-Format bevorzugt wird.
*/



//LoadBMP(x0,y0,x1,y1,Bitmap,File,Chroma)
far long LoadBMP(string *filename)
{
  long   *Width,*Height
  long   BufferSize,i
  long   *Buffer,*src,*dst
  bitmap *finalbitmap

  //Speicher fÅr Variablen dessen Adresse benîtigt wird (weil es immer noch keinen Adre·operator gibt)
  Width=getmem(4)
  Height=getmem(4)

  //Bitmapdatei îffnen und Abmessungen ablesen
  open(filename,0x3d) seek(18)
  read(Width,4)
  read(Height,4)

  //Lesepuffer anlegen, weil nur Tripletbytes
  BufferSize = (*Width) * (*Height)
  Buffer     = getmem(BufferSize*3)

  seek(54) read(Buffer,BufferSize*3) close()


  //Zielbitmap anlegen, da Grî·e jetzt bekannt
  finalbitmap = getmem(BufferSize*4+32);
  *finalbitmap.next    = 0;
  *finalbitmap.name    = strc(filename);
  *finalbitmap.prev    = 0;
  *finalbitmap.subnode = 0;
  *finalbitmap.ResX    = *Width;
  *finalbitmap.ResY    = *Height;

  //Lesepuffer in Zielpuffer 'konvertieren'
  src=Buffer
  dst=finalbitmap+32
  for(i=0,i<BufferSize,i+=1)
  {
    *dst=*src&0xffffff
    src+=3
    dst+=4
  }

  //Resourcen freigeben
  free(Width)
  free(Height)
  free(Buffer)

  //Geladenes Bitmap als Funktionsergebnis zurÅckgeben
  return(finalbitmap)
}
//Ende LOADBMP.INC