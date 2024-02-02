/*

    DISPLACEMENT-MAPPED BITMAPS            15.Januar 2019
                                           Alexander Graf




*/
far short displacementmap(bitmap *s1,bitmap *dmap,bitmap *dest)
{asm{
        long *pixel

        //Datenmenge ausrechnen
        xor eax,eax
        xor ebx,ebx
        mov bx,*dmap.ResX
        mov ax,*dmap.ResY
        imul ebx
        mov ecx,eax

        //Bitmaps auf Start der Bilddaten setzen
        add s1,32
        add dmap,32
        add dest,32

lCpyDM:
        //XY aus Displacementmap laden (unsigned EAX:EBX)
        mov eax,*dmap
        mov edi,eax
        movzx eax,ax
        shr edi,16
        movzx edi,di

        //Adresse im Quellbitmap ausrechnen
        imul ebx
        add eax,edi
        shl eax,2
        add eax,s1
        mov pixel,eax

        //Farbe laden und im Ziel speichern
        mov eax,*pixel
        mov *dest,eax

        //Zeiger weiter und Schleife abschlieáen
        add dest,4
        add dmap,4
        dec ecx
        jne lCpyDM
}}
//Ende DMAP.INC