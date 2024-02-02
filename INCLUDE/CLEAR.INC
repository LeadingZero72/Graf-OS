/************************************************************
 * CLEAR BITMAP                 18.12.2018 Alexander Graf   *

  long i

  i=*p.ResX * (*p.ResY)

  p+=32

  while((i--)!=0)
  {
    *p=q
    p+=4
  }
 ************************************************************/
far void clearimage(bitmap *p,long q)
{
  blast(p+32, (*p.ResX) * (*p.ResY) * 4,q)
}
//Ende CLEAR.INC