{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

unit QFileDlg; { QuickCrt File Dialog Boxes }
{$I QCRT.DEF}
interface

{$DEFINE INTERFACE}

uses 
   {$IFDEF DEVLOG} QLog, {$ENDIF} 
	QCrt, QObjects, QControls, QDirCntl;
	
{$I TPATHDIALOG.INC}
{$I TFILEDIALOG.INC}
{$I TOPENDIALOG.INC}
{$I TSAVEDIALOG.INC}

{$UNDEF INTERFACE}

implementation

{$DEFINE IMPLEMENTATION}

uses QErrors, QFiles, QFmtStr, QDos, QStrings;

{$I TPATHDIALOG.INC}
{$I TFILEDIALOG.INC}
{$I TOPENDIALOG.INC}
{$I TSAVEDIALOG.INC}

{$UNDEF IMPLEMENTATION}

end.

 
