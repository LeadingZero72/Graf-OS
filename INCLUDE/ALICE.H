/*

      ALICE IM WUNDERLAND                   23.Juni 2019
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
  long NeedUpdate
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
extern drivelist     *DriveList
extern long           DriveNumber
extern long           DriveCount

extern long           DriveNumberA
extern long           DriveNumberB
extern long           DriveNumberC
extern long           DriveNumberD

extern diskaddresspacket *FilesystemDAP
extern diskaddresspacket *DirDAP
extern diskaddresspacket *FileDAP

extern filesystem    *Filesystem
extern infocard      *Directory
extern infocard      *File

//Die Sektornummer der 'zuletzt gelîschten Datei', wird vom OS fÅr das Journaling genutzt und ist normalerweise <Null>. Wird von fcreate in die Infonode Åbertragen.
extern long fjournal






far void UpdateFilesystem();
far void ReadFilesystem();
far void SaveFile();
far void LoadFile();
far void SaveDir();
far void LoadDir();


/************************************************************
 *                                                          *
 *                ALICE - LAUFWERKSWECHSEL                  *
 *                                                          *
 * öbergeben wird die Laufwerks-Indexnummer (0=A,1=B...)    *
 * Heraus kommt der alte Ordner im neuen Laufwerk, als      *
 * Kopie von dem String-Pointer aus der Drive-Liste.        *
 *                                                          *
 ************************************************************/

far long fdrive(long DriveIndex);
far long fgetdrive();
far void fdriveupdate(string *neuerpfad);

far void fcreate(string *Name,long attribute);
far void fwrite(long *buffer,long size);
far long fsearch(string *filename);
far long fread(string *Dateiname);
far long fLoadBMP(string *filename);
far void f_splitfilename(string *filename);
far long f_list(string *searchstring,long attrib,long wayback);
far long f_random(string *mask);

far long fsetdir(string *Verzeichnisname);
far long fcd(string *Pfadkette);

far void frename(string *filename1,string *filename2);

far void ferase();
far void fdelete(string *filename);

far long froot();

far void fsetext(string *Datei,string *Endung);

far void ClearDisk();
//far void Transferfolder(long *buffer);
//far void Make(string *Laufwerksbezeichnung);
far void AliceInit();
far void AliceCreateWonderland(string *Laufwerksbezeichnung,long megabytes);


//Ende ALICE.H