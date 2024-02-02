/*

  EINFACHES KALEIDOSKOP, LOL                14.Januar 2019
                                          Alexander Graf


  Der linke, obere Quadrant soll gespiegelt werden.



*/
far void kaleidoskop(bitmap *source,bitmap *dest,long mode)
{
  long DeltaX,DeltaY
  long *q,qs
  long *z,zs
  long i,j
  long jump

  DeltaX = (*source.ResX)/2
  DeltaY = (*source.ResY)

  jump=DeltaX*4

  /*
     Links nach rechs spiegeln
  */
  if(mode==2) DeltaY/=2

  q=source+32
  z=dest+32+(*dest.ResX)*4-4
  zs=(*dest.ResX)*4+jump

  for(j=0,j<DeltaY,j++)
  {
    for(i=0,i<DeltaX,i++)
    {
      *z=*q
      q+=4
      z-=4
    }
    q=q+jump
    z=z+zs
  }

  /*
     Obere auf Untere Bildh„lfte spiegeln
  */
  if(mode==2)
  {
    q=source+32
    zs=(*dest.ResX)*4
    z=dest+32+((*dest.ResY)-1)*zs
    for(i=0,i<DeltaY,i++)
    {
      copy(q,z,zs)
      q=q+zs
      z=z-zs
    }
  }
}
//Ende KALEIDOS.INC