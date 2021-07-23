{ Copyright 2015 Jerome Shidel }

(* 
		
	This project and related files are subject to either the terms
	specified in the included LICENSE.TXT file or the GNU GPLv2.0.
	
*)

{ ---TRadioButton --- }

{$IFDEF INTERFACE}
const
	class_TRadioButton = 'TRadioButton';
	
type
	PRadioButton = ^TRadioButton;
	TRadioButton = object(TCheckbox)
	
    public { protected }
		function  ObjectClass ( AName : String ) : String; virtual;
	public { protected }
		function TextWidth : integer; virtual;
		procedure DoAutoSize; virtual;
	public
		constructor Create(AParent : PControl; AName : String);
		procedure ApplyTheme( AConfig : PConfigFile ); virtual;
		procedure SetState (AValue : Integer); virtual;
		procedure Draw; virtual;
	end;
{$ENDIF}

{$IFDEF IMPLEMENTATION}
function TRadioButton.ObjectClass(AName : String) : String; 
begin
	if (AName = '') or (AName = class_TRadioButton) then 
		ObjectClass := class_TRadioButton
	else
		ObjectClass := inherited ObjectClass(AName);
end;

constructor TRadioButton.Create;
begin
	inherited Create(AParent, AName);
	FTrigger := cmRadioButton;
	FCheckStr := '0;(%0)';
	FStateStr := #32'O';
	FPadStr := #32;
	FState := 0;
	FColorStr := FStateStr;
	FillChar(FColorStr[1], Length(FColorStr), TextAttr);
end;

procedure TRadioButton.ApplyTheme( AConfig : PConfigFile ); 
begin
	inherited ApplyTheme( AConfig );
	FColorStr := AConfig^.GetHexStr('COLORS', StrHex(FColorStr, False));   
	FStateStr := AConfig^.GetHexStr('STATES', StrHex(FStateStr, False));
	FPadStr := AConfig^.GetHexStr('SEPARATOR', StrHex(FPadStr, False));
	FCheckStr := AConfig^.GetValue('CHECK', FCheckStr);
end;

function TRadioButton.TextWidth : integer; 
var
	T : TFormatData;
begin
	T.ID := fmtString;
	T.StringValue := FStateStr[FState + 1];
	TextWidth := Length(GetCaption) + Length(FormatStr(FCheckStr, T, 1)) + Length(FPadStr);
end;

procedure TRadioButton.DoAutoSize; 
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

procedure TRadioButton.SetState (AValue : Integer); 
var
	P : PListItem;
	ok : boolean;
begin
	if (FState > 0) = (AValue > 0) then exit;
	if (AValue > 0) and Assigned(Parent) then begin
		P := PListItem(Parent)^.First;
		while Assigned(P) do begin
			if (P^.ClassOf(class_TRadioButton)) and (P <> @Self) then begin
				PRadioButton(P)^.SetState(0);
			end;
			P := PListItem(P)^.Next;
		end;
	end;
	inherited SetState(AValue);
end;

procedure TRadioButton.Draw; 
var
	T : TFormatData;
	C : integer;
	P, X : integer;
begin
	C := TextAttr;
	T.ID := fmtString;
	T.StringValue := FStateStr[FState + 1];
	TextAttr := Byte(FColorStr[1]);
	FWrite(FormatStr(FCheckStr, T, 1));
	P := Pos('%0', FCheckStr) - Pos(';', FCheckStr);
	if FState < Length(FColorStr) then 
		TextAttr := Byte(FColorStr[FState + 1])
	else
		TextAttr := Byte(FColorStr[Length(FColorStr)]);
	X := WhereX;
	GotoXY(P, 1);
	FWrite(FStateStr[FState + 1]);
	GotoXY(X, 1);
	TextAttr := C;
	FWrite(FPadStr + GetCaption);
end;
{$ENDIF}
