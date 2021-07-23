{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{ --- TFont --- }

{$IFDEF INTERFACE}
const
	class_TFont = 'TFont';
	
type
	PFont = ^TFont;
	TFont = object(TPersistent)
	private 
    public { protected }
		function  ObjectClass ( AName : String ) : String; virtual;
	public { protected }
		FFontPtr : Pointer;
		FFontSize : integer;
		FFileName : String;
		FEnabled : boolean;
		FWidth : byte;
		FHeight : byte;
		FHeader : TListItem;
		procedure FreeFont;
		function LoadFromFile : boolean; virtual;
		function SaveToFile(AFileName : String) : boolean; virtual;
	public
		constructor Create(AName : String);
		destructor Destroy; virtual;
		function GetFileName : String; virtual;	
		procedure SetFileName ( AValue : String ) ; virtual;
		function GetEnabled : boolean; virtual;
		procedure SetEnabled(AValue : boolean); virtual;
		function GetHeight : integer; virtual;
		function GetWidth : integer; virtual;
		function GetHeader : PListItem; virtual;
		function GetSize : integer; virtual;
		procedure Clear; virtual;
		procedure CopyROMFont( AFont : TROMFont ); virtual;
		procedure EmptyFont(AHeight : integer); virtual;
	end;
{$ENDIF}

{$IFDEF IMPLEMENTATION}
function TFont.ObjectClass(AName : String) : String; 
begin
	if (AName = '') or (AName = class_TFont) then 
		ObjectClass := class_TFont
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TFont.Create(AName : String);
begin
	inherited Create(AName);
	FFontPtr := nil;
	FFontSize := 0;
	FFileName := '';
	FEnabled := false;
	FWidth := 0;
	FHeight := 0;
	FHeader.Create('TFontHeader');
	FHeader.SetFreeOnDestroy(False);
end;

destructor TFont.Destroy; 	
begin
	FreeFont;
	FHeader.Destroy;
	inherited Destroy;
end;

function TFont.LoadFromFile : boolean; 
const
	FileID : String = 'QCRT-FONT'#0;
var
	F : TFile;
	Success : boolean;
	S : String;
begin
	Success := false;
	LoadFromFile := Success;
	S := FFileName;
	FreeFont;
	FFileName := S;
	if FFileName = '' then exit;
	{$IFDEF DEVLOG_FONTLOAD}
	WriteLog('++ Loading font ' + FFileName);
	LogInc;
	{$ENDIF}
	F.Create;
	F.Assign(FFileName);
	F.Reset;
	Success := F.IOResult = 0;
	if Success then begin
		S := ChrStr(#0, Length(FileID));
		F.ReadRecord(S[1], Length(FileID));
		Success := (S = FileID) and (IOResult = 0);
	end;
	while Success and (S <> '') do begin
		if Success then begin
			S := F.GetString;
			Success := IOResult = 0;
			if Success then FHeader.Add(New(PListItem, Create(S)));
		end;	
	end;
	if Success then begin
		F.ReadByte(FWidth);
		Success := (IOResult = 0) and (FWidth = 8); { Must be 8x?? font}						
	end;	
	if Success then begin
		F.ReadByte(FHeight);
		Success := IOResult = 0;
	end;	
	if Success then begin
		FFontSize := F.FileSize - F.FilePos;
		Success := (IOResult = 0) and (MaxAvail > FFontSize);
	end;	
	if Success then begin
		GetMem(FFontPtr, FFontSize);
		F.ReadRecord(FFontPtr^, FFontSize);
		Success := IOResult = 0;
	end;	
	if Not Success then FreeFont;
	F.Destroy;
	{$IFDEF DEVLOG_FONTLOAD}
	LogDec;
	WriteLog('++ Loading font completed ' + BoolStr(Success));
	{$ENDIF}
	LoadFromFile := Success;
end;

function TFont.SaveToFile(AFileName : String) : boolean;
const
	FileID : String = 'QCRT-FONT'#0;
var
	F : TFile;
	Success : boolean;
	S : String;
	P : PlistItem;
begin
	Success := false;
	SaveToFile := Success;
	{$IFDEF DEVLOG_FONTLOAD}
	WriteLog('++ Saving font ' + AFileName);
	LogInc;
	{$ENDIF}
	F.Create;
	F.Assign(AFileName);
	F.Rewrite;
	Success := F.IOResult = 0;
	if Success then begin
		F.WriteRecord(FileID[1], Length(FileID));
		Success := (IOResult = 0);
	end;
	P := FHeader.First;
	while Success and (Assigned(P)) do begin
		if Success then begin
			S := P^.GetNameID;
			if trim(S) <> '' then F.PutString(S);
			Success := IOResult = 0;
			P := P^.Next;
		end;	
	end;
	
	if Success and Assigned(FHeader.First) then begin
		S := '';
		F.PutString(S);
		Success := (IOResult = 0); { Header text must be ended with only one blank line }						
	end;	
	if Success then begin
		F.WriteByte(FWidth);
		Success := (IOResult = 0); { Must be 8x?? font}						
	end;	
	if Success then begin
		F.WriteByte(FHeight);
		Success := IOResult = 0;
	end;	
	if Success then begin
		F.WriteRecord(FFontPtr^, FFontSize);
		Success := IOResult = 0;
	end;	
	F.Destroy;
	{$IFDEF DEVLOG_FONTLOAD}
	LogDec;
	WriteLog('++ Saving font completed ' + BoolStr(Success));
	{$ENDIF}
	SaveToFile := Success;
end;

function TFont.GetFileName : String;
begin
	GetFileName := FFileName;
end;

procedure TFont.SetFileName ( AValue : String );
begin
	if AValue = FFileName then exit;
	FFileName := AValue;
	if not LoadFromFile then FFileName := '';
end;

procedure TFont.FreeFont;
begin
	if FEnabled then SetEnabled(False);
	if Assigned(FFontPtr) then 
		FreeMem(FFontPtr, FFontSize);
	FFontSize := 0;
	FFontPtr := nil;
	FHeader.Clear;
	FWidth := 0;
	FHeight := 0;
	FFileName := '';
end;

function TFont.GetEnabled : boolean;
begin
	GetEnabled := FEnabled;
end;

procedure TFont.SetEnabled(AValue :Boolean);
begin
	if FEnabled = AValue then exit;
	if AValue then begin
		if not Assigned(FFontPtr) then exit;
		
		{ SetUserFont(FHeight, FFontPtr);
		InitQCrt; }
		UserFontSize := FHeight;
		UserFontPtr := FFontPtr;
		TextMode(CO80 or FontUser);
	end else begin
		TextMode(CO80);
	end;
	FEnabled := AValue;
end;

function TFont.GetHeight : integer;
begin
	GetHeight := FHeight;
end;

function TFont.GetWidth : integer;
begin
	GetWidth := FWidth;
end;

function TFont.GetHeader : PListItem;
begin
	GetHeader := @FHeader;
end;

function TFont.GetSize : integer;
begin
	if Assigned(FFontPtr) then
		GetSize := FFontSize
	else
		GetSize := -1;
end;
procedure TFont.Clear;
begin
	FreeFont;
end;

procedure TFont.CopyROMFont( AFont : TROMFont );
const
	SytemFontPrefix = 'SYS_';
var
	P : Pointer;
begin
	case AFont of 
		rf8x14 : begin
			EmptyFont(14);
			FHeader.Add(New(PListItem, Create('TITLE=Default System 8x14 Font')));
			FFileName := SytemFontPrefix + '814.FNT';
		end;
		rfAlt9x14 : begin
			EmptyFont(14);
			FHeader.Add(New(PListItem, Create('TITLE=Alternate System 9x14 Font')));
			FFileName := SytemFontPrefix + '914.FNT';
		end;
		rf8x8 : begin 
			EmptyFont(8);
			FHeader.Add(New(PListItem, Create('TITLE=Default System 8x8 Font')));
			FFileName := SytemFontPrefix + '808.FNT';
		end;
		rfHigh8x8 : begin 
			EmptyFont(14);
			FHeader.Add(New(PListItem, Create('TITLE=Default System 8x8 Font')));
			FFileName := SytemFontPrefix + '808.FNT';
			AFont := rf8x8;
		end;
		rf8x16 : begin
			EmptyFont(16);
			FHeader.Add(New(PListItem, Create('TITLE=Default System 8x16 Font')));
			FFileName := SytemFontPrefix + '816.FNT';
		end;
		rfAlt8x16 : begin
			EmptyFont(16);
			FHeader.Add(New(PListItem, Create('TITLE=Alternate System 8x16 Font')));
			FFileName := SytemFontPrefix + '816A.FNT';
		end;
	end;
	FWidth := 8;

	P := GetROMFontPtr(AFont);
	Move(P^, FFontPtr^, FFontSize);
	if AFont = rf8x8 then begin
		P := GetROMFontPtr(rfHigh8x8);
		Move(Bytes(P^)[8 * $80], Bytes(FFontPtr^)[8 * $80], 8 * $1F );
	end;
end;

procedure TFont.EmptyFont(AHeight : integer);
begin
	FreeFont;
	FWidth := 8;
	FHeight := AHeight;
	FHeader.Add(New(PListItem, Create('LANGUAGE=en_US')));
	
	if Pos('DOSBOX', UCase(PascalStr(Ptr(65024, 14)^))) > 0 then begin
		FHeader.Add(New(PListItem, Create('COPYRIGHT=' + PascalStr(Ptr(65024, 14)^) )));
		FHeader.Add(New(PListItem, Create('AUTHOR=' + PascalStr(Ptr(65024, 14)^) )));
		FHeader.Add(New(PListItem, Create('VERSION=' + PascalStr(Ptr(65030, 1)^) )));
		FHeader.Add(New(PListItem, Create('URL=http://www.dosbox.com')));
		FHeader.Add(New(PListItem, Create('LICENSE=unknown')));		
		FHeader.Add(New(PListItem, Create('LURL=http://www.dosbox.com')));		
	end else begin
		FHeader.Add(New(PListItem, Create('COPYRIGHT=unknown' )));
		FHeader.Add(New(PListItem, Create('AUTHOR=unknown' )));
		FHeader.Add(New(PListItem, Create('VERSION=1.0.0')));		
		FHeader.Add(New(PListItem, Create('URL=')));		
		FHeader.Add(New(PListItem, Create('LICENSE=GNU GENERAL PUBLIC LICENSE, Version 2')));		
		FHeader.Add(New(PListItem, Create('LURL=http://www.gnu.org/licenses/gpl-2.0.txt')));		
	end;
	FHeader.Add(New(PListItem, Create('')));		
	
	FFontSize := FHeight * 256;
	if not MemCheck(FFontSize) then begin
		FreeFont;
		Exit;
	end;
	
	GetMem(FFontPtr, FFontSize);
	FillChar(FFontPtr^, FFontSize, 0);
end;
{$ENDIF}
