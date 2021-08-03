@echo off

if "%1" == "dev" goto TestTPC
if "%1" == "clean" goto CheckV8PT
if not "%1" == "" goto %1
if "%1" == "" goto CheckV8PT
goto ErrorExit

REM Simple Test that V8Power Tools for DOS is installed
:CheckV8PT
verrlvl 0
if errorlevel 1 goto ErrorNoV8PT
verrlvl 1
if errorlevel 1 goto TestPushPop
goto ErrorNoV8PT

:TestPushPop
verrlvl 0
pushd
if errorlevel 1 goto ErrorPushPop
cd ETERNITY
if errorlevel 1 goto ErrorPushPop
popd
if errorlevel 1 goto ErrorPushPop
if exist ETERNITY.PAS goto ErrorPushPop

:TestTPC
if not "%TPC%" == "" goto CheckTPC
set TPC=C:\DEVEL\TP70\BIN\TPC.EXE
if exist %TPC% goto FoundTPC
set TPC=%DOSDIR%\LINKS\TPC.COM
if exist %TPC% goto FoundTPC
set TPC=%DOSDIR%\LINKS\TPC.BAT
if exist %TPC% goto FoundTPCBAT
set TPC=C:\DEVEL\TP\BIN\TPC.EXE
if exist %TPC% goto FoundTPC
set TPC=C:\DEVEL\TPC\BIN\TPC.EXE
if exist %TPC% goto FoundTPC
set TPC=C:\TP70\BIN\TPC.EXE
if exist %TPC% goto FoundTPC
set TPC=C:\DEVEL\TP55\BIN\TPC.EXE
if exist %TPC% goto FoundTPC
set TPC=C:\DEVEL\TP55\BIN\TPC.EXE
if exist %TPC% goto FoundTPC
set TPC=
goto ErrorNoTPC

:CheckTPC
if not exist %TPC% goto ErrorNoTPC

:FoundTPC
goto MainBuild

:FoundTPCBAT
set TPC=call %TPC%
goto MainBuild

:MAKE
shift
if "%1" == "" goto End
vecho /fGrey make /fYellow "%1" /fGrey
verrlvl 1
goto MAKE_%1
goto ErrorMAKE

:MAKESUB
shift
if "%2" == "" goto End
vecho /fGrey make /fYellow "%1" /fCyan "%2" /fGrey
pushd
cd %1
verrlvl 1
goto MAKE_%1_%2
goto ErrorMAKE

REM ***************************************************************************
REM Main Build Section
:MainBuild
set SELF=%0
vecho Creating /fWhite Program Manager /fCyan Eternity /fGrey from source.
if "%1" == "dev" goto DevBuildOnly
if "%1" == "clean" goto DevBuildClean

call %SELF% make Clean Prepare
if errorlevel 1 goto ErrorExit
call %SELF% make Templates
if errorlevel 1 goto ErrorExit
call %SELF% make FirstPass Defaults
if errorlevel 1 goto ErrorExit
call %SELF% make NoTemplates SecondPass
if errorlevel 1 goto ErrorExit
call %SELF% make Collect
if errorlevel 1 goto ErrorExit
call %SELF% make Installer
if errorlevel 1 goto ErrorExit
call %SELF% make ReleaseClean
if errorlevel 1 goto ErrorExit
goto Done

:DevBuildOnly
goto Done

:DevBuildClean
    call %SELF% make Clean
goto Done

REM ***************************************************************************
:MAKE_Clean
    verrlvl 0
	if exist BIN\NUL deltree /y BIN
	if errorlevel 1 goto ErrorMake
	if exist INSTALL\NUL deltree /y INSTALL
	if errorlevel 1 goto ErrorMake
:MAKE_ReleaseClean
    verrlvl 0
	if exist QUICKCRT\BIN\NUL deltree /y QUICKCRT\BIN
	if errorlevel 1 goto ErrorMake
	if exist ETERNITY\BIN\NUL deltree /y ETERNITY\BIN
	if errorlevel 1 goto ErrorMake
	if exist ETOOLS\BIN\NUL deltree /y ETOOLS\BIN
	if errorlevel 1 goto ErrorMake
	if exist ESAVER\BIN\NUL deltree /y ESAVER\BIN
	if errorlevel 1 goto ErrorMake
	if exist ETERNITY\INSTALL.INC deltree /y ETERNITY\INSTALL.INC
	if errorlevel 1 goto ErrorMake
	if exist ETERNITY\LICENSE.INC deltree /y ETERNITY\LICENSE.INC
	if errorlevel 1 goto ErrorMake
