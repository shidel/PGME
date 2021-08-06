{ Copyright 2015 Jerome Shidel }

(*

	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.

*)

unit QObjects; { QuickCRT Objects }
{$I QCRT.DEF}

interface

{$DEFINE INTERFACE}

uses
   {$IFDEF DEVLOG} QLog, {$ENDIF}
	QCrt, QStrings, QClass, QBIOS;

const
    LowMemAvailWarn = 32768;
    LowMaxAvailWarn = 5120;

const { Anchor Bounds and Align Control Constants. Anchors can override Alignments }
	abNone   = $0000;
	abLeft   = $0001;
	abRight  = $0002;
	abTop    = $0004;
	abBottom = $0008;
	abAll    = abLeft or abRight or abTop or abBottom;
	alNone	 = abNone;
	alLeft   = $0100;
	alRight  = $0200;
	alTop    = $0400;
	alBottom = $0800;
	alClient = alLeft or alRight or alTop or alBottom;

const { TListItem Compare Results }
	cmpUnknown     = -2;
	cmpLessThan    = -1;
	cmpEqual       =  0;
	cmpGreaterThan = +1;

const { Modal Results and Button options }
	mrNoResult	= -1;
	mrNone		= 0;
	mrOk		= 1;
	mrCancel	= 2;
	mrAbort		= 4;
	mrRetry		= 8;
	{ Define more if needed, any valid integer, ie 16=Skip, 17=Revert...
		Defaults are used as bitmapped to create buttons only. if Setbuttons is Not
		used in a Dialog, then any integer other than -1 can be used. }

const { Default File Extensions }
    ConfigExt 	= '.CFG';
    LanguageExt = '.LNG';
    ThemeExt 	= '.THM';
    KeymapExt 	= '.KMP';
    FontExt 	= '.FNT';
    InfoExt 	= '.PMI';
    ESaverExt 	= '.SCR';
    EMenuExt	= '.MNU';

const { Text Config File ID Tags }
    ftConfig 	= 'CONFIG';
    ftLanguage 	= 'LANGUAGE';
    ftTheme 	= 'THEME';
    ftKeymap	= 'KEYMAP';
    ftInfo 		= 'INFORMATION';
    ftESaver 	= 'ESAVER';
    ftProgram	= 'PROGRAM';

type
	{ Strictly Text Justification }
	THorizontalAlign = (AlignLeft, AlignCenter, AlignRight);
	TVerticalAlign = (AlignTop, AlignMiddle, AlignBottom);

	{ X, Y Point}
	TPoint = record
		X, Y : integer;
	end;

	TBounds = record
		Left, Top, Width, Height : integer;
	end;
	TMargins = record
		Left, Top, Right, Bottom : integer;
	end;
	TTextAttrs = record
	   Normal, Disabled, Hover, Click, Focused : integer;
	end;
	TKeyValue = record
		ShiftCode : word;
        case Integer of
            0: (KeyCode: Word);
            1: (CharCode: Char;
                ScanCode: Byte);
    end;

{$I TListItem.inc}
{$I TComponent.inc}
{$I TConfigFile.inc}
{$I TFont.inc}

var
	BufferedVideo : boolean; { Buffered Video Drawing; Leave it alone its for debugging! }

	{ Procedure to Set a Bounds Record }
	procedure Bounds(ALeft, ATop, AWidth, AHeight : integer; var AValue : TBounds);

	{  These should only be used in Control Development }
	procedure StartDrawing;
	procedure FinishDrawing;

{$UNDEF INTERFACE}

implementation

uses QErrors, QDos, QFiles, QFmtStr;

	var
		OldExitProc : pointer;
		DrawCounter : integer;
		SavedSettings : TQCrtSettings;
		PCurrent : Pointer;
		PBuffer : Pointer;
		PSize : integer;
		ShowMemFlag : boolean;

{$DEFINE IMPLEMENTATION}

