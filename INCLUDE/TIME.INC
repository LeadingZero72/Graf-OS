/*


  TIME.INC                                   29.Januar 2019
                                             Alexander Graf


  Die Systemzeit soll in 1:1000 Sekunden ablaufen, fr
  die Wahrnehmung besser aufgel”st.

  Die Systemzeit ist nun eine fortlaufende Long-Variable,
  welche vom Timer gesteuert wird.


*/

  //Die Systemzeit in 1000stel Sekunden
  public long systime

  //Das Datum des Computers
  public short century,year,month,day

  //Die alte 'gespeicherte Zeit', fr Messungen und Synchronisation
  public long oldtime,newtime

  //Die gewnschte Frequenz fr Standardanwendungen
  public long timebase
  public long timefreq
  public long oldtimeleft





/*    .                                                              .
®ÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄ¯
      ³                 Der periodische Interrupt                    ³
®ÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄ¯
      ³ Dieser wird von 18.2 Hz auf 100Hz umprogrammiert um eine     ³
      ³ gew”hnlichere Zeiteinheit zu erreichen. Der Interrupt wird   ³
      ³ 100x pro Sekunde aufgerufen um die Systemzeit 'Hundreds'     ³
      ³ zu aktualisieren. Auáerdem werden nach jeder vollendeten     ³
      ³ Sekunde bestimmte Messz„hler statistisch erfaát und zurck   ³
      ³ gesetzt. Nach Programmende geht die MSDOS-Uhr nicht mehr     ³
      ³ richtig, da der Original-Handler fr die Uhrzeit nicht mehr  ³
      ³ aufgerufen wird. Somit geht die Uhr dann w„hrend der aus-    ³
      ³ fhrung nach und nach Programmende etwas vor.                ³
      ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
far void Handler8()
{
  asm
  {
        push ax                       //1. Register sichern
        push dx                       //
        puds                          //
        xor ax,ax                     //2. Realflat Daten in DS
        push ax                       //
        pods                          //

        add systime,1                 //3. Systemzeit aktualisieren

        mov dx,0x20                   //4. EOI an Controller senden
        mov al,0x20                   //
        outb                          //

        pods                          //5. Zerst”rte Register wiederherstellen
        pop dx                        //
        pop ax                        //
        mov sp,bp                     //6. Rcksprung ins normale Leben..
        pop bp                        //
        iret                          //
  }
}


void RTCInit()
{
  long *adr

  short hours,minutes,seconds,daylight

  asm
  {
        cli
        mov al,0x36       //Periodic Interval Timer Port,
        mov dx,0x43       //ber diesen Port periodischen
        outb              //Interrupt programmieren. (Rechteck)
        mov ax,1193       //Z„hler in ax laden,
        mov dx,0x40       //Zuerst Z„hler-High,
        outb              //dann Z„hler-Low
        shr ax,8          //ber Board tragen,
        outb              //um die Stopuhr zu starten.

        pucs
        pop ax
        shl eax,16
        mov ax,Handler8
        mov adr,32
        mov *adr,eax

        mov systime,0
        sti

        mov  ah,2         //RTC-Systemzeit im BCD-Format auslesen
        intr 0x1a         //
        mov  hours,ch     //
        mov  minutes,cl   //
        mov  seconds,dh   //
        mov  daylight,dl  //


        mov  ah,4         //RTC-Systemdatum im BCD-Format auslesen
        intr 0x1a         //
        mov  century,ch   //
        mov  year,cl      //
        mov  month,dh     //
        mov  day,dl       //
  }

  century = (century>>4)*10 + (century&15)
  year    = (year   >>4)*10 + (year   &15)
  month   = (month  >>4)*10 + (month  &15)
  day     = (day    >>4)*10 + (day    &15)

/*
      if(day-1>30)||(month-1>11)||(year>2100) { break;}           //Vorsicht vor falschen Daten!
      push(o,p,q)                                     //Datum zwischenspeichern

      if(p<3) p=p+10 q=q-1                            //Wochentag 1..7 (Sonntag..Samstag)
      else p=p-2                                      //

      x=q/100                                         //
      y=q%100                                         //
      w=26*p-2/10+o+y                                 //
      w=y/4+w                                         //
      w=x/4+w-x-x                                     //
      w=w%7                                           //
      if(w<0) w=w+7                                   //
      pop(z,y,x)                                      //Jahr,Monat,Tag zurückpoppen, WXYZ sind jetzt "Wochentag, Tag, Monat, Jahr"

      o=d[536] if(w==0) o=0xff8080
      while(1)                                        //
      {                                               //
        if(w==0) w='Sonntag' break;                   //
        if(w==1) w='Montag' break;                    //
        if(w==2) w='Dienstag' break;                  //
        if(w==3) w='Mittwoch' break;                  //
        if(w==4) w='Donnerstag' break;                //
        if(w==5) w='Freitag' break;                   //
        if(w==6) w='Samstag' break;                   //
        break;                                        //
      }                                               //
      while(1)                                        //
      {                                               //
        if(y==1)  y='Januar' break;                   //
        if(y==2)  y='Februar' break;                  //
        if(y==3)  y='M„rz' break;                     //
        if(y==4)  y='April' break;                    //
        if(y==5)  y='Mai' break;                      //
        if(y==6)  y='Juni' break;                     //
        if(y==7)  y='Juli' break;                     //
        if(y==8)  y='August' break;                   //
        if(y==9)  y='September' break;                //
        if(y==10) y='Oktober' break;                  //
        if(y==11) y='November' break;                 //
        if(y==12) y='Dezember' break;                 //
        break;                                        //
      }                                               //
*/
}
















//Standardtimerwerte einprogrammieren
far void settime(long timestep)
{
  timebase=timestep

  timefreq=1000/timestep
}


//Um die Messung zu starten, gegenw„rtigen Zeitpunkt festhalten
far void sync()
{
  oldtime=systime
}


//Wenn ich die Zeit fr den Prozess stoppe, wieviel Zeit bleibt dann noch brig, bis zum Abschluá ?
far long timeleft()
{
  newtime = systime - oldtime

  oldtimeleft = timebase - newtime

  return( oldtimeleft )
}


//Restliche Zeit mit warten verbringen, damit die Grafik nicht zu schnell wird Oo
far void wait(long time)
{
  long b

  random(100)

  if(time>0)
  {
    b = systime + time

    while(systime<b)
    {
      random(100)
/*
      asm
      {
        hlt
      }
*/
    }
  }
}


//Ende TIME.INC