:MAKE_DevBuildClean
    verrlvl 0
    if exist QUICKCRT\FAILED del QUICKCRT\FAILED
    if exist ETERNITY\FAILED del ETERNITY\FAILED
    if exist ETOOLS\FAILED del ETOOLS\FAILED
    if exist ESAVER\FAILED del ESAVER\FAILED

goto DoneMake

:MAKE_Prepare
    verrlvl 0
	if not exist BIN\NUL mkdir BIN
	if not exist BIN\NUL goto ErrorMAKE

	if exist BIN\LICENSE.TXT goto MAKE_Prep_HasLic
    if exist LICENSE.TXT copy LICENSE.TXT BIN\LICENSE.TXT
    :MAKE_Prep_HasLic

goto DoneMake

:MAKE_Templates
    verrlvl 0
	if exist BIN\TEMPLATE.THM del BIN\TEMPLATE.*
	call %SELF% MAKESUB QuickCRT Templates
	if errorlevel 1 goto ErrorMAKE
	call %SELF% MAKESUB QuickCRT Build
	if errorlevel 1 goto ErrorMAKE
goto DoneMake

:MAKE_NoTemplates
    verrlvl 0
	if exist BIN\TEMPLATE.THM del BIN\TEMPLATE.*
	call %SELF% MAKESUB QuickCRT NoTemplates
	if errorlevel 1 goto ErrorMAKE
	call %SELF% MAKESUB QuickCRT Build
	if errorlevel 1 goto ErrorMAKE
goto DoneMake

:MAKE_firstpass
    verrlvl 0
	call %SELF% MAKESUB Eternity First
	if errorlevel 1 goto ErrorMAKE
	call %SELF% MAKESUB ETOOLS First
	if errorlevel 1 goto ErrorMAKE
goto DoneMake

:MAKE_secondpass
    verrlvl 0
	call %SELF% MAKESUB Eternity Second
	if errorlevel 1 goto ErrorMAKE
	call %SELF% MAKESUB ETOOLS Second
	if errorlevel 1 goto ErrorMAKE
	call %SELF% MAKESUB ESAVER Second
	if errorlevel 1 goto ErrorMAKE
goto DoneMake

:MAKE_Collect
    verrlvl 0
    if exist eternity\bin\ETERNITY.bin move eternity\bin\ETERNITY.bin bin
    if exist eternity\bin\PGME.EXE move eternity\bin\PGME.EXE bin
    if exist etools\bin\EFNTDSGN.exe move etools\bin\*.EXE bin
    if exist esaver\bin\blanker.scr move esaver\bin\*.scr bin
    xcopy /y /d eternity\*.txt bin\

    xcopy /y /d custom\language\*.* bin\
    xcopy /y /d custom\fonts\*.* bin\
    xcopy /y /d custom\themes\*.* bin\
    xcopy /y /d custom\menu\*.* bin\

goto DoneMake

:MAKE_Installer
    verrlvl 0
    if exist INSTALL\NUL deltree /y INSTALL >NUL
    copy /y eternity\bin\install.exe bin
    ETERNITY\bin\add2ins.exe bin
    if errorlevel 1 goto ErrorMAKE
    if not exist INSTALL\NUL mkdir INSTALL
    del bin\install.exe >NUL
    move bin\install.tmp install\install.exe
    if not exist install\install.exe goto ErrorMake

goto DoneMake

