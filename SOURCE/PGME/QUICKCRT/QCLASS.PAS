{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

unit QClass; { QuickCRT Basic Object Classes }
{$I QCRT.DEF}

interface

{$DEFINE INTERFACE}

uses 
   {$IFDEF DEVLOG} QLog, {$ENDIF} 
	QCrt, QStrings;
	
{$IFNDEF STRINGPTRS}
const
	StringMax = 255;
{$ENDIF}

type
	TObjectClass = String;

{$I TObject.inc}
{$I TPersistent.inc}
		
{$UNDEF INTERFACE}

implementation

{$DEFINE IMPLEMENTATION}

{$I TObject.inc}
{$I TPersistent.inc}

{$UNDEF IMPLEMENTATION}
end.
