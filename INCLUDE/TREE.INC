/*

  TREE.INC                               31.Januar 2019
                                        Alexander Graf


  "WUCHERNDE LISTEN oder die Wurzeln des Kaninchenbaus"


  Einzelne Speicherbausteine k”nnen untereinander verh„kelt
  und zusammenh„ngend bearbeitet werden.


  Dabei k”nnen dann die berhmten Schneidetechniken verwendet
  werden.


  Es k”nnen ganze Datenb„ume wie Zeiger verwendet werden.


  "Man muá sich in keinster Weise mit Listenerzeugung
   befassen, sondern fgt einfach die Daten zusammen."


*/

//###########################################################
//Das ist die einzige Struktur, die ben”tigt wird, damit der Hase l„uft^^
//###########################################################
struct tree
{
  long *next
  long *name
  long *prev
  long *subnode
}



//###########################################################
//Erzeugt leeren Container bestimmter Gr”áe
//###########################################################
far long node(long size)
{
  tree *this
  this = getmem(size+16);

  *this.next=0;
  *this.name=0;
  *this.prev=0;
  *this.subnode=0;

  return(this)
}




//###########################################################
//Schneidet Container aus
//###########################################################
far void cut(long there)
{
  tree *this,*a,*b

  //Parameter holen und die Container links und rechts davon
  this = there
  a=*this.prev
  b=*this.next

  //Beide H„lften miteinander verbinden
  *a.next=b

  //Die andere H„lfte muá natrlich auch zurckverbunden werden, sofern nicht am Ende
  if(b!=0) *b.prev=a

  //Alte Verbindungen des Containers l”schen
  *this.next=0;
  *this.prev=0
}


//###########################################################
// H„ngt Node an eine andere Node an fgt dabei ein
//###########################################################
far void place(long item, long location)
{
  tree *this,*end,*follower

  this     = item
  end      = location
  follower = *end.next

  //Einzufgende Node auf der linken Seite verbinden
  *end.next=this

  //Links von dem neuen Container ist der Anschluápunkt, sei es auch der Anfang der Liste
  *this.prev = end;
  *this.next = follower;

  //Eventuellen Rest an der eingefgten Node zurckverbinden
  if(follower!=0) *follower.prev=this
}


//###########################################################
//L”scht von einem bestimmten Container ausgehend, den gesamten Tieferliegenden Rest
//###########################################################
far void erase(long there)
{
  tree *this,*a

  this = there

  while(this!=0)
  {
    //Untere Datens„tze nicht vergessen
    erase(*this.subnode)

    //Registrierungs-String nicht vergessen
    if(*this.name!=0) free(*this.name)

    //RAM-Baustein freigeben
    free(this)

    //Wohlwissend, daá da noch nichts ver„ndert wurde...wird noch nachtr„glich aus dem Baustein gelesen ;-P
    this=*this.next
  }
}


//###########################################################
//L”scht einzelnen Container und schneidet ihn aus
//###########################################################
far void clear(long there)
{
  tree *me

  me = there

  if(me!=0)
  {
    erase(*me.subnode)
    cut(me)
    if(*me.name!=0) free(*me.name)
    free(me)
  }
}


/************************************************************

   NAMEN IN LISTE FINDEN

   Es wird eine Liste nach einem bestimmten
   Namen durchsucht und die gefundene Node
   zurckgegeben.

   Die Strings werden nur dann verglichen,
   wenn die L„ngen bereinstimmen.

 ************************************************************/
far long find(string *what,tree *where)
{
  //Alle Elemente der Liste durchsuchen
  while(where!=0)
  {
    //Schnellen Stringvergleich durchfhren
    if(strcmp(*where.name,what)!=0) { return(where) }

    //oder weiter in den Elementen suchen O.o
    where=*where.next
  }
  //Wenn keine šbereinstimmung, Null zurckgeben .(
  return(0)
}

//Ende TREE.INC