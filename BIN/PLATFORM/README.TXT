
  PLATTFORM-SPEZIFISCHE CPU-CODES

  Um die Zielplattform variabel zu halten, wird der
  Compiler nur Prozessorbefehle einer bestimmten
  Architektur verwenden, um seine LOADs, CALLs und
  OPs umzusetzen. Die passenden Mnemonics sind f�r
  jede Plattform getrennt in einer Datei gespeichert.

  Die Plattformcodes f�r Compiler und Assembler sind
  nicht gleich! Der Compiler verwendet Mnemonics,
  um daraus eine Assemblerdatei zu erstellen. Der
  Assembler ben�tigt anstelle dessen die Bin�rcodes,
  um den ausf�hrbaren Code zu erzeugen.

  IA387.CPC    386er / 387er (MMX/SSE/REX)
  C64.CPC      Commodore 64
  AMIGA.CPC    Amiga
  COMODORE.CPC Comodore
  GAMEBOY.CPC  Nintendo Gameboy
  NES.CPC      Nintendo Entertainment System NES
  SNES.CPC     Super Nintendo Entertainment System

  In der Theorie hat sich gezeigt, da� ein C-Programm auf
  jeder Hardware zum laufen gebracht werden kann, so soll
  es sein ;-)