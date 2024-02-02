/*
  DESKTOP-FEATURES                        2.November 2019
                                           Alexander Graf
*/

//Struktur fÅr Desktop-Fenster --------------------------
struct syswindow
{
  long *next,*Image,*prev,*subnode

  long x0,y0
  long x1,y1
  long mx,my

  long *process

  short isin
}

//Liste mit Desktop-Fenstern ----------------------------
extern syswindow *syswins

//Parameterdatei -----------------------------------------
extern string         *ParamA,*ParamB,*ParamC,*ParamD

//Scrollvariablen fÅr den Hintergrund ---------------------
extern bitmap *DesktopScroller
extern long    DesktopScrollX
extern long    DesktopScrollY
extern long    DesktopScrollDX
extern long    DesktopScrollDY

//Pfade von System und Desktop ----------------------------
extern string         *DesktopRoot
extern string         *SystemRoot

//Desktop-ZustÑnde ----------------------------------------
extern short DesktopIconsVisible
extern short DesktopMouseVisible
extern short DesktopBackgroundVisible
extern short DesktopInactive
extern short DesktopWinActive

//Keyboard-Scancode und Scancode-Tabelle
extern short scancode,*scantable

//Fensterfarben -------------------------------------------
extern long WndDark
extern long WndShade
extern long WndNormal
extern long WndBright
extern long WndHighlight
extern long WndWhite

//Icongrî·e/Symbolviewport --------------------------------
extern long DesktopIconSize
extern long DesktopIconSpace
extern long DesktopIconMinX,DesktopIconMinY
extern long DesktopIconMaxX,DesktopIconMaxY

extern bitmap *oldtarget

//Zwischenablage fÅr Dateien ==============================
extern filelist *SelFile
extern long      SelSize,SelFiles,SelFolders
extern long      SelDirectory

//Schriftarten --------------------------------------------
extern fontstruct *DarkFont,*WhiteFont

//Aktivierung mit Maus-Klicks -----------------------------
extern long Activation

//Mauszeiger-Bitmap ---------------------------------------
extern bitmap *MausBitmap

//Benutzername --------------------------------------------
extern string *DesktopUser
extern string *UserPasswordA
extern string *UserPasswordB

//Desktop-Thema -------------------------------------------
extern string *DesktopThemeName,*DesktopThemeBitmapName

//Zwischenablage ------------------------------------------
extern tree   *Clipboard

//Setzt öbergabeparameter fÅr Dateien ----------------------
far void setp(string *file,string *path);

//Einfache Freigabe der Programmparameter ------------------
far void freep();

//Zeichenroutinen fÅr Fenster -------------------------------
far void WndHLine(long x0,long y0,long x1,bitmap *Image,long c);
far void WndVLine(long x0,long y0,long y1,bitmap *Image,long c);
far void WndBar(long x0,long y0,long x1,long y1,bitmap *Image,long c0,long c1);
far void WndDrawWindow(long u,long v,long x,long y,bitmap *Image);
far void WndDrawWhite(long u,long v,long x,long y,bitmap *Image);

//Fenster-Routinen ---------------------------------------
far void RestoreTarget();
far long TargetWindow(syswindow *this);
far long CreateWindow(long width,long height,string *title,string *text);
far void DeleteWindow(syswindow *this);
far void DesktopRefresh();

//Ende DESKTOP.H