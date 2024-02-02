/*

  TREE.H                               31.Januar 2019
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
far long node(long size);

//###########################################################
//Schneidet Container aus
//###########################################################
far void cut(long there);

//###########################################################
// H„ngt Node an eine andere Node an fgt dabei ein
//###########################################################
far void place(long item, long location);

//###########################################################
//L”scht von einem bestimmten Container ausgehend, den gesamten Tieferliegenden Rest
//###########################################################
far void erase(long there);

//###########################################################
//L”scht einzelnen Container und schneidet ihn aus
//###########################################################
far void clear(long there);

/************************************************************

   NAMEN IN LISTE FINDEN

   Es wird eine Liste nach einem bestimmten
   Namen durchsucht und die gefundene Node
   zurckgegeben.

   Die Strings werden nur dann verglichen,
   wenn die L„ngen bereinstimmen.

 ************************************************************/
far long find(string *what,tree *where);

//Ende TREE.INC