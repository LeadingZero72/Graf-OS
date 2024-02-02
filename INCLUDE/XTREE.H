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
far void xerase(long *there);

//###########################################################
//L”scht einzelnen Container und schneidet ihn aus
//###########################################################
far void xclear(long *there);

//###########################################################
//Findet den letzten Container in der Liste, ausgehend vom Header oder irgendeinem Container mittendrin
//###########################################################
far long endoflist(tree *this);

//###########################################################
//Findet Container anhand des Index
//###########################################################
far long search(tree *this,long index);


//Ende XTREE.H