/************************************************************
 * RECTANGLE.INC                   9.01.2019 Alexander Graf *
 ************************************************************
 * Bei einer Box ist das Clipping wichtig, damit die Box    *
 * auch auáerhalb eines Bildes liegen kann.                 *
 *                                                          *
 * Liegt die Box links, ist Breite-x die neue Breite und x  *
 * wird Null.                                               *
 * Liegt die Box oben, geht das gleiche Spiel mit der H”he. *
 * Liegt sie rechts, dann ist ResX-x die neue Breite.       *
 * Liegt sie unten, dann ist ResY-y die neue H”he.          *
 *                                                          *
 *                                                          *
 ************************************************************/

far void Rectangle(long x0,long y0,long x1,long y1, long q);



//Ende RECT.H