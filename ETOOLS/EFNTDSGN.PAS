{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{$I ETOOLS.DEF}
program Eternity_Font_Designer; { X Series }

uses QCrt, QErrors, QStrings, QDos, QFiles, QMore, QFmtStr,
	QObjects, QControls, QFileDlg, QBios;

{$I ..\ETERNITY\LICENSE.INC}

const	
	AppTitle = 'Eternity Font Designer';
	AppID = 'EFNTDSGN';
	
	cmDemo  = cmUser + 4;

{$DEFINE INTERFACE}

{$I TFontEdit.inc}
{$I TNewFont.inc}
{$I TInfoDialog.inc}

{$UNDEF INTERFACE}

{$I TFontDesigner.inc}

{$DEFINE IMPLEMENTATION}

{$I TFontEdit.inc}
{$I TNewFont.inc}
{$I TInfoDialog.inc}

{$UNDEF IMPLEMENTATION}

procedure Execute;
begin
	Write('Starting ' + AppTitle + '...');
	ETool.Create;
	ETool.Execute;
	ETool.Destroy;   
	TextAttr := $08;
	GotoXY(1,WhereY);
	Write('Goodbye.');
	ClrEol;	
	TextAttr := $07;
	WriteLn;
end;

begin
	Execute; 
end.
