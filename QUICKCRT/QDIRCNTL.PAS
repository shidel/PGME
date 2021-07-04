{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

unit QDirCntl; { QuickCrt Directory and File based Controls }
{$I QCRT.DEF}
interface

{$DEFINE INTERFACE}

uses 
   {$IFDEF DEVLOG} QLog, {$ENDIF} 
	QCrt, QObjects, QControls;
	
	
{$I TDIRDROPLIST.INC}
{$I TDIRLIST.INC}

{$UNDEF INTERFACE}

implementation

{$DEFINE IMPLEMENTATION}

uses QErrors, QFiles, QFmtStr, QDos, QStrings;

{$I TDIRDROPLIST.INC}
{$I TDIRLIST.INC}

{$UNDEF IMPLEMENTATION}

end.

 
