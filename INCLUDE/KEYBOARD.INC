/*

  KEYBOARD-TREIBER                            28.Juni 2019
                                            Alexander Graf

;®ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¯
;
; ScanCode-Liste
;      2..14:   Digits '1'..'0','á',DEL
;
;      ESC  F1 F2 F3 F4 F5 F6 F7 F8 F9 F10  F11 F12
;       1   59 60 61 62 63 64 65 66 67 68   87  88
;
;      '^'  '1' .. '`'  '#'  DEL
;       41   2     13   43    14
;
;      TAB     Q .. š    '+'  ENTER
;       15    16    26   27    28
;
;      CAPS   A .. 
;       58   30    40
;
;      SH_L  '<|>'   Y..'-'   SH_R
;       42    86    44   53    54
;
;      Strg  Alt        Space       AltGr  Strg
;       29    56         57           56    29
;          Win                          Doc
;           91                           93
;
;                       Print: 55/42  Scroll: 70  Pause 69
;                       Einfg:    82  Pos1:   71  BILDU 73
;                       Entf:     83  Ende:   79  BILDD 81
;
;                                          72 Oben
;                                 75 <-               -> 77
;                                          80 Unten
;
;®ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¯
*/
  public long *KeyBuffer
  public short KeyBufStart
  public short KeyBufEnd

  public short KeyExtended,KeyCTRL,KeyALT,KeyTAB,KeySHIFT,KeyCAPS



/*®ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¯
 *                 Tastatur-Handler IRQ1
 *®ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¯*/
far void Handler9()
{
  short *adr
  asm {
        push ax                       //CPU-Register sichern
        push dx                       //
        push ebx                      //
        push edi                      //(wegen Pointerzugriff ^.^)
        puds                          //
        xor ebx,ebx                   //EBX l”schen
        push bx                       //
        pods                          //Flatmode Segment setzen
        mov dx,0x60                   //Scancode vom Controller lesen
        inb                           //

        cmp al,224                    //Erweiterter Code 0E ?
        je DoExt                      //Dann IRQ1 beenden und auf das folgende Zeichen warten, welches gleich hier eintrifft...
        mov ah,al                     //und in AH zwischenspeichern.
        and al,127                    //Unten-/Oben-Bit ausmaskieren,
        cmp al,69                     //'Pause' gedrckt ?
        je AbortIt                    //N” ? Dann weiter.. Sonst Konsole
        mov al,ah                     //Scancode in AL
        mov dl,ah                     //(sichern)
        and ah,127                    //Tastencode in AH
        shr al,7                      //Break-Bit in LSB
        xor al,1                      //1=gedrckt, 0=losgelassen

        cmp ah,186                    //Caps-Lock (Break-Code 58+128)?
        je DoCAPS                     //
        cmp ah,29                     //Control ?
        je DoCTRL                     //
        cmp ah,56                     //Alternate ?
        je DoALT                      //
        cmp ah,15                     //Tabulator ?
        je DoTABU                     //
        xor al,KeyCAPS                //Caps Lock bercksichtigen und SHIFT-Tasten erkennen
        cmp ah,42                     //Shift-L ?
        je DoSHIFT                    //
        cmp ah,54                     //Shift-R ?
        je DoSHIFT                    //

        mov bl,KeyBufStart            //Schreibposition im Puffer
        mov al,bl                     //
        add al,1                      //und die Schreibposition verschoben.
        and al,7                      //Der Puffer l„uft zirkul„r durch 
        cmp al,KeyBufEnd              //ist aber voll, sobald die n„chste
        je kBufFul                    //Adresse beim Lesezeichen liegt.
        add ebx,KeyBuffer             //
        mov adr,ebx                   //
        mov *adr,dl                   //
        mov KeyBufStart,al            //Neue Schreibposition speichern
kBufFul:


h9Rdy:  mov KeyExtended,0             //Codes verarbeitet -> Extended-Flag AUS
h9Rdy2: pods                          //
        pop edi                       //
        pop ebx                       //
        cli
        mov dx,0x20                   //EoI an Controller senden
        mov al,0x20                   //
        outb                          //
        pop dx                        //
        pop ax                        //
        mov sp,bp                     //
        pop bp                        //
        sti
        iret                          //und ret far, pop Flags...


//®ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¯
DoSHIFT:mov bl,KeyExtended            //Extended-Shift-Breakcodes sollen ignoriert werden, damit Shift-Cursor zur Textauswahl verwendet werden kann.
        test bl,1                     //
        jne h9Rdy                     //
        mov KeySHIFT,al               //
        jmp h9Rdy                     //
DoCAPS: xor KeyCAPS,1                 //
        jmp h9Rdy                     //
DoTABU: mov KeyTAB,al                 //
        jmp h9Rdy                     //
DoCTRL: mov KeyCTRL,al                //
        jmp h9Rdy                     //
DoALT:  mov KeyALT,al                 //
        jmp h9Rdy                     //
DoExt:  mov KeyExtended,1             //Erweiterten Tastencode ankndigen
        jmp h9Rdy2                    //
AbortIt:mov dx,0x20                   //EoI an Controller senden
        mov al,0x20                   //
        outb                          //

        mov dx,0x64                   //EoI an Controller senden
        mov al,0xfe                   //
        outb                          //

lforev: nop
        nop
        nop
        jmp lforev                    //
  }

}


void KeyboardInit()
{
  int *adr

  asm {
        mov adr,36
        mov ax,Handler9
        mov *adr,ax
        mov adr,38
        pucs
        pop ax
        mov *adr,ax
      }

  KeyBuffer = getmem(16)

  KeyTAB      = 0
  KeyALT      = 0
  KeyCTRL     = 0
  KeySHIFT    = 0
  KeyCAPS     = 0
  KeyExtended = 0
  KeyBufStart = 0
  KeyBufEnd   = 0
}


far short getch()
{
  short *chr
  short a
  
  a=KeyBufEnd

  if(a!=KeyBufStart)
  {
    chr = KeyBuffer+a;

    KeyBufEnd=((a+1)&7)

    return(*chr)
  }
  return(0)
}
//Ende KEYBOARD.INC