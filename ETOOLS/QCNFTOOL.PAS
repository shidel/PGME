{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{$I ETOOLS.DEF}
program Eternity_Config_File_Toolkit; { X Series }

uses QObjects, QStrings;

{$I ..\ETERNITY\LICENSE.INC}

const
	AppTitle = 'Eternity Configuration File Toolkit';
	AppID = 'QCNFTOOL';
	
procedure Help;
begin
	WriteLn('Usage: ', AppID, ' FILE [options...]');
	WriteLn;
	WriteLn(AppTitle, ' (', AppID, ') is for manipulating PGME');
	Writeln('text based data files. These files are similar to windows INI files and');
	WriteLn('may also be edited using a simple text editor.');
	Halt(0);
end;

begin
	if ParamCount = 0 then 
		Help;
end.
