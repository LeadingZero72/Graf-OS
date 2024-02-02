/*

  KEYBOARD.INC               31.M„rz 2019 Alexander Graf


*/


  extern long *KeyBuffer
  extern short KeyBufStart
  extern short KeyBufEnd

  extern short KeyExtended,KeyCTRL,KeyALT,KeyTAB,KeySHIFT,KeyCAPS

far short getch();

//Ende KEYBOARD.H