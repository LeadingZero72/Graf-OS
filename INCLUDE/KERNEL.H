/*
  KERNEL-STRUKTUREN UND VARIABLEN           3.April 2019
                                          Alexander Graf
*/
struct globalvariable
{
  long *next,*name,*prev,*subnode
  long platzhalter1
  long platzhalter2
  long base
  long id
}

struct globalfunction
{
  long *next,*name,*prev,*subnode
  long platzhalter1
  long platzhalter2
  long offs
  long segm
  long id
}

struct globalprocess
{
  long *next,*name,*prev,*subnode
  long *root
  long *seg
  long *heap
  long  enable
  long  interval
  long  time
  long  mode
  long *caller
  long *return
}

  extern globalprocess  *processes
  extern globalfunction *functions
  extern globalvariable *variables

//Ende KERNEL.H