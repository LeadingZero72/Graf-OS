/*
   Dieses Programm dient zur Anzeige verschiedener statistischer 
   Systemdaten, wie z.B. der verbleibende Speicher. W„hrend der
   Ausfhrung dieser Anwendung ist der Disk-Writeback blockiert.
*/
  stack(100) 
  if(a) strcpy('My Memory Monitor',a) exit()

  private AppStatus        = "e[508]"

  private MLA              = "8362016"
  private MLIndex          = ">>3"
  private MLSize           = "4"
  private MCHeader         = "8"
  private MChipSizes       = "22"  //19

  private TooBigNumber     = "999999999"
  private Total            = "g"
  private List             = "i"
  private Index            = "j"
  private WindowBMP        = "k"
  private WindowColor      = "d[512]"
  private TextColor        = "d[536]"
  private Text             = "t"
  private Color            = "f"
  private New              = "v"
  private Old              = "u"
  private Red              = "255>>16"
  private Green            = "255>>8"

  Regs

  cd(ProgPath) Init(D_ResX,D_ResY,MT_Force) free(ProgPath)
  Window(512,256,512,'Memory-Monitor',0)

  getmem(u,MChipSizes*4,v,MChipSizes*4) blast(v,MChipSizes*4,0) c=MLA

  e=Program d=Data while(AppStatus&256==0)
  {
    wait(Frame)
    Receive() while(a)
    {
      WinCope() if(a) break 2;
    }

    e=Program d=Data
    if(AppStatus&16==0) continue;

    Index=0 Total=0  
    while(Index<MChipSizes)
    {
      Color=0
      b=c[Index MLIndex]
      List=b[0]
      n=TooBigNumber
      skip(@i,@n)
      n=n-New[Index>>2]
      if(n>Old[Index>>2]) Color=Green
      if(n<Old[Index>>2]) Color=Red 
      Old[j>>2]=n

      x=c[Index MLIndex + MLSize] - MCHeader
      y=n
      z=x*n
      Total=Total+z 
      Box(5,Index*8+30,500,Index*8+38,WindowBMP,WindowColor)
      Fprint(5,Index*8+30,500,Index*8+38,WindowBMP,'Size: %di * %dh = %da    ',@x,WindowColor,TextColor|Color)
      Index=Index+1
    }
    Index=Index+1
    x=Total
    Box(5,Index*8+30,500,Index*8+38,WindowBMP,WindowColor)
    Fprint(5,Index*8+30,500,Index*8+38,WindowBMP,'Total RAM free: %da Bytes',@x,WindowColor,TextColor)



    if(KeyALT) copy(u,v,MChipSizes*4) blast(u,MChipSizes*4,0)

    render()
  }
  UnIcon(Program)
//Ende MEMMON.APP