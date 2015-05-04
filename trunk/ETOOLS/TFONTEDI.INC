{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{$IFDEF INTERFACE}
const
	class_TFontEdit = 'TFontEdit';

type 
	PFontEdit = ^TFontEdit;
	TFontEdit = object(TBox)
	public { protected }
		function  ObjectClass ( AName : String ) : String; virtual;
	public { protected }
		FMUL, FOFS : TPoint;
		FLPos : TPoint;
		FNullChar, FBitChar : Char;
		FNullColor,
		FHotColor,
		FBitColor,
		FDemoColor,
		FContColor : Integer; 
		FPressKey : String;
		function GetCharByte(Row : Integer) : Integer; virtual;
		procedure SetCharByte(Row : Integer; Value : Integer); virtual;
		procedure UpdatePoint(XY : TPoint; Toggle : boolean); virtual;
		procedure SetCurrentBounds(AValue : TBounds); virtual;
		procedure CheckMouse(var AEvent : TEvent); virtual;
	public
			FFont : PFont;
			FCurrent : integer;
		constructor Create(AParent : PControl; AName : String);
		destructor Destroy; virtual;
		procedure Draw; virtual;
		function GetMaxHeight : integer; virtual;
		procedure Load(AFilename : String); virtual;
		procedure Save(AFilename : String); virtual;
		procedure NewFont(AHeight : integer; AClone, AStretch : boolean); virtual;
		procedure Stretch(AHeight : integer); virtual;
		procedure Resize(AHeight : integer); virtual;
		procedure Demo; virtual;
		procedure ShiftLeft; virtual;
		procedure ShiftRight; virtual;
		procedure ShiftUp; virtual;
		procedure ShiftDown; virtual;
		procedure DemoFont(NullDac : boolean); virtual;
		procedure ApplyTheme( AConfig : PConfigFile ); virtual;
		procedure ApplyLanguage( AConfig : PConfigFile ); virtual;
	end;
{$ENDIF}

{$IFDEF IMPLEMENTATION}
function TFontEdit.ObjectClass ( AName : String ) : String; 
begin
	if (AName = '') or (AName = class_TFontEdit) then 
		ObjectClass := class_TFontEdit
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TFontEdit.Create;
begin
	inherited Create(AParent, AName);
	FFont := New(PFont, Create('EDITFONT'));
	FStyle := bxSingle;
	FShadow := False;
	FCurrent := -1;
	FNullChar := 'X';
	FNullColor := TextAttr and $F0 + $08;
	FDemoColor := $0E;
	FContColor := $0D;
	FBitChar := #$B2;
	FBitColor := $0F;
	FHotColor := $34;
	FMUL.X := 1;
	FMUL.Y := 1;
	FOFS.X := 0;
	FOFS.Y := 0;
	FLPos.X := -1;
	FLPos.Y := -1;
	FPressKey := 'Press a key or move the Mouse to return.';
end;

destructor TFontEdit.Destroy;
begin
	if Assigned(FFont) then Dispose(FFont, Destroy);
	inherited Destroy;
end;

procedure TFontEdit.SetCurrentBounds(AValue : TBounds); 
var
	ExtraSize : integer;
begin
	ExtraSize := 0;
	if FBorder then Inc(ExtraSize, 2);
	if FShadow then Inc(ExtraSize, 2);
	FMUL.X := 1;
	FMUL.Y := 1;
	FOFS.X := 0;
	FOFS.Y := 0;
	
	{ Fix Height }
	if FShadow then Dec(ExtraSize, 1);
	
	if Assigned(FFont) and (FFont^.GetHeight > 0) then 
		AValue.Height := FFont^.GetHeight
	else if Assigned(UserFontPtr) and (Seg(UserFontPtr^) <> $C000)then
		AValue.Height := FontHeight
	else
		AValue.Height := 16;

	if AValue.Height > GetMaxHeight then 
		AValue.Height := GetMaxHeight;

	if ((Application^.FFont.GetEnabled) and (Application^.FFont.GetHeight >= 12)) or
		(GetMaxHeight < 30) then 
		FMUL.X := 2;
		
	if ExtraSize > 0 then begin
		Inc(FOFS.X);
		Inc(FOFS.Y);
	end;
	{ if running in less than an 8x10 font then dont do doublewide } 
	AValue.Width := 8 * FMUL.X + ExtraSize;

		
	AValue.Height := AValue.Height * FMUL.Y + ExtraSize;
	AValue.Top := (PControl(Parent)^.Height div 2) - (AValue.Height div 2);
	inherited SetCurrentBounds(AValue);
end;

function TFontEdit.GetMaxHeight : integer;
var
	ExtraSize : integer;
begin
	ExtraSize := 0;
	if FBorder then Inc(ExtraSize, 2);
	if FShadow then Inc(ExtraSize, 1);
	ExtraSize := (PControl(Parent)^.Height div FMUL.Y) -
	  (PControl(Parent)^.FMargin.Top) -
	  (PControl(Parent)^.FMargin.Bottom) -
	  ExtraSize - 2;
	if ExtraSize > 32 then ExtraSize := 32;
	GetMaxHeight := ExtraSize;	
end;

function TFontEdit.GetCharByte(Row : Integer) : Integer; 
begin
	if Not Assigned(FFont) then exit;
	if Not Assigned(FFont^.FFontPtr) then exit;
	if (FCurrent < 0) or (FCurrent > 255) then exit;
	if (Row < 0) or (Row >= FFont^.GetHeight) then exit;
	GetCharByte := Bytes(FFont^.FFontPtr^)[FCurrent * FFont^.GetHeight + Row];
end;

procedure TFontEdit.SetCharByte(Row : Integer; Value : Integer); 
begin
	if Not Assigned(FFont) then exit;
	if Not Assigned(FFont^.FFontPtr) then exit;
	if (FCurrent < 0) or (FCurrent > 255) then exit;
	if (Row < 0) or (Row >= FFont^.GetHeight) then exit;
	Bytes(FFont^.FFontPtr^)[FCurrent * FFont^.GetHeight + Row] := Value;
end;

procedure TFontEdit.Draw;
var
	C : Char;
	R, I, J : integer;
begin
	Inherited Draw;
	if (Not Assigned(FFont)) or (Not Assigned(FFont^.FFontPtr)) or (FCUrrent = -1) then begin
		C := TextChar;
		TextAttr := FNullColor;
		TextChar := FNullChar;
		ClrScr;
		TextChar := C;
	end else begin
		TextAttr := FBitColor;
		for J := 0 to FFont^.GetHeight - 1 do begin
			R := GetCharByte(J);
			for I := 0 to FFont^.GetWidth - 1 do
				if R and (1 shl (7 - I)) <> 0 then
					FWrite(ChrStr(FBitChar, FMUL.X))
				else
					FWrite(ChrStr(#32, FMUL.X));
		end;
	end;
end;

procedure TFontEdit.UpdatePoint(XY : TPoint; Toggle : boolean); 
var
	R : integer;
begin
	if FCurrent = -1 then exit;
	Window(FWindow.Left + FMargin.Left - 1, FWindow.Top + FMargin.Top - 1, 
		FWindow.Left + FWindow.Width - FMargin.Right + 1, 
		FWindow.Top + FWindow.Height - FMargin.Bottom + 1);
	gotoXY( FOfs.X + XY.X * FMul.X + 1, FOFS.Y + XY.Y * FMul.Y + 1);
	R := GetCharByte(XY.Y);
	if Toggle then
		TextAttr := FHotColor				
	else
		TextAttr := FBitColor;
	if R and (1 shl (7 - XY.X)) <> 0 then
		FWrite(ChrStr(FBitChar, FMUL.X))
	else
		FWrite(ChrStr(#32, FMUL.X));
end;

procedure TFontEdit.CheckMouse(var AEvent : TEvent); 
var
	P : TPoint;
	Hold : TEvent;
	R : integer;
begin
	Hold := AEvent;
	inherited CheckMouse(AEvent);
	if not GetVisible then exit;
	if not GetShown then exit;
	if (FLPos.X <> -1) then UpdatePoint(FLPos, False);
	FLPos.X := -1;
	P.X := AEvent.Where.X;
	P.Y := AEvent.Where.Y;
	With P do begin
		X := (AEvent.Where.X - FWindow.Left - FMargin.Left - FOfs.X) + 1;
		Y := (AEvent.Where.Y - FWindow.Top - FMargin.Top - FOfs.Y) + 1;
		if (X < 0) or (Y < 0) then exit;
		X := X div FMUL.X;
		Y := Y div FMUL.Y;
		if (X >= FFont^.GetWidth ) or (Y >= FFont^.GetHeight) then exit;
	end;
	if Hold.What = evMouseUp then begin
		ClearEvent(Hold);	
		SetCharByte(P.Y, GetCharByte(P.Y) xor (1 shl (7 - P.X))); 	
	end;
	UpdatePoint(P, True);
	FLPos := P;
end;

procedure TFontEdit.Load(AFilename : String); 
begin
	FFont^.SetFileName(AFilename);
	if FFont^.GetHeight > GetMaxHeight then begin
		FFont^.Clear;
		PutCommand(cmTooBig, @Self);
	end;
	FCurrent := -1;
	if FFont^.GetFileName <> '' then begin
		FCurrent := 65;		
	end;
	SetCurrentBounds(FRBounds);
end;

procedure TFontEdit.Save(AFilename : String);
begin
	FFont^.SaveToFile(AFilename);
end;

procedure TFontEdit.Demo;
var
	TempDac : array[0..$300] of byte;
	Fader : boolean;
	Size : integer;
	Screen, Temp : Pointer;
	Event : TEvent;
	QCrtSettings : TQCrtSettings;
	
begin
	with Application^ do begin
		FillChar(TempDac, Sizeof(TempDac), 0);
		FConfig.OpenSection('DISPLAY');
		Fader := StrBool(FConfig.GetValue('USE_FADER', BoolStr(FUseFader)))
			and StrBool(FConfig.GetValue('USE_DAC', BoolStr(FUseDAC)));	
		Screen := nil;
		GetQCrtState(QCrtSettings);

		Size := GetVideoSize;
		Temp := GetVideoPtr;
		if Not MemCheck(Size) then Halt(erInsufficient_Memory);
		GetMem(Screen, Size);
		SetVideoPtr(Screen);
		CopyFromVideo;
		SetVideoPtr(Temp);

		Window(1,1,Lo(ScreenMax) + 1, Hi(ScreenMax) + 1);
		if Fader then
			FadeOutDAC(0,256,FDefaultDAC^);

		TextAttr := $07;
		ClrScr;

		FFont.SetEnabled(False);

		DemoFont(Fader);	
		
		if Fader then
			FadeInDAC(0,256,FDefaultDAC^);

		ClearEvent(Event);
		While Event.What = evNothing do GetEvent(Event);
		if (Event.What and evMessage <> evCommand) then
			PutEvent(Event);

		if Fader then
			FadeOutDAC(0,256,FDefaultDAC^);

		TextAttr := $07;
		ClrScr;
		
		Self.FFont^.SetEnabled(False);		
		
		SetQCrtState(QCrtSettings);	
		
		FFont.SetEnabled(True);
		
		if Fader then
			WriteDac(0,256,TempDac);

		CheckScroll := False;
		CheckCursor := False;
		HideCursor;

		if Assigned(Screen) then
			begin
				Temp := GetVideoPtr;
				SetVideoPtr(Screen);
				CopyToVideo;
				SetVideoPtr(Temp);
				FreeMem(Screen, Size);
			end;
		if Fader then
			FadeInDAC(0,256,FDefaultDAC^);
				
	end;
end;

procedure TFontEdit.DemoFont;
var	
	I : Integer;
	TempDac : array[0..$300] of byte;
begin
	FillChar(TempDac, Sizeof(TempDac), 0);
	FFont^.SetEnabled(True);
	if NullDac then
		WriteDac(0,256,TempDac);
	TextAttr := FDemoColor;
	ClrScr;
	for I := 0 to 255 do begin
		
		gotoXY (8 + (I and $1F) * 2, 2 + I shr 5 );
		FWrite(Chr(I));
		
	end;
	FWriteLn('');
	FWriteLn('');
	TextAttr := FContColor;
	FWriteLn(CSpace(FPressKey, Lo(ScreenMax) + 1));
	TextAttr := FDemoColor;
	
end;

procedure TFontEdit.ApplyTheme (AConfig : PConfigFile);
var 
	S : String;
begin
	inherited ApplyTheme(AConfig);
	AConfig^.GetInts('NULLCOLOR', FNullColor, Sizeof(FNullColor) div SizeOf(Integer));
	S := Copy(AConfig^.GetHexStr('NULLCHAR', StrHex(FNullChar, False)),1 ,1);   
	if S = '' then FNullChar := 'X' else FNullChar := S[1];
	AConfig^.GetInts('DEMOCOLOR', FDEMOColor, Sizeof(FDemoColor) div SizeOf(Integer));
	AConfig^.GetInts('CONTCOLOR', FCONTColor, Sizeof(FContColor) div SizeOf(Integer));
	AConfig^.GetInts('HOTCOLOR', FHotColor, Sizeof(FHotColor) div SizeOf(Integer));
	AConfig^.GetInts('BITCOLOR', FBitColor, Sizeof(FBitColor) div SizeOf(Integer));
	S := Copy(AConfig^.GetHexStr('BITCHAR', StrHex(FBitChar, False)),1 ,1);   
	if S = '' then FBitChar := #$B2 else FBitChar := S[1];
end;

procedure TFontEdit.ApplyLanguage (AConfig : PConfigFile);
begin
	inherited ApplyLanguage(AConfig);
	FPressKey := AConfig^.GetValue(GetPathID + '.CONTINUE', FPressKey );
end;

procedure TFontEdit.NewFont(AHeight : integer; AClone, AStretch : boolean); 
begin
	if AHeight > GetMaxHeight then AHeight := GetMaxHeight;
	if AHeight < 1 then AHeight := 1;
	FFont^.Clear;
	if AClone then begin
		if AHeight < 14 then begin
			FFont^.CopyROMFont(rf8x8);
			if AStretch then Stretch(AHeight) else Resize(AHeight);
		end else if AHeight < 16 then begin
			FFont^.CopyROMFont(rf8x14);
			if AStretch then Stretch(AHeight) else Resize(AHeight);
		end else begin
			FFont^.CopyROMFont(rf8x16);
			if AStretch then Stretch(AHeight) else Resize(AHeight);
		end;		
	end else begin
		FFont^.EmptyFont(AHeight);
		FFont^.FHeader.Add(New(PListItem, Create('TITLE=New 8x' + IntStr(AHeight) + ' Font')));
	end;
	FCurrent := 65;
	SetCurrentBounds(FRBounds);
end;

procedure TFontEdit.Stretch(AHeight : integer); 
var
	Line : array[0..7] of integer;
	P, R : Pointer;
	I, J, K, L, T : integer;
begin
	if FFont^.GetHeight = AHeight then exit;
	if not Assigned(FFont^.FFontPtr) then exit;
	if not MemCheck(AHeight * 256 + 1024) then exit;
	GetMem(P, AHeight * 256);
	GetMem(R, 1024);
	FillChar(P^, AHeight * 256, 0);
	for J := 0 to 255 do begin
		for I := 0 to FFont^.FHeight - 1 do 
			for K := 0 to AHeight - 1 do 
				Bytes(R^)[I * AHeight + K] := Bytes(FFont^.FFontPtr^)[J * FFont^.FHeight + I];
		for I := 0 to AHeight - 1 do begin
			FillChar(Line, Sizeof(Line), 0);
			for K := 0 to FFont^.FHeight - 1 do begin
				T := Bytes(R^)[I * FFont^.FHeight + K];
				for L := 0 to 7 do	
					if T and (1 shl (7-L)) <> 0 then
						Inc(Line[L], 10);
			end;
			T := 0;
			for L := 0 to 7 do
				if Line[L] div AHeight > 4 then 
					T := T or (1 shl (7-L)); 
			Bytes(P^)[J * AHeight + I] := T;
		end;
	end;
	FreeMem(R, 1024);
	FreeMem(FFont^.FFontPtr, FFont^.GetHeight * 256);
	FFont^.FFontPtr := P;
	FFont^.FHeight := AHeight;
	FFont^.FFontSize := AHeight * 256;
	SetCurrentBounds(FRBounds);
end;

procedure TFontEdit.Resize(AHeight : integer); 
var
	P : Pointer;
	I, J : integer;
begin
	if FFont^.GetHeight = AHeight then exit;
	if not Assigned(FFont^.FFontPtr) then exit;
	if not MemCheck(AHeight * 256 + 1024) then exit;
	GetMem(P, AHeight * 256);
	FillChar(P^, AHeight * 256, 0);
	for J := 0 to 255 do begin
		for I := 0 to FFont^.FHeight - 1 do 
			Bytes(P^)[J * AHeight + I] := Bytes(FFont^.FFontPtr^)[J * FFont^.FHeight + I];
	end;
	FreeMem(FFont^.FFontPtr, FFont^.GetHeight * 256);
	FFont^.FFontPtr := P;
	FFont^.FHeight := AHeight;
	FFont^.FFontSize := AHeight * 256;
	SetCurrentBounds(FRBounds);
end;

procedure TFontEdit.ShiftLeft;
var
	I : integer;
begin;
	if not Assigned(FFont^.FFontPtr) then exit;
	for I := 0 to FFont^.GetHeight - 1 do 
		SetCharByte(I, (GetCharByte(I) shl 1) or (GetCharByte(I) shr 7) );
	Update;
end;

procedure TFontEdit.ShiftRight; 
var
	I : integer;
begin;
	if not Assigned(FFont^.FFontPtr) then exit;
	for I := 0 to FFont^.GetHeight - 1 do 
		SetCharByte(I, (GetCharByte(I) shr 1) or (GetCharByte(I) shl 7) );
	Update;
end;

procedure TFontEdit.ShiftUp; 
var
	T, I : integer;
begin;
	if not Assigned(FFont^.FFontPtr) then exit;
	T := GetCharByte(0);
	for I := 0 to FFont^.GetHeight - 2 do 
		SetCharByte(I, GetCharByte(I + 1));
	SetCharByte(FFont^.GetHeight - 1, T);
	Update;
end;

procedure TFontEdit.ShiftDown;
var
	T, I : integer;
begin;
	if not Assigned(FFont^.FFontPtr) then exit;
	T := GetCharByte(FFont^.GetHeight - 1);
	for I := 1 to FFont^.GetHeight - 1 do 
		SetCharByte(FFont^.GetHeight - I, GetCharByte(FFont^.GetHeight - I - 1));
	SetCharByte(0, T);
	Update;
end;


{$ENDIF}
