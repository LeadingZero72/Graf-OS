/*     .                                                              .
 ╝дддддеддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддеддддд╞
       Ё                        COMPILER-APP                          Ё
 ╝дддддеддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддеддддд╞
       Ё                                                              Ё
       Ё COMPILE.APP, <CPP -> Assembler> Compiler 1.0                 Ё
       Ё Date: 16.Januar 2016                                         Ё
       Ё Author: Alexander Graf                                       Ё
       Ё                                                              Ё
       Ё Launcher f│r COMPILE.DLL, macht Aufruf an COMPILE.DLL und    Ё
       Ё zeigt Laufzeitstatistiken an, wertet die CompilerfehlernummerЁ
       Ё aus, wenn n■tig.                                             Ё
       Ё                                                              Ё
       Ё Erwartet: f=Quelldatei                                       Ё
       Ё           p=Quellpfad                                        Ё
       Ё           r=Ausf│hrungspfad                                  Ё
       Ё Register: o=Zielpfad                                         Ё
       Ё           s=Datei-Endung der Zieldatei                       Ё
       Ё           t=Plattformspezifikationsdatei (zB. 'IA386.INI')   Ё
       Ё           h=Returnpointer f│r Ergebnis/Fehlernummer          Ё
       Ё                                                              Ё
       юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды    */
  stack(100) if(a) strcpy('%c661C%c000-COMPILER 1.00%n%n%n%c696Beyond scripting...',a) exit()

/*      .                                                              .
  ╝дддддеддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддеддддд╞
        Ё                     HILFSROUTINEN LADEN                      Ё
        юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды    */

  cd(ProgPath) Occlude('SCANNERS.DLL')
  wait(100)

  cd(ProgPath) Include('SCANNERS.DLL')
  wait(100)


/*      .                                                              .
  ╝дддддеддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддеддддд╞
        Ё                      APP-DATENSEGMENT                        Ё
        юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды    */
  a=Program append(0,1000,1000,a[DXCameras],@x)
  blast(x+24,4000+1000,0)

/*      .                                                              .
  ╝дддддеддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддеддддд╞
        Ё                       COMPILER STARTEN                       Ё
        юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды    */
  Regs
  strcpy(p,@w)
  strcpy(f,@z)
  strcpy(p,@o)
  strcpy('.ASM',@s)
  strcpy('IA387.CPC',@t)
  strcpy(r,@u)
  cd(ProgPath) call('COMPILE.DLL') f=z p=w o=o s=s h=x+4024 r=u t=t //Aufruf an Compiler...

  Exclude('SCANNERS.DLL')

  //F│r den Fall, daА alles ohne Fehler ablief ------------
  if(x[4024]==0)
  {
    free(f,p)
    Play(8)
    x[4024+40]=-1                                               //Quanshow beenden, endet sonst nie...
    exit()
  }
  Play(1)
