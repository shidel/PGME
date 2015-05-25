@echo off

echo Creating Program Manager Eternity Installer from source.

call make -S clean templates themes 

rem internalize themefiles for mor memory
cd bin

pgme.exe
	splice TEMPLATE.THM DEFAULT.THM
	del DEFAULT.THM
	splice TEMPLATE.KMP DEFAULT.KMP
	del DEFAULT.KMP
	splice TEMPLATE.LNG DEFAULT.LNG
	del DEFAULT.LNG

efntdsgn.exe
	splice TEMPLATE.THM DEFAULT.THM
	del DEFAULT.THM
	splice TEMPLATE.KMP DEFAULT.KMP
	del DEFAULT.KMP
	splice TEMPLATE.LNG DEFAULT.LNG
	del DEFAULT.LNG
	
install.exe
	splice TEMPLATE.THM DEFAULT.THM
	del DEFAULT.THM
	splice TEMPLATE.KMP DEFAULT.KMP
	del DEFAULT.KMP
	splice TEMPLATE.LNG DEFAULT.LNG
	del DEFAULT.LNG

cd ..

call make -S notemplates install