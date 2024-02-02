/*

  ALICE und RABBIT spielen KARTEN             19.Juni 2019
                                            Alexander Graf  


*/

//Diese Struktur wird vom BIOS zum Transfer verwendet, um Sektoren zu lesen und zu schreiben
struct diskaddresspacket
{
  short packetsize
  short rsvd1
  short secs
  short rsvd2
  long  segm
  long  sector
  long  sectorh
  long  buflow
  long  bufferh
}

//Die ersten 512 Bytes jeder Karte, danach evtl. noch Daten, z.B. die ersten Daten der Datei oder Verzeichnisdaten, wie z.B. das Hintergrundbild, Desktopthema, Benutzername oder Ordner-Passwort. Der Datei/Ordnername startet bei Offset 60 und ist max 448(+4) Bytes lang
struct infocard
{
  long subdir
  long next
  long journal
  long size
  long attributes
  long date
  long time
  long datelastmodified
  long timelastmodified
  long this
  long parent
  long last
}

//Die ersten 512 Bytes jeder Karte, danach evtl. noch Daten, z.B. die ersten Daten der Datei oder Verzeichnisdaten, wie z.B. das Hintergrundbild, Desktopthema, Benutzername oder Ordner-Passwort. Der Datei/Ordnername startet bei Offset 60 und ist max 448(+4) Bytes lang
struct filelist
{
  long next
  long name
  long prev
  long subnode

  long subinfo
  long nextinfo
  long journal
  long size
  long attributes
  long date
  long time
  long datelastmodified
  long timelastmodified
  long this
  long parent
  long last

  long posx
  long posy
  long select
  long radix
  long icon
}


//Startsektor des Dateisystems mit den Einstiegspunkten zu Dateibaum, Kartenstapel und Sektoradresse der zuletzt formatierten Karte. Der Directory-Name startet bei Offset 60 und ist max. 448(+4) Bytes lang
struct filesystem
{
  long CardsStack
  long TotalSpace
  long ForTheMilliner
}


//Array of Structure fÅr eine lÅckenlose DatentrÑgerbeschreibung
struct drivelist
{
  long biosdevice
  long *directory
  long d1,d2
}


/************************************************************
 *                                                          *
 *                   ALICE - VARIABLEN                      *
 *                                                          *
 ************************************************************/
public drivelist     *DriveList
public long           DriveNumber
public long           DriveCount

public long           DriveNumberA
public segment       *FilesystemSegment
public segment       *DirSegment
public segment       *FileSegment
public long           DriveNumberB

public diskaddresspacket *FilesystemDAP
public diskaddresspacket *DirDAP
public diskaddresspacket *FileDAP

public filesystem    *Filesystem
public infocard      *Directory
public infocard      *File

public long           DriveNumberC
public long           DAPResult
public long           DriveNumberD
public long           DAPBytesPerSector

//Strings zum Aufsplitten von Dateinamen und suchen mit 'Wildcards'
public string *fname,*fend
public string *fleft,*fright

//Die Sektornummer der Node "davor" oder <Null>, wenn die gesuchte Datei die erste Datei im Verzeichnis ist
public long fprevious

//Die Sektornummer der 'zuletzt gelîschten Datei', wird vom OS fÅr das Journaling genutzt und ist normalerweise <Null>. Wird von fcreate in die Infonode Åbertragen.
public long fjournal

//Array zum puffern der Sektoradressen der zum schreiben benîtigten Karten
public long *fsectors



/************************************************************
 *                                                          *
 *                 ALICE - GENERIC READ                     *
 *                                                          *
 ************************************************************/
void BiosDAP(long dataaccesspacket,long bios)
{
  asm
  {
    xor al,al
    mov edx,DriveNumberA
    cmp edx,DriveNumberB
    jne DriveFail
    cmp edx,DriveNumberC
    jne DriveFail
    cmp edx,DriveNumberD
    jne DriveFail
    puds
    push si
    mov eax,dataaccesspacket
    shr eax,4
    push ax
    pods
    xor si,si
    mov ax,bios
    intr 0x13
    setnc al
    pop si
    pods
DriveFail:
    mov DAPResult,al
  }
}



/************************************************************
 *                                                          *
 *                 ALICE - GENERIC WRITE                    *
 *                                                          *
 ************************************************************/
far void UpdateFilesystem()
{
  BiosDAP(FilesystemDAP,0x4300)
}

far void ReadFilesystem()
{
  BiosDAP(FilesystemDAP,0x4200)
}

far void SaveFile()
{
  BiosDAP(FileDAP,0x4300)
}

far void LoadFile()
{
  BiosDAP(FileDAP,0x4200)
}

far void SaveDir()
{
  BiosDAP(DirDAP,0x4300)
}

far void LoadDir()
{
  BiosDAP(DirDAP,0x4200)
}



