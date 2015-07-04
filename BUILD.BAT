@echo off

echo Creating Program Manager Eternity Installer from source.

call make -S clean templates themes 

rem internalize themefiles for more memory
cd bin

mkdir DEFAULTS
pgme.exe
	splice DEFAULT.THM ONDISK.THM
	splice DEFAULT.KMP ONDISK.KMP
	splice DEFAULT.LNG ONDISK.LNG
	copy DEFAULT.THM DEFAULTS\ETERNITY.THM
	copy DEFAULT.KMP DEFAULTS\ETERNITY.KMP
	copy DEFAULT.LNG DEFAULTS\ETERNITY.LNG
	del DEFAULT.*
	del ONDISK.*

efntdsgn.exe
	splice DEFAULT.THM ONDISK.THM
	splice DEFAULT.KMP ONDISK.KMP
	splice DEFAULT.LNG ONDISK.LNG
	copy DEFAULT.THM DEFAULTS\EFNTDSGN.THM
	copy DEFAULT.KMP DEFAULTS\EFNTDSGN.KMP
	copy DEFAULT.LNG DEFAULTS\EFNTDSGN.LNG
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

cd ..

call make -S notemplates install

if exist ARCHIVE call distarch.bat