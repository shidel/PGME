{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{$I ETOOLS.DEF}
program Eternity_Config_File_Toolkit; { X Series }

uses QObjects;

{$I ..\ETERNITY\LICENSE.INC}

const
	AppTitle = 'Eternity Configuration File Toolkit';
	AppID = 'QCNFTOOL';
	
{ --- Global Variables --- }
	
begin
	WriteLn('Placeholder application for ' + AppTitle);
	WriteLn('Later, it will be able to edit, merge and export text based configuration');
	WriteLn('files like ETERNITY.CFG and ETERNITY.MNU. As well as Theme, Language and');
	WriteLn('Keymap files.');
end.
