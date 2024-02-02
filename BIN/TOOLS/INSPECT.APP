/*
   F = Adresse
*/
  stack(100)
  if(a) strcpy('Variable Inspector',a) exit()


/*#########################################################################
  # SCHNELLE PROZEDUREN
  #########################################################################*/
proc GetFilename(0)
{
  push(d,m,n) d=Data
  strcpy('Enter physical 32-bit address, pleaze',@m)
  strcpy('0x00',@n)
  cd(SysRoot) call('GETS.DLL') m=m n=n q=@a
  pop(n,m,d)
}



/*     .                                                              .
 ®ÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄ¯
       ³                 Number(String) -> A=numeric                  ³
 ®ÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄ¯
       ³ Der String darf dezimale, hex und bin„re Zahlen enthalten.   ³
       ³ Unterschieden wird durch die 0x/0b als Kennung, z.B. 0xffff  ³
       ³ oder 0b00010001.                                             ³
       ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ    */
proc Number(1)
{
  pop(a) push(b,c,d,e,f) e=a[0] c=e[0] d=0 f=0   //String in E, Multiplikator fr positive Zahlen

  if(e[4]Char==45) f=1 c=c-1 d=d+1
  if(e[d+4]Char<48)||(e[d+4]Char>57) return(12)  //Falls keine Zahl, Syntaxfehler..
  while(1)                                       //Fr alle M”glichen Formate:
  {
    //Hexadezimale Eingabe ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¯
    if(e[d+4]Integer==0x7830)&&(c>1)             //Beginnt mit "0x" ? Dann hexadezimal:
    {                                            //
      c=c-2 if(c<1)||(c>8) Bail(13)              //Korrekte Anzahl Nibble ?
      d=d+2 a=0 c=c-1>>2 c=1>>c                  //Welches Nibble als Start ?
      while(d<e[0])                              //Fr alle Nibble:
      {                                          //
        b=e[d+4] Char                            //Nibble aus Quelltext
        if(b>=48)&&(b<=57)  a=b-48*c|a d=d+1 c=c<<4 continue; //Entweder Digit,
        if(b>=65)&&(b<=70)  a=b-55*c|a d=d+1 c=c<<4 continue; //groáe Pseudotetraden
        if(b>=97)&&(b<=102) a=b-87*c|a d=d+1 c=c<<4 continue; //oder kleine halt^^
        return(14)                               //Evtl. Syntaxfehler...
      }                                          //
      if(f) a=-a                                 //(negative Ergebnisse bercksichtigen)
      break;                                     //Ergebnis in A...
    }
    //Bin„re Eingabe ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¯
    if(e[d+4] Integer==0x6230)&&(c>1)            //Beginnt mit "0b" ? Dann hexadezimal:
    {                                            //
      c=c-2 if(c<1)||(c>32) Bail(15)             //Zwei Zeichen abziehen
      d=d+2 a=0 c=c-1 c=1>>c                     //Jede Stelle enth„lt nur 1 Bit
      while(d<e[0])                              //Fr alle Bits:
      {                                          //
        b=e[d+4]Char-48 if(b==0)||(b==1) a=b*c+a //Stellenbetrag addieren
        else return(13)                          //(oder Syntaxfehler)
        d=d+1 c=c<<1                             //N„chste Stelle maskieren
      }                                          //
      if(f) a=-a                                 //(negative Ergebnisse bercksichtigen)
      break;                                     //Ergebnis in A...
    }
    //Default: Dezimale Eingabe ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¯
    a=c-1 c=1                                           //H”chste 10er-Potenz in
    if(a) c=c*10 a=a-1 repeat                           //einer Schleife vorberechnen
    a=0                                                 //Beginne bei "0"
    if(d<e[0]) a=e[d+4] Char-48*c+a d=d+1 c=c/10 repeat //Stellenbetr„ge addieren
    if(f) a=-a                                          //(negative Ergebnisse bercksichtigen)
    break;                                              //Ergebnis in A...
  }
  pop(f,e,d,c,b)
}




proc UpdateBuffer(0)
{
  copy(f+v,t,1024)
}



  d=Data
  cd(ProgPath) Init(D_ResX,D_ResY,MT_Force) free(ProgPath)
  Window(512,512,512,'Zeigt Variablen',0)

  v=0
  f=0
  getmem(t,1024)
  UpdateBuffer()

  e=Program while(e[508]&256==0)
  {
    d=Data
    wait(Frame)
    if(e[508]&16==0) continue;

    Receive() while(a)
    {
      WinCope() if(a) break 2;
    }
    d=Data
    KbGetch() b=a
    if(b==72)&&(v)      v=v-1                 UpdateBuffer()
    if(b==80)           v=v+1                 UpdateBuffer()
    if(b==77)&&(v)      v=v-1024              UpdateBuffer()
    if(b==75)           v=v+1024              UpdateBuffer()
    if(b==59) GetFilename() Number(a) f=a v=0 UpdateBuffer()

    Box(5,30,500,490,k,d[512])
    x=0 y=0 j=v
    while(j<1024+v)
    {
      w=f[j]Char

      c=d[512]
      z=d[536]

      if(w!=t[j]Char) z=0xff3000 t[j]=bptr w

      if(w==0) z=0x008000

      Fprint(x+5,y+40,480,490,k,'%h2',@w,c,z)
      j=j+1
      x=x+16+2
      if(x>480) x=0 y=y+8+4 if(y>480) y=0 break;
    }
    x=v
    y=v
    z=v/16
    Fprint(5,30,510,510,k,'Offset <%da> in hex: 0x%h8,seg:0x%h4',@x,d[512],d[536])

    render()
  }
  UnIcon(Program)
//Ende INSPECT.APP