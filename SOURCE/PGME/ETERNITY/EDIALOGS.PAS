{ Copyright 2015-2021 Jerome Shidel }

(*

	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.

*)

{$I ..\QUICKCRT\TEMPLATE.DEF}
{$I ETERNITY.DEF}
unit EDialogs;

interface

uses QCrt, QErrors, QStrings, QDos, QFiles, QClass, QObjects, QControls, QFmtStr,
	QSpeaker, QDirCntl, QFileDlg;

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
{$I TDlgExecOpt.inc}
{$I TDlgLaunch.inc}
{$I TDlgAbout.inc}
{$I TDlgDosCommand.inc}
{$I TDlgMove.inc}

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
{$I TDlgExecOpt.inc}
{$I TDlgLaunch.inc}
{$I TDlgAbout.inc}
{$I TDlgDosCommand.inc}
{$I TDlgMove.inc}

{$UNDEF IMPLEMENTATION}

end.