{$I DCOMMENTS.INC}
{$I DKEYNAME.INC}
{$I DKEYMAPS.INC}
{$I DCOMMANDS.INC}

	procedure Bounds(ALeft, ATop, AWidth, AHeight : integer; var AValue : TBounds);
	begin
		AValue.Left := ALeft;
		AValue.Top := ATop;
		AValue.Width := AWidth;
		AValue.Height := AHeight;
	end;

	procedure StartDrawing;
	begin
		Inc(DrawCounter);
		if DrawCounter <> 1 then exit;
		GetQCrtState (SavedSettings);
		CheckScroll := False;
		CheckCursor := False;
		if Not BufferedVideo then exit;
		PCurrent := GetVideoPtr;
		if (PSize <> GetVideoSize) and Assigned(PBuffer) then
			begin
				FreeMem(PBuffer, PSize);
				PBuffer := nil;
			end;
		PSize := GetVideoSize;
		if Not Assigned(PBuffer) then
			begin
				if not MemCheck( PSize ) then Halt(erInsufficient_Memory);
				GetMem(PBuffer, PSize);
			end;
		SetVideoPtr(PBuffer);
		CopyFromVideo;
	end;

	procedure FinishDrawing;
	begin
		Dec(DrawCounter);
		if DrawCounter <> 0 then exit;
		SetQCrtState(SavedSettings);
		if Not BufferedVideo then exit;
		if Assigned(PBuffer) then
			CopyToVideo;
		SetVideoPtr(PCurrent);
	end;

	procedure ExitQObjects; far;
	begin
		ExitProc := OldExitProc;
		if Assigned(PBuffer) then
			FreeMem(PBuffer, PSize);
		PBuffer := nil;
		SetKeyMap(nil, 0);
		SetCommandMap(nil, 0);
		{$IFDEF MEMINFO}
			ShowMemFlag := True;
		{$ELSE}
			ShowMemFlag := (MinMemAvail < LowMemAvailWarn)
			    or (MinMaxAvail < LowMaxAvailWarn)
			    or (MemAvail <> FirstMemAvail);
		{$ENDIF}
		if ShowMemFlag then begin
			TextAttr := $0F;
			if WhereX > 1 then WriteLn;
			TextAttr := $07;
			WriteLn('MEMORY USED ', FirstMemAvail - MinMemAvail, ', FREE ', MinMaxAvail);
			Write('STATE, ');
			if MemAvail <> FirstMemAvail then begin
				TextAttr := $0C;
				Write('ERROR ', MaxAvail, '/', MemAvail);
			end else if (MinMemAvail < LowMemAvailWarn)
			or (MinMaxAvail < LowMaxAvailWarn) then  begin
				TextAttr := $0C;
				Write('LOW ');
				TextAttr := $0B;
				Write(MinMaxAvail, '/', MinMemAvail);
			end else begin
				TextAttr := $0A;
				Write('GOOD');
			end;
			TextAttr := $07;
			WriteLn;
		end;
		{$IFDEF TEMPLATES}
			if MemAvail <> FirstMemAvail then
				Delay(5000)
			else
				Delay(1500);
		{$ENDIF}
	end;

	procedure InitQObjects;
	begin
		{$IFDEF TEMPLATES}
			BufferedVideo := False;
		{$ELSE}
			BufferedVideo := True;
		{$ENDIF}
		DrawCounter := 0;
		PBuffer := nil;
		PSize := 0;
		CheckBreak  := False;
		ShowMemFlag := False;
		OldExitProc := ExitProc;
		ExitProc := @ExitQObjects;
		InitQCrtMaps;
	end;

{$I TListItem.inc}
{$I TComponent.inc}
{$I TConfigFile.inc}
{$I TFont.inc}

{$UNDEF IMPLEMENTATION}
begin
	{$IFDEF TEMPLATES}
		TextAttr := $07;
		if WhereX > 1 then WriteLn;
		TextAttr := $0F;
		Write('MEMORY: ');
		TextAttr := $0B;
		WriteLn(MemAvail);
		TextAttr := $07;
	{$ENDIF}
	InitQObjects;
	PurgeEvents;
end.
