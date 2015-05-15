{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{$I ..\QUICKCRT\TEMPLATE.DEF}
{$I ETERNITY.DEF}
program Program_Manager_Eternity_Main_Application; { X Series }

uses QCrt, QErrors, QStrings, QDos, QFiles, QClass, QObjects, QControls, QFmtStr, PGM_Call;

{$I LICENSE.INC}

const
	cmExecute 	= cmExecuteFile;
	cmNextMenu  = cmAfter;
	cmPrevMenu  = cmBefore;
		
	ftMenu	= 'MENU';

{$DEFINE INTERFACE}

{$I TCacheItems.inc}
	
{$I TPGME.inc}
{$I TMenuCache.inc}

{$UNDEF INTERFACE}
{$DEFINE IMPLEMENTATION}

{$I TPGME.inc}
{$I TMenuCache.inc}

{$UNDEF IMPLEMENTATION}

begin
	ErrorsOnExit := False;
	PGME.Create;
	PGME.Execute;
	PGME.Destroy;  
	GotoXY(1, WhereY);
end.
