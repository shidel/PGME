{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{ ---TCheckBox --- }

{$IFDEF INTERFACE}
const
	class_TCheckBox = 'TCheckBox';
	
type
	PCheckBox = ^TCheckBox;
	TCheckBox = object(TPanel)
		FCheckStr : String;
		FStateStr : String;
		FColorStr : String;
		FPadStr : String;
		FState	: integer;
		function TextWidth : integer; virtual;
		procedure DoAutoSize; virtual;
		procedure TriggerHook; virtual;
    public { protected }
		function  ObjectClass ( AName : String ) : String; virtual;
	public
		constructor Create(AParent : PControl; AName : String);
		procedure ApplyTheme( AConfig : PConfigFile ); virtual;
		function Checked : boolean; virtual;
		function GetState : integer; virtual;
		procedure SetState (AValue : Integer); virtual;
		procedure Draw; virtual;
	end;
{$ENDIF}

{$IFDEF IMPLEMENTATION}
function TCheckBox.ObjectClass(AName : String) : String; 
begin
	if (AName = '') or (AName = class_TCheckBox) then 
		ObjectClass := class_TCheckBox
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TCheckBox.Create;
begin
	inherited Create(AParent, AName);
	FCheckStr := '0;[%0]';
	FStateStr := #32'X';
	FPadStr := #32;
	FState := 0;
	FAttr.Disabled := $78;
	FColorStr := FStateStr;
	FillChar(FColorStr[1], Length(FColorStr), TextAttr);
	FTrigger := cmCheckbox;
end;

procedure TCheckBox.ApplyTheme( AConfig : PConfigFile ); 
begin
	inherited ApplyTheme( AConfig );
	FColorStr := AConfig^.GetHexStr('STATE.COLOR', StrHex(FColorStr, False));   
	FStateStr := AConfig^.GetHexStr('STATE.CHAR', StrHex(FStateStr, False));
	FPadStr := AConfig^.GetHexStr('STATE.PAD', StrHex(FPadStr, False));
	FCheckStr := AConfig^.GetValue('STATE.CHECK', FCheckStr);
end;

function TCheckBox.TextWidth : integer; 
var
	T : TFormatData;
begin
	T.ID := fmtString;
	T.StringValue := FStateStr[FState + 1];
	TextWidth := Length(GetCaption) + Length(FormatStr(FCheckStr, T, 1)) + Length(FPadStr);
end;

procedure TCheckBox.DoAutoSize; 
var
	B : TBounds;
begin
	if FAutoSize then begin
		GetBounds(B);
		B.Width := TextWidth;
		SetBounds(B);
	end else		
		inherited DoAutoSize;
end;

procedure TCheckBox.TriggerHook; 
begin
	SetState(GetState + 1);
	inherited TriggerHook;
end;

function TCheckBox.Checked : boolean; 
begin
	Checked := FState > 0;
end;

function TCheckBox.GetState : integer;
begin
	GetState := FState;
end;

procedure TCheckBox.SetState (AValue : Integer); 
begin
	if AValue = FState then exit;
	if (AValue < 0) or (AValue >= Length(FStateStr)) then AValue := 0;
	FState := AValue;
	Update;
end;

procedure TCheckBox.Draw; 
var
	T : TFormatData;
	C : integer;
	P, X : integer;
begin
	C := TextAttr;
	T.ID := fmtString;
	T.StringValue := FStateStr[FState + 1];
	if FEnabled then
		TextAttr := Byte(FColorStr[1])
	else
		TextAttr := FAttr.Disabled;
	FWrite(FormatStr(FCheckStr, T, 1));
	P := Pos('%0', FCheckStr) - Pos(';', FCheckStr);
	if FEnabled then begin
		if FState < Length(FColorStr) then 
			TextAttr := Byte(FColorStr[FState + 1])
		else
			TextAttr := Byte(FColorStr[Length(FColorStr)]);
	end;
	X := WhereX;
	GotoXY(P, 1);
	FWrite(FStateStr[FState + 1]);
	GotoXY(X, 1);
	TextAttr := C;
	FWrite(FPadStr + GetCaption);
end;
{$ENDIF}
