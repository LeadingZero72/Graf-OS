# Graf-OS
Handmade 16-bit graphical operating system

I learned how to create c++ compilers and assemblers when I was 5 years old :-) I created several operating systems in my youth. Around 2017 I had to start over because my hard-drives were stolen. It took around two years to recreate my OS.

Features are:
+ MBR boot
+ selfmade filesystem
+ 4GB ram realmode flatmode
+ c-compiler/assembler
+ SSE4.0 instructions supported
+ graphical user interface
+ software graphics (including 3D-engine)
+ software encryption (unbreakable ^.^)
+ BIOS-drivers
+ High definition audio driver (including audio input)
+ Intel Southbridge Clock Throttling (interactive power manager)
- no network drivers
- no graphics hardware drivers
- no USB drivers
- no other filesystems supported (like fat32/ntfs etc.)

Introduction

I used MS-DOS and Turbo Pascal to write a software emulator that could run scripts just like java does. The language looked a bit like java, but was much simplified. Using scripts, I created a compiler/assembler, that uses a fixed area in the conventional memory to run programs written in C. Then I wrote a C-compiler in C to compile it with the script. That was the transition from script to real c-binaries ^.^ I wrote routines for my filesystem, a text editor and uploaded everything to my USB-stick, including an MBR boot program. That was the turning point. I managed to boot my own system from USB-stick and continued working from there. 


