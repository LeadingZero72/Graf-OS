/*


  TIME.H                                    29.Januar 2019
                                             Alexander Graf


  Die Systemzeit soll in 1:1000 Sekunden ablaufen, fÅr
  die Wahrnehmung besser aufgelîst.

  Die Systemzeit ist nun eine fortlaufende Long-Variable,
  welche vom Timer gesteuert wird.


*/
  //Die Systemzeit in 1000stel Sekunden
  extern long systime

  //Das Datum des Computers
  extern short century,year,month,day

  //Die alte 'gespeicherte Zeit', fÅr Messungen und Synchronisation
  extern long oldtime,newtime

  //Die gewÅnschte Frequenz fÅr Standardanwendungen
  extern long timebase
  extern long timefreq
  extern long oldtimeleft


//Systemzeit des Emulators nehmen Oo
far void gettime();

//Standardtimerwerte einprogrammieren
far void settime(long timestep);

//Um die Messung zu starten, gegenwÑrtigen Zeitpunkt festhalten
far void sync();

//Wenn ich die Zeit fÅr den Prozess stoppe, wieviel Zeit bleibt dann noch Åbrig, bis zum Abschlu· ?
far long timeleft();

//Restliche Zeit mit warten verbringen, damit die Grafik nicht zu schnell wird Oo
far void wait(long time);


//Ende TIME.H