/*      .                                                              .
  ╝дддддеддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддеддддд╞
        Ё                         ERRORCHECKING                        Ё
        юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды    */
  y=x+24
  strcpy('All right.',y)                             y=y+4          //00
  strcpy('Unexpected end of file in SkipSome()'  ,y) y=y+4          //01
  strcpy('Unexpected end of file in SkipSpaces()',y) y=y+4          //02
  strcpy('Unexpected end of file in SkipLine(),1',y) y=y+4          //03
  strcpy('Unexpected end of file in SkipLine(),2',y) y=y+4          //04
  strcpy('Unexpected end of file in SkipLine(),3',y) y=y+4          //05
  strcpy('Unexpected end of file in CopyLine(),1',y) y=y+4          //06
  strcpy('Unexpected end of file in CopyLine(),2',y) y=y+4          //07
  strcpy('Unexpected end of file in CopyLine(),3',y) y=y+4          //08
  strcpy('Wrong token in Need()'                 ,y) y=y+4          //09
  strcpy('Unexpected end of file in Scan()'      ,y) y=y+4          //10
  strcpy('Invalid Argument in Scan()'            ,y) y=y+4          //11
  strcpy('Invalid digit in Number()'             ,y) y=y+4          //12
  strcpy('Too many hex-nibbles in Number()'      ,y) y=y+4          //13
  strcpy('Illegal hex-characters in Number()'    ,y) y=y+4          //14
  strcpy('Too many binary-bits in Number()'      ,y) y=y+4          //15
  strcpy('Illegal binary characters in Number()' ,y) y=y+4          //16
  strcpy('No output generated!'                  ,y) y=y+4          //17
  strcpy('Unexpected end of file in long comment',y) y=y+4          //18
  strcpy('Duplicate name in typedef'             ,y) y=y+4          //19
  strcpy('Structure name must begin alphanumeric',y) y=y+4          //20
  strcpy('Structure name already exists'         ,y) y=y+4          //21
  strcpy('Structure unknown type identifier used',y) y=y+4          //22
  strcpy('Function is illegal type for structure',y) y=y+4          //23
  strcpy('Invalid instruction'                   ,y) y=y+4          //24
  strcpy('Right parenthesis ..) expected'        ,y) y=y+4          //25
  strcpy('Too many open parenthesis'             ,y) y=y+4          //26
  strcpy('Invalid Argument'                      ,y) y=y+4          //27
  strcpy('Structure has no members'              ,y) y=y+4          //28
  strcpy('Unknown Member'                        ,y) y=y+4          //29
  strcpy('Codelimit reached'                     ,y) y=y+4          //30
  strcpy('Illegal base data type'                ,y) y=y+4          //31
  strcpy('Datatype stack overrun'                ,y) y=y+4          //32
  strcpy('Datatype stack underrun'               ,y) y=y+4          //33
  strcpy('Debug'                                 ,y) y=y+4          //34
  strcpy('Not a structure, members illegal'      ,y) y=y+4          //35
  strcpy('unused error 36'                       ,y) y=y+4          //36
  strcpy('Block: Unexpected }'                   ,y) y=y+4          //37
  strcpy('Blocks left open at end of main file'  ,y) y=y+4          //38
  strcpy('Function declaration needs right parenthesis',y) y=y+4    //39
  strcpy('Function parameter has unknown type'   ,y) y=y+4          //40
  strcpy('Duplicate function parameter'          ,y) y=y+4          //41
  strcpy('Illegal parameter type '               ,y) y=y+4          //42
  strcpy('Function: Too many parameters '        ,y) y=y+4          //43
  strcpy('Function: No parameters declared '     ,y) y=y+4          //44
  strcpy('Too many blocks '                      ,y) y=y+4          //45
  strcpy('Are there more blocks of that we should know ?'         ,y) y=y+4          //46
  strcpy('Addressoperator needs variable '       ,y) y=y+4          //47
  strcpy('Literal cannot be incremented/decremented ',y) y=y+4      //48
  strcpy('Literal cannot be dereferenced '       ,y) y=y+4          //49
  strcpy('Type cannot become dereferenced '      ,y) y=y+4          //50
  strcpy('Too many dereference operators '       ,y) y=y+4          //51
  strcpy('Variable already defined on heap '     ,y) y=y+4          //52




  strcpy('%c595Compiler 1.00',@t)                           //Titelzeile der Messagebox
  a=x[4024] x[4024]=x[a>>2+24]                              //Fehlernummer durch Text ersetzen
  FText('%c600ERROR %n%n  %c111%s%nin line %da    %nof file %s',x+4024) 
  d=Data cd(SysRoot)                                        //Messagebox aufrufen
  task('MSGBOX.DLL') a=0 t=t u=q x=256 y=10                 //

/*      .                                                              .
  ╝дддддеддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддеддддд╞
        Ё                WORDPAD MIT QUELLCODE AUFRUFEN                Ё
        юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды    */
/*  wait(Frame)

  FChangeExt(f,'.ASM') x=a
  cd(p) FTest(x) if(a)
  {
    strcpy(p,@y)                                                      //Quellpfad f[r [bergabe kopieren
    strcpy('C:\SYSTEM',@r)
    cd(r) task('WORDPAD.APP') f=x p=y r=r a=0
  }
*/
  free(f,p)

//Ende COMPILE.APP