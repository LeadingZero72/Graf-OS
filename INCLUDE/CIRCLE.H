/************************************************************
 * CIRCLE/FILLCIRCLE              3.01.2019 Alexander Graf  *
 *                                                          *
 * Wie zeichnet man einen perfekten Kreis ?                 *
 *                                                          *
 * Wenn ich alle Quadranten gespiegelt gleich mache,        *
 * muss ich nur 90 Grad berechnen.                          *
 *                                                          *
 * Wenn ich so anfange, da� der erste Punkt links in der    *
 * Zeile beim Mittelpunkt liegt, kann ich eine horizontale  *
 * Strecke f�llen.                                          *
 *                                                          *
 * Die n�chste Frage ist, welchen Winkel ich brauche, damit *
 * die Zeile sich �ndert. Den benutze ich f�r die n�chste   *
 * x-Koordinate.                                            *
 *                                                          *
 * So �berspringe ich immer wieder Winkel, um zu den neuen  *
 * x-Koordinaten zu kommen.                                 *
 *                                                          *
 * Ein anderer Ansatz w�rde den Umfang in Pixeln berechnen  *
 * und entsprechend hoch aufgel�ste Winkel nehmen.          *
 *                                                          *
 ************************************************************/

  extern long fillcolor


far void elipse(long xm,long ym,long a,long b);
far void fillelipse(long xm,long ym,long a,long b);

//Ende CIRCLE.H