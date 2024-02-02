/************************************************************
 * RANDOM NUMBER GENERATOR        18.12.2018 Alexander Graf *
 ************************************************************/
  public long Seed

far long random(long Range)
{
  Seed = 1107030247 * Seed +97177
  return(Seed % Range)
}
//Ende RANDOM.INC