:MAKE_Defaults
    verrlvl 0
    pushd
    cd eternity\bin
    if exist DEFAULTS\NUL deltree /y DEFAULTS >NUL
    if not exist DEFAULTS\NUL mkdir DEFAULTS
    pgme.exe
        splice DEFAULT.THM ONDISK.THM
        splice DEFAULT.KMP ONDISK.KMP
        splice DEFAULT.LNG ONDISK.LNG
        copy DEFAULT.THM DEFAULTS\ETERNITY.THM
        copy DEFAULT.KMP DEFAULTS\ETERNITY.KMP
        copy DEFAULT.LNG DEFAULTS\ETERNITY.LNG
        del DEFAULT.*
        del ONDISK.*
    install.exe
        splice DEFAULT.THM ONDISK.THM
        splice DEFAULT.KMP ONDISK.KMP
        splice DEFAULT.LNG ONDISK.LNG
        copy DEFAULT.THM DEFAULTS\INSTALL.THM
        copy DEFAULT.KMP DEFAULTS\INSTALL.KMP
        copy DEFAULT.LNG DEFAULTS\INSTALL.LNG
        del DEFAULT.*
        del ONDISK.*
    popd
    move ETERNITY\BIN\DEFAULTS BIN\DEFAULTS
    if errorlevel 1 goto ErrorMAKE
    if not exist BIN\DEFAULTS\ETERNITY.LNG goto ErrorMake
    pushd
    cd etools\bin
    if exist DEFAULTS\NUL deltree /y DEFAULTS >NUL
    if not exist DEFAULTS\NUL mkdir DEFAULTS
    efntdsgn.exe
        ..\..\eternity\bin\splice DEFAULT.THM ONDISK.THM
        ..\..\eternity\bin\splice DEFAULT.KMP ONDISK.KMP
        ..\..\eternity\bin\splice DEFAULT.LNG ONDISK.LNG
        copy DEFAULT.THM DEFAULTS\EFNTDSGN.THM
        copy DEFAULT.KMP DEFAULTS\EFNTDSGN.KMP
        copy DEFAULT.LNG DEFAULTS\EFNTDSGN.LNG
        del DEFAULT.*
        del ONDISK.*
    popd
    move ETOOlS\BIN\DEFAULTS\*.* BIN\DEFAULTS
    if errorlevel 1 goto ErrorMAKE
    if not exist BIN\DEFAULTS\EFNTDSGN.LNG goto ErrorMake

goto DoneMake


:COMPILE
if exist FAILED then del FAILED>NUL
shift
if "%1" == "" goto VeryEnd
if not exist %1.PAS goto COMPILE_MISSING
vecho /fCyan compile /fWhite %1 /fGrey
if errorlevel 1 goto VeryEnd
%TPC% %TPC_OPTS% %1.PAS
if errorlevel 1 goto COMPILE_FAILED
if exist BIN\%1.TPU goto COMPILE
if exist BIN\%1.EXE goto COMPILE
:COMPILE_FAILED
vecho /p /fyellow /bred " compile %1 failed " /e /fgrey /bblack
echo %1>>FAILED
verrlvl 1
goto VeryEnd
:COMPILE_MISSING
vecho /fLightRed missing /fWhite %1 /fGrey
goto COMPILE

REM ***************************************************************************
:MAKE_ETERNITY_First
:MAKE_ETERNITY_Second
    verrlvl 0
 	if exist INSTALL.INC del INSTALL.INC>NUL
 	if exist LICENSE.INC del LICENSE.INC>NUL
 	if exist BIN\NUL deltree /y BIN
    if not exist BIN\NUL mkdir BIN

	if not exist INSTALL.INC  copy /y INSTALL.PRE INSTALL.INC

    set TPC_OPTS=-M -L -EBIN -UBIN;..\QuickCRT\BIN -I..\QuickCRT
    call ..\%SELF% COMPILE PGME
    if exist FAILED goto ErrorMAKESUB

    REM compile tools and make license include file
    call ..\%SELF% COMPILE ADDLIC ADD2INS EXESIZE SPLICE
    if exist FAILED goto ErrorMAKESUB
    bin\addlic.exe
	if not exist LICENSE.INC ErrorMAKESUB

    REM make adjusted installer stub (2 passes)
    call ..\%SELF% COMPILE INSTALL
    if exist FAILED goto ErrorMAKESUB
    bin\exesize.exe bin\install.EXE
    del bin\install.exe >NUL
    call ..\%SELF% COMPILE INSTALL
    if exist FAILED goto ErrorMAKESUB

    rem build eternity program
    if exist bin\eternity.bin del bin\eternity.bin>NUL
    call ..\%SELF% COMPILE ETERNITY
    if exist FAILED goto ErrorMAKESUB
    move bin\eternity.exe bin\eternity.bin

goto DoneMAKESUB

:MAKE_ETOOLS_First
:MAKE_ETOOLS_Second
    verrlvl 0
 	if exist BIN\NUL deltree /y BIN
    if not exist BIN\NUL mkdir BIN

    set TPC_OPTS=-M -L -EBIN -UBIN;..\QuickCRT\BIN -I..\QuickCRT
    call ..\%SELF% COMPILE EFNTDSGN
    if exist FAILED goto ErrorMAKESUB
    if /I "%2" == "FIRST" goto DoneMakeSub
    REM call ..\%SELF% COMPILE ETOOL
    if exist FAILED goto ErrorMAKESUB
    call ..\%SELF% COMPILE QCNFTOOL QDETUNE QKEYS
    if exist FAILED goto ErrorMAKESUB
    call ..\%SELF% COMPILE QNOTHING QSCROLL
    if exist FAILED goto ErrorMAKESUB
    call ..\%SELF% COMPILE QFNTLOAD QFNT2TSR QFNT2RAW RAW2QFNT
    if exist FAILED goto ErrorMAKESUB

