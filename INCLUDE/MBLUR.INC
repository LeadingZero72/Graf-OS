/*

   BILDBLENDEN UND MOTION-BLUR              15.Januar 2019
                                              Alexander Graf


  "Nirvana steht fÅr einen Ort, an dem es keine
   Geheimnisse mehr gibt. - indischer Gelehrter"


*/
far void motionblur(bitmap *new,bitmap *old,bitmap *dest,long newmix,long oldmix) { asm
{
        //Pixelmenge ausrechnen
        emms
        xor eax,eax
        xor ecx,ecx
        mov ax,*dest.ResX
        mov cx,*dest.ResY
        imul ecx
        mov ecx,eax

        //Zeiger auf Daten vorbereiten
        add new,32
        add old,32
        add dest,32

        //Blendfaktoren in MM2 und MM3 als Words parallel laden
        pld       mm2,newmix
        punpcklbw mm2,mm2
        punpcklwd mm2,mm2
        punpcklbw mm2,mm7
        pld       mm3,oldmix
        punpcklbw mm3,mm3
        punpcklwd mm3,mm3
        punpcklbw mm3,mm7


        //Nur halb so viele dank MMX-Twinpixel-Routine Oo
        shr ecx,1

        //32Bit-AND-Mask in MM4 laden
        mov       eax,0xffffffff
        pldr      mm4,eax

        //2 Pixel aus jeder Quelle laden
lCopyMB:

        plq       mm5,*new
        plq       mm0,*old

        //Low und high Dword in zwei MMX-Register aufspalten
        pmov      mm1,mm0
        pmov      mm6,mm5
        punpckhdq mm1,mm7
        punpckhdq mm6,mm7
        pand      mm0,mm4
        pand      mm5,mm4

        //Alle Bytes in Words konvertieren, um zu multiplizieren
        punpcklbw mm0,mm7
        punpcklbw mm1,mm7
        punpcklbw mm5,mm7
        punpcklbw mm6,mm7

        //Altgewicht einberechnen
        pmullw    mm0,mm2
        pmullw    mm1,mm2
        pmullw    mm5,mm3
        pmullw    mm6,mm3

        //Entsprechende Bits rauskÅrzen
        pshimw    mm0,8
        pshimw    mm1,8
        pshimw    mm5,8
        pshimw    mm6,8

        //Neu + Alt zusammenmischen
        paddusw   mm0,mm5
        paddusw   mm1,mm6

        //16Bit-RGBWerte wieder in Bytes umwandeln
        packuswb  mm0,mm0
        packuswb  mm1,mm1

        //Pixel 1&2 zusammenpacken und speichern
        punpckldq mm0,mm1
        psq       mm0,*dest

        //Zeiger weitersetzen und Schleife abschlie·en
        add new,8
        add old,8
        add dest,8
        dec ecx
        jne lCopyMB
}}
//Ende MBLUR.INC