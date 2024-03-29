{ Copyright 2015 Jerome Shidel }

(*

	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.

*)

{$I ETOOLS.DEF}
program Eternity_Font_Designer; { X Series }

uses QCrt, QErrors, QStrings, QDos, QFiles, QMore, QFmtStr,
	QObjects, QControls, QFileDlg, QBios, QPatches;

{$I ..\ETERNITY\LICENSE.INC}

const
	AppTitle = 'Eternity Font Designer';
	AppID = 'EFNTDSGN';

	cmDemo       = cmUser + 1;

	cmTooBig     = cmDemo + 1;

	cmShiftLeft  = cmTooBig + 1;
	cmShiftRight = cmShiftLeft + 1;
	cmShiftUp 	 = cmShiftRight + 1;
	cmShiftDown  = cmShiftUp + 1;

	cmResize 	 =  cmShiftDown + 1;

	cmPrevChar 	 = cmResize + 1;
	cmNextChar   = cmPrevChar + 1;

{$DEFINE INTERFACE}

{$I TFontEdit.inc}
{$I TNewFont.inc}
{$I TResize.inc}
{$I TInfoDialog.inc}

{$UNDEF INTERFACE}

{$I TFontDesigner.inc}

{$DEFINE IMPLEMENTATION}

{$I TFontEdit.inc}
{$I TNewFont.inc}
{$I TResize.inc}
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
