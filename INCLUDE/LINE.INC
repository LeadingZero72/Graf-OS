/************************************************************
 * DRAW LINE                      18.12.2018 Alexander Graf *
 ************************************************************/
far void Line(long x0,long y0,long x1,long y1, long q,long size)
{
  long DeltaX,DeltaY,StepXA,StepYA,StepXB,StepYB,Count,Pendel

  //X komplett links au·erhalb ? Dann abbrechen..
  if(x0<viewMinX)&&(x1<viewMinX) {return()}

  //Y komplett oben au·erhalb ? Dann abbrechen..
  if(y0<viewMinY)&&(y1<viewMinY) {return()}

  //X komplett rechts au·erhalb ? Dann abbrechen..
  if(x0>viewMaxX)&&(x1>viewMaxX) {return()}

  //Y komplett unten au·erhalb ? Dann abbrechen..
  if(y0>viewMaxY)&&(y1>viewMaxY) {return()}

  //LÑnge der Linie ist Null ? Dann nur ein Fitzelchen aufmalen..
  if(x0==x1)&&(y0==y1) {Box(x0-size,y0-size,x0+size,y0+size,q) return()}

  //Deltas fÅr die Steigung berechnen
  DeltaX = x1-x0
  DeltaY = y1-y0

  //X an linkem Rand schneiden
  if(x0<viewMinX)
  {
    y0=(viewMinX-x0)*DeltaY/DeltaX+y0
    x0=viewMinX
  }

  //X an rechtem Rand schneiden
  if(x0>viewMaxX)
  {
    y0=(viewMaxX-x0)*DeltaY/DeltaX+y0
    x0=viewMaxX
  }

  //Y an oberem Rand schneiden
  if(y0<viewMinY)
  {
    x0=(viewMinY-y0)*DeltaX/DeltaY+x0
    y0=viewMinY
  }

  //Y an unterem Rand schneiden
  if(y0>viewMaxY)
  {
    x0=(viewMaxY-y0)*DeltaX/DeltaY+x0
    y0=viewMaxY
  }

  //Neue Deltas berechnen, damit der Linienzeichner die richtige LÑnge abtastet
  DeltaX = x1-x0
  DeltaY = y1-y0
  StepXA = 1
  StepYA = 1

  if(DeltaX<0) { DeltaX= -DeltaX StepXA=-1 }
  if(DeltaY<0) { DeltaY= -DeltaY StepYA=-1 }

  StepXB = StepXA
  StepYB = StepYA

  if(DeltaX<DeltaY) { Count=DeltaX DeltaX=DeltaY DeltaY=Count StepXB=0 }
  else              { StepYB=0 }

  Count  = DeltaX
  DeltaY = DeltaY * 2
  
  Pendel = DeltaY - DeltaX
  DeltaX = Pendel - DeltaX

  while( (Count--) > 0)
  {
    Box(x0-size,
        y0-size,
        x0+size,
        y0+size,
        q)

    if(Pendel<0) { x0+=StepXB y0+=StepYB Pendel+=DeltaY }
    else         { x0+=StepXA y0+=StepYA Pendel+=DeltaX }
  }
}
//Ende LINE.INC