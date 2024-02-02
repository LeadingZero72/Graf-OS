/************************************************************
 *                    GETMEM-ROUTINE                        *
 ************************************************************/
  public long MLA
  public long MLMid
  public long MLEnd

far long getmem(long Menge)
{
  long *index
  long *Header
  long *element
  long *Previous
  long *Tail
  long *Backtrack

  //Annehmen, es seien 8 Bytes mehr (wegen Header)
  Menge+=8

  //Ab der H„lfte der Liste noch zu klein ?
  //Dann die erste H„lfte der Liste berspringen
  index = MLMid
  if(*index<Menge) { index = MLMid }
  else             { index = MLA   }

  //Liste mit Speichergr”áen nach kleinster passender
  //Gr”áe durchsuchen
  while(index<MLEnd)&&(*index<Menge) { index+=8 }

  //Gengend groáer Block vorhanden ?
  if(index>=MLEnd) { return(0) }

  //Zeiger auf Memory-Container-List laden
  index-=4
  Header=*index

  //Erstbestes Element nehmen
  element = *Header

  //!!Wenn Null, dann kein freier Speicher mehr!!
  if(element==0) { return(0) }

  //Element-Nachfolger laden, sei er auch Null (end of list)
  Tail = *element

  //Rest der Liste an den Header rckverbinden und
  //damit das erste Element ausschlieáen (wird reserviert)
  *Header=Tail

  //Liste in den Container schreiben, um sp„ter die
  //Freigabe zu beschleunigen
  Backtrack  = element+4;
  *Backtrack = Header

  //Die Adresse des Speicherblocks liegt nun 8 Bytes hinter
  //der Managerstruktur
  return( (element+8) )
}


/************************************************************
 *                    GETMEM-ROUTINE                        *
 ************************************************************/
far void free(long *addr)
{
  long *index
  long *Header
  long *element
  long *Previous
  long *Tail
  long *Backtrack

  if(addr==0) {return()}

  //4 Bytes davor liegt ein Zeiger auf die Liste, wo das herkommt
  Backtrack=addr-4
  Header=*Backtrack

  //Das erste Element der Liste laden (sei es auch EoL)
  Tail=*Header

  //Der RAM-Container liegt 8 Bytes VOR der Adresse
  element=addr-8

  //Rest der Containerliste an freizugebenden Container anknpfen
  *element=Tail

  //Freizugebenden Container jetzt an den Header anfgen
  *Header=element
}
//Ende MEMORY.INC