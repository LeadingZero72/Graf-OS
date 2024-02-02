/************************************************************
  PCI-SCHREIB/LESEZUGRIFF                   13.Juli 2019
                                           Alexander Graf
 ************************************************************/

/*
; PCI equates
; PCI function address (PFA)
; bit 31 = 1
; bit 23:16 = bus number     (0-255)
; bit 15:11 = device number  (0-31)
; bit 10:8 = function number (0-7)
; bit 7:0 = register number  (0-255)

  Die Adressierung Åber Ports geschieht wie folgt: Der erste
  Zugriff geht an den Indexport, wo gleichzeitig Bus, GerÑt,
  Funktion und Registernummer Åbergeben werden, zusammen
  mit Bit 31=1 und Bit 0 und 1 = 0, da hier Flags drin sind.
  Es îffnet sich jetzt ein Dword-Fenster an Port 0xcfc, wo
  der Dword-alignten Pciregisteradresse eingeblendet wird.
  Jetzt kînnen einzelne Bytes von den Adressen 0xcfc..0xcff
  gelesen und geschrieben werden, oder aber ein Dword von
  Adresse 0xcfc oder zwei Words von 0xcfc und 0xcfe.
*/
public long pci_busdevicefunction

void pciselect(long reg)
{ asm {
        mov dx,0xcf8
        mov eax,pci_busdevicefunction
        shl eax,8
        or  eax,0x80000000
        mov al,reg
        shr al,2
        shl al,2
        outd
        mov dx,0xcfc
}}

far short pcirb(long reg)
{
        pciselect(reg) asm {

        mov ax,reg
        and ax,3
        add dx,ax
        inb
}}

far int pcirw(long reg)
{
        pciselect(reg) asm {
        mov ax,reg
        and ax,2
        add dx,ax
        inw
}}

far long pcird(long reg)
{
        pciselect(reg) asm {
        ind
}}

far void pciwb(long reg,long data)
{
        pciselect(reg) asm {
        mov ax,reg
        and ax,3
        add dx,ax
        mov al,data
        outb
}}

far void pciww(long reg,long data)
{
        pciselect(reg) asm {
        mov ax,reg
        and ax,2
        add dx,ax
        mov ax,data
        outw
}}

far void pciwd(long reg,long data)
{
        pciselect(reg) asm {
        mov eax,data
        outd
}}
//Ende PCI.CPP