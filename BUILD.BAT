@echo off

echo Creating Program Manager Eternity Installer from source.

call make -S clean templates themes 

rem internalize themefiles for mor memory
cd bin

mkdir DEFAULTS
pgme.exe
	copy DEFAULT.THM DEFAULTS\ETERNITY.THM
	copy DEFAULT.KMP DEFAULTS\ETERNITY.KMP
	copy DEFAULT.LNG DEFAULTS\ETERNITY.LNG
	del DEFAULT.THM
	del DEFAULT.KMP
	del DEFAULT.LNG

efntdsgn.exe
	copy DEFAULT.THM DEFAULTS\EFNTDSGN.THM
	copy DEFAULT.KMP DEFAULTS\EFNTDSGN.KMP
	copy DEFAULT.LNG DEFAULTS\EFNTDSGN.LNG
	del DEFAULT.THM
	del DEFAULT.KMP
	del DEFAULT.LNG
	
install.exe
	copy DEFAULT.THM DEFAULTS\INSTALL.THM
	copy DEFAULT.KMP DEFAULTS\INSTALL.KMP
	copy DEFAULT.LNG DEFAULTS\INSTALL.LNG
	del DEFAULT.THM
	del DEFAULT.KMP
	del DEFAULT.LNG

cd ..

call make -S notemplates install