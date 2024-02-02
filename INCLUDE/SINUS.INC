/*

  SINUS.INC                      3.01.2019 Alexander Graf

  Die Sinustabelle liegt an Stelle 8192 in den globalen
  Daten und besteht aus Integerwords, die erst in 32-Bit
  hochgerechnet und dann entsprechend mit der Amplitude
  multipliziert den gewÅnschten Sinuswert geben.

*/

public long *sinustabelle

void InitSinus()
{
  fcd('A:\OSAG') sinustabelle = fread('SINUS.DAT')
}



far long sinus(long winkel,long amplitude)
{
  long *sinuspointer
  asm
  {
    //Winkelangabe fÅr Sinustabelle vorbereiten (1024¯ Gradwinkel)
    mov eax,winkel
    and eax,1023
    shl eax,1
    add eax,sinustabelle
    mov sinuspointer,eax

    //Sinuswert laden und korrekt ausrechnen
    mov eax,*sinuspointer
    movsx eax,ax
    mov ecx,amplitude
    imul ecx
    sar eax,8
  }
}

//Ende SINUS.INC