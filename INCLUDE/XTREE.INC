/*

  XTREE.INC                               31.Januar 2019
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
//L”scht von einem bestimmten Container ausgehend, den gesamten Tieferliegenden Rest
//###########################################################
far void xerase(long *there)
{
  tree *this,*a

  this = there

  //Die Liste wird bei diesem Element mit EOL abgeschlossen
  a=*this.prev;*a.next=0

  while(this!=0)
  {
    //Untere Datens„tze nicht vergessen
    a=*this.subnode if(a!=0) { xerase(*a.next) free(a) }

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
far void xclear(long *there)
{
  tree *me,*a

  me = there

  if(me!=0)
  {
    //Untere Datens„tze nicht vergessen
    a=*me.subnode if(a!=0) { xerase(*a.next) free(a) }

    cut(me)
    if(*me.name!=0) free(*me.name)
    free(me)
  }
}


//###########################################################
//Findet den letzten Container in der Liste, ausgehend vom Header oder irgendeinem Container mittendrin
//###########################################################
far long endoflist(tree *this)
{
  //Solange das n„chste Element nicht EOL ist, skippe weiter durch die Liste
  while(*this.next!=0) { this = *this.next }

  //Letzten Container zurckgeben
  return(this)
}

//###########################################################
//Findet Container anhand des Index
//###########################################################
far long search(tree *this,long index)
{
  this = *this.next

  //Solange das n„chste Element nicht EOL ist, skippe weiter durch die Liste
  while(this!=0)&&(index!=0)
  {
    index-=1
    this = *this.next
  }

  //Container zurckgeben
  return(this)
}




//Ende XTREE.INC