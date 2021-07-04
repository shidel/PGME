{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{ ---TScrollBar --- }

{$IFDEF INTERFACE}
const
	class_TScrollBar = 'TScrollBar';
	
type
	PScrollBar = ^TScrollBar;
	TScrollBar = object(TControl)
	private
		function ValToPos(AValue : integer) : integer; virtual;
		function PosToVal(AValue : integer) : integer; virtual;
	
    public { protected }
		function  ObjectClass ( AName : String ) : String; virtual;
	public { protected }
		FEnds, FEndOver : boolean;
		FMin, FMax : LongInt;
		FPos : LongInt;
		FChars : record
			H, V : array[0..2] of integer;
		end;
		FThumb : String;
		FThumbColor : integer;
		procedure CheckMouse(var AEvent : TEvent); virtual;
	public
		constructor Create(AParent : PControl; AName : String);
		procedure Draw; virtual;
		procedure SetValue(AValue : integer); virtual;
		function GetValue : integer; virtual;
		procedure SetMin(AValue : integer); virtual;
		function GetMin : integer; virtual;
		procedure SetMax(AValue : integer); virtual;
		function GetMax : integer; virtual;
		procedure ApplyTheme( AConfig : PConfigFile ); virtual;
	end;
{$ENDIF}

{$IFDEF IMPLEMENTATION}
function TScrollBar.ObjectClass(AName : String) : String; 
begin
	if (AName = '') or (AName = class_TScrollBar) then 
		ObjectClass := class_TScrollBar
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TScrollBar.Create;
begin
	inherited Create(AParent, AName);
	FMin := 1;
	FMax := 10;
	FPos := 1;
	FEnds := True;
	FEndOver := True;
	{ FChars := #$11#$10#$B0#$B2 + #$1E#$1F#$B0#$B2; }
	FChars.H[0] := $11;
	FChars.H[1] := $C4;
	FChars.H[2] := $10;
	FChars.V[0] := $1E;
	FChars.V[1] := $B3;
	FChars.V[2] := $1F;
	FThumb := #$B0 { + #$B1#$B2} + #$B2 {+ #$B2#$B1} + #$B0;
	FThumbColor := FAttr.Normal;
	FTrigger := cmScrollBar;
end;

procedure TScrollBar.ApplyTheme( AConfig : PConfigFile );
begin
	inherited ApplyTheme( AConfig );
	FThumb := AConfig^.GetHexStr('THUMB', StrHex(FThumb, False));   
	AConfig^.GetInts('THUMBCOLOR', FThumbColor, Sizeof(FThumbColor) div SizeOf(Integer));
	AConfig^.GetInts('DISPLAY', FChars, Sizeof(FChars) div SizeOf(Integer));
	FEnds := StrBool(AConfig^.GetValue('ENDS', BoolStr(FEnds)));   
	FEndOver := StrBool(AConfig^.GetValue('ENDOVER', BoolStr(FEndOver)));   
end;

procedure TScrollBar.SetValue(AValue : integer);
begin
	if (AValue < FMin) then AValue := FMin;
	if (AValue > FMax) then AValue := FMax;
	if AValue = FPos then exit;
	FPos := AValue;
	Update;
end;

function TScrollBar.GetValue : integer;
begin
	GetValue := FPos;
end;

procedure TScrollBar.SetMax(AValue : integer);
begin
	if (AValue < FMin) then AValue := FMin;
	if (AValue < FPos) then FPos := AValue;
	if (AValue = FMax) then exit;
	FMax := AValue;
	Update;
end;

function TScrollBar.GetMax : integer;
begin
	GetMax := FMax;
end;

procedure TScrollBar.SetMin(AValue : integer);
begin
	if (AValue > FMax) then AValue := FMax;
	if (AValue > FPos) then FPos := AValue;
	if AValue = FMin then exit;
	FMin := AValue;
	Update;
end;

function TScrollBar.GetMin : integer;
begin
	GetMin := FMin;
end;

function TScrollBar.PosToVal(AValue : integer) : integer;
var
	TW, TH : integer;
begin
	Dec(AValue, Length(FThumb) div 2);
	if Height < Width then begin
		if FEndOver then begin
			TW := Width - Length(FThumb);
			if TW > 0 then 
				AValue := Round(AValue * (FMax - FMin) / (TW) + FMin)
			else
				AValue := 0;
		end else begin
			TW := Width - Length(FThumb) - 2;
			if TW > 0 then 
				AValue := Round(AValue * (FMax - FMin) / (TW) + FMin) - 1
			else
				AValue := 0;
		end;
	end else begin
		if FEndOver then begin
			TH := Height - Length(FThumb);
			if TH > 0 then 
				AValue := Round(AValue * (FMax - FMin) / (TH) + FMin)
			else
				AValue := 0;
		end else begin
			TH := Height - Length(FThumb) - 2;
			if TH > 0 then 
				AValue := Round(AValue * (FMax - FMin) / (TH) + FMin) - 1	
			else
				AValue := 0;
		end;
	end;
	if AValue < FMin then AValue := FMin;
	if AValue > FMax then AValue := FMax;
	
	PosToVal := AValue;
end;

function TScrollBar.ValToPos(AValue : integer) : integer;
var
	MM : integer;
begin
	MM := FMax - FMin;
	if MM = 0 then
		AValue := 0
	else if Height < Width then begin
		if FEndOver then 
			AValue := Round((FPos - FMin) * (Width - Length(FThumb)) / MM)
		else
			AValue := Round((FPos - FMin) * (Width - Length(FThumb) - 2) / MM) + 1;
	end else begin
		if FEndOver then 
			AValue := Round((FPos - FMin) * (Height - Length(FThumb)) / MM)
		else
			AValue := Round((FPos - FMin) * (Height - Length(FThumb) - 2) / MM) + 1;
	end;
	ValToPos := AValue;
end;

procedure TScrollBar.Draw;
var
	C : Char;
	P : integer;
begin
	C := TextChar;
	P := ValToPos(FPos);
	if Height < Width then begin
		TextChar := Char(FChars.H[1]);
		ClrScr;
		if FEnds then begin
			FWriteChar(Char(FChars.H[0]));
			GotoXY(Width, Height);
			FWriteChar(Char(FChars.H[2]));
		end;
		GotoXY(1 + P, 1);		
		TextAttr := FThumbColor;
		FWriteLn(FThumb);
	end else begin
		TextChar := Char(FChars.V[1]);
		ClrScr;
		if FEnds then begin
			FWriteChar(Char(FChars.V[0]));
			GotoXY(Width, Height);
			FWriteChar(Char(FChars.V[2]));
		end;
		TextAttr := FThumbColor;
		GotoXY(1, 1 + P);
		FWriteLn(FThumb);
	end;
	TextChar := C;
end;

procedure TScrollBar.CheckMouse(var AEvent : TEvent); 
var
	X, Y, P : integer;
	Hold : TEvent;
begin
	Hold := AEvent;
	inherited CheckMouse(AEvent);

	if not (FMouseOver and GetVisible and GetShown) then exit;
	X := (AEvent.Where.X - FWindow.Left);
	Y := (AEvent.Where.Y - FWindow.Top);
	if Height < Width then 
		P := PosToVal(X)
	else
		P := PosToVal(Y);
	
	if (Hold.What and evMouse <> evNothing) and (Hold.What and evMouse <> evMouseUp) then begin
		if (Hold.Buttons <> $0) and (P <> FPos) then begin
			SetValue(P);
			PutCommand(cmScrollbar, @Self);		
		end;
	end; 
end;
{$ENDIF}