goto DoneMAKESUB

:MAKE_ESAVER_First
    verrlvl 0
goto DoneMAKESUB

:MAKE_ESAVER_Second
    verrlvl 0
 	if exist BIN\NUL deltree /y BIN
    if not exist BIN\NUL mkdir BIN

    set TPC_OPTS=-M -L -EBIN -UBIN;..\QuickCRT\BIN -I..\QuickCRT
    call ..\%SELF% COMPILE BLANKER ATOM
    if exist FAILED goto ErrorMAKESUB
    if exist bin\blanker.exe move bin\blanker.exe bin\blanker.scr
    if exist bin\atom.exe move bin\atom.exe bin\atom.scr
    if exist bin\conway.exe move bin\conway.exe bin\conway.scr

goto DoneMAKESUB

REM ***************************************************************************

REM ***************************************************************************
:MAKE_QuickCRT_Templates
    verrlvl 0
    copy /y template.on template.def
    if errorlevel 1 goto ErrorMAKESUB
goto DoneMakeSub

:MAKE_QuickCRT_NoTemplates
    verrlvl 0
    copy /y template.off template.def
    if errorlevel 1 goto ErrorMAKESUB
goto DoneMakeSub

:MAKE_QuickCRT_Build
    verrlvl 0
	if exist BIN\NUL deltree /y BIN
    if errorlevel 1 goto ErrorMAKESUB
	if not exist BIN\NUL mkdir BIN
    set TPC_OPTS=-M -L -EBIN -UBIN

    call ..\%SELF% COMPILE QOBJECTS QCRT QSTRINGS QERRORS QDOS
    if exist FAILED goto ErrorMAKESUB
    call ..\%SELF% COMPILE QBIOS QFILES
    if exist FAILED goto ErrorMAKESUB
    call ..\%SELF% COMPILE QDIR QVESA
    if exist FAILED goto ErrorMAKESUB
    call ..\%SELF% COMPILE QCONTROL QDIRCNTL QFILEDLG QLOG
    if exist FAILED goto ErrorMAKESUB
    call ..\%SELF% COMPILE QMORE QSCRNSVR QSPEAKER QTIMER
    if exist FAILED goto ErrorMAKESUB
    call ..\%SELF% COMPILE QTEST
    if exist FAILED goto ErrorMAKESUB
goto DoneMakeSub

REM ***************************************************************************
:DoneMAKESUB
popd
shift

:DoneMAKE
if errorlevel 1 goto ErrorEnd
if exist FAILED goto ErrorEnd
if exist QUICKCRT\FAILED goto ErrorEnd
if exist ETERNITY\FAILED goto ErrorEnd
if exist ETOOLS\FAILED goto ErrorEnd
if exist ESAVER\FAILED goto ErrorEnd
shift
if not "%1" == "" call %SELF% make %1 %2 %3 %4 %5 %6 %7 %8 %9
if errorlevel 1 goto ErrorEnd
goto VeryEnd

:ErrorMakeSub
echo >>FAILED
popd
goto ErrorEnd

:ErrorNoV8PT
echo V8Power Tools for DOS required to build PGME.
goto End

:ErrorPushPop
echo Unsupported COMMAND shell. PUSHD and POPD support required.
verrlvl 1
goto VeryEnd

:ErrorNoTPC
echo Unable to find Turbo Pascal Compiler.
verrlvl 1
goto VeryEnd

:ErrorExit
vecho /fYellow /bRed " Error building PGME" /e /fGrey /bBlack
:ErrorEnd
:ErrorMake
verrlvl 1
goto End


:Done
if exist QUICKCRT\FAILED goto ErrorExit
if exist ETERNITY\FAILED goto ErrorExit
if exist ETOOLS\FAILED goto ErrorExit
if exist ESAVER\FAILED goto ErrorExit

vecho /fLightGreen Finished. /fGrey

:End
SET SELF=
SET TPC=
SET TPC_OPTS=

:VeryEnd
