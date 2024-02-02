/************************************************************
 * BLAST - ROUTINEN               29.12.2018 Alexander Graf *
 ************************************************************/
far void blast(long *addr,long Menge,long Wert) { asm
{
        long v1,v2,v3,v4
        mov ecx,Menge
        mov eax,Wert
        cmp ecx,8
        jb  no64b
        emms
        mov v1,eax
        mov v2,eax
        mov v3,eax
        mov v4,eax
        xld xmm0,v1
        plq mm0,v1
  loopb:cmp ecx,16
        jb n128b
        xst xmm0,*addr
        add addr,16
        sub ecx,16
        jmp loopb
  n128b:test cl,8
        je  no64b
        psq mm0,*addr
        add addr,8
        sub cl,8

  no64b:test cl,4
        je  no32b
        mov *addr,eax
        add addr,4
        sub cl,4

  no32b:test cl,2
        je  no16b
        mov *addr,ax
        add addr,2
        sub cl,2

  no16b:test cl,cl
        je  no8b
        mov *addr,al
  no8b:
}}


far void copy(long *quelle,long *ziel,long Menge_) { asm
{
        mov ecx,Menge_
        emms
  loopc:cmp ecx,16
        jb n128c
        xld xmm0,*quelle
        xst xmm0,*ziel
        add quelle,16
        add ziel,16
        sub ecx,16
        jmp loopc

  n128c:test cl,8
        je no64c
        plq mm0,*quelle
        psq mm0,*ziel
        add quelle,8
        add ziel,8
        sub cl,8

  no64c:test cl,4
        je no32c
        mov eax,*quelle
        mov *ziel,eax
        add quelle,4
        add ziel,4
        sub cl,4

  no32c:test cl,2
        je no16c
        mov ax,*quelle
        mov *ziel,ax
        add quelle,2
        add ziel,2
        sub cl,2

  no16c:test cl,1
        je no8c
        mov al,*quelle
        mov *ziel,al
  no8c:
}}
//Ende BLAST.INC