{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

unit EDialogs;

interface

uses QCrt, QErrors, QStrings, QDos, QFiles, QClass, QObjects, QControls, QFmtStr, 
	QSpeaker;
	
{$DEFINE INTERFACE}

{$I TDLGOpts.inc}
{$I TDLGCfg.inc}
{$I TDLGProg.inc}
{$I TDLGPAdd.inc}
{$I TDLGPEdit.inc}
{$I TDLGPDelete.inc}
{$I TDLGMenu.inc}
{$I TDLGMAdd.inc}
{$I TDLGMEdit.inc}
{$I TDLGMDelete.inc}

{$UNDEF INTERFACE}

implementation

uses EConst, ECache, EPGM;

{$DEFINE IMPLEMENTATION}

{$I TDLGOpts.inc}
{$I TDLGCfg.inc}
{$I TDLGProg.inc}
{$I TDLGPAdd.inc}
{$I TDLGPEdit.inc}
{$I TDLGPDelete.inc}
{$I TDLGMenu.inc}
{$I TDLGMAdd.inc}
{$I TDLGMEdit.inc}
{$I TDLGMDelete.inc}

{$UNDEF IMPLEMENTATION}

end.
