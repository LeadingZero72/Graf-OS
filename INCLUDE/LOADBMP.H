/*
  Der Header der Bitmapdatei ist 54 Bytes lang
  und enth„lt an Offsets 18 und 22 die Abmessungen
  als DWORD.

  Es soll Speicher entsprechender Gr”áe reserviert werden.

  Die Datei wird geladen und muá dann aber noch konvertiert
  werden, da das 32BPP-Format bevorzugt wird.
*/



//LoadBMP(x0,y0,x1,y1,Bitmap,File,Chroma)
far long LoadBMP(string *filename);




//Ende LOADBMP.H