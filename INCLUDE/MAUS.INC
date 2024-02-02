/*

  SENIOR MAUS                               28.Juni 2019
                                          Alexander Graf
*/

public long  MausX
public long  MausY
public long  MausDX
public long  MausDY
public long  MausDeltaX
public long  MausDeltaY
public long  MausWheel
public short MausButtons
public short MausEvent



/*----------------------------------------------------------------------------
 *
 * DIE öBERTRAGUNGSPROTOKOLLE
 *
 * MÑuse:       Y,X,Buttons,-
 * Wheel-MÑuse: W,Y,Buttons,X
 * Buttonflags: Y,X,Ysign,Xsign,1,Button3,Button1,Button2
 *
 * "Das Wort zum Offset"
 * push eax&edx = 8 Bytes + 2 Bytes fÅr DS.
 * Dann nochmal 4 Byte fÅr den Far-return
 *
 *  MDOfs=14
 *  Konfiguration 1: 0=Wheel, 2=Y, 4=X, 6=Buttons
 *  Konfiguration 2:
 *----------------------------------------------------------------------------*/
/*

  Wie fake ich jetzt den Stackzugriff ??

  wenn ich mov bp,sp sub sp,xx rechne, zeigt bp tatsÑchlich
  auf den original stackpointer. Der Trick bei den lokalen
  Variablen ist aber der, da· hier negative Offsets
  verwendet werden und die sind in die Opcodes gehardcodet

  Ich mu· BP also positiv verschieben, damit es wieder passt

  DafÅr brauche ich dann auch nicht mehr soviele Dummies...





*/


far void MausBiosHandler()
{
  int i0,i1,i2,i3
  asm
  {
        cli                          //Interrupts sperren
        push eax                     //Register retten
        push edx                     //
        puds                         //

        xor ax,ax                    //4GB Daten
        push ax                      //
        pods                         //

        push bp
        add bp,14
        mov al,i0                    //
        shl al,4                     //
        movsx eax,al                 //
        mov MausWheel,eax            //

        mov al,i1                    //
        movsx eax,al                 //
        mov MausDeltaY,eax           //

        mov al,i2                    //
        movsx eax,al                 //
        mov MausDeltaX,eax           //

        mov al,i3                    //Mausbuttons in AL
        mov ah,MausButtons           //Alte Mausbuttons in AH
        and al,3                     //obere Bits lîschen
        mov MausButtons,al           //Buttons speichern

        test al,1
        je MausCL
MausNoL:test al,2
        je MausCR
MausNoR:

        pop bp
        pods                         //
        pop edx                      //
        pop eax                      //
        sti                          //

        mov sp,bp
        pop bp
        retf

MausCL: test ah,1                    //ButtonL war vorhin noch gedrÅckt ?
        je MausNoL                   //Wenn nicht, dann kein Klick
        or MausEvent,4               //Ansonsten Klick-Ereignis speichern
        jmp MausNoL                  //..

MausCR: test ah,8
        je MausNoR
        or ah,8
        jmp MausNoR



  }
}






/*----------------------------------------------------------------------------
 *
 * - DIE BIOSMAUS -                                   Mai 2018, Alexander Graf
 *
 * Dem erleuchtenden Beispiel von Cutemouse folgend, gehe ich hier wieder
 * einmal den Weg Åber das Bios und die Funktionen von int15h, lol.
 *
 * Die Maus kann geprobet und dann mit Werten programmiert werden, ganz simpel.
 * DarÅberhinaus wird eine far-Prozedur Åbergeben, die dann in Echtzeit als
 * Maushandler dient, d.h. die Daten korrigiert und ans OS leitet.
 *
 * Zur Initialisierung wird zunÑchst geprÅft, ob das Bios PS/2 anbietet.
 * Danach werden die gewÅnschte Packetgrî·e und die Zeigerauflîsung
 * eingegeben. Ein far-Zeiger auf die Handlingroutine wird Åbergeben.
 *
 * Der Abschaltvorgang geht dann Ñhnlich. Die Maus wird deaktiviert und
 * der far-Zeiger auf Null gesetzt, damit keine Packete mehr gesendet
 * werden.
 *
 * Die Handlingroutine mu· auf den Stack zugreifen, um die Werte von der
 * Maus zu bekommen und darf selbst keine Register verletzen, damit das
 * Bios intakt bleibt!
 *
 *----------------------------------------------------------------------------*/
void MausBiosInit()
{
  asm
  {
            intr 0x11                  //Equipmentliste in AL
            test al,4                  //PS/2-UnterstÅtzung ?
            je NoBiosMouse             //Wenn nicht, keine Biosmaus...

            mov bh,3                   //Standard 3-Byte Packete
            mov ax,0xc205              //
            intr 0x15                  //

            mov bh,200                 //Abtastrate festlegen
            mov ax,0xc202              //
            intr 0x15                  //

            mov bh,3                   //Auflîsung 2^BH Counts/mm
            mov ax,0xc203              //
            intr 0x15                  //

            mov bx,MausBiosHandler     //Handleradresse eintragen
            pucs                       //
            poes                       //
            mov ax,0xc207              //
            intr 0x15                  //

            mov bh,1                   //Maus jetzt einschalten
            mov ax,0xc200              //
            intr 0x15                  //


            xor eax,eax
            mov MausX,eax
            mov MausY,eax
            mov MausWheel,eax
            mov MausButtons,al
            mov MausEvent,al
NoBiosMouse:

  }
}





//Ende MAUSBIOS.INC