/*
   Dieses Programm zeigt eine Datei als Hexcodes an

   F/P = Dateiname/Pfad der Quelldatei
*/
  stack(100)
  if(a) strcpy('Hex-Code Viewer',a) exit()

  cd(ProgPath) root(@p)
  strcpy('PROTO.BIN',@f)

  if(f==0)||(p==0) exit()

  d=Data
  cd(ProgPath) Init(D_ResX,D_ResY,MT_Force) free(ProgPath)
  Window(512,512,512,'Hexadezimalcodes',0)

  cd(p) open(f,ReadMode) size(@s) getmem(q,s+4) read(q+4,s) close() q[0]=s
  v=0

  e=Program while(e[508]&256==0)
  {
    d=Data
    wait(Frame)
    if(e[508]&16==0) continue;

    Receive() while(a)
    {
      WinCope() if(a) break 2;
    }
    d=Data

    KbGetch()
    if(a==72)&&(v)      v=v-1
    if(a==80)&&(v<q[0]) v=v+1

    Box(5,30,500,490,k,d[512])

    x=0 y=0 j=v
    while(j<q[0])
    {
      w=q[j+4]Char
      c=d[512]
      if(w==0x66)||(q[j+4]Integer==0x6766) c=0xffff00 x=0 y=y+8+4 if(y>490) y=0 break;
      Fprint(x+5,y+30,480,490,k,'%h2',@w,c,d[536])
      j=j+1
      x=x+16+2
      if(x>480) x=0 y=y+8+4 if(y>490) y=0 break;
    }
    render()
  }
  free(f,p,q)

  UnIcon(Program)
//Ende HEXVIEW.APP