/************************************************************
 *                                                          *
 *                ALICE - LAUFWERKSWECHSEL                  *
 *                                                          *
 * öbergeben wird die Laufwerks-Indexnummer (0=A,1=B...)    *
 * Heraus kommt der alte Ordner im neuen Laufwerk, als      *
 * Kopie von dem String-Pointer aus der Drive-Liste.        *
 *                                                          *
 ************************************************************/
far long fgetdrive()
{
  return(DriveList + DriveNumber * 16)
}


//Setzt anderes Laufwerk und gibt aktuellen Pfad darin zurÅck (string)
far long fdrive(long DriveIndex)
{
  drivelist  *l

  //Arrayoffset anhand des Laufwerksbuchstabens berechnen
  l = DriveList + DriveIndex * 16

  //Laufwerksnummer festhalten
  DriveNumber=DriveIndex

  //Bios-GerÑte-ID festlegen
  DriveNumberA = *l.biosdevice
  DriveNumberB = *l.biosdevice
  DriveNumberC = *l.biosdevice
  DriveNumberD = *l.biosdevice

  //Nach dem Laufwerkswechsel unbedingt den Filesystemsektor laden
//  ReadFilesystem()

  //Letzten Ordner zurÅckgeben
  return(*l.directory)
}

/************************************************************
 *                                                          *
 *        PFAD ALS 'AKTUELLES VERZEICHNIS' SPEICHERN        *
 *                                                          *
 * FÅr jeden DatentrÑger gibt es einen String, der besagt   *
 * welches das 'aktuelle Verzeichnis' ist, damit man bequem *
 * zwischen den Laufwerken wechseln kann und nicht immer im *
 * Stammverzeichnis landet.                                 *
 *                                                          *
 ************************************************************/
far void fdriveupdate(string *neuerpfad)
{
  drivelist *l

  //Drive-List, Zeiger auf aktuelles Laufwerk laden
  l = fgetdrive()

  //Letzten Pfad freigeben
  free(*l.directory)

  //Neuen Pfad einschreiben
  *l.directory = strc(neuerpfad)
}


/************************************************************
 *                                                          *
 *                ALICE - INITIALISIERUNG                   *
 *                                                          *
 * Alice wird immer erst auf Laufwerk A initialisiert.      *
 * Die Partition kann jederzeit geÑndert werden, indem      *
 * die FilesystemDAP auf einen anderen Sektor gelegt wird.  *
 *                                                          *
 ************************************************************/
