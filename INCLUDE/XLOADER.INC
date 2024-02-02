/*

  EXE/DLL-LOADER                       3.April 2019
                                    Alexander Graf

*/
far long xtask(string *filename,long interval)
{
  globalprocess  *p
  globalfunction *f
  globalvariable *v
  long i,j,      Count,*q,*a,c,x,z,o,s
  string         *Code,*Name
  segment        *seg
  long           *filebuf,*pos

  //Prozess erzeugen
  p              = node(400);
  *p.root        = froot();
  *p.name        = strc(filename);
  *p.interval    = interval;
  *p.time        = systime;
  *p.enable      = 1;
  *p.mode        = 0;
  *p.heap        = getmem(100000);

  //TemporÑren Variablenspeicher reservieren (ich brauche endlich einen Adressoperator fÅr sowas, sieht langsam etwas lÑcherlich aus^^)
  Name     = getmem(10000)

  //Datei zum lesen îffnen
  filebuf = fread(filename) pos=filebuf

  //LÑnge der Codes auslesen
  z = *pos pos+=4

  //Realmode-Segment klarmachen
  *p.seg = getseg()

  //Realmode-Segment als Prozess speichern
  seg=*p.seg

  //BinÑrcode aus der Datei lesen
  copy(pos,*seg.base,z) pos+=z

  //Anzahl benîtigter DLLs aus der Datei lesen
  Count = *pos pos+=4

  //Nacheinander alle DLLs laden
  for(i=Count,i>0,i-=1)
  {
    *Name.Length = *pos pos+=4
    copy(pos,Name+4,*Name.Length) pos += *Name.Length
  }

  //Nacheinander alle FAR-Funktionen identifizieren
  Count=*pos pos+=4 q=*seg.base c=q>>4

  for(i=Count,i>0,i-=1)
  {
    *Name.Length = *pos pos+=4
    copy(pos,Name+4,*Name.Length) pos += *Name.Length

    o = *pos pos+=4

    //Die Funktion mu· erreichbar sein, wenn hier noch kein Offset vermerkt ist (Externe Funktion)
    if(o==0)
    {
      f = find(Name,*functions.next)
      x = (*f.offs)+((*f.segm)<<16)
    }
    else
    {
      f=node(200) place(f,functions);
      *f.name = strc(Name);
      *f.segm = c;
      *f.offs = o;
      *f.id   = p;
    }

    x = (*f.offs)+((*f.segm)<<16)

    //Alle Aufrufe neu patchen
    Count=*pos pos+=4

    for(j=Count,j>0,j-=1)
    {
       a = q + *pos; pos+=4;
      *a = x
    }
  }

  //Alle Variablen identifizieren -----------------------
  Count=*pos pos+=4

  for(i=Count,i>0,i-=1)
  {
    *Name.Length = *pos pos+=4
    copy(pos,Name+4,*Name.Length) pos += *Name.Length

    //Variablen-Offset laden
    o = *pos pos+=4

    //<extern>-Variablen sind registriert und haben eine Adresse
    v = find(Name,*variables.next)


    /*
      Hier scheint ein Denkfehler von mir zu liegen...

      Die lokalen publics, also die Variablen, die noch
      nicht in der Variablenliste gefunden werden kînnen,
      weil sie just erst entstehen, werden mit Heap+Offset
      in die Liste Åbernommen ABER das Patch braucht die
      komplette Adresse nicht, weil der Offset schon in den
      Befehlen drinsteckt.

      Bei lokalen externs, also Variablen, die vorher schon
      in der Variablenliste zu finden sind, wird immer die
      komplette Base+Offset HINZUADDIERT, um die Struktur-
      Elemente UND die Variable (also Heap+Offset) zu erreichen^.^

      Was fÅr ein Chaos ^.^

    */
    //<public>-Variablen werden per Namen in die Liste eingefÅgt und bekommen Base+Offset zur Relokation
    if(v==0)
    {
      v       = node(12);
      *v.name = strc(Name);
      *v.base = (*p.heap) + o;
      *v.id   = p;
      place(v,variables)
      x=*p.heap
    }
    else
    {
      x=*v.base
    }

    //Alle Zugriffe hier im Code neu patchen, da Heap+Ofs erst jetzt feststeht
    Count=*pos pos+=4

    for(j=Count,j>0,j-=1)
    {
       a = q + *pos; pos+=4;
      *a+=x
    }
  }
  //Zeiger-Variablen wieder freigeben
  free(Name)

  //Prozess fÅr ParameterÅbergaben als Handle zurÅckliefern
  return((p))
}
//Ende LOADER.INC