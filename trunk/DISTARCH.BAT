@echo off

echo Creating Program Manager Eternity distribution archives.

if exist ARCHIVE\DEFAULTS.ZIP del ARCHIVE\DEFAULTS.ZIP
if exist ARCHIVE\INSTALL.ZIP del ARCHIVE\INSTALL.ZIP

c:\dos\pkz204g\pkzip.exe -en -r ARCHIVE\DEFAULTS.ZIP BIN\DEFAULTS\*.*
c:\dos\pkz204g\pkzip.exe -en ARCHIVE\INSTALL.ZIP INSTALL\INSTALL.EXE