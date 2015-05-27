@echo off

echo Creating Program Manager Eternity Installer from source.

call make -S clean templates themes 

rem internalize themefiles for mor memory
cd bin

mkdir TEMPLATES
pgme.exe
	copy DEFAULT.THM ETERNITY.THM
	copy DEFAULT.KMP ETERNITY.KMP
	copy DEFAULT.LNG ETERNITY.LNG
	del DEFAULT.THM
	del DEFAULT.KMP
	del DEFAULT.LNG

efntdsgn.exe
	copy DEFAULT.THM EFNTDSGN.THM
	copy DEFAULT.KMP EFNTDSGN.KMP
	copy DEFAULT.LNG EFNTDSGN.LNG
	del DEFAULT.THM
	del DEFAULT.KMP
	del DEFAULT.LNG
	
install.exe
	copy DEFAULT.THM INSTALL.THM
	copy DEFAULT.KMP INSTALL.KMP
	copy DEFAULT.LNG INSTALL.LNG
	del DEFAULT.THM
	del DEFAULT.KMP
	del DEFAULT.LNG

cd ..

call make -S notemplates install