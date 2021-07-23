{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{$I ..\QUICKCRT\TEMPLATE.DEF}
{$I ETERNITY.DEF}
program Program_Manager_Eternity_Main_Application; { X Series }

uses QCrt, QErrors, QStrings, QDos, QFiles, QClass, QObjects, QControls, QFmtStr, PGM_Call,
	EConst, ECache, EDialogs, EPGM;

{$DEFINE INTERFACE}

{$UNDEF INTERFACE}
{$DEFINE IMPLEMENTATION}

{$UNDEF IMPLEMENTATION}

	procedure RunEternity;
	begin
		ErrorsOnExit := True; 
		PGME.Create;
		PGME.Execute;
		PGME.Destroy;  
		GotoXY(1, WhereY); 
	end;

begin
	RunEternity; 
end.
