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
		FNullChar : Char;
		FNullColor,
		FDemoColor,
		FContColor : Integer; 
		procedure SetCurrentBounds(AValue : TBounds); virtual;
	public
			FFont : PFont;
			FCurrent : integer;
		constructor Create(AParent : PControl; AName : String);
		destructor Destroy; virtual;
		procedure Draw; virtual;
		function GetMaxHeight : integer; virtual;
		procedure Load(AFilename : String); virtual;
		procedure Save(AFilename : String); virtual;
		procedure Demo; virtual;
		procedure DemoFont(NullDac : boolean); virtual;
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
	FMUL.X := 1;
	FMUL.Y := 1;
	FOFS.X := 0;
	FOFS.Y := 0;
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
		(AValue.Height >= 12) then 
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
	GetMaxHeight := (PControl(Parent)^.Height div FMUL.Y) -
	  (PControl(Parent)^.FMargin.Top) -
	  (PControl(Parent)^.FMargin.Bottom) -
	  ExtraSize - 2;
end;

procedure TFontEdit.Draw;
var
	C : Char;
begin
	Inherited Draw;
	if FCUrrent = -1 then begin
		C := TextChar;
		TextAttr := FNullColor;
		TextChar := FNullChar;
		ClrScr;
		TextChar := C;
		Exit;
	end;
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
		if (Event.What or evKeyboard <> evKeyboard) or (Event.What and evMouse <> evMouse) then
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
	FWriteLn(CSpace('Press a key or move the Mouse to return.', Lo(ScreenMax) + 1));
	TextAttr := FDemoColor;
	
end;

{$ENDIF}
