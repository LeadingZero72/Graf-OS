/*

  ERSTE TYPENDEFINITIONEN

*/

#typedef short,1,0xff      ,0
#typedef int  ,2,0xffff    ,1
#typedef long ,4,0xffffffff,2
#typedef void ,4,0xffffffff,3



struct list
{
  long *Next
  long *Reg
  long *Head
  long *Prev
  long Lists
  long Arrays
}

struct color
{
  short Blue,Green,Red,Alpha  
}

//  list Container
struct bitmap
{
  long *next
  long *name
  long *prev
  long *subnode
  long Lists
  long Arrays

  int ResX,ResY
  short Shift,Flags
}

//Ende SYSTEM.INC