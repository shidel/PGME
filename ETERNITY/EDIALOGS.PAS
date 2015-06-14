{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

unit EDialogs;

interface

uses QCrt, QErrors, QStrings, QDos, QFiles, QClass, QObjects, QControls, QFmtStr, 
	QSpeaker, QDirCntl;
	
{$DEFINE INTERFACE}

{$I TDlgOpts.inc}
{$I TDlgCfg.inc}
{$I TDlgProg.inc}
{$I TDlgPAdd.inc}
{$I TDlgPEdit.inc}
{$I TDlgPDelete.inc}
{$I TDlgMenu.inc}
{$I TDlgMAdd.inc}
{$I TDlgMEdit.inc}
{$I TDlgMDelete.inc}
{$I TDlgLaunch.inc}

{$UNDEF INTERFACE}

implementation

uses EConst, ECache, EPGM;

{$DEFINE IMPLEMENTATION}

{$I TDlgOpts.inc}
{$I TDlgCfg.inc}
{$I TDlgProg.inc}
{$I TDlgPAdd.inc}
{$I TDlgPEdit.inc}
{$I TDlgPDelete.inc}
{$I TDlgMenu.inc}
{$I TDlgMAdd.inc}
{$I TDlgMEdit.inc}
{$I TDlgMDelete.inc}
{$I TDlgLaunch.inc}

{$UNDEF IMPLEMENTATION}

end.
