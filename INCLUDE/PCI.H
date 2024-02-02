/************************************************************
  PCI-SCHREIB/LESEZUGRIFF                   13.Juli 2019
                                           Alexander Graf
 ************************************************************/
extern int pci_busdevicefunction

far short pcirb(long reg);
far int   pcirw(long reg);
far long  pcird(long reg);
far void  pciwb(long reg,long data);
far void  pciww(long reg,long data);
far void  pciwd(long reg,long data);

//Ende PCI.CPP