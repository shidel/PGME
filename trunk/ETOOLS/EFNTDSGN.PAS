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
	class_TFontDesigner = 'TFontDesigner';
	
	AppTitle = 'Eternity Font Designer';
	AppID = 'EFNTDSGN';
	
	cmDemo  = cmUser + 4;
	
{$I TFontEdit.inc}
{$I TFontDesigner.inc}

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
