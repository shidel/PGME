@echo off

call make eternity
bin\pgm.exe /BAT %0 %1 %2
type c:\temp\pgmegorn.bat