far void AliceInit()
{
  drivelist *l

  //Laufwerksliste anlegen und Laufwerke A und B registrieren
  DriveList = getmem(416) blast(DriveList,416,0)

  l = DriveList;

  *l.biosdevice = 0x81;
  *l.directory  = strc('A:\')

  l+=16;

  *l.biosdevice = 0x82;
  *l.directory  = strc('B:\');

  DriveNumber=0
  DriveCount=2

  //Fehlerstatus clearen
  DAPResult = 0


  //Aktuelles Laufwerk festlegen
  fdrive(0)

  //DAP-Adresse festlegen (fÅrs Stammverzeichnis)
  FilesystemSegment = getseg() //blast(FilesystemSegment,32768,0)
  FilesystemDAP     = *FilesystemSegment.base

  //Reservierte Felder sollen '0' sein
  *FilesystemDAP.rsvd1 = 0;
  *FilesystemDAP.rsvd2 = 0;

  //Empfohlene DAP-Grî·e fÅr Operationen im 16-Bit Realmode = 16 Bytes
  *FilesystemDAP.packetsize=16

  //Seg:Ofs-Adresse des Transferpuffers aus Sicht des BIOS, Segment in obere 16 Bit, Offset ist immer Null, dann kommt es auch nicht zu 'SegmentÅberlÑufen' ^,^
  *FilesystemDAP.segm = ((FilesystemDAP+512) >> 4)<<16

  //64-Bit-Flat-Adresse erstmal nicht benutzen, da ich vermute, da· das BIOS nur 640kB adressieren kann. (Bit4 der Extension-Bitmap mÅ·te gesetzt sein, ist sie aber nicht, also Realmode-way-of-life)
  *FilesystemDAP.buflow  = 0;
  *FilesystemDAP.bufferh = 0;

  //LBA-High ist die meiste Zeit Åber <0>
  *FilesystemDAP.sector  = 2048;
  *FilesystemDAP.sectorh = 0;

  //Sektorenzahl festlegen (1 Sektor wÑre fÅr den DatentrÑger eine Read/Modify/Write-Operation, von daher lieber gleich 8-kB schreiben, das sollte flÅssiger laufen ^.^)
  *FilesystemDAP.secs = 16

  //Schnelle Variable, um ins Wunderland zu kommen
  Filesystem = FilesystemDAP + 512



  //DAP-Adresse festlegen (fÅrs Stammverzeichnis)
  DirSegment = getseg() //blast(DirSegment,32768,0)
  DirDAP     = *DirSegment.base

  //Reservierte Felder sollen '0' sein
  *DirDAP.rsvd1 = 0;
  *DirDAP.rsvd2 = 0;

  //Empfohlene DAP-Grî·e fÅr Operationen im 16-Bit Realmode = 16 Bytes
  *DirDAP.packetsize=16

  //Seg:Ofs-Adresse des Transferpuffers aus Sicht des BIOS, Segment in obere 16 Bit, Offset ist immer Null, dann kommt es auch nicht zu 'SegmentÅberlÑufen' ^,^
  *DirDAP.segm = ((DirDAP+512) >> 4)<<16

  //64-Bit-Flat-Adresse erstmal nicht benutzen, da ich vermute, da· das BIOS nur 640kB adressieren kann. (Bit4 der Extension-Bitmap mÅ·te gesetzt sein, ist sie aber nicht, also Realmode-way-of-life)
  *DirDAP.buflow = 0;
  *DirDAP.bufferh = 0;

  //LBA-High ist die meiste Zeit Åber <0>
  *DirDAP.sector  = 2064;
  *DirDAP.sectorh = 0;

  //Sektorenzahl festlegen (1 Sektor wÑre fÅr den DatentrÑger eine Read/Modify/Write-Operation, von daher lieber gleich 8-kB schreiben, das sollte flÅssiger laufen ^.^)
  *DirDAP.secs = 16

  //Schnelle Variable, um ins Wunderland zu kommen
  Directory = DirDAP + 512



  //Gro·e DAP-Adresse festlegen (fÅr gro·e Transfers)
  FileSegment = getseg() //blast(FileSegment,32768,0)
  FileDAP     = *FileSegment.base

  //Reservierte Felder sollen '0' sein
  *FileDAP.rsvd1 = 0;
  *FileDAP.rsvd2 = 0;

  //Empfohlene DAP-Grî·e fÅr Operationen im 16-Bit Realmode = 16 Bytes
  *FileDAP.packetsize=16

  //Seg:Ofs-Adresse des Transferpuffers aus Sicht des BIOS, Segment in obere 16 Bit, Offset ist immer Null, dann kommt es auch nicht zu 'SegmentÅberlÑufen' ^,^
  *FileDAP.segm = ((FileDAP+512) >> 4)<<16

  //64-Bit-Flat-Adresse erstmal nicht benutzen, da ich vermute, da· das BIOS nur 640kB adressieren kann. (Bit4 der Extension-Bitmap mÅ·te gesetzt sein, ist sie aber nicht, also Realmode-way-of-life)
  *FileDAP.buflow  = 0;
  *FileDAP.bufferh = 0;

  //LBA-High ist die meiste Zeit Åber <0>
  *FileDAP.sector  = 2064;
  *FileDAP.sectorh = 0;

  //Sektorenzahl festlegen (63 Sektorn wÑre fÅr den DatentrÑger am Ende eine Read/Modify/Write-Operation, von daher lieber gleich 'ganze 8-kB-Blîcke' schreiben, das sollte flÅssiger laufen ^.^)
  *FileDAP.secs = 63;

  //Schnelle Variable, um ins Wunderland zu kommen
  File = FileDAP + 512

  //Stringpuffer fÅr Dateilisten-Erzeugung mit Wildcards
  fname  = getmem(516)
  fend   = getmem(516)
  fleft  = getmem(516)
  fright = getmem(516)

  //Sektorpuffer zum sequenziellen Schreiben von Dateien
  fsectors = getmem(1000000) blast(fsectors,1000000,0)

  //Die Sektornummer der 'zuletzt gelîschten Datei', wird vom OS fÅr das Journaling genutzt und ist normalerweise <Null>. Wird von fcreate in die Infonode Åbertragen.
  fjournal = 0
}


/************************************************************
 *                                                          *
 *              DATENTRéGER-PARTITION ANLEGEN               *
 *                                                          * 
 * Der Partitionssektor erfÅllt mehrere Funktionen. Er ist  *
 * das Filesystem-Directory, also der Start des Dateibaums. *
 *                                                          *
 * Gleichzeitig enthÑlt er noch den Kartenstapel, sowie die *
 * Sektornummer des verbleibenden unformatierten Speichers. *
 *                                                          *
 ************************************************************/
far void AliceCreateWonderland(string *Laufwerksbezeichnung,long megabytes)
{
  //Stammverzeichnis mit Daten fÅllen
  blast(Filesystem,512,0)

  //Der Kartenstapel enthÑlt noch keine Karten und mu· erst gefÅllt werden (geschieht beim Formatieren)
  *Filesystem.CardsStack = 0

  //Die Nummer des nÑchsten unformatierten Sektors (bedenke, dass die meisten Festplatten/SSDs heute 8kB-Alignment erwarten, weil sonst Read/Modify/Write stattfindet, was 20% schlechtere Performance mit sich bringt!)
  *Filesystem.ForTheMilliner = 2080

  //Anzahl Sektoren in der Partition (GB * 2048 oder *1048576/512 oder <<20>>9 oder <<11)
  *Filesystem.TotalSpace = (megabytes<<11)

  //Sektor schreiben
  UpdateFilesystem()

  //Directory erstmal komplett leeren
  blast(Directory,512,0)

  //Das Stammverzeichnis hei·t natÅrlich : "Eingang des Kaninchenbau"
  copy(Laufwerksbezeichnung,Directory+60,*Laufwerksbezeichnung.Length+4)

  //Falls Unterverzeichnisse betreten werden, brauchen sie eine RÅckinformation auf das Verzeichnis
  *Directory.parent = 2064

  //Dem Stammverzeichnis folgt z.Zt noch nichts
  *Directory.subdir = 0

  //Das wei·e Kaninchen befindet sich am Eingang zum Wunderland (Verzeichnisse belegen immer nur 1v64 Sektoren bei gleicher Kartengrî·e, damit das Format noch Åbereinstimmt
  *DirDAP.sector=2064

  SaveDir()
}




/************************************************************
 *                                                          *
 *                    ZIEHE EINE KARTE                      *
 *                                                          *
 ************************************************************/
void PickCard()
{
  //Solange der DatentrÑger, bzw. die Partition noch nicht voll ist, lasse alte Dateien unberÅhrt und nimm frischen ungenutzten Speicher
  if(*Filesystem.ForTheMilliner < *Filesystem.TotalSpace)
  {
    //Ein leerer Kartenstapel ist nicht das Ende der Welt
    *FileDAP.sector = *Filesystem.ForTheMilliner

    //Das Ende rÅckt nÑher^^
    *Filesystem.ForTheMilliner += 64
  }
  else
  {
    *FileDAP.sector = *Filesystem.CardsStack

    //Die Karte vom Stapel enthÑlt einen Zeiger auf die darunterliegende Karte
    LoadFile()

    //Darunterliegende Karte auf dem Kartenstapel sichtbar machen
    *Filesystem.CardsStack = *File.next
  }
}


/************************************************************
 *                                                          *
 *               DATEI/VERZEICHNIS ERSTELLEN                *
 *                                                          *
 ************************************************************/
far void fcreate(string *Name,long attribute)
{
  //Systemsektor lesen. Dort liegt die nÑchste Karte oben auf dem Stapel (oder <0>, dann mu· der Hutmacher neue Karten machen)
  ReadFilesystem();

  //Zieh eine neue Karte vom Kartenhaufen (rufe den Hutmacher, wenn keine Karten mehr da sind)
  PickCard()

  //Systemsektor neu schreiben, weil entweder der Kartenstapel oder der "nÑchste freie Sektor" geÑndert wurde
  UpdateFilesystem()

  //Teller ablecken^^ den du gerade vom alten KÅchenstapel genommen hast! Der ist bestimmt noch dreckig!
  blast(File,512,0)

  //Trage den neuen Datei-/Verzeichnisnnamen ein
  copy(Name,File+60,*Name.Length+4)

  //Teile jedem mit, da· es ein Verzeichnis ist
  *File.attributes = attribute

  //Die bisher erste Datei im Verzeichnis wird nun dahintergeklemmt
  *File.next = *Directory.subdir

  //Um auch "rÅckwÑrts" durch Verzeichnisse springen zu kînnen, wird der Sektor des aktuellen Verzeichnisses zum HÑuptling erklÑrt
  *File.parent = *DirDAP.sector

  //Jede Datendatei, die mit 31744 Bytes auskommt, hat keine Datenkette, also auch keinen 'letzte Datenkarte', also mu· beim lîschen auch nichts davon mit dem Kartenstapel gehookt werden
  *File.last = 0

  //Jede neue Datendatei ha, die mit 31744 Bytes auskommt, hat keine Datenkette, also auch keinen 'letzte Datenkarte', also mu· beim lîschen auch nichts davon mit dem Kartenstapel gehookt werden
  *File.journal = fjournal

  //Jede gÅltige Karte kann ihren eigenen Sektor wiedergeben, z.B. damit Dateilisten fÅr jeden Eintrag wenigstens auch den Sektor der Karte kennen o.o
  *File.this = *FileDAP.sector

  //Die neue Spielkarte wird zwischen dem aktuellen Verzeichnis und dem schon existierenden Rest eingeschoben
  *Directory.subdir = *FileDAP.sector

  //Dann das aktuelle Directory und der neue Verzeichniseintrag zurÅckgeschrieben
  SaveDir()
  SaveFile()
}


/************************************************************
 *                                                          *
 *               DATEN IN EINE DATEI SCHREIBEN              *
 *                                                          *
 ************************************************************/
far void fwrite(long *buffer,long size)
{
  long *Dateizeiger,*Sektorliste,DummySektor,i,n,Last

  //Nullbyte-Dateien werden nicht gespeichert!
  if(size==0) { return() }

  //Annehmen, da· keine Datenkarten gebraucht werden
  Last=0

  //Anzahl der Karten fÅr diese Datei berechnen
  if(size<=31744) { *fsectors=0 n=0 }
  else
  {
    n = (size / 31744)

    Sektorliste = fsectors

    DummySektor = *FileDAP.sector

    ReadFilesystem()
    for(i=0,i<n,i+=1) { PickCard(); *Sektorliste = *FileDAP.sector Sektorliste+=4 }
    UpdateFilesystem()

    Last=*FileDAP.sector;

    *Sektorliste=0;
    *FileDAP.sector=DummySektor LoadFile()
  }
  //Dateigrî·e im Header speichern ------------------
  *File.size = size

  //Sektoradresse der letzten Datenkarte ------------
  *File.last = Last

  //Zeiger auf Puffer und Sektor-Array laden --------
  Sektorliste = fsectors

  //Header mit Daten fÅllen -------------------------
  *File.subdir=*Sektorliste

  copy(buffer,File+512,31744) SaveFile() Dateizeiger=buffer+31744

  for(i=0,i<n,i+=1)
  {
    *FileDAP.sector=*Sektorliste Sektorliste+=4;

    blast(File,512,0);

    *File.next=*Sektorliste copy(Dateizeiger,File+512,31744) SaveFile() Dateizeiger+=31744
  }
}



/************************************************************
 *                                                          *
 *            FINDE DATEI IM AKTUELLEN VERZEICHNIS          *
 *                                                          *
 ************************************************************/
far long fsearch(string *filename)
{
  LoadDir()

  //Erste Node des aktuellen Verzeichnisses
  *FileDAP.sector = *Directory.subdir

  //Bei der Dateisuche wird ausserdem noch der "VorgÑnger" zurÅckgeliefert
  fprevious = 0

  //FÅr alle weiteren DateisystemeintrÑge
  while(*FileDAP.sector != 0)
  {
    //Infosektor ins RAM laden
    LoadFile()

    //Es muss nur der Dateiname stimmen, der File-Record ist dann das Ergebnis und wird 'im Stillen' returned
    if(strcmp(File+60,filename) != 0) { return(1) }

    //Da diese Node Åbersprungen wird, wird sie jetzt zum "VorgÑnger"
    fprevious = *FileDAP.sector

    //Die Datei enthÑlt die Nodennummer der folgenden Datei oder NULL, wenn am Ende des Verzeichnisses angekommen
    *FileDAP.sector = *File.next
  }
  return(0)
}




/************************************************************
 *                                                          *
 *               DATEN AUS EINER DATEI LESEN                *
 *                                                          *
 ************************************************************/
far long fread(string *Dateiname)
{
  long *Dateipuffer,Size,Rest,Datenzeiger,NextSector

  //Datei finden und Puffer anlegen ----------------
  if(fsearch(Dateiname)==0) { return(0) }

  Size = *File.size

  Dateipuffer = getmem(Size)
  Datenzeiger = Dateipuffer

  //Ersten Satz kopieren ---------------------------
  if(Size<31744) { Rest = Size  }
  else           { Rest = 31744 }

  copy(File+512,Datenzeiger,Rest)

  //War das noch nicht alles ? ---------------------
  if(Size>31744)
  {
    Size -= Rest Datenzeiger += Rest NextSector = *File.subdir

    while(Size>0)
    {
      //Noch einen Satz laden ----------------------
      *FileDAP.sector = NextSector LoadFile()

      //NÑchsten Satz kopieren ---------------------
      if(Size<31744) { Rest = Size  }

      copy(File+512,Datenzeiger,Rest)

      Size -= Rest Datenzeiger += Rest NextSector = *File.next
    }
  }
  //Fertige Datei im RAM zurÅckgeben ---------------
  return(Dateipuffer)
}



/************************************************************
 *                                                          *
 *                  BITMAP-DATEI LADEN                      *
 *                                                          *
 ************************************************************/
far long fLoadBMP(string *filename)
{
  long   Width,Height
  long   BufferSize,i
  long   *Buffer,*src,*dst,*filebuf
  bitmap *finalbitmap

  //Bitmapdatei îffnen und Abmessungen ablesen
  filebuf = fread(filename)

  src=filebuf+18 Width =*src
  src=filebuf+22 Height=*src

  //Lesepuffer anlegen, weil nur Tripletbytes
  BufferSize = (Width) * (Height)

  //Zielbitmap anlegen, da Grî·e jetzt bekannt
  finalbitmap = getmem(BufferSize*4+32);
  *finalbitmap.next    = 0;
  *finalbitmap.name    = strc(filename);
  *finalbitmap.prev    = 0;
  *finalbitmap.subnode = 0;
  *finalbitmap.ResX    = Width;
  *finalbitmap.ResY    = Height;

  //Lesepuffer in Zielpuffer 'konvertieren'
  src = filebuf+54
  dst = finalbitmap+32
  for(i=0,i<BufferSize,i+=1)
  {
    *dst = *src & 0xffffff
    src += 3
    dst += 4
  }

  //Resourcen freigeben
  free(filebuf)

  //Geladenes Bitmap als Funktionsergebnis zurÅckgeben
  return(finalbitmap)
}


/************************************************************
 *                                                          *
 *                  DATEINAMEN ZERLEGEN                     *
 *                                                          *
 ************************************************************/
far void f_splitfilename(string *filename)
{
  long i,l
  short *c

  //Annehmen, da· beide Ergebnisse murks sind
  *fname.Length = 0;
  *fend.Length  = 0;

  //Suche nach dem '.'-Punkt initialisieren
  c = filename+4
  l = *filename.Length
  i = 0

  //Alle Zeichen bis zum Punkt auszÑhlen
  while(i<l)
  {
    if(*c==46) {break;}
    c+=1 i+=1
  }
  if(i!=0)&&(i<l)
  {
    copy(filename+4,fname+4,i); *fname.Length=i
    copy(c+1,fend+4,l-1-i);     *fend.Length=l-1-i
  }
  else
  {
    copy(filename,fname,*filename.Length+4)
    copy('*',fend,5)
  }
}


/************************************************************
 *                                                          *
 *                 DATEILISTE ERSTELLEN                     *
 *                                                          *
 ************************************************************/
far long f_list(string *searchstring,long attrib,long wayback)
{
  filelist      *liste,*f
  long          n,*f4
  int           *f2
  short         *c,nameflag,endflag

  //Suchmaske in linke und rechte Seite aufsplitten
  f_splitfilename(searchstring)
  copy(fname,fleft ,1000)
  copy(fend ,fright,1000)

  c=fleft +4 nameflag=(*c!=42)
  c=fright+4 endflag =(*c!=42)

  //Neue Liste anlegen
  liste = node(0)

  //Erste Node des aktuellen Verzeichnisses
  *FileDAP.sector = *Directory.subdir

  //FÅr alle weiteren DateisystemeintrÑge
  while(*FileDAP.sector != 0)
  {
    //Infosektor ins RAM laden
    LoadFile()

    //Wie ist es mit Ordnern ? [0=egal, 1=Zeige nur Ordner, 2=Zeige nur Dateien]
    if(*File.attributes==1)
    {
      if(attrib==2) { *FileDAP.sector = *File.next continue; }
    }
    else
    {
      if(attrib==1) { *FileDAP.sector = *File.next continue; }
    }

    //Gefundene Datei vom Namen her in zwei HÑlften aufteilen
    f_splitfilename(File+60)

    //Ist der vordere Teil nicht egal, dann teste, ob er mit dem Suchstring Åbereinstimmt!
    if(nameflag)
    {
      if(strcmp(fname,fleft)==0) { *FileDAP.sector = *File.next continue; }
    }

    //Ist der hintere Teil nicht egal, dann teste, ob er mit dem Suchstring Åbereinstimmt!
    if(endflag)
    {
      if(strcmp(fend,fright)==0) { *FileDAP.sector = *File.next continue; }
    }

    //Dateiliste um einen Eintrag erweitern
    f = node(512) place(f,liste)

    //Akzeptierten Eintrag dorthin kopieren
    copy(File,f+16,512)

    //Dateinamen zusÑtzlich in das 'name'-Feld kopieren
    *f.name = strc(File+60)

    //Die Datei enthÑlt die Nodennummer der folgenden Datei oder NULL, wenn am Ende des Verzeichnisses angekommen
    *FileDAP.sector = *File.next
  }

  //Wird ein '..'-Ordner zur Navigation benîtigt und befinden wir uns noch nicht im Stammverzeichnis des Dateisystems ? -> Baue zusÑtzlich zu den Dateien einen '..'-Ordner ein.
  if(wayback==1)&&(*DirDAP.sector!=2064)
  {
    //Dateiliste um einen (fake-)Eintrag erweitern    
    f = node(512) place(f,liste) blast(f+16,512,0)

    //Eintrag als 'Ordner' markieren
    *f.attributes = 1

    //Namen des Ordners '..' eintragen
    *f.name = strc('..')

    //Das aktuelle Verzeichnis hat eine nÑchsthîhere Ebene, in die zurÅckgesprungen werden kann
    *f.this = *Directory.parent
  }
  return(liste)
}


/************************************************************
 *                                                          *
 *                RANDOM EINTRAG AUSWéHLEN                  *
 *                                                          *
 ************************************************************/
far long f_random(string *mask)
{
  filelist *files,*thisfile,*fileselected
  long i,prize
  string *winner

  files = f_list(mask,2,0)  

  //Erste Chancen vergeben
  thisfile=*files.next prize=0
  while(thisfile!=0)
  {
    i=random(10000)
    if(i>prize)
    {
      prize=i
      fileselected=thisfile
    }
    thisfile=*thisfile.next
  }
  winner = strc(*fileselected.name)

  xerase(*files.next)
  free(files)
  return(winner)
}

/************************************************************
 *                                                          *
 *                    VERZEICHNIS SETZEN                    *
 *                                                          *
 ************************************************************/
far long fsetdir(string *Verzeichnisname)
{
  long *chr

  //Ist der Verzeichnisname etwa '..' ?
  chr=Verzeichnisname+4 if(*chr & 0xffff == 0x2e2e)
  {
    *DirDAP.sector = *Directory.parent
     LoadDir()
  }
  else
  {
    //Aktuelles Verzeichnis nach Eintrag durchsuchen
    if(fsearch(Verzeichnisname)==0)||(*File.attributes!=1) { return(0) }

    //Verzeichnisdaten Åbernehmen
    *DirDAP.sector = *FileDAP.sector copy(File,Directory,512)
  }
  //Verzeichnis-Node zurÅckgeben
  return(1)
}


/************************************************************
 *                                                          *
 *              VERZEICHNISKETTE DURCHLAUFEN                *
 *                                                          *
 ************************************************************/
far long fcd(string *Pfadkette)
{
  short *v,*p,c,weitere
  string *Verzeichnis
//  drivelist *l
  long e
  short *dp


  //Verzeichniskette darf nicht Null oder zu kurz sein!
  if(Pfadkette         == 0) { return(0) }
  if(*Pfadkette.Length == 0) { return(0) }

  //Stringpuffer fÅr einzelne Verzeichnisse in der Pfadkette
  Verzeichnis = getmem(1000)

  //Annehmen, da· es mehr ein Ordner in der Kette ist
  weitere     = 1

  //Laufadresse und Ende der Pfadkette
  p           = Pfadkette+4
  e           = *Pfadkette.Length + Pfadkette+4
  dp          = p+1

  //Ist das zweite Zeichen ein ':', so ist das erste der Laufwerksbuchstabe
  if(*dp==58)
  {
//    l = *p
//    fdrive(l-65)
    fdrive(*p - 65)
    p+=2
  }

  //Falls der Pfad mit einem '\'-Backslash beginnt->absoluter Pfad
  if(*p==92) { *DirDAP.sector=2064 LoadDir() p+=1 }

  //FÅr alle Ordner in der Verzeichniskette
  while(weitere)
  {
    //Alle Zeichen bis zum nÑchsten Backslash oder Pfadende kopieren
    v = Verzeichnis + 4

    while(1)
    {
      //Zeichen aus Pfadkette entnehmen
      c = *p

      //Bei Backslash erstmal unterbrechen und das jÅngste Verzeichnis setzen
      if(c==92) { p+=1      break; }

      //Normale Zeichen werden kopiert
      *v=c v+=1 p+=1

      //War es das letzte Zeichen der Pfadkette ?
      if(p>=e)  { weitere=0 break; }
    }
    //LÑngenbyte des Ordnernamens speichern
    *Verzeichnis.Length = v-Verzeichnis-4

    //Ordnernamen auf korrekte LÑnge prÅfen ('\\\')
    if(*Verzeichnis.Length==0) { continue; }

    //Sprung ins neue Verzeichnis machen, auf Fehler achten
    if(fsetdir(Verzeichnis)==0) { free(Verzeichnis) return(0) }
  }
  //Puffer am Ende freigeben und return TRUE
  free(Verzeichnis) return(1)
}



/************************************************************
 *                                                          *
 *              DATEI/VERZEICHNIS UMBENENNEN                *
 *                                                          *
 ************************************************************/
far void frename(string *filename1,string *filename2)
{
  //Existiert eine Datei unter diesem Namen ?
  if(fsearch(filename1)==1)
  {
    //Lîsche den alten Namen
    blast(File+60,452,0)

    //Trage den neuen Namen ein
    copy(filename2,File+60,*filename2.Length+4)

    //Dateisystem auf den neuesten Stand bringen
    SaveFile()
  }  
}


/************************************************************
 *                                                          *
 *                DATEI/VERZEICHNIS LôSCHEN                 *
 *                                                          *
 * Um den GrundsÑtzen des Multitasking zu genÅgen, werden   *
 * keine Sammeloperationen durchgefÅhrt, d.h. Es werden nur *
 * nur 'einzelne' Dateien gelîscht.                         *
 * Sollen ganze Verzeichnisse gelîscht werden, mu· das OS   *
 * rekursiv erst die Dateien und dann den Ordner lîschen.   *
 * Das kann dann in gewohnter Weise am Bildschirm Åberwacht *
 * werden.                                                  *
 *                                                          *
 * Bei Datendateien werden die Karten so aneinandergelegt:  *
 * <Datei>-<Datenschlange>-<Kartenstapel>                   *
 *                                                          *
 * Bei kurzen Datendateien/Verzeichnissen:                  *
 * <Datei/Verzeichnis>-<Kartenstapel>                       *
 *                                                          * 
 * Wird die erste Datei im Verzeichnis gelîscht, wird das   *
 * Directory aktualisiert, ansonsten der VorgÑnger.         *
 *                                                          *
 ************************************************************/
far void ferase()
{
  long This,Next,Last

  This=*FileDAP.sector
  Next=*File.next
  Last=*File.last

  ReadFilesystem()

  //Bei Datendateien mÅssen die ganzen Datensegmente auf den Kartenstapel zurÅckgelegt werden, damit der enthaltene Speicher nicht verlorengeht und weiter genutzt werden kann (Recycling lebt vom "mitmachen" :-)
  if(*File.attributes==0)&&(Last!=0)
  {
    //Datenrest wird an die Datei angehÑngt
    *File.next = *File.subdir SaveFile()

    //Der bestehende Kartenstapel wird dann an den Datenrest angehÑngt
    *FileDAP.sector = Last;
    *File.next = *Filesystem.CardsStack SaveFile()

    //Die Datei liegt jetzt oben auf dem Kartenstapel
    *Filesystem.CardsStack = This UpdateFilesystem()
  }  
  else
  {
    //(Klein-)Datei/Ordner als freie Karte zurÅck auf den Kartenstapel legen
    *File.next = *Filesystem.CardsStack SaveFile(); *Filesystem.CardsStack = This UpdateFilesystem()
  }

  //Die erste Datei im Verzeichnis stellt einen Sonderfall dar, weil sie mit dem Directory verknÅpft ist
  if(fprevious == 0) { *Directory.subdir = Next SaveDir() }
  else
  {
    *FileDAP.sector = fprevious LoadFile();
    *File.next      = Next      SaveFile();
  }
}

far void fdelete(string *filename)
{
  if(fsearch(filename)!=0) { ferase() }
}


/************************************************************
 *                                                          *
 *              AKTUELLEN PFAD ALS PFADKETTE                *
 *                                                          *
 * 1. Das Stammverzeichnis wird als '\' angegeben           *
 * 2. Alle von dieser Funktion erzeugten Pfade sind absolut *
 *    und beginnen mit '\'                                  *
 *                                                          *
 ************************************************************/
long fdriveletter()
{
  string *result
  short  *p

  result = strc('A:\')

  p = result+4; *p = DriveNumber+65

  return(result)
}


far long froot()
{
  string *DriveLetter,*Mitte,*Ende

  //Zuerst den Laufwerksbuchstaben generieren
  DriveLetter = fdriveletter()

  //Stammverzeichnis erkannt ? Dann nur die Laufwerksbezeichnung zurÅckgeben
  if(*Directory.this   ==    0) { return(DriveLetter)              }

  //Erste Verzeichnisebene ? Dann nur die Laufwerksbezeichnung + Verzeichnisnamen zurÅckgeben
  if(*Directory.parent == 2064) { Ende = stra(DriveLetter,Directory+60) free(DriveLetter) return(Ende) }


  Ende = strc(Directory+60)

  //NÑchsthîhere Verzeichnisebene finden
  *FileDAP.sector = *Directory.parent LoadFile()

  while(1)
  {
    Mitte = stra(File+60,'\')

    Mitte = stra(Mitte,Ende) free(Ende) Ende = Mitte

    //Ist die nÑchsthîhere Verzeichnisebene das Root ? Dann abbrechen..
    if(*File.parent==2064) { break; }

    //Ansonsten nÑchsthîheres Verzeichnis laden und wiederholen
    *FileDAP.sector = *File.parent LoadFile()
  }
  Ende = stra(DriveLetter,Mitte) free(Mitte) free(DriveLetter) return(Ende)
}






far void ClearDisk()
{
  *FileDAP.sector=2048

  blast(File,32256,0)
  SaveFile();
}




//Datei-Endung umbenennen ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒØ
far void fsetext(string *Datei,string *Endung)
{
  long *chr
  long *end

  //Die Endung sind immer die letzten 4 Buchstaben (einschlie·liche des Punktes)
  chr = Datei + *Datei.Length

  end = Endung+4;

  *chr = *end
}
//Ende ALICE.INC