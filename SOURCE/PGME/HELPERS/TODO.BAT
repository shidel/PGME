@echo off

if "%LANG%" == "" goto NoLang
if not exist SOMEDAY.%LANG% goto NoLang
QSCROLL.EXE SOMEDAY.%LANG%
goto Done
:NoLang
QSCROLL.EXE SOMEDAY.EN
:Done
