@echo off

if "%LANG%" == "" goto NoLang
if not exist BLAHBLAH.%LANG% goto NoLang
QSCROLL.EXE BLAHBLAH.%LANG%
goto Done
:NoLang
QSCROLL.EXE BLAHBLAH.EN
:Done
