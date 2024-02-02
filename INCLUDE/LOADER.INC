/*

  EXE/DLL-LOADER                       3.April 2019
                                    Alexander Graf

*/
far long task(string *filename,long interval)
{
  globalprocess  *p
  globalfunction *f
  globalvariable *v
  long i,j,      *Count,*Offset,*ToPatch,*q,*a,c,x,z,o,s
  string         *Code,*Name
  segment        *seg

  //Prozess erzeugen
  p              = node(400);
  *p.root        = root();
  *p.name        = strc(filename);
  *p.interval    = interval;
  *p.time        = systime;
  *p.enable      = 1;
  *p.mode        = 0;
  *p.heap        = getmem(100000);

  //TemporÑren Variablenspeicher reservieren (ich brauche endlich einen Adressoperator fÅr sowas, sieht langsam etwas lÑcherlich aus^^)
  Count    = getmem(4)
  Name     = getmem(10000)
  Offset   = getmem(4)
  ToPatch  = getmem(4)

  //Datei zum lesen îffnen
  open(filename,0x3d)

  //Grî·e des Codes auslesen
  read(Count,4) z=*Count

  //Realmode-Segment klarmachen
  *p.seg = getseg()

  //Realmode-Segment als Prozess speichern
  seg=*p.seg

  //BinÑrcode aus der Datei lesen
  read(*seg.base,z)

  //Anzahl benîtigter DLLs aus der Datei lesen
  read(Count,4)

  //Nacheinander alle DLLs laden
  for(i=*Count,i>0,i-=1)
  {
    read(Name,4)
    read(Name+4,*Name.Length)
  }

  //Nacheinander alle FAR-Funktionen identifizieren
  read(Count,4) q=*seg.base c=q>>4
  for(i=*Count,i>0,i-=1)
  {
    read(Name,4)
    read(Name+4,*Name.Length)
    read(Offset,4)

    //Die Funktion mu· erreichbar sein, wenn hier noch kein Offset vermerkt ist (Externe Funktion)
    o=*Offset
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
    read(Count,4)
    for(j=*Count,j>0,j-=1)
    {
      read(ToPatch,4)

       a = q + *ToPatch;
      *a = x
    }
  }

  //Alle Variablen identifizieren -----------------------
  read(Count,4)
  for(i=*Count,i>0,i-=1)
  {
    read(Name,4)
    read(Name+4,*Name.Length)
    read(Offset,4)

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
      *v.base = (*p.heap) + (*Offset);
      *v.id   = p;
      place(v,variables)
      x=*p.heap
    }
    else
    {
      x=*v.base
    }

    //Alle Zugriffe hier im Code neu patchen, da Heap+Ofs erst jetzt feststeht
    read(Count,4)

    for(j=*Count,j>0,j-=1)
    {
      read(ToPatch,4)
       a = q + *ToPatch;
      *a+=x
    }
  }

  //Datei am Ende wieder schlie·en
  close()

  //Zeiger-Variablen wieder freigeben
  free(Count)
  free(Name)
  free(Offset)
  free(ToPatch)

  //Prozess fÅr ParameterÅbergaben als Handle zurÅckliefern
  return((p))
}
//Ende LOADER.INC