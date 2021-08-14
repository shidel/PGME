@echo off

if "%LANG%" == "" goto NoLang
if not exist WHATSUP.%LANG% goto NoLang
QSCROLL.EXE WHATSUP.%LANG%
goto Done
:NoLang
QSCROLL.EXE WHATSUP.EN
:Done
