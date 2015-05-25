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
{$I ../VERSION.INC}

const
	cmExecute 	= cmExecuteFile;
	cmNextMenu  = cmAfter;
	cmPrevMenu  = cmBefore;
	
	cmProgAdd 	= $1100;
	cmProgEdit	= cmProgAdd + 1;
	cmProgDel	= cmProgEdit + 1;
	
	cmMenuAdd 	= cmProgDel + 1;
	cmMenuEdit	= cmMenuAdd + 1;
	cmMenuDel	= cmMenuEdit + 1;
	
	cmPGMOpts   = cmMenuDel + 1;
	cmPGMCfg	= cmPGMOpts + 1;
		
	ftMenu	= 'MENU';

{$DEFINE INTERFACE}

{$I TCacheItems.inc}

{$I TDLGOpts.inc}
{$I TDLGProg.inc}
{$I TDLGPAdd.inc}
{$I TDLGPEdit.inc}
{$I TDLGPDelete.inc}
{$I TDLGMenu.inc}
{$I TDLGMAdd.inc}
{$I TDLGMEdit.inc}
{$I TDLGMDelete.inc}

{$I TPGME.inc}
{$I TMenuCache.inc}

{$UNDEF INTERFACE}
{$DEFINE IMPLEMENTATION}

{$I TDLGOpts.inc}
{$I TDLGProg.inc}
{$I TDLGPAdd.inc}
{$I TDLGPEdit.inc}
{$I TDLGPDelete.inc}
{$I TDLGMenu.inc}
{$I TDLGMAdd.inc}
{$I TDLGMEdit.inc}
{$I TDLGMDelete.inc}

{$I TPGME.inc}
{$I TMenuCache.inc}

{$UNDEF IMPLEMENTATION}

begin
{	ErrorsOnExit := True; }
	PGME.Create;
	PGME.Execute;
	PGME.Destroy;  
	GotoXY(1, WhereY);
end.
